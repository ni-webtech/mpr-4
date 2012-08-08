/**
    mprKevent.c - Wait for I/O by using kevent on BSD based Unix systems.

    This module augments the mprWait wait services module by providing kqueue() based waiting support.
    Also see mprAsyncSelectWait and mprSelectWait. This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if MPR_EVENT_KQUEUE
/********************************** Forwards **********************************/

static int growEvents(MprWaitService *ws);
static void serviceIO(MprWaitService *ws, int count);

/************************************ Code ************************************/

int mprCreateNotifierService(MprWaitService *ws)
{
    ws->interestMax = MPR_FD_MIN;
    ws->eventsMax = MPR_FD_MIN;
    ws->handlerMax = MPR_FD_MIN;
    ws->interest = mprAllocZeroed(sizeof(struct kevent) * ws->interestMax);
    ws->events = mprAllocZeroed(sizeof(struct kevent) * ws->eventsMax);
    ws->handlerMap = mprAllocZeroed(sizeof(MprWaitHandler*) * ws->handlerMax);
    if (ws->interest == 0 || ws->events == 0 || ws->handlerMap == 0) {
        return MPR_ERR_CANT_INITIALIZE;
    }
    if ((ws->kq = kqueue()) < 0) {
        mprError("Call to kqueue() failed");
        return MPR_ERR_CANT_INITIALIZE;
    }
    /*
        Initialize the "wakeup" pipe. This is used to wakeup the service thread if other threads need to wait for I/O.
     */
    if (pipe(ws->breakPipe) < 0) {
        mprError("Can't open breakout pipe");
        return MPR_ERR_CANT_INITIALIZE;
    }
    fcntl(ws->breakPipe[0], F_SETFL, fcntl(ws->breakPipe[0], F_GETFL) | O_NONBLOCK);
    fcntl(ws->breakPipe[1], F_SETFL, fcntl(ws->breakPipe[1], F_GETFL) | O_NONBLOCK);
    EV_SET(&ws->interest[ws->interestCount], ws->breakPipe[MPR_READ_PIPE], EVFILT_READ, EV_ADD, 0, 0, 0);
    ws->interestCount++;
    return 0;
}


void mprManageKqueue(MprWaitService *ws, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ws->events);
        mprMark(ws->interest);
        mprMark(ws->stableInterest);

    } else if (flags & MPR_MANAGE_FREE) {
        if (ws->kq) {
            close(ws->kq);
        }
        if (ws->breakPipe[0] >= 0) {
            close(ws->breakPipe[0]);
        }
        if (ws->breakPipe[1] >= 0) {
            close(ws->breakPipe[1]);
        }
    }
}


