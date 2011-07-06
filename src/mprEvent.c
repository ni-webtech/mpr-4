/*
    mprEvent.c - Event and dispatch services

    This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/***************************** Forward Declarations ***************************/

static void dequeueEvent(MprEvent *event);
static void initEvent(MprDispatcher *dispatcher, MprEvent *event, cchar *name, int period, void *proc, void *data, int flgs);
static void initEventQ(MprEvent *q);
static void manageEvent(MprEvent *event, int flags);
static void queueEvent(MprEvent *prior, MprEvent *event);

/************************************* Code ***********************************/
/*
    Create and queue a new event for service. Period is used as the delay before running the event and as the period between 
    events for continuous events.
 */
MprEvent *mprCreateEventQueue(cchar *name)
{
    MprEvent    *queue;

    if ((queue = mprAllocObj(MprEvent, manageEvent)) == 0) {
        return 0;
    }
    initEventQ(queue);
    return queue;
}


/*
    Create and queue a new event for service. Period is used as the delay before running the event and as the period between 
    events for continuous events.
 */
MprEvent *mprCreateEvent(MprDispatcher *dispatcher, cchar *name, int period, void *proc, void *data, int flags)
{
    MprEvent    *event;

    if ((event = mprAllocObj(MprEvent, manageEvent)) == 0) {
        return 0;
    }
    if (dispatcher == 0) {
        dispatcher = (flags & MPR_EVENT_QUICK) ? MPR->nonBlock : MPR->dispatcher;
    }
    initEvent(dispatcher, event, name, period, proc, data, flags);
    if (!(flags & MPR_EVENT_DONT_QUEUE)) {
        mprQueueEvent(dispatcher, event);
    }
    return event;
}


static void manageEvent(MprEvent *event, int flags)
{
    mprAssert(event->magic == MPR_EVENT_MAGIC);

    if (flags & MPR_MANAGE_MARK) {
        /*
            Events in dispatcher queues are marked by the dispatcher managers, not via event->next,prev
         */
        mprAssert(event->dispatcher == 0 || event->dispatcher->magic == MPR_DISPATCHER_MAGIC);
        mprMark(event->name);
        mprMark(event->dispatcher);
        mprMark(event->handler);
#if UNUSED
        mprMark(event->next);
        mprMark(event->prev);
#endif
        if (!(event->flags & MPR_EVENT_STATIC_DATA)) {
            mprMark(event->data);
        }

    } else if (flags & MPR_MANAGE_FREE) {
        if (event->next) {
            mprAssert(event->dispatcher == 0 || event->dispatcher->magic == MPR_DISPATCHER_MAGIC);
            mprRemoveEvent(event);
            event->magic = 1;
        }
    }
}


static void initEvent(MprDispatcher *dispatcher, MprEvent *event, cchar *name, int period, void *proc, void *data, 
    int flags)
{
    mprAssert(dispatcher);
    mprAssert(event);
    mprAssert(proc);
    mprAssert(event->next == 0);
    mprAssert(event->prev == 0);

    dispatcher->service->now = mprGetTime();
    event->name = sclone(name);
    event->timestamp = dispatcher->service->now;
    event->proc = proc;
    event->period = period;
    event->due = event->timestamp + period;
    event->data = data;
    event->dispatcher = dispatcher;
    event->next = event->prev = 0;
    event->flags = flags;
    event->continuous = (flags & MPR_EVENT_CONTINUOUS) ? 1 : 0;
    event->magic = MPR_EVENT_MAGIC;
}


/*
    Create an interval timer
 */
MprEvent *mprCreateTimerEvent(MprDispatcher *dispatcher, cchar *name, int period, void *proc, void *data, int flags)
{
    return mprCreateEvent(dispatcher, name, period, proc, data, MPR_EVENT_CONTINUOUS | flags);
}


void mprQueueEvent(MprDispatcher *dispatcher, MprEvent *event)
{
    MprEventService     *es;
    MprEvent            *prior, *q;

    mprAssert(dispatcher);
    mprAssert(event);
    mprAssert(event->timestamp);
    mprAssert(dispatcher->magic == MPR_DISPATCHER_MAGIC);
    mprAssert(event->magic == MPR_EVENT_MAGIC);

    es = dispatcher->service;

    lock(es);
    q = dispatcher->eventQ;
    for (prior = q->prev; prior != q; prior = prior->prev) {
        if (event->due > prior->due) {
            break;
        } else if (event->due == prior->due) {
            break;
        }
    }
    mprAssert(event->next == 0);
    mprAssert(event->prev == 0);
    mprAssert(prior->next);
    mprAssert(prior->prev);
    
    queueEvent(prior, event);
    es->eventCount++;
    if (dispatcher->enabled) {
        mprScheduleDispatcher(dispatcher);
    }
    unlock(es);
}


