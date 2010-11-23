/*
    mprDispatcher.c - Event dispatch services

    This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/***************************** Forward Declarations ***************************/

static void dequeueDispatcher(MprDispatcher *dispatcher);
static int getIdleTime(MprEventService *ds, MprDispatcher *dispatcher);
static MprDispatcher *getNextReadyDispatcher(MprEventService *es);
static void initDispatcherQ(MprEventService *ds, MprDispatcher *q, cchar *name);
static bool isIdle(MprEventService *es, MprDispatcher *dispatcher);
static void manageDispatcher(MprDispatcher *dispatcher, int flags);
static void manageEventService(MprEventService *es, int flags);
static void queueDispatcher(MprDispatcher *prior, MprDispatcher *dispatcher);

#define isRunning(dispatcher) (dispatcher->parent == &dispatcher->service->runQ)
#define isReady(dispatcher) (dispatcher->parent == &dispatcher->service->readyQ)
#define isWaiting(dispatcher) (dispatcher->parent == &dispatcher->service->waitQ)
#define isEmpty(dispatcher) (dispatcher->eventQ.next == &dispatcher->eventQ)

/************************************* Code ***********************************/
/*
    Create the overall dispatch service. There may be many event dispatchers.
 */
MprEventService *mprCreateEventService(MprCtx ctx)
{
    MprEventService     *es;
    Mpr                 *mpr;

    mpr = mprGetMpr(ctx);
    if ((es = mprAllocObj(ctx, MprEventService, manageEventService)) == 0) {
        return 0;
    }
    mpr->eventService = es;
    es->now = mprGetTime(ctx);
    es->mutex = mprCreateLock(es);
    es->waitCond = mprCreateCond(es);
    initDispatcherQ(es, &es->runQ, "running");
    initDispatcherQ(es, &es->readyQ, "ready");
    initDispatcherQ(es, &es->idleQ, "idle");
    initDispatcherQ(es, &es->waitQ, "waiting");

    //  MOB -- move this to MPR
    mpr->dispatcher = mprCreateDispatcher(es, "mpr", 1);
    return es;
}


static void manageEventService(MprEventService *es, int flags)
{
    MprDispatcher   *dp, *q;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(es->waitCond);
        mprMark(es->mutex);

        q = &es->runQ;
        for (dp = q->next; dp != q; dp = dp->next) {
            mprMark(dp);
        }
        q = &es->readyQ;
        for (dp = q->next; dp != q; dp = dp->next) {
            mprMark(dp);
        }
        q = &es->waitQ;
        for (dp = q->next; dp != q; dp = dp->next) {
            mprMark(dp);
        }
        q = &es->idleQ;
        for (dp = q->next; dp != q; dp = dp->next) {
            mprMark(dp);
        }
    }
}


/*
    Dispatch events for a dispatcher. This is invoked on a worker thread, but the dispatcher service ensures only this
    thread has control over the dispatcher. This routine MUST, ABSOLUTELY, ALWAYS, UNCONDITIONALLY run single-threaded. 
    There may be many dispatchers however. 

    Returns a count of the events serviced. Returns -1 if the dispatcher was deleted in a callback.
 */
static int dispatchEvents(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    MprEvent            *event;
    int                 count;

    mprAssert(dispatcher->enabled);
    mprAssert(isRunning(dispatcher));

    es = dispatcher->service;

#if BLD_DEBUG
    lock(es);
    if (dispatcher->active && dispatcher->active != mprGetCurrentThread(es)) {
        unlock(es);
        return 0;
    }
    dispatcher->active = mprGetCurrentThread(es);
    unlock(es);
#endif
    LOG(dispatcher, 7, "dispatchEvents for %s", dispatcher->name);

    /* No locking because this must run single-threaded */

    for (count = 0; (event = mprGetNextEvent(dispatcher)) != 0; count++) {
        if (event->continuous) {
            /* Reschedule if continuous */
            event->timestamp = dispatcher->service->now;
            event->due = event->timestamp + (event->period ? event->period : 1);
            mprQueueEvent(dispatcher, event);
        }
        mprAssert(event->proc);
        LOG(dispatcher, 7, "Call event %s", event->name);
        dispatcher->inUse++;

        (*event->proc)(event->data, event);

        if (--dispatcher->inUse == 0 && dispatcher->deleted) {
            mprFree(dispatcher);
            return -1;
        }
    }
    if (count) {
        lock(es);
        es->eventCount += count;
        if (es->waiting) {
            mprWakeWaitService(es);
        }
        unlock(es);
    }
#if BLD_DEBUG
    dispatcher->active = 0;
#endif
    return count;
}


/*
    Service a single dispatcher. Runs in a worker thread.
 */
