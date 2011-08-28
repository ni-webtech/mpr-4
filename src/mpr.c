/*
    mpr.c - Multithreaded Portable Runtime (MPR). Initialization, start/stop and control of the MPR.

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/**************************** Forward Declarations ****************************/

static void getArgs(Mpr *mpr, int argc, char **argv);
static void manageMpr(Mpr *mpr, int flags);
static void serviceEventsThread(void *data, MprThread *tp);
static void startThreads(int flags);

/************************************* Code ***********************************/
/*
    Create and initialize the MPR service.
 */
Mpr *mprCreate(int argc, char **argv, int flags)
{
    MprFileSystem   *fs;
    Mpr             *mpr;
    char            *cp, *name;

    srand((uint) time(NULL));

    if ((mpr = mprCreateMemService((MprManager) manageMpr, flags)) == 0) {
        mprAssert(mpr);
        return 0;
    }
    getArgs(mpr, argc, argv);
    mpr->exitStrategy = MPR_EXIT_NORMAL;
    mpr->emptyString = sclone("");
    mpr->title = sclone(BLD_NAME);
    mpr->version = sclone(BLD_VERSION);
    mpr->idleCallback = mprServicesAreIdle;
    mpr->mimeTypes = mprCreateMimeTypes(NULL);

    if (mpr->argv && mpr->argv[0] && *mpr->argv[0]) {
        name = mpr->argv[0];
        if ((cp = strrchr(name, '/')) != 0 || (cp = strrchr(name, '\\')) != 0) {
            name = &cp[1];
        }
        mpr->name = sclone(name);
        if ((cp = strrchr(mpr->name, '.')) != 0) {
            *cp = '\0';
        }
    } else {
        mpr->name = sclone(BLD_PRODUCT);
    }
    mprCreateTimeService();
    mprCreateOsService();
    mpr->mutex = mprCreateLock();
    mpr->spin = mprCreateSpinLock();

    fs = mprCreateFileSystem("/");
    mprAddFileSystem(fs);

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

    startThreads(flags);

    if (MPR->hasError || mprHasMemError()) {
        return 0;
    }
    return mpr;
}


static void manageMpr(Mpr *mpr, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(mpr->logFile);
        mprMark(mpr->mimeTypes);
        mprMark(mpr->timeTokens);
        mprMark(mpr->name);
        mprMark(mpr->title);
        mprMark(mpr->version);
        mprMark(mpr->domainName);
        mprMark(mpr->hostName);
        mprMark(mpr->ip);
        mprMark(mpr->serverName);
        mprMark(mpr->appDir);
        mprMark(mpr->appPath);
        mprMark(mpr->cmdService);
        mprMark(mpr->fileSystem);
        mprMark(mpr->eventService);
        mprMark(mpr->moduleService);
        mprMark(mpr->osService);
        mprMark(mpr->signalService);
        mprMark(mpr->socketService);
        mprMark(mpr->threadService);
        mprMark(mpr->workerService);
        mprMark(mpr->waitService);
        mprMark(mpr->dispatcher);
        mprMark(mpr->nonBlock);
        mprMark(mpr->ejsService);
        mprMark(mpr->httpService);
        mprMark(mpr->appwebService);
        mprMark(mpr->testService);
        mprMark(mpr->mutex);
        mprMark(mpr->spin);
        mprMark(mpr->emptyString);
        mprMark(mpr->pathEnv);
        mprMark(mpr->heap.markerCond);
    }
}


/*
    Destroy the Mpr and all services
 */
void mprDestroy(int how)
{
    MprTime     mark;
    int         gmode;

    if (how != MPR_EXIT_DEFAULT) {
        MPR->exitStrategy = how;
    }
    how = MPR->exitStrategy;
    if (how == MPR_EXIT_IMMEDIATE) {
        exit(0);
    }
    mprYield(MPR_YIELD_STICKY);
    if (MPR->state < MPR_STOPPING) {
        mprTerminate(how, -1);
    }
    gmode = MPR_FORCE_GC | MPR_COMPLETE_GC | MPR_WAIT_GC;
    mprRequestGC(gmode);

    if (how == MPR_EXIT_GRACEFUL) {
        mprWaitTillIdle(MPR_TIMEOUT_STOP);
    }
    MPR->state = MPR_STOPPING_CORE;
    MPR->exitStrategy = MPR_EXIT_IMMEDIATE;

    mprStopCmdService();
    mprStopModuleService();
    mprStopEventService();
    mprStopSignalService();

    /* Final GC to run all finalizers */
    mprRequestGC(gmode);

    MPR->state = MPR_FINISHED;
    mprStopGCService();
    mprStopThreadService();

    /*
        Must wait for the GC, ServiceEvents, threads and worker threads to exit. Otherwise we have races when freeing memory.
     */
    mark = mprGetTime();
    while (MPR->marker || MPR->eventing || mprGetListLength(MPR->workerService->busyThreads) > 0 ||
            mprGetListLength(MPR->threadService->threads) > 0) {
        if (mprGetRemainingTime(mark, MPR_TIMEOUT_STOP) <= 0) {
            break;
        }
#if UNUSED && KEEP
        printf("marker %d, eventing %d, busyThreads %d, threads %d\n", MPR->marker, MPR->eventing, 
                MPR->workerService->busyThreads->length,
                MPR->threadService->threads->length);
#endif
        mprSleep(1);
    }
    mprStopOsService();
    mprDestroyMemService();
}



