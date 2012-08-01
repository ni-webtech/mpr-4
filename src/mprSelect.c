/**
    mprSelect.c - Wait for I/O by using select.

    This module provides I/O wait management for sockets on VxWorks and systems that use select(). Windows and Unix
    uses different mechanisms. See mprAsyncSelectWait and mprPollWait. This module is thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
/********************************* Includes ***********************************/

#include    "mpr.h"

#if MPR_EVENT_SELECT
/********************************** Forwards **********************************/

static void serviceIO(MprWaitService *ws, int maxfd);
static void readPipe(MprWaitService *ws);

/************************************ Code ************************************/

int mprCreateNotifierService(MprWaitService *ws)
{
    int     rc, retries, breakPort, breakSock, maxTries;

    ws->highestFd = 0;
    ws->handlerMax = MPR_FD_MIN;
    if ((ws->handlerMap = mprAllocZeroed(sizeof(MprWaitHandler*) * ws->handlerMax)) == 0) {
        return MPR_ERR_CANT_INITIALIZE;
    }
    FD_ZERO(&ws->readMask);
    FD_ZERO(&ws->writeMask);

    /*
        Try to find a good port to use to break out of the select wait
     */ 
    maxTries = 100;
    breakPort = MPR_DEFAULT_BREAK_PORT;
    for (rc = retries = 0; retries < maxTries; retries++) {
        breakSock = socket(AF_INET, SOCK_DGRAM, 0);
        if (breakSock < 0) {
            mprLog(MPR_WARN, "Can't open port %d to use for select. Retrying.\n");
        }
#if BIT_UNIX_LIKE
        fcntl(breakSock, F_SETFD, FD_CLOEXEC);
#endif
        ws->breakAddress.sin_family = AF_INET;
#if CYGWIN || VXWORKS
        /*
            Cygwin & VxWorks don't work with INADDR_ANY
         */
        ws->breakAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
        ws->breakAddress.sin_addr.s_addr = INADDR_ANY;
#endif
        ws->breakAddress.sin_port = htons((short) breakPort);
        rc = bind(breakSock, (struct sockaddr *) &ws->breakAddress, sizeof(ws->breakAddress));
        if (breakSock >= 0 && rc == 0) {
#if VXWORKS
            /* VxWorks 6.0 bug workaround */
            ws->breakAddress.sin_port = htons((short) breakPort);
#endif
            break;
        }
        if (breakSock >= 0) {
            closesocket(breakSock);
        }
        breakPort++;
    }

    if (breakSock < 0 || rc < 0) {
        mprLog(MPR_WARN, "Can't bind any port to use for select. Tried %d-%d\n", breakPort, breakPort - maxTries);
        return MPR_ERR_CANT_OPEN;
    }
    ws->breakSock = breakSock;
    FD_SET(breakSock, &ws->readMask);
    ws->highestFd = breakSock;
    return 0;
}


void mprManageSelect(MprWaitService *ws, int flags)
{
    if (flags & MPR_MANAGE_FREE) {
        if (ws->breakSock >= 0) {
            close(ws->breakSock);
        }
    }
}


