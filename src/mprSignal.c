/**
    mprSignal.c - Signal handling for Unix systems

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/*********************************** Includes *********************************/

#include    "mpr.h"

#if BLD_UNIX_LIKE
/*********************************** Forwards *********************************/

static void manageSignal(MprSignal *sp, int flags);
static void manageSignalService(MprSignalService *ssp, int flags);
static void signalEvent(MprSignal *sp, MprEvent *event);
static void signalHandler(int signo, siginfo_t *info, void *arg);

/************************************ Code ************************************/

MprSignalService *mprCreateSignalService()
{
    MprSignalService    *ssp;

    if ((ssp = mprAllocObj(MprSignalService, manageSignalService)) == 0) {
        return 0;
    }
    ssp->mutex = mprCreateLock();
    ssp->signals = mprAllocZeroed(sizeof(MprSignal*) * MPR_MAX_SIGNAL);
    return ssp;
}


static void manageSignalService(MprSignalService *ssp, int flags)
{
    MprSignal   *sp;
    int         i;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(ssp->mutex);
        mprMark(ssp->signals);
        for (i = 0; i < MPR_MAX_SIGNAL; i++) {
            if ((sp = ssp->signals[i]) != 0) {
                mprMark(sp);
            }
        }
    }
}


/*
    Add a safe-signal handler. This creates a signal handler that will run from a dispatcher without the
    normal async-safe strictures of normal signal handlers. This manages a chain of signal handlers and ensures
    that prior handlers will be called appropriately.
 */
MprSignal *mprAddSignalHandler(int signo, void *handler, void *data, MprDispatcher *dispatcher, int flags)
{
    MprSignal           *sp;
    MprSignalService    *ssp;
    struct sigaction    act, old;
    int                 rc;

    if (signo < 0 || signo >= MPR_MAX_SIGNAL) {
        mprError("Bad signal: %d", signo);
        return 0;
    }
    if (!(flags & MPR_SIGNAL_BEFORE)) {
        flags |= MPR_SIGNAL_AFTER;
    }
    if ((sp = mprAllocObj(MprSignal, manageSignal)) == 0) {
        return 0;
    }
    sp->signo = signo;
    sp->flags = flags;
    sp->handler = handler;
    sp->dispatcher = dispatcher;
    sp->data = data;
    ssp = MPR->signalService;
    lock(ssp);
    if (ssp->signals[signo] != 0) {
        sp->chain.sp = ssp->signals[signo];
    } else {
        rc = sigaction(signo, 0, &old);
        if (old.sa_sigaction && old.sa_sigaction != signalHandler) {
            sp->chain.sigaction = old.sa_sigaction;
            sp->flags |= MPR_SIGNAL_NATIVE;
        }
    }
    ssp->signals[signo] = sp;
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signalHandler;
    sigemptyset(&act.sa_mask);

    /* Mask these when processing signals */
    sigaddset(&act.sa_mask, SIGALRM);
    sigaddset(&act.sa_mask, SIGCHLD);
    sigaddset(&act.sa_mask, SIGPIPE);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaddset(&act.sa_mask, SIGUSR1);
    sigaddset(&act.sa_mask, SIGUSR2);
    if (!mprGetDebugMode(NULL)) {
        sigaddset(&act.sa_mask, SIGINT);
    }
    if (sigaction(signo, &act, 0) != 0) {
        mprError("Can't add signal %d, errno %d", mprGetOsError());
        return 0;
    }
    unlock(ssp);
    return sp;
}


int mprRemoveSignalHandler(MprSignal *sp)
{
    MprSignalService    *ssp;
    MprSignal           *np, *prev;
    struct sigaction    old;
    int                 rc;

    ssp = MPR->signalService;
    lock(ssp);
    for (prev = 0, np = ssp->signals[sp->signo]; np; ) {
        if (sp == np) {
            if (prev) {
                prev->chain.sp = sp;
            } else {
                ssp->signals[sp->signo] = 0;
                if (np->flags & MPR_SIGNAL_NATIVE) {
                    rc = sigaction(sp->signo, 0, &old);
                    if (old.sa_sigaction == signalHandler) {
                        old.sa_sigaction = sp->chain.sigaction;
                        rc = sigaction(sp->signo, &old, 0);
                    }
                }
            }
            return 0;
        }
        prev = np;
        np = (np->flags & MPR_SIGNAL_NATIVE) ? 0 : np->chain.sp;
    }
    unlock(ssp);
    return MPR_ERR_CANT_FIND;
}


