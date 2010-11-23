/*
    mpr.c - Multithreaded Portable Runtime (MPR). Initialization, start/stop and control of the MPR.

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/**************************** Forward Declarations ****************************/

static void memoryNotifier(int flags, size_t size);
static void manageMpr(Mpr *mpr, int flags);
static void serviceEventsThread(void *data, MprThread *tp);

/************************************* Code ***********************************/
/*
    Create the MPR service. This routine is the first call an MPR application must do. It creates the top 
    level memory context.
 */
Mpr *mprCreate(int argc, char **argv, MprMemNotifier cback)
{
    MprFileSystem   *fs;
    Mpr             *mpr;
    char            *cp;

    if (cback == 0) {
        cback = memoryNotifier;
    }
    srand((uint) time(NULL));

    if ((mpr = (Mpr*) mprCreateMemService(cback, (MprManager) manageMpr)) == 0) {
        mprAssert(mpr);
        return 0;
    }
    /*
        Wince and Vxworks passes an arg via argc, and the program name in argv. NOTE: this will only work on 32-bit systems.
        TODO - refactor this
     */
#if WINCE
    mprMakeArgv(mpr, (char*) argv, mprToMulti(mpr, (uni*) argc), &argc, &argv);
#elif VXWORKS
    mprMakeArgv(mpr, NULL, (char*) argc, &argc, &argv);
#endif
    mpr->argc = argc;
    mpr->argv = argv;
    mpr->logFd = -1;

    mpr->title = sclone(mpr, BLD_NAME);
    mpr->version = sclone(mpr, BLD_VERSION);
    mpr->idleCallback = mprServicesAreIdle;

    if (mpr->argv && mpr->argv[0] && *mpr->argv[0]) {
        mpr->name = sclone(mpr, basename(mpr->argv[0]));
        if ((cp = strchr(mpr->name, '.')) != 0) {
            *cp = '\0';
        }
    } else {
        mpr->name = sclone(mpr, BLD_PRODUCT);
    }
    if (mprCreateTimeService(mpr) < 0) {
        goto error;
    }
    if (mprCreateOsService() < 0) {
        goto error;
    }

    /*
        See if any of the preceeding allocations failed and mark all blocks allocated so far as required.
        They will then be omitted from leak reports.
     */
    if (mprHasMemError()) {
        goto error;
    }
    if ((mpr->threadService = mprCreateThreadService(mpr)) == 0) {
        goto error;
    }
    mpr->mutex = mprCreateLock(mpr);
    mpr->spin = mprCreateSpinLock(mpr);

    if ((fs = mprCreateFileSystem(mpr, "/")) == 0) {
        goto error;
    }
    mprAddFileSystem(mpr, fs);

    if ((mpr->moduleService = mprCreateModuleService(mpr)) == 0) {
        goto error;
    }
    if ((mpr->eventService = mprCreateEventService(mpr)) == 0) {
        goto error;
    }
    if ((mpr->cmdService = mprCreateCmdService(mpr)) == 0) {
        goto error;
    }
    if ((mpr->workerService = mprCreateWorkerService(mpr)) == 0) {
        goto error;
    }
    if ((mpr->waitService = mprCreateWaitService(mpr)) == 0) {
        goto error;
    }
    if ((mpr->socketService = mprCreateSocketService(mpr)) == 0) {
        goto error;
    }
    /*
        Now catch all memory allocation errors up to this point. Should be none.
     */
    if (mprHasMemError()) {
        goto error;
    }
    return mpr;

error:
    mprFree(mpr);
    return 0;
}


static void manageMpr(Mpr *mpr, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(mpr->altLogData);
        mprMark(mpr->appDir);
        mprMark(mpr->appPath);
        mprMark(mpr->cmdService);
        mprMark(mpr->dispatcher);
        mprMark(mpr->domainName);
        mprMark(mpr->ejsService);
        mprMark(mpr->eventService);
        mprMark(mpr->fileSystem);
        mprMark(mpr->hostName);
        mprMark(mpr->ip);
        mprMark(mpr->logData);
        mprMark(mpr->moduleService);
        mprMark(mpr->mutex);
        mprMark(mpr->name);
        mprMark(mpr->osService);
        mprMark(mpr->serverName);
        mprMark(mpr->spin);
        mprMark(mpr->socketService);
        mprMark(mpr->threadService);
        mprMark(mpr->timeTokens);
        mprMark(mpr->title);
        mprMark(mpr->version);
        mprMark(mpr->waitService);
        mprMark(mpr->workerService);
#if UNUSED
        mprMark(mpr->heap.cond);
#endif

    } else if (flags & MPR_MANAGE_FREE) {
        if ((mpr->flags & MPR_STARTED) && !(mpr->flags & MPR_STOPPED)) {
            mprStop(mpr);
        }
        mprDestroyMemService();
    }
}