static int growFds(MprWaitService *ws, int fd)
{
    ws->handlerMax = max(ws->handlerMax * 2, fd);
    if ((ws->handlerMap = mprRealloc(ws->handlerMap, sizeof(MprWaitHandler*) * ws->handlerMax)) == 0) {
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    return 0;
}


int mprNotifyOn(MprWaitService *ws, MprWaitHandler *wp, int mask)
{
    int     fd;

    fd = wp->fd;
    if (fd >= FD_SETSIZE) {
        mprError("File descriptor exceeds configured maximum in FD_SETSIZE (%d vs %d)", fd, FD_SETSIZE);
        return MPR_ERR_CANT_INITIALIZE;
    }
    lock(ws);
    if (wp->desiredMask != mask) {
        if (wp->desiredMask & MPR_READABLE && !(mask & MPR_READABLE)) {
            FD_CLR(fd, &ws->readMask);
        }
        if (wp->desiredMask & MPR_WRITABLE && !(mask & MPR_WRITABLE)) {
            FD_CLR(fd, &ws->writeMask);
        }
        if (mask & MPR_READABLE) {
            FD_SET(fd, &ws->readMask);
        }
        if (mask & MPR_WRITABLE) {
            FD_SET(fd, &ws->writeMask);
        }
        if (mask) {
            if (fd >= ws->handlerMax && growFds(ws, fd) < 0) {
                unlock(ws);
                mprAssert(!MPR_ERR_MEMORY);
                return MPR_ERR_MEMORY;
            }
        }
        mprAssert(ws->handlerMap[fd] == 0 || ws->handlerMap[fd] == wp);
        ws->handlerMap[fd] = (mask) ? wp : 0;
        wp->desiredMask = mask;
        ws->highestFd = max(fd, ws->highestFd);
        if (mask == 0 && fd == ws->highestFd) {
            while (--fd > 0) {
                if (FD_ISSET(fd, &ws->readMask) || FD_ISSET(fd, &ws->writeMask)) {
                    break;
                }
            }
            ws->highestFd = fd;
        }
    }
    unlock(ws);
    return 0;
}


/*
    Wait for I/O on a single file descriptor. Return a mask of events found. Mask is the events of interest.
    timeout is in milliseconds.
 */
int mprWaitForSingleIO(int fd, int mask, MprTime timeout)
{
    MprWaitService  *ws;
    struct timeval  tval;
    fd_set          readMask, writeMask;
    int             rc;

    if (timeout < 0 || timeout > MAXINT) {
        timeout = MAXINT;
    }
    ws = MPR->waitService;
    tval.tv_sec = (int) (timeout / 1000);
    tval.tv_usec = (int) ((timeout % 1000) * 1000);

    FD_ZERO(&readMask);
    if (mask & MPR_READABLE) {
        FD_SET(fd, &readMask);
    }
    FD_ZERO(&writeMask);
    if (mask & MPR_WRITABLE) {
        FD_SET(fd, &writeMask);
    }
    mask = 0;
    rc = select(fd + 1, &readMask, &writeMask, NULL, &tval);
    if (rc < 0) {
        mprLog(2, "Select returned %d, errno %d", rc, mprGetOsError());
    } else if (rc > 0) {
        if (FD_ISSET(fd, &readMask)) {
            mask |= MPR_READABLE;
        }
        if (FD_ISSET(fd, &writeMask)) {
            mask |= MPR_WRITABLE;
        }
    }
    return mask;
}


/*
    Wait for I/O on all registered file descriptors. Timeout is in milliseconds. Return the number of events detected.
 */
void mprWaitForIO(MprWaitService *ws, MprTime timeout)
{
    struct timeval  tval;
    int             rc, maxfd;

    if (timeout < 0 || timeout > MAXINT) {
        timeout = MAXINT;
    }
#if BIT_DEBUG
    if (mprGetDebugMode() && timeout > 30000) {
        timeout = 30000;
    }
#endif
#if VXWORKS
    /* Minimize VxWorks task starvation */
    timeout = max(timeout, 50);
#endif
    tval.tv_sec = (int) (timeout / 1000);
    tval.tv_usec = (int) ((timeout % 1000) * 1000);

    if (ws->needRecall) {
        mprDoWaitRecall(ws);
        return;
    }
    lock(ws);
    ws->stableReadMask = ws->readMask;
    ws->stableWriteMask = ws->writeMask;
    maxfd = ws->highestFd + 1;
    unlock(ws);

    mprYield(MPR_YIELD_STICKY);
    rc = select(maxfd, &ws->stableReadMask, &ws->stableWriteMask, NULL, &tval);
    mprResetYield();

    if (rc > 0) {
        serviceIO(ws, maxfd);
    }
    ws->wakeRequested = 0;
}


static void serviceIO(MprWaitService *ws, int maxfd)
{
    MprWaitHandler      *wp;
    int                 fd, mask;

    lock(ws);
    for (fd = 0; fd < maxfd; fd++) {
        mask = 0;
        if (FD_ISSET(fd, &ws->stableReadMask)) {
            mask |= MPR_READABLE;
        }
        if (FD_ISSET(fd, &ws->stableWriteMask)) {
            mask |= MPR_WRITABLE;
        }
        if (mask == 0) {
            continue;
        }
        if ((wp = ws->handlerMap[fd]) == 0) {
            if (fd == ws->breakSock) {
                readPipe(ws);
            }
            continue;
        }
        wp->presentMask = mask & wp->desiredMask;
        if (wp->presentMask) {
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
    ssize           rc;
    int             c;

    ws = MPR->waitService;
    if (!ws->wakeRequested) {
        ws->wakeRequested = 1;
        c = 0;
        rc = sendto(ws->breakSock, (char*) &c, 1, 0, (struct sockaddr*) &ws->breakAddress, (int) sizeof(ws->breakAddress));
        if (rc < 0) {
            static int warnOnce = 0;
            if (warnOnce++ == 0) {
                mprError("Can't send wakeup to breakout socket: errno %d", errno);
            }
        }
    }
}


static void readPipe(MprWaitService *ws)
{
    char        buf[128];

    //  MOB - refactor
#if VXWORKS
    int len = sizeof(ws->breakAddress);
    (void) recvfrom(ws->breakSock, buf, (int) sizeof(buf), 0, (struct sockaddr*) &ws->breakAddress, (int*) &len);
#else
    socklen_t   len = sizeof(ws->breakAddress);
    (void) recvfrom(ws->breakSock, buf, (int) sizeof(buf), 0, (struct sockaddr*) &ws->breakAddress, (socklen_t*) &len);
#endif
}

#else
void stubMprSelectWait() {}
#endif /* MPR_EVENT_SELECT */

/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire
    a commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.TXT distributed with
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
