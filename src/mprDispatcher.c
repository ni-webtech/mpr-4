/*
    mprDispatcher.c - Event dispatch services

    This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/***************************** Forward Declarations ***************************/

static void dequeueDispatcher(MprDispatcher *dispatcher);
static void dispatchEvents(MprDispatcher *dispatcher);
static MprTime getDispatcherIdleTime(MprDispatcher *dispatcher, MprTime timeout);
static MprTime getIdleTime(MprEventService *es, MprTime timeout);
static MprDispatcher *getNextReadyDispatcher(MprEventService *es);
static void initDispatcherQ(MprEventService *es, MprDispatcher *q, cchar *name);
static int makeRunnable(MprDispatcher *dispatcher);
static void manageDispatcher(MprDispatcher *dispatcher, int flags);
static void manageEventService(MprEventService *es, int flags);
static void queueDispatcher(MprDispatcher *prior, MprDispatcher *dispatcher);
static void scheduleDispatcher(MprDispatcher *dispatcher);
static void serviceDispatcherMain(MprDispatcher *dispatcher);
static void serviceDispatcher(MprDispatcher *dp);

#define isRunning(dispatcher) (dispatcher->parent == &dispatcher->service->runQ)
#define isReady(dispatcher) (dispatcher->parent == &dispatcher->service->readyQ)
#define isWaiting(dispatcher) (dispatcher->parent == &dispatcher->service->waitQ)
#define isEmpty(dispatcher) (dispatcher->eventQ.next == &dispatcher->eventQ)

/************************************* Code ***********************************/
/*
    Create the overall dispatch service. There may be many event dispatchers.
 */
MprEventService *mprCreateEventService()
{
    MprEventService     *es;

    if ((es = mprAllocObj(MprEventService, manageEventService)) == 0) {
        return 0;
    }
    MPR->eventService = es;
    es->now = mprGetTime();
    es->mutex = mprCreateLock();
    es->waitCond = mprCreateCond();
    initDispatcherQ(es, &es->runQ, "running");
    initDispatcherQ(es, &es->readyQ, "ready");
    initDispatcherQ(es, &es->idleQ, "idle");
    initDispatcherQ(es, &es->waitQ, "waiting");
    return es;
}


static void manageEventService(MprEventService *es, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(es->waitCond);
        mprMark(es->mutex);

    } else if (flags & MPR_MANAGE_FREE) {
        /* Needed for race with manageDispatcher */
        es->mutex = 0;
    }
}


/*
    Create a disabled dispatcher. A dispatcher schedules events on a single dispatch queue.
 */
MprDispatcher *mprCreateDispatcher(cchar *name, int enable)
{
    MprEventService     *es;
    MprDispatcher       *dispatcher;

    if ((dispatcher = mprAllocObj(MprDispatcher, manageDispatcher)) == 0) {
        return 0;
    }
    dispatcher->name = sclone(name);
    dispatcher->cond = mprCreateCond();
    dispatcher->enabled = enable;
    es = dispatcher->service = MPR->eventService;
    mprInitEventQ(&dispatcher->eventQ);
    queueDispatcher(&es->idleQ, dispatcher);
    return dispatcher;
}


static void manageDispatcher(MprDispatcher *dispatcher, int flags)
{
    MprEventService     *es;
    MprEvent            *q, *event;

    es = dispatcher->service;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(dispatcher->name);
        mprMark(dispatcher->cond);
        lock(es);
        q = &dispatcher->eventQ;
        for (event = q->next; event != q; event = event->next) {
            if (!(event->flags & MPR_EVENT_STATIC)) {
                mprMark(event);
            }
        }
        unlock(es);
        
    } else if (flags & MPR_MANAGE_FREE) {
        if (es->mutex) {
            lock(es);
            dequeueDispatcher(dispatcher);
            unlock(es);
        }
    }
}


