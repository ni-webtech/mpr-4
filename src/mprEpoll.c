/**
    mprEpoll.c - Wait for I/O by using epoll on unix like systems.

    This module augments the mprWait wait services module by providing kqueue() based waiting support.
    Also see mprAsyncSelectWait and mprSelectWait. This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if MPR_EVENT_EPOLL
/********************************** Forwards **********************************/

static int growEvents(MprWaitService *ws);
static void serviceIO(MprWaitService *ws, int count);

/************************************ Code ************************************/

int mprCreateNotifierService(MprWaitService *ws)
{
    struct epoll_event  ev;

    ws->eventsMax = MPR_EPOLL_SIZE;
    ws->handlerMax = MPR_FD_MIN;
    ws->events = mprAllocZeroed(sizeof(struct epoll_event) * ws->eventsMax);
    ws->handlerMap = mprAllocZeroed(sizeof(MprWaitHandler*) * ws->handlerMax);
    if (ws->events == 0 || ws->handlerMap == 0) {
        return MPR_ERR_CANT_INITIALIZE;
    }
    if ((ws->epoll = epoll_create(MPR_EPOLL_SIZE)) < 0) {
        mprError("Call to epoll() failed");
        return MPR_ERR_CANT_INITIALIZE;
    }
    /*
        Initialize the "wakeup" pipe. This is used to wakeup the service thread if other threads need 
     *  to wait for I/O.
     */
    if (pipe(ws->breakPipe) < 0) {
        mprError("Can't open breakout pipe");
        return MPR_ERR_CANT_INITIALIZE;
    }
    fcntl(ws->breakPipe[0], F_SETFL, fcntl(ws->breakPipe[0], F_GETFL) | O_NONBLOCK);
    fcntl(ws->breakPipe[1], F_SETFL, fcntl(ws->breakPipe[1], F_GETFL) | O_NONBLOCK);

    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    ev.data.fd = ws->breakPipe[MPR_READ_PIPE];
    epoll_ctl(ws->epoll, EPOLL_CTL_ADD, ws->breakPipe[MPR_READ_PIPE], &ev);
    return 0;
}


void mprManageEpoll(MprWaitService *ws, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ws->events);
    
    } else if (flags & MPR_MANAGE_FREE) {
        if (ws->epoll) {
            close(ws->epoll);
            ws->epoll = 0;
        }
    }
}


static int growEvents(MprWaitService *ws)
{
    ws->eventsMax *= 2;
    ws->events = mprRealloc(ws->events, sizeof(struct epoll_event) * ws->eventsMax);
    if (ws->events == 0) {
        return MPR_ERR_MEMORY;
    }
    return 0;
}


int mprAddNotifier(MprWaitService *ws, MprWaitHandler *wp, int mask)
{
    struct epoll_event  ev;
    int                 fd, oldlen;

    mprAssert(wp);

    lock(ws);
    if (wp->desiredMask != mask) {
        fd = wp->fd;
        memset(&ev, 0, sizeof(ev));
        ev.data.fd = fd;
        if (mask & MPR_READABLE) {
            ev.events |= EPOLLIN;
        }
        if (mask & MPR_WRITABLE) {
            ev.events |= EPOLLOUT;
        }
        epoll_ctl(ws->epoll, EPOLL_CTL_ADD, fd, &ev);

        if (fd >= ws->handlerMax) {
            oldlen = ws->handlerMax;
            ws->handlerMax = fd + 32;
            if ((ws->handlerMap = mprRealloc(ws->handlerMap, sizeof(MprWaitHandler*) * ws->handlerMax)) == 0) {
                return MPR_ERR_MEMORY;
            }
            memset(&ws->handlerMap[oldlen], 0, sizeof(MprWaitHandler*) * (ws->handlerMax - oldlen));
        }
        mprAssert(ws->handlerMap[fd] == 0);
        ws->handlerMap[fd] = wp;
        wp->desiredMask = mask;
    }
    unlock(ws);
    return 0;
}