void mprRemoveEvent(MprEvent *event)
{
    MprEventService     *es;
    MprDispatcher       *dispatcher;

    dispatcher = event->dispatcher;
    if (dispatcher) {
        es = dispatcher->service;
        lock(es);
        if (event->next) {
            dequeueEvent(event);
        }
        if (dispatcher->enabled && event->due == es->willAwake && dispatcher->eventQ->next != dispatcher->eventQ) {
            mprScheduleDispatcher(dispatcher);
        }
        event->dispatcher = 0;
        unlock(es);
    }
}


void mprRescheduleEvent(MprEvent *event, int period)
{
    MprEventService     *es;
    MprDispatcher       *dispatcher;

    mprAssert(event->magic == MPR_EVENT_MAGIC);
    dispatcher = event->dispatcher;
    mprAssert(dispatcher->magic == MPR_DISPATCHER_MAGIC);

    es = dispatcher->service;

    lock(es);
    event->period = period;
    event->timestamp = es->now;
    event->due = event->timestamp + period;
    if (event->next) {
        mprRemoveEvent(event);
    }
    unlock(es);
    mprQueueEvent(dispatcher, event);
}


void mprStopContinuousEvent(MprEvent *event)
{
    event->continuous = 0;
}


void mprRestartContinuousEvent(MprEvent *event)
{
    event->continuous = 1;
    mprRescheduleEvent(event, event->period);
}


void mprEnableContinuousEvent(MprEvent *event, int enable)
{
    event->continuous = enable;
}


/*
    Get the next due event from the front of the event queue.
 */
MprEvent *mprGetNextEvent(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    MprEvent            *event, *next;

    mprAssert(dispatcher->magic == MPR_DISPATCHER_MAGIC);

    es = dispatcher->service;
    event = 0;

    lock(es);
    next = dispatcher->eventQ->next;
    if (next != dispatcher->eventQ) {
        if (next->due <= es->now) {
            event = next;
            dequeueEvent(event);
            mprAssert(event->magic == MPR_EVENT_MAGIC);
        }
    }
    unlock(es);
    return event;
}


int mprGetEventCount(MprDispatcher *dispatcher)
{
    MprEventService     *es;
    MprEvent            *event;
    int                 count;

    mprAssert(dispatcher->magic == MPR_DISPATCHER_MAGIC);

    es = dispatcher->service;

    lock(es);
	count = 0;
    for (event = dispatcher->eventQ->next; event != dispatcher->eventQ; event = event->next) {
        mprAssert(event->magic == MPR_EVENT_MAGIC);
        count++;
    }
    unlock(es);
    return count;
}


static void initEventQ(MprEvent *q)
{
    mprAssert(q);

    q->next = q;
    q->prev = q;
    q->magic = MPR_EVENT_MAGIC;
}


/*
    Append a new event. Must be locked when called.
 */
static void queueEvent(MprEvent *prior, MprEvent *event)
{
    mprAssert(prior);
    mprAssert(event);
    mprAssert(prior->next);
    mprAssert(event->magic == MPR_EVENT_MAGIC);
    mprAssert(event->dispatcher == 0 || event->dispatcher->magic == MPR_DISPATCHER_MAGIC);

    if (event->next) {
        dequeueEvent(event);
    }
    event->prev = prior;
    event->next = prior->next;
    prior->next->prev = event;
    prior->next = event;
}


/*
    Remove an event. Must be locked when called.
 */
static void dequeueEvent(MprEvent *event)
{
    mprAssert(event);
    mprAssert(event->next);
    mprAssert(event->magic == MPR_EVENT_MAGIC);
    mprAssert(event->dispatcher == 0 || event->dispatcher->magic == MPR_DISPATCHER_MAGIC);

    if (event->next) {
        event->next->prev = event->prev;
        event->prev->next = event->next;
        event->next = 0;
        event->prev = 0;
    }
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