void mprEnableDispatcher(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    int                 mustWake;

    if (dispatcher == 0) {
        dispatcher = MPR->dispatcher;
    }
    es = dispatcher->service;
    mustWake = 0;
    lock(es);
    if (!dispatcher->enabled) {
        dispatcher->enabled = 1;
        LOG(7, "mprEnableDispatcher: %s", dispatcher->name);
        if (!isEmpty(dispatcher) && !isReady(dispatcher) && !isRunning(dispatcher)) {
            queueDispatcher(&es->readyQ, dispatcher);
            if (es->waiting) {
                mustWake = 1;
            }
        }
    }
    unlock(es);
    if (mustWake) {
        mprWakeWaitService();
    }
}


/*
    Schedule events. This can be called by any thread. Typically an app will dedicate one thread to be an event service 
    thread. This call will service events until the timeout expires or if MPR_SERVICE_ONE_THING is specified in flags, 
    after one event. This will service all enabled dispatcher queues and pending I/O events.
    @param dispatcher Primary dispatcher to service. This dispatcher is set to the running state and events on this
        dispatcher will be serviced without starting a worker thread. This can be set to NULL.
    @param timeout Time in milliseconds to wait. Set to zero for no wait. Set to -1 to wait forever.
    @returns Zero if not events occurred. Otherwise returns non-zero.
 */
int mprServiceEvents(int timeout, int flags)
{
    MprEventService     *es;
    MprDispatcher       *dp;
    MprTime             start, expires, delay;
    int                 beginEventCount, eventCount, justOne;

    es = MPR->eventService;
    beginEventCount = eventCount = es->eventCount;

    start = es->now = mprGetTime();
    expires = timeout < 0 ? (es->now + MPR_MAX_TIMEOUT) : (es->now + timeout);
    justOne = (flags & MPR_SERVICE_ONE_THING) ? 1 : 0;

    do {
        eventCount = es->eventCount;
        if (mprIsExiting()) {
            if (mprIsComplete()) {
                break;
            }
            expires = min(expires, start + MPR_TIMEOUT_STOP);
        }
        while ((dp = getNextReadyDispatcher(es)) != NULL) {
            serviceDispatcher(dp);
            if (justOne) {
                return abs(es->eventCount - beginEventCount);
            }
        } 
        if (es->eventCount == eventCount) {
            lock(es);
            delay = getIdleTime(es, expires - es->now);
            if (delay > 0) {
                es->waiting = 1;
                es->willAwake = es->now + delay;
                unlock(es);
                mprWaitForIO(MPR->waitService, (int) delay);
            } else {
                unlock(es);
            }
        }
        es->now = mprGetTime();
    } while (es->now < expires && !justOne);

    return abs(es->eventCount - beginEventCount);
}


/*
    Wait for an event to occur. Expect the event to signal the cond var.
    WARNING: this will enable GC while sleeping
 */
int mprWaitForEvent(MprDispatcher *dispatcher, int timeout)
{
    MprEventService     *es;
    MprTime             start, expires, delay;
    MprOsThread         thread;
    int                 claimed, signalled, wasRunning, runEvents;

    es = MPR->eventService;
    start = es->now = mprGetTime();

    if (dispatcher == NULL) {
        dispatcher = MPR->dispatcher;
    }
    //  MOB -- should have a re-entrancy check
    mprAssert(!dispatcher->waitingOnCond);
    if (dispatcher->waitingOnCond) {
        return MPR_ERR_BUSY;
    }
    thread = mprGetCurrentOsThread();
    expires = timeout < 0 ? (es->now + MPR_MAX_TIMEOUT) : (es->now + timeout);
    claimed = signalled = 0;

    lock(es);
    /*
        Acquire dedicates the dispatcher to this thread. If acquire fails, another thread is servicing this dispatcher.
        makeRunnable() prevents mprServiceEvents from servicing this dispatcher
     */
    wasRunning = isRunning(dispatcher);
    runEvents = (!wasRunning || dispatcher->owner == thread);
    if (runEvents) {
        if (!wasRunning) {
            makeRunnable(dispatcher);
        }
        dispatcher->owner = thread;
    }
    unlock(es);

    do {
        if (mprIsExiting()) {
            if (mprIsComplete()) {
                break;
            }
            expires = min(expires, start + MPR_TIMEOUT_STOP);
        }
        if (runEvents) {
            makeRunnable(dispatcher);
            dispatchEvents(dispatcher);
        }
        lock(es);
        delay = getDispatcherIdleTime(dispatcher, expires - es->now);
        dispatcher->waitingOnCond = 1;
        unlock(es);
        
        mprYield(MPR_YIELD_STICKY);
        if (mprWaitForCond(dispatcher->cond, (int) delay) == 0) {
            signalled++;
            dispatcher->waitingOnCond = 0;
            mprResetYield();
            break;
        }
        dispatcher->waitingOnCond = 0;
        mprResetYield();
        es->now = mprGetTime();
    } while (es->now < expires && !mprIsComplete());

    if (!wasRunning) {
        scheduleDispatcher(dispatcher);
        if (claimed) {
            dispatcher->owner = 0;
        }
    }
    return signalled ? 0 : MPR_ERR_TIMEOUT;
}


