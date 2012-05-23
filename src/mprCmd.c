/* 
    mprCmd.c - Run external commands

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/******************************* Forward Declarations *************************/

static int blendEnv(MprCmd *cmd, cchar **env, int flags);
static void closeFiles(MprCmd *cmd);
static ssize cmdCallback(MprCmd *cmd, int channel, void *data);
static int makeChannel(MprCmd *cmd, int index);
static int makeCmdIO(MprCmd *cmd);
static void manageCmdService(MprCmdService *cmd, int flags);
static void manageCmd(MprCmd *cmd, int flags);
static void reapCmd(MprCmd *cmd, MprSignal *sp);
static void resetCmd(MprCmd *cmd);
static int sanitizeArgs(MprCmd *cmd, int argc, cchar **argv, cchar **env, int flags);
static int startProcess(MprCmd *cmd);
static void stdinCallback(MprCmd *cmd, MprEvent *event);
static void stdoutCallback(MprCmd *cmd, MprEvent *event);
static void stderrCallback(MprCmd *cmd, MprEvent *event);
static void vxCmdManager(MprCmd *cmd);
#if BIT_WIN_LIKE
static cchar *makeWinEnvBlock(MprCmd *cmd);
#endif

#if VXWORKS
typedef int (*MprCmdTaskFn)(int argc, char **argv, char **envp);
static void cmdTaskEntry(char *program, MprCmdTaskFn entry, int cmdArg);
#endif

/*
    Cygwin process creation is not thread-safe (1.7)
 */
#if CYGWIN
    #define slock(cmd) mprLock(MPR->cmdService->mutex)
    #define sunlock(cmd) mprUnlock(MPR->cmdService->mutex)
#else
    #define slock(cmd) 
    #define sunlock(cmd) 
#endif

/************************************* Code ***********************************/

MprCmdService *mprCreateCmdService()
{
    MprCmdService   *cs;

    if ((cs = (MprCmdService*) mprAllocObj(MprCmd, manageCmdService)) == 0) {
        return 0;
    }
    cs->cmds = mprCreateList(0, MPR_LIST_STATIC_VALUES);
    cs->mutex = mprCreateLock();
    return cs;
}


void mprStopCmdService()
{
    mprClearList(MPR->cmdService->cmds);
}


static void manageCmdService(MprCmdService *cs, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(cs->cmds);
        mprMark(cs->mutex);

    } else if (flags & MPR_MANAGE_FREE) {
        mprStopCmdService();
        cs->mutex = 0;
    }
}


MprCmd *mprCreateCmd(MprDispatcher *dispatcher)
{
    MprCmd          *cmd;
    MprCmdFile      *files;
    int             i;
    
    if ((cmd = mprAllocObj(MprCmd, manageCmd)) == 0) {
        return 0;
    }
#if UNUSED && KEEP
    cmd->timeoutPeriod = MPR_TIMEOUT_CMD;
    cmd->timestamp = mprGetTime();
#endif
    cmd->forkCallback = (MprForkCallback) closeFiles;
    cmd->dispatcher = dispatcher ? dispatcher : MPR->dispatcher;
    cmd->status = -1;

#if VXWORKS
    cmd->startCond = semCCreate(SEM_Q_PRIORITY, SEM_EMPTY);
    cmd->exitCond = semCCreate(SEM_Q_PRIORITY, SEM_EMPTY);
#endif
    files = cmd->files;
    for (i = 0; i < MPR_CMD_MAX_PIPE; i++) {
        files[i].clientFd = -1;
        files[i].fd = -1;
    }
    cmd->mutex = mprCreateLock();
    mprAddItem(MPR->cmdService->cmds, cmd);
    return cmd;
}


static void manageCmd(MprCmd *cmd, int flags)
{
    int             i;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(cmd->program);
        mprMark(cmd->makeArgv);
        mprMark(cmd->defaultEnv);
        mprMark(cmd->env);
        mprMark(cmd->dir);
        for (i = 0; i < MPR_CMD_MAX_PIPE; i++) {
            mprMark(cmd->files[i].name);
        }
        for (i = 0; i < MPR_CMD_MAX_PIPE; i++) {
            mprMark(cmd->handlers[i]);
        }
        mprMark(cmd->dispatcher);
        mprMark(cmd->callbackData);
        mprMark(cmd->signal);
        mprMark(cmd->forkData);
        mprMark(cmd->stdoutBuf);
        mprMark(cmd->stderrBuf);
        mprMark(cmd->userData);
        mprMark(cmd->mutex);
        mprMark(cmd->searchPath);
#if BIT_WIN_LIKE
        mprMark(cmd->command);
        mprMark(cmd->arg0);
#endif

    } else if (flags & MPR_MANAGE_FREE) {
        resetCmd(cmd);
        vxCmdManager(cmd);
        if (cmd->signal) {
            mprRemoveSignalHandler(cmd->signal);
            cmd->signal = 0;
        }
        mprRemoveItem(MPR->cmdService->cmds, cmd);
    }
}


static void vxCmdManager(MprCmd *cmd)
{
#if VXWORKS
    MprCmdFile      *files;
    int             i;

    if (cmd->startCond) {
        semDelete(cmd->startCond);
    }
    if (cmd->exitCond) {
        semDelete(cmd->exitCond);
    }
    files = cmd->files;
    for (i = 0; i < MPR_CMD_MAX_PIPE; i++) {
        if (files[i].name) {
            DEV_HDR *dev;
#if _WRS_VXWORKS_MAJOR >= 6
            cchar   *tail;
#else
            char    *tail;
#endif
            if ((dev = iosDevFind(files[i].name, &tail)) != NULL) {
                iosDevDelete(dev);
            }
        }
    }
#endif
}


void mprDestroyCmd(MprCmd *cmd)
{
    mprAssert(cmd);
    resetCmd(cmd);
    if (cmd->signal) {
        mprRemoveSignalHandler(cmd->signal);
        cmd->signal = 0;
    }
}


static void resetCmd(MprCmd *cmd)
{
    MprCmdFile      *files;
    int             i;

    mprAssert(cmd);
    files = cmd->files;
    for (i = 0; i < MPR_CMD_MAX_PIPE; i++) {
        if (cmd->handlers[i]) {
            mprRemoveWaitHandler(cmd->handlers[i]);
            cmd->handlers[i] = 0;
        }
        if (files[i].clientFd >= 0) {
            close(files[i].clientFd);
            files[i].clientFd = -1;
        }
        if (files[i].fd >= 0) {
            close(files[i].fd);
            files[i].fd = -1;
        }
    }
    cmd->eofCount = 0;
    cmd->complete = 0;
    cmd->status = -1;

    if (cmd->pid && !(cmd->flags & MPR_CMD_DETACH)) {
        mprStopCmd(cmd, -1);
        reapCmd(cmd, 0);
        cmd->pid = 0;
    }
}


