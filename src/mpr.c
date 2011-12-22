/*
    mpr.c - Multithreaded Portable Runtime (MPR). Initialization, start/stop and control of the MPR.

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/**************************** Forward Declarations ****************************/

static void fixArgs(Mpr *mpr, int argc, char **argv);
static void manageMpr(Mpr *mpr, int flags);
static void serviceEventsThread(void *data, MprThread *tp);

/************************************* Code ***********************************/
/*
    Create and initialize the MPR service.
 */
Mpr *mprCreate(int argc, char **argv, int flags)
{
    MprFileSystem   *fs;
    Mpr             *mpr;

    srand((uint) time(NULL));

    if ((mpr = mprCreateMemService((MprManager) manageMpr, flags)) == 0) {
        mprAssert(mpr);
        return 0;
    }
    if (argv) {
        mpr->argc = argc;
        mpr->argv = argv;
        mpr->argv[0] = mprGetAppPath();
        mpr->name = mprTrimPathExt(mprGetPathBase(mpr->argv[0]));
    }
    mpr->exitStrategy = MPR_EXIT_NORMAL;
    mpr->emptyString = sclone("");
    mpr->title = sclone(BLD_NAME);
    mpr->version = sclone(BLD_VERSION);
    mpr->idleCallback = mprServicesAreIdle;
    mpr->mimeTypes = mprCreateMimeTypes(NULL);
    mpr->terminators = mprCreateList(0, MPR_LIST_STATIC_VALUES);

    mprCreateTimeService();
    mprCreateOsService();
    mpr->mutex = mprCreateLock();
    mpr->spin = mprCreateSpinLock();
    mpr->dtoaSpin[0] = mprCreateSpinLock();
    mpr->dtoaSpin[1] = mprCreateSpinLock();

    fs = mprCreateFileSystem("/");
    mprAddFileSystem(fs);
    mprCreateLogService();

    mpr->signalService = mprCreateSignalService();
    mpr->threadService = mprCreateThreadService();
    mpr->moduleService = mprCreateModuleService();
    mpr->eventService = mprCreateEventService();
    mpr->cmdService = mprCreateCmdService();
    mpr->workerService = mprCreateWorkerService();
    mpr->waitService = mprCreateWaitService();
    mpr->socketService = mprCreateSocketService();

    mpr->dispatcher = mprCreateDispatcher("main", 1);
    mpr->nonBlock = mprCreateDispatcher("nonblock", 1);
    mpr->pathEnv = sclone(getenv("PATH"));

    if (flags & MPR_USER_EVENTS_THREAD) {
        if (!(flags & MPR_NO_WINDOW)) {
            mprInitWindow();
        }
    } else {
        mprStartEventsThread();
    }
    mprStartGCService();

    if (MPR->hasError || mprHasMemError()) {
        return 0;
    }
    return mpr;
}


static void manageMpr(Mpr *mpr, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(mpr->logPath);
        mprMark(mpr->logFile);
        mprMark(mpr->mimeTypes);
        mprMark(mpr->timeTokens);
        mprMark(mpr->pathEnv);
        mprMark(mpr->name);
        mprMark(mpr->title);
        mprMark(mpr->version);
        mprMark(mpr->domainName);
        mprMark(mpr->hostName);
        mprMark(mpr->ip);
        mprMark(mpr->stdError);
        mprMark(mpr->stdInput);
        mprMark(mpr->stdOutput);
        mprMark(mpr->serverName);
        mprMark(mpr->appPath);
        mprMark(mpr->appDir);
        mprMark(mpr->cmdService);
        mprMark(mpr->eventService);
        mprMark(mpr->fileSystem);
        mprMark(mpr->moduleService);
        mprMark(mpr->osService);
        mprMark(mpr->signalService);
        mprMark(mpr->socketService);
        mprMark(mpr->threadService);
        mprMark(mpr->workerService);
        mprMark(mpr->waitService);
        mprMark(mpr->dispatcher);
        mprMark(mpr->nonBlock);
        mprMark(mpr->appwebService);
        mprMark(mpr->ediService);
        mprMark(mpr->ejsService);
        mprMark(mpr->espService);
        mprMark(mpr->httpService);
        mprMark(mpr->testService);
        mprMark(mpr->terminators);
        mprMark(mpr->mutex);
        mprMark(mpr->spin);
        mprMark(mpr->dtoaSpin[0]);
        mprMark(mpr->dtoaSpin[1]);
        mprMark(mpr->cond);
        mprMark(mpr->emptyString);
        mprMark(mpr->heap.markerCond);
    }
}