int mprStart(Mpr *mpr)
{
    int     rc;

    rc = mprStartOsService(mpr->osService);
    rc += mprStartModuleService(mpr->moduleService);
    rc += mprStartWorkerService(mpr->workerService);
    rc += mprStartSocketService(mpr->socketService);
    if (rc != 0) {
        mprUserError(mpr, "Can't start MPR services");
        return MPR_ERR_CANT_INITIALIZE;
    }
    mpr->flags |= MPR_STARTED;
    mprLog(mpr, MPR_INFO, "MPR services are ready");
    return 0;
}


bool mprStop(Mpr *mpr)
{
    int     stopped;

    stopped = 1;
    mprLock(mpr->mutex);
    if ((!(mpr->flags & MPR_STARTED)) || (mpr->flags & MPR_STOPPED)) {
        mprUnlock(mpr->mutex);
        return 0;
    }
    mpr->flags |= MPR_STOPPED;

    /*
        Trigger graceful termination. This will prevent further tasks and events being created.
     */
    mprTerminate(mpr, 1);
    mprStopSocketService(mpr->socketService);
    if (!mprStopWorkerService(mpr->workerService, MPR_TIMEOUT_STOP_TASK)) {
        stopped = 0;
    }
    if (!mprStopThreadService(mpr->threadService, MPR_TIMEOUT_STOP_TASK)) {
        stopped = 0;
    }
    mprStopModuleService(mpr->moduleService);
    mprStopOsService(mpr->osService);
    return stopped;
}


//  MOB -- is this being used?
//  MOB - problem on windows. Causes the message pump to run in a thread that does not own the HWND.
/*
    Thread to service the event queue. Used if the user does not have their own main event loop.
 */
int mprStartEventsThread(Mpr *mpr)
{
    MprThread   *tp;

    mprLog(mpr, MPR_CONFIG, "Starting service thread");
    if ((tp = mprCreateThread(mpr, "events", serviceEventsThread, NULL, 0)) == 0) {
        return MPR_ERR_CANT_CREATE;
    }
    mpr->hasDedicatedService = 1;
    mprStartThread(tp);
    return 0;
}


static void serviceEventsThread(void *data, MprThread *tp)
{
    mprServiceEvents(tp, NULL, -1, 0);
}


/*
    Exit the mpr gracefully. Instruct the event loop to exit.
 */
void mprTerminate(MprCtx ctx, bool graceful)
{
    if (! graceful) {
        exit(0);
    }
    mprSignalExit(ctx);
}


bool mprIsExiting(MprCtx ctx)
{
    Mpr *mpr;

    mpr = mprGetMpr(ctx);
    if (mpr == 0) {
        return 1;
    }
    return mpr->flags & MPR_EXITING;
}


bool mprIsComplete(MprCtx ctx)
{
    Mpr *mpr;

    mpr = mprGetMpr(ctx);
    if (mpr == 0) {
        return 1;
    }
    return (mpr->flags & MPR_EXITING) && mprIsIdle(ctx);
}


//  MOB - order file

/*
    Make an argv array. Caller must free by calling mprFree(argv) to free everything.
 */