void mprDisconnectCmd(MprCmd *cmd)
{
    int     i;

    mprAssert(cmd);

    for (i = 0; i < MPR_CMD_MAX_PIPE; i++) {
        if (cmd->handlers[i]) {
            mprRemoveWaitHandler(cmd->handlers[i]);
            cmd->handlers[i] = 0;
        }
    }
}


/*
    Close a command channel. Must be able to be called redundantly.
 */
void mprCloseCmdFd(MprCmd *cmd, int channel)
{
    mprAssert(cmd);
    mprAssert(0 <= channel && channel <= MPR_CMD_MAX_PIPE);

    if (cmd->handlers[channel]) {
        mprRemoveWaitHandler(cmd->handlers[channel]);
        cmd->handlers[channel] = 0;
    }
    if (cmd->files[channel].fd != -1) {
        close(cmd->files[channel].fd);
        cmd->files[channel].fd = -1;
#if BIT_WIN_LIKE
        cmd->files[channel].handle = 0;
#endif
        if (channel != MPR_CMD_STDIN) {
            cmd->eofCount++;
            if (cmd->eofCount >= cmd->requiredEof) {
#if VXWORKS
                reapCmd(cmd, 0);
#endif
                if (cmd->pid == 0) {
                    cmd->complete = 1;
                }
            }
        }
    }
    mprLog(6, "Close channel %d eof %d/%d, pid %d", channel, cmd->eofCount, cmd->requiredEof, cmd->pid);
}


void mprFinalizeCmd(MprCmd *cmd)
{
    mprLog(6, "mprFinalizeCmd");
    mprAssert(cmd);
    mprCloseCmdFd(cmd, MPR_CMD_STDIN);
}


int mprIsCmdComplete(MprCmd *cmd)
{
    return cmd->complete;
}


/*
    Run a simple blocking command. See arg usage below in mprRunCmdV.
 */
int mprRunCmd(MprCmd *cmd, cchar *command, cchar **envp, char **out, char **err, MprTime timeout, int flags)
{
    cchar   **argv;
    int     argc;

    mprAssert(cmd);
    if ((argc = mprMakeArgv(command, &argv, 0)) < 0 || argv == 0) {
        return 0;
    }
    cmd->makeArgv = argv;
    return mprRunCmdV(cmd, argc, argv, envp, out, err, timeout, flags);
}


/*
    Env is an array of "KEY=VALUE" strings. Null terminated
    The user must preserve the environment. This module does not clone the environment and uses the supplied reference.
 */
void mprSetCmdDefaultEnv(MprCmd *cmd, cchar **env)
{
    /* WARNING: defaultEnv is not cloned, but is marked */
    cmd->defaultEnv = env;
}


void mprSetCmdSearchPath(MprCmd *cmd, cchar *search)
{
    cmd->searchPath = sclone(search);
}


/*
    This routine runs a command and waits for its completion. Stdoutput and Stderr are returned in *out and *err 
    respectively. The command returns the exit status of the command.
    Valid flags are:
        MPR_CMD_NEW_SESSION     Create a new session on Unix
        MPR_CMD_SHOW            Show the commands window on Windows
        MPR_CMD_IN              Connect to stdin
 */
int mprRunCmdV(MprCmd *cmd, int argc, cchar **argv, cchar **envp, char **out, char **err, MprTime timeout, int flags)
{
    int     rc, status;

    mprAssert(cmd);
    if (err) {
        *err = 0;
        flags |= MPR_CMD_ERR;
    } else {
        flags &= ~MPR_CMD_ERR;
    }
    if (out) {
        *out = 0;
        flags |= MPR_CMD_OUT;
    } else {
        flags &= ~MPR_CMD_OUT;
    }
    if (flags & MPR_CMD_OUT) {
        cmd->stdoutBuf = mprCreateBuf(MPR_BUFSIZE, -1);
    }
    if (flags & MPR_CMD_ERR) {
        cmd->stderrBuf = mprCreateBuf(MPR_BUFSIZE, -1);
    }
    mprSetCmdCallback(cmd, cmdCallback, NULL);
    rc = mprStartCmd(cmd, argc, argv, envp, flags);

    /*
        Close the pipe connected to the client's stdin
     */
    if (cmd->files[MPR_CMD_STDIN].fd >= 0) {
        mprFinalizeCmd(cmd);
    }
    if (rc < 0) {
        if (err) {
            if (rc == MPR_ERR_CANT_ACCESS) {
                *err = sfmt("Can't access command %s", cmd->program);
            } else if (MPR_ERR_CANT_OPEN) {
                *err = sfmt("Can't open standard I/O for command %s", cmd->program);
            } else if (rc == MPR_ERR_CANT_CREATE) {
                *err = sfmt("Can't create process for %s", cmd->program);
            }
        }
        return rc;
    }
    if (cmd->flags & MPR_CMD_DETACH) {
        return 0;
    }
    if (mprWaitForCmd(cmd, timeout) < 0) {
        return MPR_ERR_NOT_READY;
    }
    if ((status = mprGetCmdExitStatus(cmd)) < 0) {
        return MPR_ERR;
    }
    if (err && flags & MPR_CMD_ERR) {
        *err = mprGetBufStart(cmd->stderrBuf);
    }
    if (out && flags & MPR_CMD_OUT) {
        *out = mprGetBufStart(cmd->stdoutBuf);
    }
    return status;
}


static void addCmdHandlers(MprCmd *cmd)
{
    int     stdinFd, stdoutFd, stderrFd;
  
    stdinFd = cmd->files[MPR_CMD_STDIN].fd; 
    stdoutFd = cmd->files[MPR_CMD_STDOUT].fd; 
    stderrFd = cmd->files[MPR_CMD_STDERR].fd; 

    if (stdinFd >= 0 && cmd->handlers[MPR_CMD_STDIN] == 0) {
        cmd->handlers[MPR_CMD_STDIN] = mprCreateWaitHandler(stdinFd, MPR_WRITABLE, cmd->dispatcher, stdinCallback, cmd, 0);
    }
    if (stdoutFd >= 0 && cmd->handlers[MPR_CMD_STDOUT] == 0) {
        cmd->handlers[MPR_CMD_STDOUT] = mprCreateWaitHandler(stdoutFd, MPR_READABLE, cmd->dispatcher, stdoutCallback, cmd,0);
    }
    if (stderrFd >= 0 && cmd->handlers[MPR_CMD_STDERR] == 0) {
        cmd->handlers[MPR_CMD_STDERR] = mprCreateWaitHandler(stderrFd, MPR_READABLE, cmd->dispatcher, stderrCallback, cmd,0);
    }
}


/*
    Start the command to run (stdIn and stdOut are named from the client's perspective). This is the lower-level way to 
    run a command. The caller needs to do code like mprRunCmd() themselves to wait for completion and to send/receive data.
    The routine does not wait. Callers must call mprWaitForCmd to wait for the command to complete.
 */