static void wgc(int mode)
{
    mprRequestGC(mode);
}

/*
    Destroy the Mpr and all services
 */
void mprDestroy(int how)
{
    int         gmode;

    if (!(how & MPR_EXIT_DEFAULT)) {
        MPR->exitStrategy = how;
    }
    how = MPR->exitStrategy;
    if (how & MPR_EXIT_IMMEDIATE) {
        if (how & MPR_EXIT_RESTART) {
            mprRestart();
            /* No return */
            return;
        }
        exit(0);
    }
    mprYield(MPR_YIELD_STICKY);
    if (MPR->state < MPR_STOPPING) {
        mprTerminate(how, -1);
    }
    gmode = MPR_FORCE_GC | MPR_COMPLETE_GC | MPR_WAIT_GC;
    mprRequestGC(gmode);

    if (how & MPR_EXIT_GRACEFUL) {
        mprWaitTillIdle(MPR_TIMEOUT_STOP);
    }
    MPR->state = MPR_STOPPING_CORE;
    MPR->exitStrategy &= MPR_EXIT_GRACEFUL;
    MPR->exitStrategy |= MPR_EXIT_IMMEDIATE;

    mprWakeWorkers();
    mprStopCmdService();
    mprStopModuleService();
    mprStopEventService();
    mprStopSignalService();

    /* Final GC to run all finalizers */
    wgc(gmode);

    if (how & MPR_EXIT_RESTART) {
        mprLog(2, "Restarting\n\n");
    } else {
        mprLog(2, "Exiting");
    }
    MPR->state = MPR_FINISHED;
    mprStopGCService();
    mprStopThreadService();
    mprStopOsService();
    mprDestroyMemService();

    if (how & MPR_EXIT_RESTART) {
        mprRestart();
    }
}



/*
    Start termination of the Mpr. May be called by mprDestroy or elsewhere.
 */
void mprTerminate(int how, int status)
{
    MprTerminator   terminator;
    int             next;

    MPR->exitStatus = status;
    if (!(how & MPR_EXIT_DEFAULT)) {
        MPR->exitStrategy = how;
    }
    how = MPR->exitStrategy;
    if (how & MPR_EXIT_IMMEDIATE) {
        mprLog(2, "Immediate exit. Aborting all requests and services.");
        exit(status);

    } else if (how & MPR_EXIT_NORMAL) {
        mprLog(2, "Normal exit. Flush buffers, close files and aborting existing requests.");

    } else if (how & MPR_EXIT_GRACEFUL) {
        mprLog(2, "Graceful exit. Waiting for existing requests to complete.");

    } else {
        mprLog(7, "mprTerminate: how %d", how);
    }

    /*
        Set the stopping flag. Services should stop accepting new requests. Current requests should be allowed to
        complete if graceful exit strategy.
     */
    if (MPR->state >= MPR_STOPPING) {
        /* Already stopping and done the code below */
        return;
    }
    MPR->state = MPR_STOPPING;

    /*
        Invoke terminators, set stopping state and wake up everybody
        Must invoke terminators before setting stopping state. Otherwise, the main app event loop will return from
        mprServiceEvents and starting calling destroy before we have completed this routine.
     */
    for (ITERATE_ITEMS(MPR->terminators, terminator, next)) {
        (terminator)(how, status);
    }
    mprWakeWorkers();
    mprWakeGCService();
    mprWakeDispatchers();
    mprWakeNotifier();
}


int mprGetExitStatus()
{
    return MPR->exitStatus;
}


void mprAddTerminator(MprTerminator terminator)
{
    mprAddItem(MPR->terminators, terminator);
}


void mprRestart()
{
    //  MOB TODO - Other systems
#if BLD_UNIX_LIKE
    int     i;
    for (i = 3; i < MPR_MAX_FILE; i++) {
        close(i);
    }
    execv(MPR->argv[0], MPR->argv);

    /*
        Last-ditch trace. Can only use stdout. Logging may be closed.
     */
    printf("Failed to exec errno %d: ", errno);
    for (i = 0; MPR->argv[i]; i++) {
        printf("%s ", MPR->argv[i]);
    }
    printf("\n");
#endif
}


