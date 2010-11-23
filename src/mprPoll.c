/**
    mprPoll.c - Wait for I/O by using poll on unix like systems.

    This module augments the mprWait wait services module by providing poll() based waiting support.
    Also see mprAsyncSelectWait and mprSelectWait. This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if MPR_EVENT_POLL
/********************************** Forwards **********************************/

static void serviceIO(MprWaitService *ws);

/************************************ Code ************************************/

int mprCreateNotifierService(MprWaitService *ws)
{
    struct pollfd   *pollfd;
    int             fd;

    ws->fdsCount = 0;
    ws->fdMax = MPR_FD_MIN;

    ws->fds = mprAllocZeroed(ws, sizeof(struct pollfd) * ws->fdMax);
    ws->handlerMap = mprAllocZeroed(ws, sizeof(MprWaitHandler*) * ws->fdMax);
    if (ws->fds == 0 || ws->handlerMap == 0) {
        return MPR_ERR_CANT_INITIALIZE;
    }
    /*
        Initialize the "wakeup" pipe. This is used to wakeup the service thread if other threads need to wait for I/O.
     */
    if (pipe(ws->breakPipe) < 0) {
        mprError(ws, "Can't open breakout pipe");
        return MPR_ERR_CANT_INITIALIZE;
    }
    fcntl(ws->breakPipe[0], F_SETFL, fcntl(ws->breakPipe[0], F_GETFL) | O_NONBLOCK);
    fcntl(ws->breakPipe[1], F_SETFL, fcntl(ws->breakPipe[1], F_GETFL) | O_NONBLOCK);

    fd = ws->breakPipe[MPR_READ_PIPE];
    pollfd = &ws->fds[ws->fdsCount];
    pollfd->fd = ws->breakPipe[MPR_READ_PIPE];
    pollfd->events = POLLIN;
    ws->fdsCount++;
    return 0;
}


void mprManagePoll(MprWebService *ws, int flags)
{
    if (flags & MPR_MANAGE_FREE) {
        mprMark(ws->fds);

    } else if (flags & MPR_MANAGE_FREE) {
        if (ws->breakPipe[0] >= 0) {
            close(ws->breakPipe[0]);
        }
        if (ws->breakPipe[1] >= 0) {
            close(ws->breakPipe[1]);
        }
    }
}


static int growFds(MprWaitService *ws)
{
    ws->fdMax *= 2;
    ws->fds = mprRealloc(ws, ws->fds, sizeof(struct pollfd)   ws->fdMax);
    ws->handlerMap = mprRealloc(ws, ws->handlerMap, sizeof(MprWaitHandler*) * ws->fdMax);
    if (ws->fds == 0 || ws->handlerMap) {
        return MPR_ERR_MEMORY;
    }
    memset(&ws->fds[ws->fdMax / 2], 0, sizeof(struct pollfd)   ws->fdMax / 2);
    memset(&ws->handlerMap[ws->fdMax / 2], 0, sizeof(MprWaitHandler*) * ws->fdMax / 2);
    return 0;
}


int mprAddNotifier(MprWaitService *ws, MprWaitHandler *wp, int mask)
{
    struct pollfd   *pollfd;
    int             fd;

    lock(ws);
    if (wp->desiredMask != mask) {
        fd = wp->fd;
        if (wp->notifierIndex < 0) {
            if (ws->fdsCount >= ws->fdMax && growFds(ws) < 0) {
                unlock(ws);
                return MPR_ERR_MEMORY;
            }
            mprAssert(ws->handlerMap[fd] == 0);
            ws->handlerMap[fd] = wp;
            wp->notifierIndex = ws->fdsCount++;
            pollfd = &ws->fds[wp->notifierIndex];
            pollfd->fd = fd;
        } else {
            pollfd = &ws->fds[wp->notifierIndex];
        }
        pollfd->events = 0;
        if (mask & MPR_READABLE) {
            pollfd->events |= POLLIN;
        }
        if (mask & MPR_WRITABLE) {
            pollfd->events |= POLLOUT;
        }
        wp->desiredMask = mask;
    }
    unlock(ws);
    return 0;
}