int mprStartCmd(MprCmd *cmd, int argc, cchar **argv, cchar **envp, int flags)
{
    MprPath     info;
    cchar       *program, *search, *pair;
    int         rc, next, i;

    mprAssert(cmd);
    mprAssert(argv);
    mprAssert(argc > 0);

    if (argc <= 0 || argv == NULL || argv[0] == NULL) {
        return MPR_ERR_BAD_STATE;
    }
    resetCmd(cmd);
    program = argv[0];
    cmd->program = sclone(program);
    cmd->flags = flags;

    if (sanitizeArgs(cmd, argc, argv, envp, flags) < 0) {
        return MPR_ERR_MEMORY;
    }
    if (envp == 0) {
        envp = cmd->defaultEnv;
    }
    if (blendEnv(cmd, envp, flags) < 0) {
        return MPR_ERR_MEMORY;
    }
    search = cmd->searchPath ? cmd->searchPath : MPR->pathEnv;
    if ((program = mprSearchPath(program, MPR_SEARCH_EXE, search, NULL)) == 0) {
        mprLog(1, "cmd: can't access %s, errno %d", cmd->program, mprGetOsError());
        return MPR_ERR_CANT_ACCESS;
    }
    cmd->program = cmd->argv[0] = program;

    if (mprGetPathInfo(program, &info) == 0 && info.isDir) {
        mprLog(1, "cmd: program \"%s\", is a directory", program);
        return MPR_ERR_CANT_ACCESS;
    }
    mprLog(4, "mprStartCmd %s", cmd->program);
    for (i = 0; i < cmd->argc; i++) {
        mprLog(4, "    arg[%d]: %s", i, cmd->argv[i]);
    }
    for (ITERATE_ITEMS(cmd->env, pair, next)) {
        mprLog(4, "    env[%d]: %s", next, pair);
    }
    slock(cmd);
    if (makeCmdIO(cmd) < 0) {
        sunlock(cmd);
        return MPR_ERR_CANT_OPEN;
    }
    /*
        Determine how many end-of-files will be seen when the child dies
     */
    cmd->requiredEof = 0;
    if (cmd->flags & MPR_CMD_OUT) {
        cmd->requiredEof++;
    }
    if (cmd->flags & MPR_CMD_ERR) {
        cmd->requiredEof++;
    }
    addCmdHandlers(cmd);
    rc = startProcess(cmd);
    sunlock(cmd);
    return rc;
}


static int makeCmdIO(MprCmd *cmd)
{
    int     rc;

    rc = 0;
    if (cmd->flags & MPR_CMD_IN) {
        rc += makeChannel(cmd, MPR_CMD_STDIN);
    }
    if (cmd->flags & MPR_CMD_OUT) {
        rc += makeChannel(cmd, MPR_CMD_STDOUT);
    }
    if (cmd->flags & MPR_CMD_ERR) {
        rc += makeChannel(cmd, MPR_CMD_STDERR);
    }
    return rc;
}


/*
    Stop the command
 */
int mprStopCmd(MprCmd *cmd, int signal)
{
    mprLog(7, "cmd: stop");

    if (signal < 0) {
        signal = SIGTERM;
    }
    cmd->stopped = 1;
    if (cmd->pid) {
#if BIT_WIN_LIKE
        return TerminateProcess(cmd->process, 2) == 0;
#elif VXWORKS
        return taskDelete(cmd->pid);
#else
        return kill(cmd->pid, signal);
#endif
    }
    return 0;
}


/*
    Do non-blocking I/O - except on windows - will block
 */
ssize mprReadCmd(MprCmd *cmd, int channel, char *buf, ssize bufsize)
{
#if BIT_WIN_LIKE
    int     rc, count;
    /*
        Need to detect EOF in windows. Pipe always in blocking mode, but reads block even with no one on the other end.
     */
    mprAssert(cmd->files[channel].handle);
    rc = PeekNamedPipe(cmd->files[channel].handle, NULL, 0, NULL, &count, NULL);
    if (rc > 0 && count > 0) {
        return read(cmd->files[channel].fd, buf, (uint) bufsize);
    } 
    if (cmd->process == 0 || WaitForSingleObject(cmd->process, 0) == WAIT_OBJECT_0) {
        /* Process has exited - EOF */
        return 0;
    }
    /* This maps to EAGAIN */
    SetLastError(WSAEWOULDBLOCK);
    return -1;

#elif VXWORKS
    /*
        Only needed when using non-blocking I/O
     */
    int     rc;

    rc = read(cmd->files[channel].fd, buf, bufsize);

    /*
        VxWorks can't signal EOF on non-blocking pipes. Need a pattern indicator.
     */
    if (rc == MPR_CMD_VXWORKS_EOF_LEN && strncmp(buf, MPR_CMD_VXWORKS_EOF, MPR_CMD_VXWORKS_EOF_LEN) == 0) {
        /* EOF */
        return 0;

    } else if (rc == 0) {
        rc = -1;
        errno = EAGAIN;
    }
    return rc;

#else
    mprAssert(cmd->files[channel].fd >= 0);
    return read(cmd->files[channel].fd, buf, bufsize);
#endif
}


/*
    Do non-blocking I/O - except on windows - will block
 */
ssize mprWriteCmd(MprCmd *cmd, int channel, char *buf, ssize bufsize)
{
#if BIT_WIN_LIKE
    /*
        No waiting. Use this just to check if the process has exited and thus EOF on the pipe.
     */
    if (cmd->pid == 0 || WaitForSingleObject(cmd->process, 0) == WAIT_OBJECT_0) {
        return -1;
    }
#endif
    return write(cmd->files[channel].fd, buf, (wsize) bufsize);
}


void mprEnableCmdEvents(MprCmd *cmd, int channel)
{
    int mask = (channel == MPR_CMD_STDIN) ? MPR_WRITABLE : MPR_READABLE;
    if (cmd->handlers[channel]) {
        mprWaitOn(cmd->handlers[channel], mask);
    }
}


void mprDisableCmdEvents(MprCmd *cmd, int channel)
{
    if (cmd->handlers[channel]) {
        mprWaitOn(cmd->handlers[channel], 0);
    }
}


#if BIT_WIN_LIKE && !WINCE
/*
    Windows only routine to wait for I/O on the channels to the gateway and the child process.
    NamedPipes can't use WaitForMultipleEvents (can use overlapped I/O)
    WARNING: this should not be called from a dispatcher other than cmd->dispatcher. If so, then the calls to
    mprWaitForEvent may occur after the event has been processed.
 */
