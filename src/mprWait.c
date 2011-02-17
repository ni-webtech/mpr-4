/*
    mprWait.c - Wait for I/O service.

    This module provides wait management for sockets and other file descriptors and allows users to create wait
    handlers which will be called when I/O events are detected. Multiple backends (one at a time) are supported.

    This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

/***************************** Forward Declarations ***************************/

static void ioEvent(void *data, MprEvent *event);
static void manageWaitService(MprWaitService *ws, int flags);
static void manageWaitHandler(MprWaitHandler *wp, int flags);

/************************************ Code ************************************/
/*
    Initialize the service
 */
MprWaitService *mprCreateWaitService()
{
    MprWaitService  *ws;

    ws = mprAllocObj(MprWaitService, manageWaitService);
    if (ws == 0) {
        return 0;
    }
    MPR->waitService = ws;
    ws->handlers = mprCreateList(-1, MPR_LIST_STATIC_VALUES);
    ws->mutex = mprCreateLock();
    ws->spin = mprCreateSpinLock();
    mprCreateNotifierService(ws);
    return ws;
}


static void manageWaitService(MprWaitService *ws, int flags)
{
    lock(ws);
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ws->handlers);
        mprMark(ws->handlerMap);
        mprMark(ws->mutex);
        mprMark(ws->spin);
    }
#if MPR_EVENT_ASYNC
    /* Nothing to manage */
#endif
#if MPR_EVENT_KQUEUE
    mprManageKqueue(ws, flags);
#endif
#if MPR_EVENT_EPOLL
    mprManageEpoll(ws, flags);
#endif
#if MPR_EVENT_POLL
    mprManagePoll(ws, flags);
#endif
#if MPR_EVENT_SELECT
    mprManageSelect(ws, flags);
#endif
    unlock(ws);
}


//  MOB -- inline in createWaitHandler

static MprWaitHandler *mprInitWaitHandler(MprWaitHandler *wp, int fd, int mask, MprDispatcher *dispatcher, 
        MprEventProc proc, void *data, int flags)
{
    MprWaitService  *ws;

    mprAssert(fd >= 0);

    ws = MPR->waitService;
    if (mprGetListLength(ws->handlers) == FD_SETSIZE) {
        mprError("io: Too many io handlers: %d\n", FD_SETSIZE);
        return 0;
    }
#if BLD_UNIX_LIKE || VXWORKS
    if (fd >= FD_SETSIZE) {
        mprError("File descriptor %d exceeds max io of %d", fd, FD_SETSIZE);
    }
#endif
    wp->fd              = fd;
    wp->notifierIndex   = -1;
    wp->dispatcher      = dispatcher;
    wp->proc            = proc;
    wp->flags           = 0;
    wp->handlerData     = data;
    wp->service         = ws;
    wp->state           = MPR_HANDLER_DISABLED;
    wp->flags           = flags;

    if (mask) {
        lock(ws);
        wp->state = MPR_HANDLER_ENABLED;
        if (mprAddItem(ws->handlers, wp) < 0) {
            unlock(ws);
            return 0;
        }
        mprAddNotifier(ws, wp, mask);
        unlock(ws);
        mprWakeWaitService();
    }
    return wp;
}


MprWaitHandler *mprCreateWaitHandler(int fd, int mask, MprDispatcher *dispatcher, MprEventProc proc, void *data, int flags)
{
    MprWaitHandler  *wp;

    mprAssert(fd >= 0);

    if ((wp = mprAllocObj(MprWaitHandler, manageWaitHandler)) == 0) {
        return 0;
    }
    return mprInitWaitHandler(wp, fd, mask, dispatcher, proc, data, flags);
}


static void manageWaitHandler(MprWaitHandler *wp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(wp->handlerData);
        mprMark(wp->dispatcher);
        mprMark(wp->service);
        mprMark(wp->next);
        mprMark(wp->prev);
        mprMark(wp->requiredWorker);
        mprMark(wp->thread);
        mprMark(wp->callbackComplete);

    } else if (flags & MPR_MANAGE_FREE) {
        mprRemoveWaitHandler(wp);
    }
}


