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

static int handlerDestructor(MprWaitHandler *wp);

/************************************ Code ************************************/
/*
    Initialize the service
 */
MprWaitService *mprCreateWaitService(Mpr *mpr)
{
    MprWaitService  *ws;

    ws = mprAllocObjZeroed(mpr, MprWaitService);
    if (ws == 0) {
        return 0;
    }
    mprGetMpr()->waitService = ws;
    ws->handlers = mprCreateList(ws);
    ws->mutex = mprCreateLock(ws);
    ws->spin = mprCreateSpinLock(ws);
    mprCreateNotifierService(ws);
    return ws;
}


MprWaitHandler *mprInitWaitHandler(MprCtx ctx, MprWaitHandler *wp, int fd, int mask, MprDispatcher *dispatcher, 
        MprEventProc proc, void *data)
{
    MprWaitService  *ws;

    mprAssert(fd >= 0);

    ws = mprGetMpr()->waitService;
    if (mprGetListCount(ws->handlers) == FD_SETSIZE) {
        mprError(ws, "io: Too many io handlers: %d\n", FD_SETSIZE);
        return 0;
    }
#if BLD_UNIX_LIKE || VXWORKS
    if (fd >= FD_SETSIZE) {
        mprError(ws, "File descriptor %d exceeds max io of %d", fd, FD_SETSIZE);
    }
#endif
    wp->fd              = fd;
    wp->notifierIndex   = -1;
    wp->dispatcher      = dispatcher;
    wp->proc            = proc;
    wp->flags           = 0;
    wp->handlerData     = data;
    wp->service         = ws;

    lock(ws);
    if (mprAddItem(ws->handlers, wp) < 0) {
        unlock(ws);
        mprFree(wp);
        return 0;
    }
    mprAddNotifier(ws, wp, mask);
    unlock(ws);
    mprWakeWaitService(wp->service);
    return wp;
}


MprWaitHandler *mprCreateWaitHandler(MprCtx ctx, int fd, int mask, MprDispatcher *dispatcher, MprEventProc proc, void *data)
{
    MprWaitService  *ws;
    MprWaitHandler  *wp;

    mprAssert(fd >= 0);

    ws = mprGetMpr()->waitService;
    wp = mprAllocObjWithDestructorZeroed(ws, MprWaitHandler, handlerDestructor);
    if (wp == 0) {
        return 0;
    }
    return mprInitWaitHandler(ctx, wp, fd, mask, dispatcher, proc, data);
}


/*
    Wait handler Destructor. Called from mprFree.
 */
static int handlerDestructor(MprWaitHandler *wp)
{
    mprRemoveWaitHandler(wp);
    return 0;
}


void mprRemoveWaitHandler(MprWaitHandler *wp)
{
    MprWaitService      *ws;

    ws = wp->service;

    /*
        Lock the service to stabilize the list, then lock the handler to prevent callbacks. 
     */
    lock(ws);
    mprRemoveNotifier(wp);
    mprRemoveItem(ws->handlers, wp);
    mprWakeWaitService(ws);
    unlock(ws);
}


void mprWakeWaitService(MprCtx ctx)
{
    mprWakeNotifier(ctx);
}


void mprQueueIOEvent(MprWaitHandler *wp)
{
    MprDispatcher   *dispatcher;
    MprEvent        *event;

    dispatcher = (wp->dispatcher) ? wp->dispatcher: mprGetDispatcher(wp);
    event = &wp->event;
    mprInitEvent(dispatcher, event, "IOEvent", 0, (MprEventProc) wp->proc, (void*) wp->handlerData, 0);
    event->fd = wp->fd;
    event->mask = wp->presentMask;
    mprQueueEvent(dispatcher, event);
}


void mprDisableWaitEvents(MprWaitHandler *wp)
{
    if (wp->desiredMask) {
        mprRemoveNotifier(wp);
        mprWakeWaitService(wp->service);
    }
}


void mprEnableWaitEvents(MprWaitHandler *wp, int mask)
{
    if (mask != wp->desiredMask) {
        mprAddNotifier(wp->service, wp, mask);
        mprWakeWaitService(wp->service);
    }
}


/*
    Set a handler to be recalled without further I/O. May be called with a null wp.
 */
void mprRecallWaitHandler(MprCtx ctx, int fd)
{
    MprWaitService  *ws;
    MprWaitHandler  *wp;
    int             index;

    ws = mprGetMpr()->waitService;
    lock(ws);
    for (index = 0; (wp = (MprWaitHandler*) mprGetNextItem(ws->handlers, &index)) != 0; ) {
        if (wp->fd == fd) {
            wp->flags |= MPR_WAIT_RECALL_HANDLER;
            ws->needRecall = 1;
            mprWakeWaitService(wp->service);
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