static void waitForWinEvent(MprCmd *cmd, MprTime timeout)
{
    MprTime     mark, remaining, delay;
    int         i, rc, nbytes;

    mark = mprGetTime();
    if (cmd->stopped) {
        timeout = 0;
    }
    for (i = MPR_CMD_STDOUT; i < MPR_CMD_MAX_PIPE; i++) {
        if (cmd->files[i].handle) {
            rc = PeekNamedPipe(cmd->files[i].handle, NULL, 0, NULL, &nbytes, NULL);
            if (rc && nbytes > 0 || cmd->process == 0) {
                mprQueueIOEvent(cmd->handlers[i]);
                mprWaitForEvent(cmd->dispatcher, timeout);
                return;
            }
        }
    }
    if (cmd->files[MPR_CMD_STDIN].handle) {
        /* Not finalized */
        mprQueueIOEvent(cmd->handlers[MPR_CMD_STDIN]);
        mprWaitForEvent(cmd->dispatcher, timeout);
        return;
    }
    if (cmd->process) {
        delay = (cmd->eofCount == cmd->requiredEof && cmd->files[MPR_CMD_STDIN].handle == 0) ? timeout : 0;
        mprYield(MPR_YIELD_STICKY);
        if (WaitForSingleObject(cmd->process, (DWORD) delay) == WAIT_OBJECT_0) {
            mprResetYield();
            reapCmd(cmd, 0);
            return;
        }
        mprResetYield();
        if (cmd->eofCount == cmd->requiredEof) {
            remaining = mprGetRemainingTime(mark, timeout);
            mprYield(MPR_YIELD_STICKY);
            rc = WaitForSingleObject(cmd->process, (DWORD) remaining);
            mprResetYield();
            if (rc == WAIT_OBJECT_0) {
                reapCmd(cmd, 0);
                return;
            }
            mprError("Error waiting CGI I/O, error %d", mprGetOsError());
        }
    }
    /* Stop busy waiting */
    mprSleep(10);
}
#endif


/*
    Wait for a command to complete. Return 0 if the command completed, otherwise it will return MPR_ERR_TIMEOUT. 
 */
int mprWaitForCmd(MprCmd *cmd, MprTime timeout)
{
    MprTime     expires, remaining;

    mprAssert(cmd);

    if (timeout < 0) {
        timeout = MAXINT;
    }
    if (mprGetDebugMode()) {
        timeout = MAXINT;
    }
    if (cmd->stopped) {
        timeout = 0;
    }
    expires = mprGetTime() + timeout;
    remaining = timeout;

    /* Add root to allow callers to use mprRunCmd without first managing the cmd */
    mprAddRoot(cmd);

    while (!cmd->complete && remaining > 0) {
        if (mprShouldAbortRequests()) {
            break;
        }
#if BIT_WIN_LIKE
        waitForWinEvent(cmd, remaining);
#else
        mprWaitForEvent(cmd->dispatcher, remaining);
#endif
        remaining = (expires - mprGetTime());
    }
    mprRemoveRoot(cmd);
    if (cmd->pid) {
        return MPR_ERR_TIMEOUT;
    }
    mprLog(6, "cmd: waitForChild: status %d", cmd->status);
    return 0;
}


/*
    Gather the child's exit status. 
    WARNING: this may be called with a false-positive, ie. SIGCHLD will get invoked for all process deaths and not just
    when this cmd has completed.
 */
static void reapCmd(MprCmd *cmd, MprSignal *sp)
{
    ssize   got, nbytes;
    int     status, rc;

    mprLog(6, "reapCmd CHECK pid %d, eof %d, required %d\n", cmd->pid, cmd->eofCount, cmd->requiredEof);
    
    status = 0;
    if (cmd->pid == 0) {
        return;
    }
#if BIT_UNIX_LIKE
    if ((rc = waitpid(cmd->pid, &status, WNOHANG | __WALL)) < 0) {
        mprLog(6, "waitpid failed for pid %d, errno %d", cmd->pid, errno);

    } else if (rc == cmd->pid) {
        mprLog(6, "waitpid pid %d, thread %s", cmd->pid, mprGetCurrentThreadName());
        if (!WIFSTOPPED(status)) {
            if (WIFEXITED(status)) {
                cmd->status = WEXITSTATUS(status);
                mprLog(6, "waitpid exited pid %d, status %d", cmd->pid, cmd->status);
            } else if (WIFSIGNALED(status)) {
                cmd->status = WTERMSIG(status);
            } else {
                mprLog(7, "waitpid FUNNY pid %d, errno %d", cmd->pid, errno);
            }
            cmd->pid = 0;
            mprAssert(cmd->signal);
            mprRemoveSignalHandler(cmd->signal);
            cmd->signal = 0;
        } else {
            mprLog(7, "waitpid ELSE pid %d, errno %d", cmd->pid, errno);
        }
    } else {
        mprLog(6, "waitpid still running pid %d, thread %s", cmd->pid, mprGetCurrentThreadName());
    }
#endif
#if VXWORKS
    /*
        The command exit status (cmd->status) is set in cmdTaskEntry
     */
    if (!cmd->stopped) {
        if (semTake(cmd->exitCond, MPR_TIMEOUT_STOP_TASK) != OK) {
            mprError("cmd: child %s did not exit, errno %d", cmd->program);
            return;
        }
    }
    semDelete(cmd->exitCond);
    cmd->exitCond = 0;
    cmd->pid = 0;
    rc = 0;
#endif
#if BIT_WIN_LIKE
    if (GetExitCodeProcess(cmd->process, (ulong*) &status) == 0) {
        mprLog(3, "cmd: GetExitProcess error");
        return;
    }
    if (status != STILL_ACTIVE) {
        cmd->status = status;
        rc = CloseHandle(cmd->process);
        mprAssert(rc != 0);
        rc = CloseHandle(cmd->thread);
        mprAssert(rc != 0);
        cmd->process = 0;
        cmd->thread = 0;
        cmd->pid = 0;
    }
#endif
    if (cmd->pid == 0) {
        if (cmd->eofCount >= cmd->requiredEof) {
            cmd->complete = 1;
        }
        if (cmd->callback) {
            (cmd->callback)(cmd, -1, cmd->callbackData);
        }
        mprLog(6, "Cmd reaped: status %d, pid %d, eof %d / %d\n", cmd->status, cmd->pid, cmd->eofCount, cmd->requiredEof);

        if (cmd->callback) {
            /*
                Read outstanding data
             */  
            while (cmd->eofCount < cmd->requiredEof) {
                got = 0;
                if (cmd->files[MPR_CMD_STDERR].fd >= 0) {
                    if ((nbytes = (cmd->callback)(cmd, MPR_CMD_STDERR, cmd->callbackData)) > 0) {
                        got += nbytes;
                    }
                }
                if (cmd->files[MPR_CMD_STDOUT].fd >= 0) {
                    if ((nbytes = (cmd->callback)(cmd, MPR_CMD_STDOUT, cmd->callbackData)) > 0) {
                        got += nbytes;
                    }
                }
                if (got <= 0) {
                    break;
                }
            }
            if (cmd->files[MPR_CMD_STDERR].fd >= 0) {
                mprCloseCmdFd(cmd, MPR_CMD_STDERR);
            }
            if (cmd->files[MPR_CMD_STDOUT].fd >= 0) {
                mprCloseCmdFd(cmd, MPR_CMD_STDOUT);
            }
#if UNUSED && DONT_USE && KEEP
            if (cmd->eofCount != cmd->requiredEof) {
                mprLog(0, "reapCmd: insufficient EOFs %d %d, complete %d", cmd->eofCount, cmd->requiredEof, cmd->complete);
            }
            mprAssert(cmd->eofCount == cmd->requiredEof);
            mprAssert(cmd->complete);
#endif
        }
    }
}