static void serviceDispatcher(MprDispatcher *dispatcher)
{
    MprEventService     *es;

    mprAssert(isRunning(dispatcher));

    es = dispatcher->service;
    if (dispatchEvents(dispatcher) < 0) {
        return;
    }
    lock(es);
    mprAssert(isRunning(dispatcher));
    dequeueDispatcher(dispatcher);
    mprScheduleDispatcher(dispatcher);
    unlock(es);
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
int mprServiceEvents(MprCtx ctx, MprDispatcher *dispatcher, int timeout, int flags)
{
    MprEventService     *es;
    MprDispatcher       *dp;
    MprTime             expires;
    Mpr                 *mpr;
    int                 count, delay, wasRunning, beginEventCount, eventCount, justOne, idle;

    mprAssert(ctx);
    mpr = mprGetMpr(ctx);

    es = mpr->eventService;
    es->now = mprGetTime(es);
    expires = timeout < 0 ? (es->now + MPR_MAX_TIMEOUT) : (es->now + timeout);
    beginEventCount = eventCount = es->eventCount;
    justOne = flags & MPR_SERVICE_ONE_THING;
    wasRunning = 0;

    lock(es);
    if (dispatcher) {
        wasRunning = isRunning(dispatcher);
        if (!isRunning(dispatcher)) {
            queueDispatcher(&es->runQ, dispatcher);
        }
    }
    unlock(es);

    do {
        eventCount = es->eventCount;
        if (dispatcher) {
            if ((count = dispatchEvents(dispatcher)) < 0) {
                return abs(es->eventCount - eventCount);
            } else if (count > 0 && justOne) {
                break;
            }
        }
        delay = (int) (expires - es->now);
        if (delay > 0 && dispatcher) {
            idle = getIdleTime(es, dispatcher);
            delay = min(delay, idle);
        }
        while ((dp = getNextReadyDispatcher(es)) != NULL && !mprIsComplete(mpr)) {
            mprAssert(isRunning(dp));
            if (dp->requiredWorker) {
                mprActivateWorker(dp->requiredWorker, (MprWorkerProc) serviceDispatcher, dp);
            } else {
//  MOB -- need option to run events without starting a worker
                if (mprStartWorker(dp, (MprWorkerProc) serviceDispatcher, dp) < 0) {
                    /* Can't start a worker thread. Put back on the wait queue */
                    queueDispatcher(&es->waitQ, dp);
                } 
            }
            if (justOne) {
                break;
            }
        } 
        lock(es);
        if (delay > 0) {
            if (es->eventCount == eventCount && isIdle(es, dispatcher)) {
                mprYieldThread(NULL);
                if (es->waiting) {
                    unlock(es);
                    mprWaitForMultiCond(es->waitCond, delay);
                } else {
                    es->waiting = 1;
                    unlock(es);
#if MPR_GC_WORKERS == 0
                    if (mprIsTimeForGC(delay)) {
                        mprCollectGarbage();
                    }
#endif
                    mprWaitForIO(mpr->waitService, delay);
                    es->waiting = 0;
                    mprSignalMultiCond(es->waitCond);
                }
                mprResumeThread(NULL);
            } else unlock(es);
        } else unlock(es);

        es->now = mprGetTime(mpr);
    } while (es->now < expires && !justOne && !mprIsComplete(es));

    if (dispatcher && !wasRunning) {
        lock(es);
        dequeueDispatcher(dispatcher);
        mprScheduleDispatcher(dispatcher);
        unlock(es);
    }
    return abs(es->eventCount - beginEventCount);
}


/*
    Create a disabled dispatcher. A dispatcher schedules events on a single dispatch queue.
 */
MprDispatcher *mprCreateDispatcher(MprCtx ctx, cchar *name, int enable)
{
    MprEventService     *es;
    MprDispatcher       *dispatcher;

    if ((dispatcher = mprAllocObj(ctx, MprDispatcher, manageDispatcher)) == 0) {
        return 0;
    }
    //  MOB - cleanup. Requires name to be static. It is this way because some dispatchers are not allocated.
    dispatcher->name = name;
    dispatcher->enabled = enable;
    es = dispatcher->service = mprGetMpr(ctx)->eventService;
    mprInitEventQ(&dispatcher->eventQ);
    queueDispatcher(&es->idleQ, dispatcher);
    return dispatcher;
}


static void manageDispatcher(MprDispatcher *dispatcher, int flags)
{
    MprEvent        *q, *event;

    if (flags & MPR_MANAGE_MARK) {
        q = &dispatcher->eventQ;
        for (event = q->next; event != q; event = event->next) {
            mprMark(event);
        }
    } else if (flags & MPR_MANAGE_FREE) {
        MprEventService     *es;
        es = dispatcher->service;
        lock(es);
        dequeueDispatcher(dispatcher);
        dispatcher->deleted = 1;
        if (dispatcher->inUse) {
            mprAssert(!dispatcher->inUse);
            unlock(es);
        }
        unlock(es);
    }
}


void mprEnableDispatcher(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    int                 mustWake;

    es = dispatcher->service;
    mustWake = 0;
    lock(es);
    if (!dispatcher->enabled) {
        dispatcher->enabled = 1;
        LOG(es, 7, "mprEnableDispatcher: %s", dispatcher->name);
        if (!isEmpty(dispatcher) && !isReady(dispatcher) && !isRunning(dispatcher)) {
            queueDispatcher(&es->readyQ, dispatcher);
            if (es->waiting) {
                mustWake = 1;
            }
        }
    }
    unlock(es);
    if (mustWake) {
        mprWakeWaitService(es);
    }
}


/*
    Relay an event to a new dispatcher. This invokes the callback proc as though it was invoked from the given
    dispatcher. 
 */
void mprRelayEvent(MprDispatcher *dispatcher, MprEventProc proc, void *data, MprEvent *event)
{
    MprEventService     *es;
    int                 wasRunning;

    es = dispatcher->service;
    dispatcher->enabled = 1;
    if (event) {
        event->timestamp = es->now;
    }
    lock(es);
    wasRunning = isRunning(dispatcher);
    if (!wasRunning) {
        queueDispatcher(&es->runQ, dispatcher);
    }
    unlock(es);

    dispatcher->inUse++;

    (proc)(data, event);

    if (--dispatcher->inUse == 0 && dispatcher->deleted) {
        mprFree(dispatcher);
    } else if (!wasRunning) {
        //  MOB -- why reschedule?
        lock(es);
        dequeueDispatcher(dispatcher);
        mprScheduleDispatcher(dispatcher);
        unlock(es);
    }
}


/*
    Schedule the dispatcher. If the dispatcher is already running then it is not modified. If empty, it is moved to 
    the idleQ. If there is a past-due event, it is moved to the readQ. If there is a future event, it is put on the waitQ.
 */
void mprScheduleDispatcher(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    MprEvent            *event;
    MprWaitService      *ws;
    int                 mustAwake;
   
    mprAssert(dispatcher);
    mprAssert(dispatcher->enabled);

    es = dispatcher->service;
    ws = mprGetMpr(dispatcher)->waitService;

    lock(es);
    //  MOB - why awake if already running?
    if (isRunning(dispatcher) || !dispatcher->enabled) {
        mustAwake = es->waiting;
    } else {
        if (isEmpty(dispatcher)) {
            queueDispatcher(&es->idleQ, dispatcher);
            unlock(es);
            return;
        }
        event = dispatcher->eventQ.next;
        mustAwake = 0;

        if (event->due > es->now) {
            queueDispatcher(&es->waitQ, dispatcher);
            if (event->due < ws->willAwake) {
                mustAwake = 1;
            }
        } else {
            queueDispatcher(&es->readyQ, dispatcher);
            mustAwake = es->waiting;
        }
    }
    unlock(es);
    if (mustAwake) {
        mprWakeWaitService(es);
    }
}


static bool isIdle(MprEventService *es, MprDispatcher *dispatcher)
{
    MprEvent    *next;
    bool        idle;

    lock(es);
    idle = 1;
    if (es->readyQ.next != &es->readyQ) {
        idle = 0;
    } else if (dispatcher) {
        next = dispatcher->eventQ.next;
        if (dispatcher && next != &dispatcher->eventQ && next->due <= es->now) {
            idle = 0;
        }
    }
    unlock(es);
    return idle;
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
        unlock(es);
        mprAssert(dispatcher->enabled);
        mprAssert(isRunning(dispatcher));
        return dispatcher;
    }
    unlock(es);
    return 0;
}