static int growEvents(MprWaitService *ws)
{
    ws->interestMax *= 2;
    ws->eventsMax = ws->interestMax;
    ws->interest = mprRealloc(ws->interest, sizeof(struct kevent) * ws->interestMax);
    ws->events = mprRealloc(ws->events, sizeof(struct kevent) * ws->eventsMax);
    if (ws->interest == 0 || ws->events == 0) {
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    return 0;
}


int mprNotifyOn(MprWaitService *ws, MprWaitHandler *wp, int mask)
{
    struct kevent   *kp, *start;
    int             fd;

    mprAssert(wp);
    fd = wp->fd;

    lock(ws);
    mprLog(7, "mprNotifyOn: fd %d, mask %x, old mask %x", wp->fd, mask, wp->desiredMask);
    if (wp->desiredMask != mask) {
        mprAssert(fd >= 0);
        while ((ws->interestCount + 4) >= ws->interestMax) {
            growEvents(ws);
        }
        start = kp = &ws->interest[ws->interestCount];
        if (wp->desiredMask & MPR_READABLE && !(mask & MPR_READABLE)) {
            EV_SET(kp, fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
            kp++;
        }
        if (wp->desiredMask & MPR_WRITABLE && !(mask & MPR_WRITABLE)) {
            EV_SET(kp, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
            kp++;
        }
        if (mask & MPR_READABLE) {
            EV_SET(kp, fd, EVFILT_READ, EV_ADD, 0, 0, 0);
            kp++;
        }
        if (mask & MPR_WRITABLE) {
            EV_SET(kp, fd, EVFILT_WRITE, EV_ADD, 0, 0, 0);
            kp++;
        }
        ws->interestCount += (int) (kp - start);
        if (fd >= ws->handlerMax) {
            ws->handlerMax = fd + 32;
            if ((ws->handlerMap = mprRealloc(ws->handlerMap, sizeof(MprWaitHandler*) * ws->handlerMax)) == 0) {
                mprAssert(!MPR_ERR_MEMORY);
                return MPR_ERR_MEMORY;
            }
        }
        mprAssert(ws->handlerMap[fd] == 0 || ws->handlerMap[fd] == wp);
        wp->desiredMask = mask;
    }
    ws->handlerMap[fd] = (mask) ? wp : 0;
    unlock(ws);
    return 0;
}


/*
    Wait for I/O on a single file descriptor. Return a mask of events found. Mask is the events of interest.
    timeout is in milliseconds.
 */
int mprWaitForSingleIO(int fd, int mask, MprTime timeout)
{
    struct timespec ts;
    struct kevent   interest[2], events[1];
    int             kq, interestCount, rc;

    if (timeout < 0) {
        timeout = MAXINT;
    }
    interestCount = 0; 
    if (mask & MPR_READABLE) {
        EV_SET(&interest[interestCount++], fd, EVFILT_READ, EV_ADD, 0, 0, 0);
    }
    if (mask & MPR_WRITABLE) {
        EV_SET(&interest[interestCount++], fd, EVFILT_WRITE, EV_ADD, 0, 0, 0);
    }
    kq = kqueue();
    ts.tv_sec = ((int) (timeout / 1000));
    ts.tv_nsec = ((int) (timeout % 1000)) * 1000 * 1000;

    mask = 0;
    rc = kevent(kq, interest, interestCount, events, 1, &ts);
    close(kq);
    if (rc < 0) {
        mprLog(7, "Kevent returned %d, errno %d", rc, errno);
    } else if (rc > 0) {
        if (rc > 0) {
            if (events[0].filter == EVFILT_READ) {
                mask |= MPR_READABLE;
            }
            if (events[0].filter == EVFILT_WRITE) {
                mask |= MPR_WRITABLE;
            }
        }
    }
    return mask;
}


/*
    Wait for I/O on all registered file descriptors. Timeout is in milliseconds. Return the number of events detected.
 */
void mprWaitForIO(MprWaitService *ws, MprTime timeout)
{
    struct timespec ts;
    int             rc;

    mprAssert(timeout > 0);

    if (timeout < 0) {
        timeout = MAXINT;
    }
#if BIT_DEBUG
    if (mprGetDebugMode() && timeout > 30000) {
        timeout = 30000;
    }
#endif
    ts.tv_sec = ((int) (timeout / 1000));
    ts.tv_nsec = ((int) ((timeout % 1000) * 1000 * 1000));

    if (ws->needRecall) {
        mprDoWaitRecall(ws);
        return;
    }
    lock(ws);
    ws->stableInterest = mprMemdup(ws->interest, sizeof(struct kevent) * ws->interestCount);
    ws->stableInterestCount = ws->interestCount;
    /* Preserve the wakeup pipe fd */
    ws->interestCount = 1;
    unlock(ws);

    LOG(8, "kevent sleep for %d", timeout);
    mprYield(MPR_YIELD_STICKY);
    rc = kevent(ws->kq, ws->stableInterest, ws->stableInterestCount, ws->events, ws->eventsMax, &ts);
    mprResetYield();
    LOG(8, "kevent wakes rc %d", rc);

    if (rc < 0) {
        mprLog(7, "Kevent returned %d, errno %d", rc, mprGetOsError());
    } else if (rc > 0) {
        serviceIO(ws, rc);
    }
    ws->wakeRequested = 0;
}


static void serviceIO(MprWaitService *ws, int count)
{
    MprWaitHandler      *wp;
    struct kevent       *kev;
    char                buf[128];
    int                 fd, i, mask, err;

    lock(ws);
    for (i = 0; i < count; i++) {
        kev = &ws->events[i];
        fd = (int) kev->ident;
        mprAssert(fd < ws->handlerMax);
        if ((wp = ws->handlerMap[fd]) == 0) {
            if (kev->filter == EVFILT_READ && fd == ws->breakPipe[MPR_READ_PIPE]) {
                (void) read(fd, buf, sizeof(buf));
            }
            continue;
        }
        if (kev->flags & EV_ERROR) {
            err = (int) kev->data;
            if (err == ENOENT) {
                /* File descriptor was closed and re-opened */
                mask = wp->desiredMask;
                mprNotifyOn(ws, wp, 0);
                wp->desiredMask = 0;
                mprNotifyOn(ws, wp, mask);
                mprLog(7, "kqueue: file descriptor closed and reopened, fd %d", wp->fd);

            } else if (err == EBADF) {
                /* File descriptor was closed */
                mask = wp->desiredMask;
                mprNotifyOn(ws, wp, 0);
                wp->desiredMask = 0;
                mprNotifyOn(ws, wp, mask);
                mprLog(7, "kqueue: invalid file descriptor %d, fd %d", wp->fd);
            }
            continue;
        }
        mask = 0;
        if (kev->filter == EVFILT_READ) {
            mask |= MPR_READABLE;
        }
        if (kev->filter == EVFILT_WRITE) {
            mask |= MPR_WRITABLE;
        }
        wp->presentMask = mask & wp->desiredMask;
        LOG(7, "Got I/O event mask %x", wp->presentMask);
        if (wp->presentMask) {
            LOG(7, "ServiceIO for wp %p", wp);
            /* Suppress further events while this event is being serviced. User must re-enable */
            mprNotifyOn(ws, wp, 0);            
            mprQueueIOEvent(wp);
        }
    }
    unlock(ws);
}


/*
    Wake the wait service. WARNING: This routine must not require locking. MprEvents in scheduleDispatcher depends on this.
    Must be async-safe.
 */
void mprWakeNotifier()
{
    MprWaitService  *ws;
    int             c;

    ws = MPR->waitService;
    if (!ws->wakeRequested) {
        ws->wakeRequested = 1;
        c = 0;
        (void) write(ws->breakPipe[MPR_WRITE_PIPE], (char*) &c, 1);
    }
}

#else
void stubMprKqueue() {}
#endif /* MPR_EVENT_KQUEUE */

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire
    a commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details.

    This software is open source; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version. See the GNU General Public License for more
    details at: http://embedthis.com/downloads/gplLicense.html

    This program is distributed WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    This GPL license does NOT permit incorporating this software into
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses
    for this software and support services are available from Embedthis
    Software at http://embedthis.com

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