/*
    Default callback routine for the mprRunCmd routines. Uses may supply their own callback instead of this routine. 
    The callback is run whenever there is I/O to read/write to the CGI gateway.
 */
static ssize cmdCallback(MprCmd *cmd, int channel, void *data)
{
    MprBuf      *buf;
    ssize       len, space;

    /*
        Note: stdin, stdout and stderr are named from the client's perspective
     */
    buf = 0;
    switch (channel) {
    case MPR_CMD_STDIN:
        return 0;
    case MPR_CMD_STDOUT:
        buf = cmd->stdoutBuf;
        break;
    case MPR_CMD_STDERR:
        buf = cmd->stderrBuf;
        break;
    default:
        /* Child death notification */
        return 0;
    }
    /*
        Read and aggregate the result into a single string
     */
    space = mprGetBufSpace(buf);
    if (space < (MPR_BUFSIZE / 4)) {
        if (mprGrowBuf(buf, MPR_BUFSIZE) < 0) {
            mprCloseCmdFd(cmd, channel);
            return 0;
        }
        space = mprGetBufSpace(buf);
    }
    len = mprReadCmd(cmd, channel, mprGetBufEnd(buf), space);
    mprLog(6, "cmdCallback channel %d, read len %d, pid %d, eof %d/%d", channel, len, cmd->pid, cmd->eofCount, 
        cmd->requiredEof);
    if (len <= 0) {
        if (len == 0 || (len < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK))) {
            mprCloseCmdFd(cmd, channel);
            return len;
        }
    } else {
        mprAdjustBufEnd(buf, len);
    }
    mprAddNullToBuf(buf);
    mprEnableCmdEvents(cmd, channel);
    return len;
}


static void stdinCallback(MprCmd *cmd, MprEvent *event)
{
    if (cmd->callback && cmd->files[MPR_CMD_STDIN].fd >= 0) {
        (cmd->callback)(cmd, MPR_CMD_STDIN, cmd->callbackData);
    }
}


static void stdoutCallback(MprCmd *cmd, MprEvent *event)
{
    /*
        reapCmd can consume data from the client and close the fd
     */
    if (cmd->callback && cmd->files[MPR_CMD_STDOUT].fd >= 0) {
        (cmd->callback)(cmd, MPR_CMD_STDOUT, cmd->callbackData);
    }
}


static void stderrCallback(MprCmd *cmd, MprEvent *event)
{
    /*
        reapCmd can consume data from the client and close the fd
     */
    if (cmd->callback && cmd->files[MPR_CMD_STDERR].fd >= 0) {
        (cmd->callback)(cmd, MPR_CMD_STDERR, cmd->callbackData);
    }
}


void mprSetCmdCallback(MprCmd *cmd, MprCmdProc proc, void *data)
{
    cmd->callback = proc;
    cmd->callbackData = data;
}


int mprGetCmdExitStatus(MprCmd *cmd)
{
    mprAssert(cmd);

    if (cmd->pid == 0) {
        return cmd->status;
    }
    return MPR_ERR_NOT_READY;
}


bool mprIsCmdRunning(MprCmd *cmd)
{
    return cmd->pid > 0;
}


void mprSetCmdTimeout(MprCmd *cmd, MprTime timeout)
{
    mprAssert(0);
#if UNUSED
    cmd->timeoutPeriod = timeout;
#endif
}


int mprGetCmdFd(MprCmd *cmd, int channel) 
{ 
    return cmd->files[channel].fd; 
}


MprBuf *mprGetCmdBuf(MprCmd *cmd, int channel)
{
    return (channel == MPR_CMD_STDOUT) ? cmd->stdoutBuf : cmd->stderrBuf;
}


void mprSetCmdDir(MprCmd *cmd, cchar *dir)
{
#if VXWORKS
    mprError("WARNING: Setting working directory on VxWorks is global: %s", dir);
#else
    mprAssert(dir && *dir);

    cmd->dir = sclone(dir);
#endif
}


#if BIT_WIN_LIKE
static int sortEnv(char **str1, char **str2)
{
    cchar    *s1, *s2;
    int     c1, c2;

    for (s1 = *str1, s2 = *str2; *s1 && *s2; s1++, s2++) {
        c1 = tolower((uchar) *s1);
        c2 = tolower((uchar) *s2);
        if (c1 < c2) {
            return -1;
        } else if (c1 > c2) {
            return 1;
        }
    }
    if (*s2) {
        return -1;
    } else if (*s1) {
        return 1;
    }
    return 0;
}
#endif


/*
    Match two environment keys up to the '='
 */
static bool matchEnvKey(cchar *s1, cchar *s2) 
{
    for (; *s1 && *s2; s1++, s2++) {
        if (*s1 != *s2) {
            break;
        } else if (*s1 == '=') {
            return 1;
        }
    }
    return 0;
}


static int blendEnv(MprCmd *cmd, cchar **env, int flags)
{
    cchar       **ep, *prior;
    int         next;

    cmd->env = 0;

    if ((cmd->env = mprCreateList(128, MPR_LIST_STATIC_VALUES)) == 0) {
        return MPR_ERR_MEMORY;
    }
#if !VXWORKS
    /*
        Add prior environment to the list
     */
    if (!(flags & MPR_CMD_EXACT_ENV)) {
        for (ep = (cchar**) environ; ep && *ep; ep++) {
            mprAddItem(cmd->env, *ep);
        }
    }
#endif
    /*
        Add new env keys. Detect and overwrite duplicates
     */
    for (ep = env; ep && *ep; ep++) {
        for (ITERATE_ITEMS(cmd->env, prior, next)) {
            if (matchEnvKey(*ep, prior)) {
                mprSetItem(cmd->env, next - 1, *ep);
                break;
            }
        }
        if (prior == 0) {
            mprAddItem(cmd->env, *ep);
        }
    }
#if BIT_WIN_LIKE
    /*
        Windows requires a caseless sort with two trailing nulls
     */
    mprSortList(cmd->env, sortEnv);
#endif
    mprAddItem(cmd->env, NULL);
    return 0;
}


