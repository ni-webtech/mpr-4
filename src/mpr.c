/*
    mpr.c - Multithreaded Portable Runtime (MPR). Initialization, start/stop and control of the MPR.

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/**************************** Forward Declarations ****************************/

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
    /*
        Wince and Vxworks passes an arg via argc, and the program name in argv. NOTE: this will only work on 32-bit systems.
        TODO - refactor this
     */
#if WINCE
    mprMakeArgv((char*) argv, mprToMulti((uni*) argc), &argc, &argv);
#elif VXWORKS
    mprMakeArgv(NULL, (char*) argc, &argc, &argv);
#endif
    mpr->argc = argc;
    mpr->argv = argv;
    mpr->logFd = -1;

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

    mpr->threadService = mprCreateThreadService();
    mpr->moduleService = mprCreateModuleService();
    mpr->eventService = mprCreateEventService();
    mpr->cmdService = mprCreateCmdService();
    mpr->workerService = mprCreateWorkerService();
    mpr->waitService = mprCreateWaitService();
    mpr->socketService = mprCreateSocketService();

    mpr->dispatcher = mprCreateDispatcher("main", 1);
    mpr->nonBlock = mprCreateDispatcher("nonblock", 1);

    startThreads(flags);

    if (MPR->hasError || mprHasMemError()) {
        return 0;
    }
    return mpr;
}


static void manageMpr(Mpr *mpr, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(mpr->logData);
        mprMark(mpr->altLogData);
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
        mprMark(mpr->heap.markerCond);
    }
}


/*
    Destroy the Mpr and all services
 */
void mprDestroy(int flags)
{
    int     gcflags;

    mprYield(MPR_YIELD_STICKY);
    mprTerminate(flags);

    gcflags = MPR_FORCE_GC | MPR_COMPLETE_GC | ((flags & MPR_GRACEFUL) ? MPR_WAIT_GC : 0);
    mprRequestGC(gcflags);

    if (flags & MPR_GRACEFUL) {
        mprWaitTillIdle();
    }
    MPR->state = MPR_STOPPING_CORE;
    mprStopCmdService();
    mprStopModuleService();
    mprStopEventService();
    mprRequestGC(gcflags);
    mprStopThreadService();
    MPR->state = MPR_FINISHED;
    mprStopOsService();
    mprDestroyMemService();
}


/*
    Start termination of the Mpr. May be called by mprDestroy or elsewhere.
 */
void mprTerminate(int flags)
{
    if (! (flags & MPR_GRACEFUL)) {
        exit(0);
    }

    /*
        Set the stopping flag. Services should stop accepting new requests.
     */
    if (MPR->state >= MPR_STOPPING) {
        return;
    }
    mprLog(MPR_CONFIG, "Exiting started");
    
    /*
        Set stopping state and wake up everybody
     */
    MPR->state = MPR_STOPPING;
    mprWakeDispatchers();
    mprWakeWorkers();
    mprWakeGCService();
    mprWakeWaitService();
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


void mprWaitTillIdle()
{
    MprTime     mark;

    mark = mprGetTime(); 
    while (!mprIsIdle() && mprGetRemainingTime(mark, MPR_TIMEOUT_STOP) > 0) {
        mprSleep(10);
    }
}


/*
    Test if the Mpr services are idle. Use mprIsIdle to determine if the entire process is idle.
 */
bool mprServicesAreIdle()
{
    bool    idle;

    idle = mprGetListLength(MPR->workerService->busyThreads) == 0 && 
           mprGetListLength(MPR->cmdService->cmds) == 0 && 
           mprDispatchersAreIdle() && !MPR->eventing;
    if (!idle) {
        mprLog(1, "Testing idle: cmds %d, threads %d, dispatchers %d, marker %d, sweeper %d",
            mprGetListLength(MPR->workerService->busyThreads),
           mprDispatchersAreIdle(), mprGetListLength(MPR->cmdService->cmds), MPR->marker, MPR->sweeper);
    }
    return idle;
}


bool mprIsIdle()
{
    return (MPR->idleCallback)();
}


/*
    Make an argv array
    MOB - genericize and remove "program"
 */
int mprMakeArgv(cchar *program, cchar *cmd, int *argcp, char ***argvp)
{
    char        *cp, **argv, *vector, *args;
    ssize       size;
    int         argc;

    /*
        Allocate one vector for argv and the actual args themselves
     */
    size = strlen(cmd) + 1;

    //  MOB - remove MPR_MAX_ARGC and calculate it
    vector = (char*) mprAlloc((MPR_MAX_ARGC * sizeof(char*)) + size);
    if (vector == 0) {
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    args = &vector[MPR_MAX_ARGC * sizeof(char*)];
    strcpy(args, cmd);
    argv = (char**) vector;

    argc = 0;
    if (program) {
        argv[argc++] = (char*) sclone(program);
    }
    for (cp = args; cp && *cp != '\0'; argc++) {
        if (argc >= MPR_MAX_ARGC) {
            mprAssert(argc < MPR_MAX_ARGC);
            *argvp = 0;
            if (argcp) {
                *argcp = 0;
            }
            return MPR_ERR_TOO_MANY;
        }
        while (isspace((int) *cp)) {
            cp++;
        }
        if (*cp == '\0')  {
            break;
        }
        if (*cp == '"') {
            cp++;
            argv[argc] = cp;
            while ((*cp != '\0') && (*cp != '"')) {
                cp++;
            }
        } else if (*cp == '\'') {
            cp++;
            argv[argc] = cp;
            while ((*cp != '\0') && (*cp != '\'')) {
                cp++;
            }
        } else {
            argv[argc] = cp;
            while (*cp != '\0' && !isspace((int) *cp)) {
                cp++;
            }
        }
        if (*cp != '\0') {
            *cp++ = '\0';
        }
    }
    argv[argc] = 0;

    if (argcp) {
        *argcp = argc;
    }
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


void mprSetLogHandler(MprLogHandler handler, void *handlerData)
{
    MPR->logHandler = handler;
    MPR->logData = handlerData;
}


MprLogHandler mprGetLogHandler()
{
    return MPR->logHandler;
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