int mprStart()
{
    int     rc;

    rc = mprStartOsService();
    rc += mprStartModuleService();
    rc += mprStartWorkerService();
    if (rc != 0) {
        mprUserError("Can't start MPR services");
        return MPR_ERR_CANT_INITIALIZE;
    }
    MPR->state = MPR_STARTED;
    mprLog(MPR_INFO, "MPR services are ready");
    return 0;
}


int mprStartEventsThread()
{
    MprThread   *tp;

    if ((tp = mprCreateThread("events", serviceEventsThread, NULL, 0)) == 0) {
        MPR->hasError = 1;
    } else {
        MPR->cond = mprCreateCond();
        mprStartThread(tp);
        mprWaitForCond(MPR->cond, MPR_TIMEOUT_START_TASK);
    }
    return 0;
}


static void serviceEventsThread(void *data, MprThread *tp)
{
    mprLog(MPR_CONFIG, "Service thread started");
    if (!(MPR->flags & MPR_NO_WINDOW)) {
        mprInitWindow();
    }
    mprSignalCond(MPR->cond);
    mprServiceEvents(-1, 0);
}


/*
    Services should call this to determine if they should accept new services
 */
bool mprShouldAbortRequests()
{
    return (mprIsStopping() && !(MPR->exitStrategy & MPR_EXIT_GRACEFUL));
}


bool mprShouldDenyNewRequests()
{
    return mprIsStopping();
}


bool mprIsStopping()
{
    return MPR->state >= MPR_STOPPING;
}


bool mprIsStoppingCore()
{
    return MPR->state >= MPR_STOPPING_CORE;
}


bool mprIsFinished()
{
    return MPR->state >= MPR_FINISHED;
}


int mprWaitTillIdle(MprTime timeout)
{
    MprTime     mark, remaining, lastTrace;

    lastTrace = mark = mprGetTime(); 
    while (!mprIsIdle() && (remaining = mprGetRemainingTime(mark, timeout)) > 0) {
        mprSleep(1);
        if ((lastTrace - remaining) > MPR_TICKS_PER_SEC) {
            mprLog(1, "Waiting for requests to complete, %d secs remaining ...", remaining / MPR_TICKS_PER_SEC);
            lastTrace = remaining;
        }
    }
    return mprIsIdle();
}


/*
    Test if the Mpr services are idle. Use mprIsIdle to determine if the entire process is idle.
 */
bool mprServicesAreIdle()
{
    bool    idle;

    /*
        Only test top level services. Dispatchers may have timers scheduled, but that is okay.
     */
    idle = mprGetListLength(MPR->workerService->busyThreads) == 0 && mprGetListLength(MPR->cmdService->cmds) == 0;
    if (!idle) {
        mprLog(4, "Not idle: cmds %d, busy threads %d, eventing %d",
            mprGetListLength(MPR->cmdService->cmds), mprGetListLength(MPR->workerService->busyThreads), MPR->eventing);
    }
    return idle;
}


bool mprIsIdle()
{
    return (MPR->idleCallback)();
}


/*
    Parse the args and return the count of args. If argv is NULL, the args are parsed read-only. If argv is set,
    then the args will be extracted, back-quotes removed and argv will be set to point to all the args.
 */
int mprParseArgs(char *args, char **argv, int maxArgc)
{
    char    *dest, *src, *start;
    int     quote, argc;

    /*
        Example     "showColors" red 'light blue' "yellow white" 'Can\'t \"render\"'
        Becomes:    ["showColors", "red", "light blue", "yellow white", "Can't \"render\""]
     */
    for (argc = 0, src = args; src && *src != '\0' && argc < maxArgc; argc++) {
        while (isspace((int) *src)) {
            src++;
        }
        if (*src == '\0')  {
            break;
        }
        start = dest = src;
        if (*src == '"' || *src == '\'') {
            quote = *src;
            src++; 
            dest++;
        } else {
            quote = 0;
        }
        if (argv) {
            argv[argc] = src;
        }
        while (*src) {
            if (*src == '\\' && src[1] && (src[1] == '\\' || src[1] == '"' || src[1] == '\'')) { 
                src++;
            } else {
                if (quote) {
                    if (*src == quote && !(src > start && src[-1] == '\\')) {
                        break;
                    }
                } else if (*src == ' ') {
                    break;
                }
            }
            if (argv) {
                *dest++ = *src;
            }
            src++;
        }
        if (*src != '\0') {
            src++;
        }
        if (argv) {
            *dest++ = '\0';
        }
    }
    return argc;
}