#if BIT_WIN_LIKE
static cchar *makeWinEnvBlock(MprCmd *cmd)
{
    char    *item, *dp, *ep, *env;
    ssize   len;
    int     next;

    for (len = 2, ITERATE_ITEMS(cmd->env, item, next)) {
        len += slen(item) + 1;
    }
    if ((env = mprAlloc(len)) == 0) {
        return 0;
    }
    ep = &env[len];
    dp = env;
    for (ITERATE_ITEMS(cmd->env, item, next)) {
        strcpy(dp, item);
        dp += slen(item) + 1;
    }
    /* Windows requires two nulls */
    *dp++ = '\0';
    *dp++ = '\0';
    mprAssert(dp <= ep);
    return env;
}
#endif


/*
    Sanitize args. Convert "/" to "\" and converting '\r' and '\n' to spaces, quote all args and put the program as argv[0].
 */
static int sanitizeArgs(MprCmd *cmd, int argc, cchar **argv, cchar **env, int flags)
{
#if BIT_UNIX_LIKE || VXWORKS
    cmd->argv = argv;
    cmd->argc = argc;
#endif

#if BIT_WIN_LIKE
    /*
        WARNING: If starting a program compiled with Cygwin, there is a bug in Cygwin's parsing of the command
        string where embedded quotes are parsed incorrectly by the Cygwin CRT runtime. If an arg starts with a 
        drive spec, embedded backquoted quotes will be stripped and the backquote will be passed in. Windows CRT 
        handles this correctly.  For example:  
            ./args "c:/path \"a b\"
            Cygwin will parse as  argv[1] == c:/path \a \b
            Windows will parse as argv[1] == c:/path "a b"
     */
    cchar       *saveArg0, **ap, *start, *cp;
    char        *pp, *program, *dp, *localArgv[2];
    ssize       len;
    int         quote;

    mprAssert(argc > 0 && argv[0] != NULL);

    cmd->argv = argv;
    cmd->argc = argc;

    program = cmd->arg0 = mprAlloc(slen(argv[0]) * 2 + 1);
    strcpy(program, argv[0]);

    for (pp = program; *pp; pp++) {
        if (*pp == '/') {
            *pp = '\\';
        } else if (*pp == '\r' || *pp == '\n') {
            *pp = ' ';
        }
    }
    if (*program == '\"') {
        if ((pp = strrchr(++program, '"')) != 0) {
            *pp = '\0';
        }
    }
    if (argv == 0) {
        argv = localArgv;
        argv[1] = 0;
        saveArg0 = program;
    } else {
        saveArg0 = argv[0];
    }
    /*
        Set argv[0] to the program name while creating the command line. Restore later.
     */
    argv[0] = program;
    argc = 0;
    for (len = 0, ap = argv; *ap; ap++) {
        len += (slen(*ap) * 2) + 1 + 2;         /* Space and possible quotes and worst case backquoting */
        argc++;
    }
    cmd->command = mprAlloc(len + 1);
    cmd->command[len] = '\0';
    
    /*
        Add quotes around all args that have spaces and backquote [", ', \\]
        Example:    ["showColors", "red", "light blue", "Can't \"render\""]
        Becomes:    "showColors" "red" "light blue" "Can't \"render\""
     */
    dp = cmd->command;
    for (ap = &argv[0]; *ap; ) {
        start = cp = *ap;
        quote = '"';
        if (strchr(cp, ' ') != 0 && cp[0] != quote) {
            for (*dp++ = quote; *cp; ) {
                if (*cp == quote && !(cp > start && cp[-1] == '\\')) {
                    *dp++ = '\\';
                }
                *dp++ = *cp++;
            }
            *dp++ = quote;
        } else {
            strcpy(dp, cp);
            dp += strlen(cp);
        }
        if (*++ap) {
            *dp++ = ' ';
        }
    }
    *dp = '\0';
    argv[0] = saveArg0;
    mprLog(5, "Windows command line: %s", cmd->command);
#endif /* BIT_WIN_LIKE */
    return 0;
}


#if BIT_WIN_LIKE
static int startProcess(MprCmd *cmd)
{
    PROCESS_INFORMATION procInfo;
    STARTUPINFO         startInfo;
    cchar               *envBlock;
    int                 err;

    memset(&startInfo, 0, sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);

    startInfo.dwFlags = STARTF_USESHOWWINDOW;
    if (cmd->flags & MPR_CMD_SHOW) {
        startInfo.wShowWindow = SW_SHOW;
    } else {
        startInfo.wShowWindow = SW_HIDE;
    }
    startInfo.dwFlags |= STARTF_USESTDHANDLES;

    if (cmd->flags & MPR_CMD_IN) {
        if (cmd->files[MPR_CMD_STDIN].clientFd > 0) {
            startInfo.hStdInput = (HANDLE) _get_osfhandle(cmd->files[MPR_CMD_STDIN].clientFd);
        }
    } else {
        startInfo.hStdInput = (HANDLE) _get_osfhandle((int) fileno(stdin));
    }
    if (cmd->flags & MPR_CMD_OUT) {
        if (cmd->files[MPR_CMD_STDOUT].clientFd > 0) {
            startInfo.hStdOutput = (HANDLE)_get_osfhandle(cmd->files[MPR_CMD_STDOUT].clientFd);
        }
    } else {
        startInfo.hStdOutput = (HANDLE)_get_osfhandle((int) fileno(stdout));
    }
    if (cmd->flags & MPR_CMD_ERR) {
        if (cmd->files[MPR_CMD_STDERR].clientFd > 0) {
            startInfo.hStdError = (HANDLE) _get_osfhandle(cmd->files[MPR_CMD_STDERR].clientFd);
        }
    } else {
        startInfo.hStdError = (HANDLE) _get_osfhandle((int) fileno(stderr));
    }
    envBlock = makeWinEnvBlock(cmd);
    if (! CreateProcess(0, cmd->command, 0, 0, 1, 0, (char*) envBlock, cmd->dir, &startInfo, &procInfo)) {
        err = mprGetOsError();
        if (err == ERROR_DIRECTORY) {
            mprError("Can't create process: %s, directory %s is invalid", cmd->program, cmd->dir);
        } else {
            mprError("Can't create process: %s, %d", cmd->program, err);
        }
        return MPR_ERR_CANT_CREATE;
    }
    cmd->thread = procInfo.hThread;
    cmd->process = procInfo.hProcess;
    cmd->pid = procInfo.dwProcessId;
    return 0;
}