/*
    Start termination of the Mpr. May be called by mprDestroy or elsewhere.
 */
void mprTerminate(int how, int status)
{
    MPR->exitStatus = status;
    if (how != MPR_EXIT_DEFAULT) {
        MPR->exitStrategy = how;
    }
    how = MPR->exitStrategy;
    if (how == MPR_EXIT_IMMEDIATE) {
        mprLog(5, "Immediate exit. Aborting all requests and services.");
        exit(status);
    } else if (how == MPR_EXIT_NORMAL) {
        mprLog(5, "Normal exit. Flush buffers, close files and aborting existing requests.");
    } else if (how == MPR_EXIT_GRACEFUL) {
        mprLog(5, "Graceful exit. Waiting for existing requests to complete.");
    } else {
        mprLog(7, "HOW %d", how);
    }

    /*
        Set the stopping flag. Services should stop accepting new requests. Current requests should be allowed to
        complete if graceful exit strategy.
     */
    if (MPR->state >= MPR_STOPPING) {
        return;
    }
    /*
        Set stopping state and wake up everybody
     */
    MPR->state = MPR_STOPPING;
    mprWakeDispatchers();
    mprWakeWorkers();
    mprWakeGCService();
    mprWakeNotifier();
}


/*
    Wince and Vxworks passes an arg via argc, and the program name in argv. NOTE: this will only work on 32-bit systems.
 */
static void getArgs(Mpr *mpr, int argc, char **argv) 
{
#if WINCE
    MprArgs *args = (MprArgs*) argv;
    command = mprToMulti((uni*) args->command);
    mprMakeArgv(command, &argc, &argv, MPR_ARGV_ARGS_ONLY);
    argv[0] = sclone(args->program);
#elif VXWORKS
    MprArgs *args = (MprArgs*) argv;
    mprMakeArgv("", &argc, &argv, MPR_ARGV_ARGS_ONLY);
    argv[0] = sclone(args->program);
#endif
    mpr->argc = argc;
    mpr->argv = argv;
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


static void startThreads(int flags)
{
    MprThread   *tp;

    if (flags & MPR_USER_EVENTS_THREAD) {
        mprInitWindow();
    } else {
        if ((tp = mprCreateThread("events", serviceEventsThread, NULL, 0)) == 0) {
            MPR->hasError = 1;
        } else {
            MPR->cond = mprCreateCond();
            mprStartThread(tp);
            mprWaitForCond(MPR->cond, MPR_TIMEOUT_START_TASK);
        }
    }
    mprStartGCService();
}


static void serviceEventsThread(void *data, MprThread *tp)
{
    mprLog(MPR_CONFIG, "Service thread started");
    mprInitWindow();
    mprSignalCond(MPR->cond);
    mprServiceEvents(-1, 0);
}


/*
    Services should call this to determine if they should accept new services
 */
bool mprShouldAbortRequests()
{
    return (mprIsStopping() && MPR->exitStrategy != MPR_EXIT_GRACEFUL);
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
    MprTime     mark;

    mark = mprGetTime(); 
    while (!mprIsIdle() && mprGetRemainingTime(mark, timeout) > 0) {
        mprSleep(1);
    }
    return mprIsIdle();
}


/*
    Test if the Mpr services are idle. Use mprIsIdle to determine if the entire process is idle.
 */
bool mprServicesAreIdle()
{
    bool    idle;

    //  FUTURE - should also measure open sockets?

    idle = mprGetListLength(MPR->workerService->busyThreads) == 0 && 
           mprGetListLength(MPR->cmdService->cmds) == 0 && 
           mprDispatchersAreIdle() && !MPR->eventing;
    if (!idle) {
        mprLog(1, "Not idle: cmds %d, busy threads %d, dispatchers %d, eventing %d",
            mprGetListLength(MPR->cmdService->cmds), mprGetListLength(MPR->workerService->busyThreads),
           !mprDispatchersAreIdle(), MPR->eventing);
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
static int parseArgs(char *args, char **argv)
{
    char    *dest, *src, *start;
    int     quote, argc;

    /*
        Example     "showColors" red 'light blue' "yellow white" 'Can\'t \"render\"'
        Becomes:    ["showColors", "red", "light blue", "yellow white", "Can't \"render\""]
     */
    for (argc = 0, src = args; src && *src != '\0'; argc++) {
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
    Make an argv array
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
    argc = parseArgs((char*) command, NULL);
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

    parseArgs(args, argv);
    if (flags & MPR_ARGV_ARGS_ONLY) {
        argv[0] = MPR->emptyString;
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