void mprRemoveWaitHandler(MprWaitHandler *wp)
{
    MprWaitService      *ws;

    if (wp == 0) {
        return;
    }
    ws = wp->service;

    /*
        Lock the service to stabilize the list, then lock the handler to prevent callbacks. 
     */
    lock(ws);
    if (wp->fd >= 0) {
        if (wp->desiredMask) {
            mprRemoveNotifier(wp);
        }
        mprRemoveItem(ws->handlers, wp);
        wp->fd = -1;
        if (wp->event.next) {
            mprRemoveEvent(&wp->event);
        }
    }
    mprWakeWaitService();
    unlock(ws);
}


void mprWakeWaitService()
{
    mprWakeNotifier();
}


void mprQueueIOEvent(MprWaitHandler *wp)
{
    MprDispatcher   *dispatcher;
    MprEvent        *event;

    mprAssert(wp->state == MPR_HANDLER_ENABLED);
    mprAssert(wp->desiredMask == 0);

    if (wp->flags & MPR_WAIT_NEW_DISPATCHER) {
        dispatcher = mprCreateDispatcher("IO", 1);
    } else {
        dispatcher = (wp->dispatcher) ? wp->dispatcher: mprGetDispatcher();
    }
    wp->state = MPR_HANDLER_QUEUED;
    event = &wp->event;
    mprInitEvent(dispatcher, event, "IOEvent", 0, ioEvent, (void*) wp->handlerData, MPR_EVENT_STATIC);
    event->fd = wp->fd;
    event->mask = wp->presentMask;
    event->handler = wp;
    mprQueueEvent(dispatcher, event);
}


static void ioEvent(void *data, MprEvent *event)
{
    MprWaitHandler  *wp;

    wp = event->handler;
    mprAssert(wp->state == MPR_HANDLER_QUEUED);
    mprAssert(wp->desiredMask == 0);
    wp->state = MPR_HANDLER_ACTIVE;
    wp->proc(data, event);
}


void mprDisableWaitEvents(MprWaitHandler *wp)
{
    //  MOB Check events already disabled - generally a programming error
    mprAssert(wp->desiredMask);
    mprAssert(wp->state == MPR_HANDLER_ENABLED);

    wp->state = MPR_HANDLER_DISABLED;
    if (wp->desiredMask) {
        mprRemoveNotifier(wp);
        mprWakeWaitService();
    }
}


void mprEnableWaitEvents(MprWaitHandler *wp, int mask)
{
    //  Check events already enabled - generally a programming error
    mprAssert(!(mask & wp->desiredMask));
    mprAssert(wp->state == MPR_HANDLER_DISABLED || wp->state == MPR_HANDLER_ACTIVE);

    wp->state = MPR_HANDLER_ENABLED;
    if (mask != wp->desiredMask) {
        mprAddNotifier(wp->service, wp, mask);
        mprWakeWaitService();
    }
}


/*
    Set a handler to be recalled without further I/O
 */
void mprRecallWaitHandler(int fd)
{
    MprWaitService  *ws;
    MprWaitHandler  *wp;
    int             index;

    ws = MPR->waitService;
    lock(ws);
    for (index = 0; (wp = (MprWaitHandler*) mprGetNextItem(ws->handlers, &index)) != 0; ) {
        if (wp->fd == fd) {
            wp->flags |= MPR_WAIT_RECALL_HANDLER;
            ws->needRecall = 1;
            mprWakeWaitService();
            break;
        }
    }
    unlock(ws);
}


/*
    Recall a handler which may have buffered data
 */
void mprDoWaitRecall(MprWaitService *ws)
{
    MprWaitHandler      *wp;
    int                 index;

    lock(ws);
    ws->needRecall = 0;
    for (index = 0; (wp = (MprWaitHandler*) mprGetNextItem(ws->handlers, &index)) != 0; ) {
        if ((wp->flags & MPR_WAIT_RECALL_HANDLER) && (wp->desiredMask & MPR_READABLE)) {
            wp->presentMask |= MPR_READABLE;
            wp->flags &= ~MPR_WAIT_RECALL_HANDLER;
            mprRemoveNotifier(wp);
            mprQueueIOEvent(wp);
        }
    }
    unlock(ws);
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