void mprWakeDispatchers()
{
    MprEventService     *es;
    MprDispatcher       *runQ, *dp;

    es = MPR->eventService;
    lock(es);
    runQ = &es->runQ;
    for (dp = runQ->next; dp != runQ; dp = dp->next) {
        mprSignalCond(dp->cond);
    }
    unlock(es);
}


int mprDispatchersAreIdle()
{
    MprEventService     *es;
    MprDispatcher       *runQ, *dispatcher;
    int                 idle;

    es = MPR->eventService;
    runQ = &es->runQ;
    lock(es);
    dispatcher = runQ->next;
    if (dispatcher == runQ) {
        idle = 1;
    } else {
        idle = (&dispatcher->eventQ == dispatcher->eventQ.next);
    }
    unlock(es);
    return idle;
}


/*
    Relay an event to a new dispatcher. This invokes the callback proc as though it was invoked from the given dispatcher. 
 */
void mprRelayEvent(MprDispatcher *dispatcher, MprEventProc proc, void *data, MprEvent *event)
{
    mprAssert(!isRunning(dispatcher));
    mprAssert(dispatcher->owner == 0);

    if (event) {
        event->timestamp = dispatcher->service->now;
    }
    dispatcher->enabled = 1;
    dispatcher->owner = mprGetCurrentOsThread();
    makeRunnable(dispatcher);
    (proc)(data, event);
    scheduleDispatcher(dispatcher);
}


/*
    Schedule the dispatcher. If the dispatcher is already running then it is not modified. If the event queue is empty, 
    the dispatcher is moved to the idleQ. If there is a past-due event, it is moved to the readyQ. If there is a future 
    event pending, it is put on the waitQ.
 */
void mprScheduleDispatcher(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    MprEvent            *event;
    int                 mustWakeWaitService, mustWakeCond;
   
    mprAssert(dispatcher);
    mprAssert(dispatcher->enabled);
    es = dispatcher->service;

    lock(es);
    if (isRunning(dispatcher) || !dispatcher->enabled) {
        /* Wake up if waiting in mprWaitForIO */
        mustWakeWaitService = es->waiting;
        mustWakeCond = dispatcher->waitingOnCond;

    } else {
        if (isEmpty(dispatcher)) {
            queueDispatcher(&es->idleQ, dispatcher);
            unlock(es);
            return;
        }
        event = dispatcher->eventQ.next;
        mustWakeWaitService = mustWakeCond = 0;
        if (event->due > es->now) {
            queueDispatcher(&es->waitQ, dispatcher);
            if (event->due < es->willAwake) {
                mustWakeWaitService = 1;
                mustWakeCond = dispatcher->waitingOnCond;
            }
        } else {
            queueDispatcher(&es->readyQ, dispatcher);
            mustWakeWaitService = es->waiting;
            mustWakeCond = dispatcher->waitingOnCond;
        }
    }
    unlock(es);
    if (mustWakeCond) {
        mprSignalDispatcher(dispatcher);
    }
    if (mustWakeWaitService) {
        mprWakeWaitService();
    }
}