void mprRemoveNotifier(MprWaitHandler *wp)
{
    MprWaitService  *ws;
    int             fd, index;

    ws = wp->service;
    fd = wp->fd;
    mprAssert(fd >= 0);

    lock(ws);
    index = wp->notifierIndex;
    if (index >= 0 && --ws->fdsCount > index) {
        /*
            If not the last entry, copy last poll entry to replace the deleted fd.
         */
        ws->fds[index] = ws->fds[ws->fdsCount];
        ws->handlerMap[ws->fds[index].fd]->notifierIndex = index;
        fd = ws->fds[index].fd;
        ws->fds[ws->fdsCount].fd = -1;
    }
    mprAssert(ws->handlerMap[wp->fd] == 0 || ws->handlerMap[wp->fd] == wp);
    ws->handlerMap[wp->fd] = 0;
    wp->notifierIndex = -1;
    wp->desiredMask = 0;
    unlock(ws);
}


/*
    Wait for I/O on a single file descriptor. Return a mask of events found. Mask is the events of interest.
    timeout is in milliseconds.
 */
int mprWaitForSingleIO(MprCtx ctx, int fd, int mask, int timeout)
{
    struct pollfd   fds[1];
    int             rc;

    if (timeout < 0) {
        timeout = MAXINT;
    }
    fds[0].fd = fd;
    fds[0].events = 0;
    fds[0].revents = 0;

    if (mask & MPR_READABLE) {
        fds[0].events |= POLLIN;
    }
    if (mask & MPR_WRITABLE) {
        fds[0].events |= POLLOUT;
    }
    mask = 0;
    rc = poll(fds, 1, timeout);
    if (rc < 0) {
        mprLog(ctx, 2, "Poll returned %d, errno %d", rc, mprGetOsError());
    } else if (rc > 0) {
        if (fds[0].revents & POLLIN) {
            mask |= MPR_READABLE;
        }
        if (fds[0].revents & POLLOUT) {
            mask |= MPR_WRITABLE;
        }
    }
    return mask;
}


/*
    Wait for I/O on all registered file descriptors. Timeout is in milliseconds. Return the number of events detected.
 */
void mprWaitForIO(MprWaitService *ws, int timeout)
{
    struct pollfd   *fds;
    int             count, rc;

#if BLD_DEBUG
    if (mprGetDebugMode(ws) && timeout > 30000) {
        timeout = 30000;
    }
#endif
    if (ws->needRecall) {
        mprDoWaitRecall(ws);
        return;
    }
    lock(ws);
    count = ws->fdsCount;
    if ((fds = mprMemdup(ws, ws->fds, sizeof(struct pollfd) * count)) == 0) {
        unlock(ws);
        return MPR_ERR_MEMORY;
    }
    unlock(ws);

    rc = poll(fds, count, timeout);
    if (rc < 0) {
        mprLog(ws, 2, "Poll returned %d, errno %d", rc, mprGetOsError());
    } else if (rc > 0) {
        serviceIO(ws, fds, count);
    }
    mprFree(fds);
    ws->wakeRequested = 0;
}


/*
    Service I/O events
 */
static void serviceIO(MprWaitService *ws, struct poll *fds, int count)
{
    MprWaitHandler      *wp;
    struct pollfd       *fp;
    int                 mask;

    lock(ws);
    for (fp = fds; fp < &fds[count]; fp++) {
        if (fp->revents == 0) {
           continue;
        }
        mask = 0;
        if (fp->revents & (POLLIN | POLLHUP | POLLERR | POLLNVAL)) {
            mask |= MPR_READABLE;
        }
        if (fp->revents & POLLOUT) {
            mask |= MPR_WRITABLE;
        }
        mprAssert(mask);
        if ((wp = ws->handlerMap[fp->fd]) == 0) {
            char    buf[128];
            if (fp->fd == ws->breakPipe[MPR_READ_PIPE]) {
                read(fp->fd, buf, sizeof(buf));
            }
            continue;
        }
        wp->presentMask = mask & wp->desiredMask;
        fp->revents = 0;
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
    int             c, rc;

    ws = mprGetMpr(ctx)->waitService;
    if (!ws->wakeRequested) {
        ws->wakeRequested = 1;
        c = 0;
        rc = write(ws->breakPipe[MPR_WRITE_PIPE], (char*) &c, 1);
    }
}

#else
void stubMprPollWait() {}
#endif /* MPR_EVENT_POLL */

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