int mprMakeArgv(MprCtx ctx, cchar *program, cchar *cmd, int *argcp, char ***argvp)
{
    char        *cp, **argv, *buf, *args;
    int         size, argc;

    /*
        Allocate one buffer for argv and the actual args themselves
     */
    size = strlen(cmd) + 1;

    buf = (char*) mprAlloc(ctx, (MPR_MAX_ARGC * sizeof(char*)) + size);
    if (buf == 0) {
        return MPR_ERR_MEMORY;
    }
    args = &buf[MPR_MAX_ARGC * sizeof(char*)];
    strcpy(args, cmd);
    argv = (char**) buf;

    argc = 0;
    if (program) {
        argv[argc++] = (char*) sclone(ctx, program);
    }

    for (cp = args; cp && *cp != '\0'; argc++) {
        if (argc >= MPR_MAX_ARGC) {
            mprAssert(argc < MPR_MAX_ARGC);
            mprFree(buf);
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


/*
    Just the Mpr services are idle. Use mprIsIdle to determine if the entire process is idle
 */
bool mprServicesAreIdle(MprCtx ctx)
{
#if MOB
    Mpr     *mpr;
    
    mpr = mprGetMpr(ctx);
    return mprGetListCount(mpr->workerService->busyThreads) == 0 && mprGetListCount(mpr->cmdService->cmds) == 0 && 
        //MOB -- dispatcher here not right
       !(mpr->dispatcher->flags & MPR_DISPATCHER_DO_EVENT);
#endif
    return 1;
}


bool mprIsIdle(MprCtx ctx)
{
    return (mprGetMpr(ctx)->idleCallback)(ctx);
}


MprIdleCallback mprSetIdleCallback(MprCtx ctx, MprIdleCallback idleCallback)
{
    MprIdleCallback old;
    Mpr             *mpr;
    
    mpr = mprGetMpr(ctx);
    old = mpr->idleCallback;
    mpr->idleCallback = idleCallback;
    return old;
}


void mprSignalExit(MprCtx ctx)
{
    Mpr     *mpr;

    mpr = mprGetMpr(ctx);

    mprSpinLock(mpr->spin);
    mpr->flags |= MPR_EXITING;
    mprSpinUnlock(mpr->spin);
    mprWakeWaitService(mpr);
}


int mprSetAppName(MprCtx ctx, cchar *name, cchar *title, cchar *version)
{
    Mpr     *mpr;
    char    *cp;

    mpr = mprGetMpr(ctx);

    if (name) {
        mprFree(mpr->name);
        if ((mpr->name = (char*) mprGetPathBase(mpr, name)) == 0) {
            return MPR_ERR_CANT_ALLOCATE;
        }
        if ((cp = strrchr(mpr->name, '.')) != 0) {
            *cp = '\0';
        }
    }
    if (title) {
        mprFree(mpr->title);
        if ((mpr->title = sclone(mpr, title)) == 0) {
            return MPR_ERR_CANT_ALLOCATE;
        }
    }
    if (version) {
        mprFree(mpr->version);
        if ((mpr->version = sclone(mpr, version)) == 0) {
            return MPR_ERR_CANT_ALLOCATE;
        }
    }
    return 0;
}


cchar *mprGetAppName(MprCtx ctx)
{
    return mprGetMpr(ctx)->name;
}


cchar *mprGetAppTitle(MprCtx ctx)
{
    return mprGetMpr(ctx)->title;
}


/*
    Full host name with domain. E.g. "server.domain.com"
 */
void mprSetHostName(MprCtx ctx, cchar *s)
{
    Mpr     *mpr;

    mpr = mprGetMpr(ctx);
    mprLock(mpr->mutex);
    mprFree(mpr->hostName);
    mpr->hostName = sclone(mpr, s);
    mprUnlock(mpr->mutex);
    return;
}


/*
    Return the fully qualified host name
 */
cchar *mprGetHostName(MprCtx ctx)
{
    return mprGetMpr(ctx)->hostName;
}


/*
    Server name portion (no domain name)
 */
void mprSetServerName(MprCtx ctx, cchar *s)
{
    Mpr     *mpr;

    mpr = mprGetMpr(ctx);
    if (mpr->serverName) {
        mprFree(mpr->serverName);
    }
    mpr->serverName = sclone(mpr, s);
    return;
}


/*
    Return the server name
 */
cchar *mprGetServerName(MprCtx ctx)
{
    return mprGetMpr(ctx)->serverName;
}


/*
    Set the domain name
 */
void mprSetDomainName(MprCtx ctx, cchar *s)
{
    Mpr     *mpr;

    mpr = mprGetMpr(ctx);
    mprFree(mpr->domainName);
    mpr->domainName = sclone(mpr, s);
    return;
}


/*
    Return the domain name
 */
cchar *mprGetDomainName(MprCtx ctx)
{
    return mprGetMpr(ctx)->domainName;
}


/*
    Set the IP address
 */
void mprSetIpAddr(MprCtx ctx, cchar *s)
{
    Mpr     *mpr;

    mpr = mprGetMpr(ctx);
    if (mpr->ip) {
        mprFree(mpr->ip);
    }
    mpr->ip = sclone(mpr, s);
    return;
}


/*
    Return the IP address
 */
cchar *mprGetIpAddr(MprCtx ctx)
{
    return mprGetMpr(ctx)->ip;
}


cchar *mprGetAppVersion(MprCtx ctx)
{
    Mpr *mpr;

    mpr = mprGetMpr(ctx);
    return mpr->version;
}


bool mprGetDebugMode(MprCtx ctx)
{
    return mprGetMpr(ctx)->debugMode;
}


void mprSetDebugMode(MprCtx ctx, bool on)
{
    mprGetMpr(ctx)->debugMode = on;
}


void mprSetLogHandler(MprCtx ctx, MprLogHandler handler, void *handlerData)
{
    Mpr     *mpr;

    mpr = mprGetMpr(ctx);

    mpr->logHandler = handler;
    mpr->logData = handlerData;
}


MprLogHandler mprGetLogHandler(MprCtx ctx)
{
    return mprGetMpr(ctx)->logHandler;
}


cchar *mprCopyright()
{
    return  "Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.\n"
            "Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.";
}


int mprGetEndian(MprCtx ctx)
{
    char    *probe;
    int     test;

    test = 1;
    probe = (char*) &test;
    return (*probe == 1) ? MPR_LITTLE_ENDIAN : MPR_BIG_ENDIAN;
}


/*
    Default memory handler
 */
static void memoryNotifier(int flags, size_t size)
{
    if (flags & MPR_ALLOC_DEPLETED) {
        mprPrintfError(NULL, "Can't allocate memory block of size %d\n", size);
        mprPrintfError(NULL, "Total memory used %d\n", mprGetUsedMemory());
        exit(255);
    } else if (flags & MPR_ALLOC_LOW) {
        mprPrintfError(NULL, "Memory request for %d bytes exceeds memory red-line\n", size);
        mprPrintfError(NULL, "Total memory used %d\n", mprGetUsedMemory());
    }
}


void mprNop(void *ptr) {}

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