void mprRemoveNotifier(MprWaitHandler *wp)
{
    MprWaitService  *ws;
    int             fd;

    ws = wp->service;
    fd = wp->fd;
    mprAssert(fd >= 0);
    lock(ws);
    epoll_ctl(ws->epoll, EPOLL_CTL_DEL, fd, NULL);
    mprAssert(ws->handlerMap[fd] == 0 || ws->handlerMap[fd] == wp);
    ws->handlerMap[fd] = 0;
    wp->desiredMask = 0;
    unlock(ws);
}


/*
    Wait for I/O on a single file descriptor. Return a mask of events found. Mask is the events of interest.
    timeout is in milliseconds.
 */
int mprWaitForSingleIO(int fd, int mask, int timeout)
{
    MprWaitService      *ws;
    struct epoll_event  ev, events[2];
    int                 epfd, rc, err;

    ws = mprGetMpr()->waitService;
    if (timeout < 0) {
        timeout = MAXINT;
    }
    memset(&ev, 0, sizeof(ev));
    memset(events, 0, sizeof(events));
    ev.data.fd = fd;
    if ((epfd = epoll_create(MPR_EPOLL_SIZE)) < 0) {
        mprError("Call to epoll() failed");
        return MPR_ERR_CANT_INITIALIZE;
    }
    if (mask & MPR_READABLE) {
        ev.events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    }
    if (mask & MPR_WRITABLE) {
        ev.events = EPOLLOUT;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    }
    mask = 0;
    rc = epoll_wait(epfd, events, sizeof(events) / sizeof(struct epoll_event), timeout);
    err = errno;
    close(epfd);
    if (rc < 0) {
        mprLog(2, "Epoll returned %d, errno %d", rc, errno);
    } else if (rc > 0) {
        if (rc > 0) {
            if (events[0].events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
                mask |= MPR_READABLE;
            }
            if (events[0].events & (EPOLLOUT)) {
                mask |= MPR_WRITABLE;
            }
        }
    }
    return mask;
}


/*
    Wait for I/O on all registered file descriptors. Timeout is in milliseconds. Return the number of events detected. 
 */
void mprWaitForIO(MprWaitService *ws, int timeout)
{
    int     rc;

#if BLD_DEBUG
    if (mprGetDebugMode() && timeout > 30000) {
        timeout = 30000;
    }
#endif
    if (ws->needRecall) {
        mprDoWaitRecall(ws);
        return;
    }
    rc = epoll_wait(ws->epoll, ws->events, ws->eventsMax, timeout);
    if (rc < 0) {
        if (errno != EINTR) {
            mprLog(2, "Kevent returned %d, errno %d", mprGetOsError());
        }
    } else if (rc > 0) {
        serviceIO(ws, rc);
        if (rc == ws->eventsMax) {
            growEvents(ws);
        }
    }
    ws->wakeRequested = 0;
}


static void serviceIO(MprWaitService *ws, int count)
{
    MprWaitHandler      *wp;
    struct epoll_event  *ev;
    int                 fd, i, mask, rc;

    lock(ws);
    for (i = 0; i < count; i++) {
        ev = &ws->events[i];
        fd = ev->data.fd;
        mprAssert(fd < ws->handlerMax);
        if ((wp = ws->handlerMap[fd]) == 0) {
            char    buf[128];
            if ((ev->events & (EPOLLIN | EPOLLERR | EPOLLHUP)) && (fd == ws->breakPipe[MPR_READ_PIPE])) {
                rc = read(fd, buf, sizeof(buf));
            }
            continue;
        }
        mask = 0;
        if (ev->events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
            mask |= MPR_READABLE;
        }
        if (ev->events & EPOLLOUT) {
            mask |= MPR_WRITABLE;
        }
        if (mask == 0) {
            continue;
        }
        wp->presentMask = mask & wp->desiredMask;
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
void mprWakeNotifier()
{
    MprWaitService  *ws;
    int             c, rc;

    ws = mprGetMpr()->waitService;
    if (!ws->wakeRequested) {
        ws->wakeRequested = 1;
        c = 0;
        rc = write(ws->breakPipe[MPR_WRITE_PIPE], (char*) &c, 1);
    }
}

#else
void stubMmprEpoll() {}
#endif /* MPR_EVENT_EPOLL */

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