/*
    Dispatch events for a dispatcher. A dispatcher is ever only run on one thread, and so this code is single-threaded.
 */
static void dispatchEvents(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    MprEvent            *event;
    int                 count;

    mprAssert(dispatcher->enabled);

    es = dispatcher->service;
    LOG(7, "dispatchEvents for %s", dispatcher->name);

    /* No locking because this must run in a single owning thread */
    for (count = 0; (event = mprGetNextEvent(dispatcher)) != 0; count++) {
        if (event->continuous) {
            /* Reschedule if continuous */
            event->timestamp = dispatcher->service->now;
            event->due = event->timestamp + (event->period ? event->period : 1);
            mprQueueEvent(dispatcher, event);
        }
        mprAssert(event->proc);
        LOG(7, "Call event %s", event->name);
        (*event->proc)(event->data, event);
    }
    if (count && es->waiting) {
        es->eventCount += count;
        mprWakeWaitService();
    }
}


static void serviceDispatcher(MprDispatcher *dispatcher)
{
    mprAssert(isRunning(dispatcher));
    mprAssert(dispatcher->owner == 0);
    
    dispatcher->owner = mprGetCurrentOsThread();

    if (dispatcher == MPR->nonBlock) {
        serviceDispatcherMain(dispatcher);

    } else if (dispatcher->requiredWorker) {
        mprActivateWorker(dispatcher->requiredWorker, (MprWorkerProc) serviceDispatcherMain, dispatcher);

    } else {
        if (mprStartWorker((MprWorkerProc) serviceDispatcherMain, dispatcher) < 0) {
            /* Can't start a worker thread. Put back on the wait queue */
            queueDispatcher(&dispatcher->service->waitQ, dispatcher);
        } 
    }
}


/*
    Service a single dispatcher
 */
static void serviceDispatcherMain(MprDispatcher *dispatcher)
{
    mprAssert(isRunning(dispatcher));

    dispatcher->owner = mprGetCurrentOsThread();
    dispatchEvents(dispatcher);
    dispatcher->owner = 0;
    scheduleDispatcher(dispatcher);
}


/*
    Get the next (ready) dispatcher off given runQ and move onto the runQ
 */
static MprDispatcher *getNextReadyDispatcher(MprEventService *es)
{
    MprDispatcher   *dp, *next, *readyQ, *waitQ, *dispatcher;
    MprEvent        *event;

    waitQ = &es->waitQ;
    readyQ = &es->readyQ;

    lock(es);
    if (readyQ->next == readyQ) {
        /*
            ReadyQ is empty, try to transfer a dispatcher with due events onto the readyQ
         */
        for (dp = waitQ->next; dp != waitQ; dp = next) {
            next = dp->next;
            event = dp->eventQ.next;
            if (event->due <= es->now) {
                queueDispatcher(&es->readyQ, dp);
                break;
            }
        }
    }
    if (readyQ->next != readyQ) {
        dispatcher = readyQ->next;
        queueDispatcher(&es->runQ, dispatcher);
        mprAssert(dispatcher->enabled);
        dispatcher->owner = 0;
    } else {
        dispatcher = NULL;
    }
    unlock(es);
    mprAssert(dispatcher == NULL || isRunning(dispatcher));
    return dispatcher;
}


/*
    Get the time to sleep till the next pending event. Must be called locked.
 */
static MprTime getIdleTime(MprEventService *es, MprTime timeout)
{
    MprDispatcher   *readyQ, *waitQ, *dp;
    MprEvent        *event;
    MprTime         delay;

    waitQ = &es->waitQ;
    readyQ = &es->readyQ;

    if (readyQ->next != readyQ) {
        delay = 0;
    } else if (mprIsExiting()) {
        delay = 10;
    } else {
        delay = MPR_MAX_TIMEOUT;
        /*
            Examine all the dispatchers on the waitQ
         */
        for (dp = waitQ->next; dp != waitQ; dp = dp->next) {
            event = dp->eventQ.next;
            if (event != &dp->eventQ) {
                delay = min(delay, (event->due - es->now));
                if (delay <= 0) {
                    break;
                }
            }
        }
        delay = min(delay, timeout);
    }
    return delay;
}