#if WINCE
//  FUTURE - merge this with WIN
static int makeChannel(MprCmd *cmd, int index)
{
    SECURITY_ATTRIBUTES clientAtt, serverAtt, *att;
    HANDLE              readHandle, writeHandle;
    MprCmdFile          *file;
    char                *path;
    int                 readFd, writeFd;

    memset(&clientAtt, 0, sizeof(clientAtt));
    clientAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
    clientAtt.bInheritHandle = 1;

    /*
        Server fds are not inherited by the child
     */
    memset(&serverAtt, 0, sizeof(serverAtt));
    serverAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
    serverAtt.bInheritHandle = 0;

    file = &cmd->files[index];
    path = mprGetTempPath(cmd, NULL);

    att = (index == MPR_CMD_STDIN) ? &clientAtt : &serverAtt;
    readHandle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, att, OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL,0);
    if (readHandle == INVALID_HANDLE_VALUE) {
        mprError(cmd, "Can't create stdio pipes %s. Err %d\n", path, mprGetOsError());
        return MPR_ERR_CANT_CREATE;
    }
    readFd = (int) (int64) _open_osfhandle((int*) readHandle, 0);

    att = (index == MPR_CMD_STDIN) ? &serverAtt: &clientAtt;
    writeHandle = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, att, OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 0);
    writeFd = (int) _open_osfhandle((int*) writeHandle, 0);

    if (readFd < 0 || writeFd < 0) {
        mprError(cmd, "Can't create stdio pipes %s. Err %d\n", path, mprGetOsError());
        return MPR_ERR_CANT_CREATE;
    }
    if (index == MPR_CMD_STDIN) {
        file->clientFd = readFd;
        file->fd = writeFd;
        file->handle = writeHandle;
    } else {
        file->clientFd = writeFd;
        file->fd = readFd;
        file->handle = readHandle;
    }
    return 0;
}

#else /* !WINCE */
static int makeChannel(MprCmd *cmd, int index)
{
    SECURITY_ATTRIBUTES clientAtt, serverAtt, *att;
    HANDLE              readHandle, writeHandle;
    MprCmdFile          *file;
    MprTime             now;
    char                *pipeName;
    int                 openMode, pipeMode, readFd, writeFd;
    static int          tempSeed = 0;

    memset(&clientAtt, 0, sizeof(clientAtt));
    clientAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
    clientAtt.bInheritHandle = 1;

    /*
        Server fds are not inherited by the child
     */
    memset(&serverAtt, 0, sizeof(serverAtt));
    serverAtt.nLength = sizeof(SECURITY_ATTRIBUTES);
    serverAtt.bInheritHandle = 0;

    file = &cmd->files[index];
    now = ((int) mprGetTime() & 0xFFFF) % 64000;

    lock(MPR->cmdService);
    pipeName = sfmt("\\\\.\\pipe\\MPR_%d_%d_%d.tmp", getpid(), (int) now, ++tempSeed);
    unlock(MPR->cmdService);

    /*
        Pipes are always inbound. The file below is outbound. we swap whether the client or server
        inherits the pipe or file. MPR_CMD_STDIN is the clients input pipe.
        Pipes are blocking since both ends share the same blocking mode. Client must be blocking.
     */
    openMode = PIPE_ACCESS_INBOUND;
    pipeMode = 0;

    att = (index == MPR_CMD_STDIN) ? &clientAtt : &serverAtt;
    readHandle = CreateNamedPipe(pipeName, openMode, pipeMode, 1, 0, 256 * 1024, 1, att);
    if (readHandle == INVALID_HANDLE_VALUE) {
        mprError("Can't create stdio pipes %s. Err %d\n", pipeName, mprGetOsError());
        return MPR_ERR_CANT_CREATE;
    }
    readFd = (int) (int64) _open_osfhandle((long) readHandle, 0);

    att = (index == MPR_CMD_STDIN) ? &serverAtt: &clientAtt;
    writeHandle = CreateFile(pipeName, GENERIC_WRITE, 0, att, OPEN_EXISTING, openMode, 0);
    writeFd = (int) _open_osfhandle((long) writeHandle, 0);

    if (readFd < 0 || writeFd < 0) {
        mprError("Can't create stdio pipes %s. Err %d\n", pipeName, mprGetOsError());
        return MPR_ERR_CANT_CREATE;
    }
    if (index == MPR_CMD_STDIN) {
        file->clientFd = readFd;
        file->fd = writeFd;
        file->handle = writeHandle;
    } else {
        file->clientFd = writeFd;
        file->fd = readFd;
        file->handle = readHandle;
    }
    return 0;
}
#endif /* WINCE */


#elif BIT_UNIX_LIKE
static int makeChannel(MprCmd *cmd, int index)
{
    MprCmdFile      *file;
    int             fds[2];

    file = &cmd->files[index];

    if (pipe(fds) < 0) {
        mprError("Can't create stdio pipes. Err %d", mprGetOsError());
        return MPR_ERR_CANT_CREATE;
    }
    if (index == MPR_CMD_STDIN) {
        file->clientFd = fds[0];        /* read fd */
        file->fd = fds[1];              /* write fd */
    } else {
        file->clientFd = fds[1];        /* write fd */
        file->fd = fds[0];              /* read fd */
    }
    fcntl(file->fd, F_SETFL, fcntl(file->fd, F_GETFL) | O_NONBLOCK);
    mprLog(7, "makeChannel: pipe handles[%d] read %d, write %d", index, fds[0], fds[1]);
    return 0;
}

#elif VXWORKS
static int makeChannel(MprCmd *cmd, int index)
{
    MprCmdFile      *file;
    int             nonBlock;
    static int      tempSeed = 0;

    file = &cmd->files[index];
    file->name = sfmt("/pipe/%s_%d_%d", BIT_PRODUCT, taskIdSelf(), tempSeed++);

    if (pipeDevCreate(file->name, 5, MPR_BUFSIZE) < 0) {
        mprError("Can't create pipes to run %s", cmd->program);
        return MPR_ERR_CANT_OPEN;
    }
    /*
        Open the server end of the pipe. MPR_CMD_STDIN is from the client's perspective.
     */
    if (index == MPR_CMD_STDIN) {
        file->fd = open(file->name, O_WRONLY, 0644);
    } else {
        file->fd = open(file->name, O_RDONLY, 0644);
    }
    if (file->fd < 0) {
        mprError("Can't create stdio pipes. Err %d", mprGetOsError());
        return MPR_ERR_CANT_CREATE;
    }
    nonBlock = 1;
    ioctl(file->fd, FIONBIO, (int) &nonBlock);
    return 0;
}
#endif