static void manageSignal(MprSignal *sp, int flags)
{
    MprSignalService    *ssp;
    
    ssp = MPR->signalService;
    if (flags & MPR_MANAGE_MARK) {
        mprMark(sp->dispatcher);
        mprMark(sp->data);
        mprMark((sp->flags & MPR_SIGNAL_NATIVE) ? 0 : sp->chain.sp);
    }
}


/*
    Actual signal handler - must be async-safe. Do very, very little here. Just set a global flag and wakeup
    the wait service (mprWakeWaitService is async safe).
    WARNING: Don't put memory allocation or logging here.
 */
static void signalHandler(int signo, siginfo_t *info, void *arg)
{
    MprSignal           *sp;
    MprSignalService    *ssp;

    if (signo < 0 || signo >= MPR_MAX_SIGNAL) {
        return;
    }
    ssp = MPR->signalService;
    if ((sp = ssp->signals[signo]) != 0) {
        mprAssert(sp);
        mprAssert(sp->signo == signo);
        sp->info = *info;
        sp->arg = arg;
        sp->triggered = 1;
        ssp->hasSignals = 1;
        mprWakeWaitService();
    }
}


void mprServiceSignals()
{
    MprSignalService    *ssp;
    MprSignal           *sp;
    int                 i;

    ssp = MPR->signalService;
    //  MOB - OPT if these were linked or kept min/max limits
    for (i = 0; i < MPR_MAX_SIGNAL; i++) {
        if ((sp = ssp->signals[i]) != 0 && sp->triggered) {
            sp->triggered = 0;
            mprCreateEvent(sp->dispatcher, "signalEvent", 0, signalEvent, sp, 0);
        }
    }
}


/*
    Invoke chain of signal handlers. Runs from the dispatcher so signal handlers don't have to be async-safe.
 */
static void signalEvent(MprSignal *sp, MprEvent *event)
{
    mprAssert(sp);
    mprAssert(event);

    mprLog(7, "signalEvent signo %d, flags %x", sp->signo, sp->flags);
    do {
        if (sp->flags & MPR_SIGNAL_BEFORE) {
            (sp->handler)(sp->data, sp);
        } 
        if (sp->chain.sp && sp->flags & MPR_SIGNAL_NATIVE) {
            (sp->chain.sigaction)(sp->signo, &sp->info, sp->arg);
        }
        if (sp->flags & MPR_SIGNAL_AFTER) {
            (sp->handler)(sp->data, sp);
        }
        sp = (sp->flags & MPR_SIGNAL_NATIVE) ? 0 : sp->chain.sp;
    } while (sp);
}


/*
    Standard signal handler. Do graceful shutdown for SIGTERM. Ignore SIGPIPE and SIGXFSZ. All others do immediate exit.
 */
static void standardSignalHandler(void *ignored, MprSignal *sp)
{
    mprLog(7, "standardSignalHandler signo %d, flags %x", sp->signo, sp->flags);
#if DEBUG_IDE
    if (sp->signo == SIGINT) return;
#endif
    mprLog(2, "Received signal %d", sp->signo);
    if (sp->signo == SIGTERM) {
        mprLog(1, "Executing a graceful exit. Waiting for all requests to complete.");
        mprTerminate(MPR_GRACEFUL);
    } else if (sp->signo == SIGPIPE || sp->signo == SIGXFSZ) {
        /* Ignore */
    } else {
        mprLog(1, "Exiting immediately ...");
        mprTerminate(0);
    }
}


void mprAddStandardSignals()
{
    mprAddSignalHandler(SIGINT,  standardSignalHandler, 0, 0, MPR_SIGNAL_AFTER);
    mprAddSignalHandler(SIGQUIT, standardSignalHandler, 0, 0, MPR_SIGNAL_AFTER);
    mprAddSignalHandler(SIGTERM, standardSignalHandler, 0, 0, MPR_SIGNAL_AFTER);
    mprAddSignalHandler(SIGUSR1, standardSignalHandler, 0, 0, MPR_SIGNAL_AFTER);
    mprAddSignalHandler(SIGPIPE, standardSignalHandler, 0, 0, MPR_SIGNAL_AFTER);
#if SIGXFSZ
    mprAddSignalHandler(SIGXFSZ, standardSignalHandler, 0, 0, MPR_SIGNAL_AFTER);
#endif
}

#else /* BLD_UNIX_LIKE */

MprSignalService *mprCreateSignalService() { return mprAlloc(0); }
int mprRemoveSignalHandler(MprSignal *sp) { return 0; }
void mprServiceSignals() {}
#endif /* BLD_UNIX_LIKE */

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