static MprTime getDispatcherIdleTime(MprDispatcher *dispatcher, MprTime timeout)
{
    MprEvent    *next;
    MprTime     delay;

    if (timeout < 0) {
        timeout = 0;
    } else {
        next = dispatcher->eventQ.next;
        delay = MPR_MAX_TIMEOUT;
        if (next != &dispatcher->eventQ) {
            delay = (next->due - dispatcher->service->now);
            if (delay < 0) {
                delay = 0;
            }
        }
        timeout = min(delay, timeout);
    }
    return timeout;
}


static void initDispatcherQ(MprEventService *es, MprDispatcher *q, cchar *name)
{
    q->next = q;
    q->prev = q;
    q->parent = q;
    q->name = name;
    q->service = es;
}


/*
    Append a new dispatcher
 */
static void queueDispatcher(MprDispatcher *prior, MprDispatcher *dispatcher)
{
    lock(dispatcher->service);
    if (dispatcher->parent) {
        dequeueDispatcher(dispatcher);
    }
    dispatcher->parent = prior->parent;
    dispatcher->prev = prior;
    dispatcher->next = prior->next;
    prior->next->prev = dispatcher;
    prior->next = dispatcher;
    unlock(dispatcher->service);
}


/*
    Remove an dispatcher
 */
static void dequeueDispatcher(MprDispatcher *dispatcher)
{
    lock(dispatcher->service);
    if (dispatcher->next) {
        dispatcher->next->prev = dispatcher->prev;
        dispatcher->prev->next = dispatcher->next;
        dispatcher->next = 0;
        dispatcher->prev = 0;
        dispatcher->parent = 0;
    }
    unlock(dispatcher->service);
}


static void scheduleDispatcher(MprDispatcher *dispatcher)
{
    MprEventService     *es;

    es = dispatcher->service;

    lock(es);
    dequeueDispatcher(dispatcher);
    mprScheduleDispatcher(dispatcher);
    unlock(es);
}


static int makeRunnable(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    int                 wasRunning;

    es = dispatcher->service;

    lock(es);
    wasRunning = isRunning(dispatcher);
    if (!isRunning(dispatcher)) {
        queueDispatcher(&es->runQ, dispatcher);
    }
    unlock(es);
    return wasRunning;
}


/*
    Designate the required worker thread to run the event
 */
void mprDedicateWorkerToDispatcher(MprDispatcher *dispatcher, MprWorker *worker)
{
    dispatcher->requiredWorker = worker;
    mprDedicateWorker(worker);
}


void mprReleaseWorkerFromDispatcher(MprDispatcher *dispatcher, MprWorker *worker)
{
    dispatcher->requiredWorker = 0;
    mprReleaseWorker(worker);
}


void mprSignalDispatcher(MprDispatcher *dispatcher)
{
    if (dispatcher == NULL) {
        dispatcher = MPR->dispatcher;
    }
    mprSignalCond(dispatcher->cond);
}


/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
    
    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire 
    a commercial license from Embedthis Software. You agree to be fully bound 
    by the terms of either license. Consult the LICENSE.TXT distributed with 
    this software for full details.
    
    This software is open source; you can redistribute it and/or modify it 
    under the terms of the GNU General Public License as published by the 
    Free Software Foundation; either version 2 of the License, or (at your 
    option) any later version. See the GNU General Public License for more 
    details at: http://www.embedthis.com/downloads/gplLicense.html
    
    This program is distributed WITHOUT ANY WARRANTY; without even the 
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
    
    This GPL license does NOT permit incorporating this software into 
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses 
    for this software and support services are available from Embedthis 
    Software at http://www.embedthis.com 
    
    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
