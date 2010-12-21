/**
    mprLog.c - Multithreaded Portable Runtime (MPR) Logging and error reporting.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/****************************** Forward Declarations **************************/

static void defaultLogHandler(int flags, int level, cchar *msg);
static void logOutput(int flags, int level, cchar *msg);

/************************************ Code ************************************/
/*
    Put first in file so it is easy to locate in a debugger
 */
void mprBreakpoint()
{
#if BLD_DEBUG && DEBUG_IDE
	#if BLD_WIN_LIKE && !MPR_64_BIT
        __asm { int 3 };
    #elif (MACOSX || LINUX) && (BLD_HOST_CPU_ARCH == MPR_CPU_IX86 || BLD_HOST_CPU_ARCH == MPR_CPU_IX64)
        asm("int $03");
        /*  __asm__ __volatile__ ("int $03"); */
    #endif
#endif
}


void mprLog(int level, cchar *fmt, ...)
{
    va_list     args;
    char        *buf;

    if (level > mprGetLogLevel()) {
        return;
    }
    va_start(args, fmt);
    buf = mprAsprintfv(fmt, args);
    va_end(args);

    logOutput(MPR_LOG_SRC, level, buf);
    mprFree(buf);
}


void mprRawLog(int level, cchar *fmt, ...)
{
    va_list     args;
    char        *buf;

    if (level > mprGetLogLevel()) {
        return;
    }
    va_start(args, fmt);
    buf = mprAsprintfv(fmt, args);
    va_end(args);
    
    logOutput(MPR_RAW, 0, buf);
    mprFree(buf);
}


void mprError(cchar *fmt, ...)
{
    va_list     args;
    char        *buf;

    va_start(args, fmt);
    buf = mprAsprintfv(fmt, args);
    va_end(args);
    
    logOutput(MPR_ERROR_MSG | MPR_ERROR_SRC, 0, buf);

    mprFree(buf);
    mprBreakpoint();
}


void mprMemoryError(cchar *fmt, ...)
{
    va_list     args;
    char        *buf;

    if (fmt == 0) {
        logOutput(MPR_ERROR_MSG | MPR_ERROR_SRC, 0, "Memory allocation error");
    } else {
        va_start(args, fmt);
        buf = mprAsprintfv(fmt, args);
        va_end(args);
        logOutput(MPR_ERROR_MSG | MPR_ERROR_SRC, 0, buf);
        mprFree(buf);
    }
}


void mprUserError(cchar *fmt, ...)
{
    va_list     args;
    char        *buf;

    va_start(args, fmt);
    buf = mprAsprintfv(fmt, args);
    va_end(args);
    
    logOutput(MPR_USER_MSG | MPR_ERROR_SRC, 0, buf);
    mprFree(buf);
}


void mprFatalError(cchar *fmt, ...)
{
    va_list     args;
    char        *buf;

    va_start(args, fmt);
    buf = mprAsprintfv(fmt, args);
    va_end(args);
    
    logOutput(MPR_USER_MSG | MPR_FATAL_SRC, 0, buf);
    mprFree(buf);
    exit(2);
}


#if UNUSED
/*
    Handle an error without allocating memory.
 */
void mprStaticError(cchar *fmt, ...)
{
    va_list     args;
    char        buf[MPR_MAX_STRING];

    va_start(args, fmt);
    mprSprintfv(buf, sizeof(buf), fmt, args);
    va_end(args);
    logOutput(NULL, MPR_ERROR_MSG | MPR_ERROR_SRC, 0, buf);
}
#endif


/*
    Direct output to the standard error. Does not hook into the logging system and does not allocate memory.
 */
void mprAssertError(cchar *loc, cchar *msg)
{
#if BLD_DEBUG
    char    buf[MPR_MAX_STRING];

#if BLD_UNIX_LIKE
    snprintf(buf, sizeof(buf), "Assertion %s, failed at %s\n", msg, loc);
#else
    sprintf(buf, "Assertion %s, failed at %s\n", msg, loc);
#endif
    
#if BLD_UNIX_LIKE || VXWORKS
    (void) write(2, buf, strlen(buf));
#elif BLD_WIN_LIKE
    /*
        Only time we use printf. We can't get an alloc context so we have to use real print
     */
    fprintf(stderr, "%s\n", buf);
#endif
    mprBreakpoint();
#endif
}