/*
    Make an argv array. All args are in a single memory block of which argv points to the start.
    Set MPR_ARGV_ARGS_ONLY if not passing in a program name. 
    Always returns and argv[0] reserved for the program name or empty string.  First arg starts at argv[1].
 */
int mprMakeArgv(cchar *command, char ***argvp, int flags)
{
    char    **argv, *vector, *args;
    ssize   len;
    int     argc;

    mprAssert(command);

    /*
        Allocate one vector for argv and the actual args themselves
     */
    len = slen(command) + 1;
    argc = mprParseArgs((char*) command, NULL, INT_MAX);
    if (flags & MPR_ARGV_ARGS_ONLY) {
        argc++;
    }
    if ((vector = (char*) mprAlloc(((argc + 1) * sizeof(char*)) + len)) == 0) {
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    args = &vector[(argc + 1) * sizeof(char*)];
    strcpy(args, command);
    argv = (char**) vector;

    if (flags & MPR_ARGV_ARGS_ONLY) {
        mprParseArgs(args, &argv[1], argc);
        argv[0] = MPR->emptyString;
    } else {
        mprParseArgs(args, argv, argc);
    }
    argv[argc] = 0;
    *argvp = argv;
    return argc;
}


MprIdleCallback mprSetIdleCallback(MprIdleCallback idleCallback)
{
    MprIdleCallback old;
    
    old = MPR->idleCallback;
    MPR->idleCallback = idleCallback;
    return old;
}


int mprSetAppName(cchar *name, cchar *title, cchar *version)
{
    char    *cp;

    if (name) {
        if ((MPR->name = (char*) mprGetPathBase(name)) == 0) {
            return MPR_ERR_CANT_ALLOCATE;
        }
        if ((cp = strrchr(MPR->name, '.')) != 0) {
            *cp = '\0';
        }
    }
    if (title) {
        if ((MPR->title = sclone(title)) == 0) {
            return MPR_ERR_CANT_ALLOCATE;
        }
    }
    if (version) {
        if ((MPR->version = sclone(version)) == 0) {
            return MPR_ERR_CANT_ALLOCATE;
        }
    }
    return 0;
}


cchar *mprGetAppName()
{
    return MPR->name;
}


cchar *mprGetAppTitle()
{
    return MPR->title;
}


/*
    Full host name with domain. E.g. "server.domain.com"
 */
void mprSetHostName(cchar *s)
{
    MPR->hostName = sclone(s);
}


/*
    Return the fully qualified host name
 */
cchar *mprGetHostName()
{
    return MPR->hostName;
}


/*
    Server name portion (no domain name)
 */
void mprSetServerName(cchar *s)
{
    MPR->serverName = sclone(s);
}


cchar *mprGetServerName()
{
    return MPR->serverName;
}


void mprSetDomainName(cchar *s)
{
    MPR->domainName = sclone(s);
}


cchar *mprGetDomainName()
{
    return MPR->domainName;
}


/*
    Set the IP address
 */
void mprSetIpAddr(cchar *s)
{
    MPR->ip = sclone(s);
}


/*
    Return the IP address
 */
cchar *mprGetIpAddr()
{
    return MPR->ip;
}


cchar *mprGetAppVersion()
{
    return MPR->version;
}


bool mprGetDebugMode()
{
    return MPR->debugMode;
}


void mprSetDebugMode(bool on)
{
    MPR->debugMode = on;
}


MprDispatcher *mprGetDispatcher()
{
    return MPR->dispatcher;
}


MprDispatcher *mprGetNonBlockDispatcher()
{
    return MPR->nonBlock;
}


cchar *mprCopyright()
{
    return  "Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.\n"
            "Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.";
}


int mprGetEndian()
{
    char    *probe;
    int     test;

    test = 1;
    probe = (char*) &test;
    return (*probe == 1) ? MPR_LITTLE_ENDIAN : MPR_BIG_ENDIAN;
}


char *mprEmptyString()
{
    return MPR->emptyString;
}


void mprSetExitStrategy(int strategy)
{
    MPR->exitStrategy = strategy;
}


void mprLockDtoa(int n)
{
    mprSpinLock(MPR->dtoaSpin[n]);
}


void mprUnlockDtoa(int n)
{
    mprSpinUnlock(MPR->dtoaSpin[n]);
}


void mprNop(void *ptr) {}

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
