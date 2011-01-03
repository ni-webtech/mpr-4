/*
    mprEvent.c - Event and dispatch services

    This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/***************************** Forward Declarations ***************************/

static void dequeueEvent(MprEvent *event);
static void manageEvent(MprEvent *event, int flags);
static void queueEvent(MprEvent *prior, MprEvent *event);

/************************************* Code ***********************************/
/*
    Create and queue a new event for service. Period is used as the delay before running the event and as the period between 
    events for continuous events.
 */
MprEvent *mprCreateEvent(MprDispatcher *dispatcher, cchar *name, int period, MprEventProc proc, void *data, int flags)
{
    MprEvent    *event;

    if ((event = mprAllocObj(MprEvent, manageEvent)) == 0) {
        return 0;
    }
    if (dispatcher == 0) {
        dispatcher = (flags & MPR_EVENT_QUICK) ? MPR->nonBlock : MPR->dispatcher;
    }
    mprInitEvent(dispatcher, event, name, period, proc, data, flags);
    mprQueueEvent(dispatcher, event);
    return event;
}


static void manageEvent(MprEvent *event, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        /*
            Events in dispatcher queues are marked by the dispatcher managers, not via event->next,prev
         */
    } else if (flags & MPR_MANAGE_FREE) {
        if (event->next) {
            mprRemoveEvent(event);
        }
    }
}


/*
    Statically initialize an event
 */
void mprInitEvent(MprDispatcher *dispatcher, MprEvent *event, cchar *name, int period, MprEventProc proc, void *data, 
    int flags)
{
    mprAssert(dispatcher);
    mprAssert(event);
    mprAssert(proc);
    mprAssert(event->next == 0);
    mprAssert(event->prev == 0);

    dispatcher->service->now = mprGetTime();
    event->name = name;
    event->timestamp = dispatcher->service->now;
    event->proc = proc;
    event->period = period;
    event->due = event->timestamp + period;
    event->data = data;
    event->dispatcher = dispatcher;
    event->next = event->prev = 0;
    event->flags = flags;
    event->continuous = (flags & MPR_EVENT_CONTINUOUS) ? 1 : 0;
}


/*
    Create an interval timer
 */
MprEvent *mprCreateTimerEvent(MprDispatcher *dispatcher, cchar *name, int period, MprEventProc proc, void *data, int flags)
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

    es = dispatcher->service;

    lock(es);
    q = &dispatcher->eventQ;
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
    es = dispatcher->service;
    lock(es);
    if (event->next) {
        dequeueEvent(event);
    }
    if (dispatcher->enabled && event->due == es->willAwake) {
        mprScheduleDispatcher(dispatcher);
    }
    unlock(es);
}


void mprRescheduleEvent(MprEvent *event, int period)
{
    MprEventService     *es;
    MprDispatcher       *dispatcher;

    dispatcher = event->dispatcher;
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

    es = dispatcher->service;
    event = 0;

    lock(es);
    next = dispatcher->eventQ.next;
    if (next != &dispatcher->eventQ) {
        if (next->due <= es->now) {
            event = next;
            dequeueEvent(event);
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

    es = dispatcher->service;

    lock(es);
    for (event = dispatcher->eventQ.next; event != &dispatcher->eventQ; event = event->next) {
        count++;
    }
    unlock(es);
    return count;
}


void mprInitEventQ(MprEvent *q)
{
    mprAssert(q);

    q->next = q;
    q->prev = q;
}


/*
    Append a new event. Must be locked when called.
 */
static void queueEvent(MprEvent *prior, MprEvent *event)
{
    mprAssert(prior);
    mprAssert(event);
    mprAssert(prior->next);

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