int mprGetLogLevel()
{
    Mpr     *mpr;

    /*
        Leave the code like this so debuggers can patch logLevel before returning.
     */
    mpr = mprGetMpr();
    return mpr->logLevel;
}


void mprSetLogLevel(int level)
{
    mprGetMpr()->logLevel = level;
}


void mprSetAltLogData(void *data)
{
    mprGetMpr()->altLogData = data;
}


/*
    Output a log message to the log handler
 */
static void logOutput(int flags, int level, cchar *msg)
{
    MprLogHandler   handler;

    handler = mprGetMpr()->logHandler;
    if (handler != 0) {
        (handler)(flags, level, msg);
        return;
    }
    defaultLogHandler(flags, level, msg);
}


static void defaultLogHandler(int flags, int level, cchar *msg)
{
    Mpr     *mpr;
    char    *prefix;

    mpr = mprGetMpr();
    prefix = mpr->name;

    if (msg == 0) {
        return;
    }
    while (*msg == '\n') {
        mprPrintfError("\n");
        msg++;
    }
    if (flags & MPR_LOG_SRC) {
        mprPrintfError("%s: %d: %s\n", prefix, level, msg);
    } else if (flags & MPR_ERROR_SRC) {
        mprPrintfError("%s: Error: %s\n", prefix, msg);
    } else if (flags & MPR_FATAL_SRC) {
        mprPrintfError("%s: Fatal: %s\n", prefix, msg);
    } else if (flags & MPR_RAW) {
        mprPrintfError("%s", msg);
    }
}


/*
    Return the raw O/S error code
 */
int mprGetOsError()
{
#if BLD_WIN_LIKE
    int     rc;
    rc = GetLastError();

    /*
        Client has closed the pipe
     */
    if (rc == ERROR_NO_DATA) {
        return EPIPE;
    }
    return rc;
#elif BLD_UNIX_LIKE || VXWORKS
    return errno;
#else
    return 0;
#endif
}


int mprGetLogFd()
{
    return mprGetMpr()->logFd;
}


int mprSetLogFd(int fd)
{
    return mprGetMpr()->logFd = fd;
}


/*
    Return the mapped (portable, Posix) error code
 */
int mprGetError()
{
#if !BLD_WIN_LIKE
    return mprGetOsError();
#else
    int     err;

    err = mprGetOsError();

    switch (err) {
    case ERROR_SUCCESS:
        return 0;
    case ERROR_FILE_NOT_FOUND:
        return ENOENT;
    case ERROR_ACCESS_DENIED:
        return EPERM;
    case ERROR_INVALID_HANDLE:
        return EBADF;
    case ERROR_NOT_ENOUGH_MEMORY:
        return ENOMEM;
    case ERROR_PATH_BUSY:
    case ERROR_BUSY_DRIVE:
    case ERROR_NETWORK_BUSY:
    case ERROR_PIPE_BUSY:
    case ERROR_BUSY:
        return EBUSY;
    case ERROR_FILE_EXISTS:
        return EEXIST;
    case ERROR_BAD_PATHNAME:
    case ERROR_BAD_ARGUMENTS:
        return EINVAL;
    case WSAENOTSOCK:
        return ENOENT;
    case WSAEINTR:
        return EINTR;
    case WSAEBADF:
        return EBADF;
    case WSAEACCES:
        return EACCES;
    case WSAEINPROGRESS:
        return EINPROGRESS;
    case WSAEALREADY:
        return EALREADY;
    case WSAEADDRINUSE:
        return EADDRINUSE;
    case WSAEADDRNOTAVAIL:
        return EADDRNOTAVAIL;
    case WSAENETDOWN:
        return ENETDOWN;
    case WSAENETUNREACH:
        return ENETUNREACH;
    case WSAECONNABORTED:
        return ECONNABORTED;
    case WSAECONNRESET:
        return ECONNRESET;
    case WSAECONNREFUSED:
        return ECONNREFUSED;
    case WSAEWOULDBLOCK:
        return EAGAIN;
    }
    return MPR_ERR;
#endif
}


#if MACOSX
/*
    Just for conditional breakpoints when debugging in Xcode
 */
int _cmp(char *s1, char *s2)
{
    return !strcmp(s1, s2);
}
#endif

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
    
    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=8 ts=8 expandtab

    @end
 */