static int getIdleTime(MprEventService *es, MprDispatcher *dispatcher)
{
    MprDispatcher   *readyQ, *waitQ, *dp;
    MprEvent        *event;
    int             delay;

    waitQ = &es->waitQ;
    readyQ = &es->readyQ;

    lock(es);
    if (readyQ->next != readyQ) {
        delay = 0;
    } else {
        delay = MPR_MAX_TIMEOUT;
        /*
            Examine the primary dispatcher
         */
        event = dispatcher->eventQ.next;
        if (event != &dispatcher->eventQ) {
            delay = min(delay, (int) (event->due - es->now));
        }
        /*
            Examine all the dispatchers on the waitQ
         */
        for (dp = waitQ->next; dp != waitQ; dp = dp->next) {
            event = dp->eventQ.next;
            if (event != &dp->eventQ) {
                delay = min(delay, (int) (event->due - es->now));
                if (delay <= 0) {
                    break;
                }
            }
        }
    }
    unlock(es);
    return delay;
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


/*
    Get the primary Mpr dispatcher
 */
MprDispatcher *mprGetDispatcher(MprCtx ctx)
{
    Mpr     *mpr;
    
    mpr = mprGetMpr(ctx);
    return mpr->dispatcher;
}


/*
 *  Designate the required worker thread to run the event
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


/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
    
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
    
    @end
 */
