/**
    mprAsync.c - Wait for I/O on Windows.

    This module provides io management for sockets on Windows like systems. 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if MPR_EVENT_ASYNC
/***************************** Forward Declarations ***************************/

static LRESULT msgProc(HWND hwnd, uint msg, uint wp, long lp);

/************************************ Code ************************************/

int mprCreateNotifierService(MprWaitService *ws)
{   
    ws->socketMessage = MPR_SOCKET_MESSAGE;
    mprInitWindow(ws);
    return 0;
}


int mprAddNotifier(MprWaitService *ws, MprWaitHandler *wp, int mask)
{
    int     winMask;

    lock(ws);
    winMask = 0;
    if (wp->desiredMask != mask) {
        if (mask & MPR_READABLE) {
            winMask |= FD_ACCEPT | FD_CONNECT | FD_CLOSE | FD_READ;
        }
        if (mask & MPR_WRITABLE) {
            winMask |= FD_WRITE;
        }
        WSAAsyncSelect(wp->fd, ws->hwnd, ws->socketMessage, winMask);
        wp->desiredMask = mask;
    }
    unlock(ws);
    return 0;
}


void mprRemoveNotifier(MprWaitHandler *wp)
{
    MprWaitService      *ws;

    ws = wp->service;
    lock(ws);
    mprAssert(wp->fd >= 0);
    WSAAsyncSelect(wp->fd, ws->hwnd, ws->socketMessage, 0);
    wp->desiredMask = 0;
    unlock(ws);
}


/*
    Wait for I/O on a single descriptor. Return the number of I/O events found. Mask is the events of interest.
    Timeout is in milliseconds.
 */
int mprWaitForSingleIO(MprCtx ctx, int fd, int desiredMask, int timeout)
{
    HANDLE      h;
    int         winMask;

    if (timeout < 0) {
        timeout = MAXINT;
    }
    winMask = 0;
    if (desiredMask & MPR_READABLE) {
        winMask |= FD_CLOSE | FD_READ;
    }
    if (desiredMask & MPR_WRITABLE) {
        winMask |= FD_WRITE;
    }
    h = CreateEvent(NULL, FALSE, FALSE, "mprWaitForSingleIO");
    WSAEventSelect(fd, h, winMask);
    if (WaitForSingleObject(h, timeout) == WAIT_OBJECT_0) {
        CloseHandle(h);
        return desiredMask;
    }
    CloseHandle(h);
    return 0;
}


/*
    Wait for I/O on all registered descriptors. Timeout is in milliseconds. Return the number of events serviced.
 */
void mprWaitForIO(MprWaitService *ws, int timeout)
{
    MSG     msg;

    mprAssert(ws->hwnd);

#if BLD_DEBUG
    if (mprGetDebugMode(ws) && timeout > 30000) {
        timeout = 30000;
    }
#endif
    if (mprGetCurrentThread(ws)->isMain) {
        if (ws->needRecall) {
            mprDoWaitRecall(ws);
            return;
        }
        ws->willAwake = mprGetMpr(ws)->eventService->now + timeout;
        SetTimer(ws->hwnd, 0, timeout, NULL);

        if (GetMessage(&msg, NULL, 0, 0) == 0) {
            mprTerminate(ws, 1);
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        ws->wakeRequested = 0;
    }
}


void mprServiceWinIO(MprWaitService *ws, int sockFd, int winMask)
{
    MprWaitHandler      *wp;
    int                 index;

    lock(ws);

    for (index = 0; (wp = (MprWaitHandler*) mprGetNextItem(ws->handlers, &index)) != 0; ) {
        if (wp->fd == sockFd) {
            break;
        }
    }
    if (wp == 0) {
        /* If the server forcibly closed the socket, we may still get a read event. Just ignore it.  */
        unlock(ws);
        return;
    }
    /*
        Mask values: READ==1, WRITE=2, ACCEPT=8, CONNECT=10, CLOSE=20
     */
    wp->presentMask = 0;
    if (winMask & (FD_READ | FD_ACCEPT | FD_CLOSE)) {
        wp->presentMask |= MPR_READABLE;
    }
    if (winMask & (FD_WRITE | FD_CONNECT)) {
        wp->presentMask |= MPR_WRITABLE;
    }
    wp->presentMask &= wp->desiredMask;
    if (wp->presentMask & wp->desiredMask) {
        mprRemoveNotifier(wp);
        if (wp->presentMask) {
            mprQueueIOEvent(wp);
        }
    }
    unlock(ws);
}


/*
    Wake the wait service. WARNING: This routine must not require locking. MprEvents in scheduleDispatcher depends on this.
 */
void mprWakeNotifier(MprCtx ctx)
{
    MprWaitService  *ws;
   
    ws = mprGetMpr(ctx)->waitService;
    if (!ws->wakeRequested && ws->hwnd) {
        ws->wakeRequested = 1;
        PostMessage(ws->hwnd, WM_NULL, 0, 0L);
    }
}


/*
    Create a default window if the application has not already created one.
 */ 
int mprInitWindow(MprWaitService *ws)
{
    Mpr         *mpr;
    WNDCLASS    wc;
    HWND        hwnd;
    int         rc;

    mpr = mprGetMpr(ws);
    if (ws->hwnd) {
        return 0;
    }
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground    = (HBRUSH) (COLOR_WINDOW+1);
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = 0;
    wc.hIcon            = NULL;
    wc.lpfnWndProc      = (WNDPROC) msgProc;

    wc.lpszMenuName     = wc.lpszClassName = mprGetAppName(mpr);

    rc = RegisterClass(&wc);
    if (rc == 0) {
        mprError(mpr, "Can't register windows class");
        return MPR_ERR_CANT_INITIALIZE;
    }
    hwnd = CreateWindow(mprGetAppName(mpr), mprGetAppTitle(mpr), WS_OVERLAPPED, CW_USEDEFAULT, 0, 0, 0, NULL, NULL, 0, NULL);
    if (!hwnd) {
        mprError(mpr, "Can't create window");
        return -1;
    }
    ws->hwnd = hwnd;
    ws->socketMessage = MPR_SOCKET_MESSAGE;
    return 0;
}


/*
    Windows message processing loop for wakeup and socket messages
 */
static LRESULT msgProc(HWND hwnd, uint msg, uint wp, long lp)
{
    Mpr                 *mpr;
    MprWaitService      *ws;
    int                 sock, winMask;

    mpr = mprGetMpr(NULL);
    ws = mpr->waitService;

    if (msg == WM_DESTROY || msg == WM_QUIT) {
        mprTerminate(mpr, 1);

    } else if (msg && msg == ws->socketMessage) {
        sock = wp;
        winMask = LOWORD(lp);
        mprServiceWinIO(mpr->waitService, sock, winMask);

    } else if (ws->msgCallback) {
        ws->msgCallback(hwnd, msg, wp, lp);

    } else {
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}


void mprSetWinMsgCallback(MprWaitService *ws, MprMsgCallback callback)
{
    ws->msgCallback = callback;
}


#else
void stubMprAsync() {}
#endif /* MPR_EVENT_ASYNC */

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