#if BIT_UNIX_LIKE
static int startProcess(MprCmd *cmd)
{
    MprCmdFile      *files;
    int             rc, i, err;

    files = cmd->files;
    if (!cmd->signal) {
        cmd->signal = mprAddSignalHandler(SIGCHLD, reapCmd, cmd, cmd->dispatcher, MPR_SIGNAL_BEFORE);
    }
    /*
        Create the child
     */
    cmd->pid = vfork();

    if (cmd->pid < 0) {
        mprError("start: can't fork a new process to run %s, errno %d", cmd->program, mprGetOsError());
        return MPR_ERR_CANT_INITIALIZE;

    } else if (cmd->pid == 0) {
        /*
            Child
         */
        umask(022);
        if (cmd->flags & MPR_CMD_NEW_SESSION) {
            setsid();
        }
        if (cmd->dir) {
            if (chdir(cmd->dir) < 0) {
                mprError("cmd: Can't change directory to %s", cmd->dir);
                return MPR_ERR_CANT_INITIALIZE;
            }
        }
        if (cmd->flags & MPR_CMD_IN) {
            if (files[MPR_CMD_STDIN].clientFd >= 0) {
                dup2(files[MPR_CMD_STDIN].clientFd, 0);
                close(files[MPR_CMD_STDIN].fd);
            } else {
                close(0);
            }
        }
        if (cmd->flags & MPR_CMD_OUT) {
            if (files[MPR_CMD_STDOUT].clientFd >= 0) {
                dup2(files[MPR_CMD_STDOUT].clientFd, 1);
                close(files[MPR_CMD_STDOUT].fd);
            } else {
                close(1);
            }
        }
        if (cmd->flags & MPR_CMD_ERR) {
            if (files[MPR_CMD_STDERR].clientFd >= 0) {
                dup2(files[MPR_CMD_STDERR].clientFd, 2);
                close(files[MPR_CMD_STDERR].fd);
            } else {
                close(2);
            }
        }
        cmd->forkCallback(cmd->forkData);
        if (cmd->env) {
            rc = execve(cmd->program, (char**) cmd->argv, (char**) &cmd->env->items[0]);
        } else {
            rc = execv(cmd->program, (char**) cmd->argv);
        }
        err = errno;
        printf("Can't exec %s, rc %d, err %d\n", cmd->program, rc, err);

        /*
            Use _exit to avoid flushing I/O any other I/O.
         */
        _exit(-(MPR_ERR_CANT_INITIALIZE));

    } else {
        /*
            Close the client handles
         */
        for (i = 0; i < MPR_CMD_MAX_PIPE; i++) {
            if (files[i].clientFd >= 0) {
                close(files[i].clientFd);
                files[i].clientFd = -1;
            }
        }
    }
    return 0;
}


#elif VXWORKS
/*
    Start the command to run (stdIn and stdOut are named from the client's perspective)
 */
int startProcess(MprCmd *cmd)
{
    MprCmdTaskFn    entryFn;
    MprModule       *mp;
    SYM_TYPE        symType;
    char            *entryPoint, *program, *pair;
    int             pri, next;

    mprLog(4, "cmd: start %s", cmd->program);

    entryPoint = 0;
    if (cmd->env) {
        for (ITERATE_ITEMS(cmd->env, pair, next)) {
            if (sncmp(pair, "entryPoint=", 11) == 0) {
                entryPoint = sclone(&pair[11]);
            }
        }
    }
    program = mprGetPathBase(cmd->program);
    if (entryPoint == 0) {
        program = mprTrimPathExt(program);
        entryPoint = program;
    }
    if (symFindByName(sysSymTbl, entryPoint, (char**) &entryFn, &symType) < 0) {
        if ((mp = mprCreateModule(cmd->program, cmd->program, NULL, NULL)) == 0) {
            mprError("start: can't create module");
            return MPR_ERR_CANT_CREATE;
        }
        if (mprLoadModule(mp) < 0) {
            mprError("start: can't load DLL %s, errno %d", program, mprGetOsError());
            return MPR_ERR_CANT_READ;
        }
        if (symFindByName(sysSymTbl, entryPoint, (char**) &entryFn, &symType) < 0) {
            mprError("start: can't find symbol %s, errno %d", entryPoint, mprGetOsError());
            return MPR_ERR_CANT_ACCESS;
        }
    }
    taskPriorityGet(taskIdSelf(), &pri);

    cmd->pid = taskSpawn(entryPoint, pri, VX_FP_TASK | VX_PRIVATE_ENV, MPR_DEFAULT_STACK, (FUNCPTR) cmdTaskEntry, 
        (int) cmd->program, (int) entryFn, (int) cmd, 0, 0, 0, 0, 0, 0, 0);

    if (cmd->pid < 0) {
        mprError("start: can't create task %s, errno %d", entryPoint, mprGetOsError());
        return MPR_ERR_CANT_CREATE;
    }
    mprLog(7, "cmd, child taskId %d", cmd->pid);

    if (semTake(cmd->startCond, MPR_TIMEOUT_START_TASK) != OK) {
        mprError("start: child %s did not initialize, errno %d", cmd->program, mprGetOsError());
        return MPR_ERR_CANT_CREATE;
    }
    semDelete(cmd->startCond);
    cmd->startCond = 0;
    return 0;
}


/*
    Executed by the child process
 */
static void cmdTaskEntry(char *program, MprCmdTaskFn entry, int cmdArg)
{
    MprCmd          *cmd;
    MprCmdFile      *files;
    WIND_TCB        *tcb;
    char            *item;
    int             inFd, outFd, errFd, id, next;

    cmd = (MprCmd*) cmdArg;

    /*
        Open standard I/O files (in/out are from the server's perspective)
     */
    files = cmd->files;
    inFd = open(files[MPR_CMD_STDIN].name, O_RDONLY, 0666);
    outFd = open(files[MPR_CMD_STDOUT].name, O_WRONLY, 0666);
    errFd = open(files[MPR_CMD_STDERR].name, O_WRONLY, 0666);

    if (inFd < 0 || outFd < 0 || errFd < 0) {
        exit(255);
    }
    id = taskIdSelf();
    ioTaskStdSet(id, 0, inFd);
    ioTaskStdSet(id, 1, outFd);
    ioTaskStdSet(id, 2, errFd);

    /*
        Now that we have opened the stdin and stdout, wakeup our parent.
     */
    semGive(cmd->startCond);

    /*
        Create the environment
     */
    if (envPrivateCreate(id, -1) < 0) {
        exit(254);
    }
    for (ITERATE_ITEMS(cmd->env, item, next)) {
        putenv(item);
    }

#if !VXWORKS
{
    char    *dir;
    int     rc;

    /*
        Set current directory if required
        WARNING: Setting working directory on VxWorks is global
     */
    if (cmd->dir) {
        rc = chdir(cmd->dir);
    } else {
        dir = mprGetPathDir(cmd->program);
        rc = chdir(dir);
    }
    if (rc < 0) {
        mprError("cmd: Can't change directory to %s", cmd->dir);
        exit(255);
    }
}
#endif

    /*
        Call the user's entry point
     */
    (entry)(cmd->argc, (char**) cmd->argv, (char**) cmd->env);

    tcb = taskTcb(id);
    cmd->status = tcb->exitCode;

    /*
        Cleanup
     */
    envPrivateDestroy(id);
    close(inFd);
    close(outFd);
    close(errFd);
    semGive(cmd->exitCond);
}


#endif /* VXWORKS */


static void closeFiles(MprCmd *cmd)
{
    int     i;
    for (i = 3; i < MPR_MAX_FILE; i++) {
        close(i);
    }
}


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
