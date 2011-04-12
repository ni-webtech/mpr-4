/**
    mprUnix.c - Unix specific adaptions

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if BLD_UNIX_LIKE
/*********************************** Code *************************************/

int mprCreateOsService()
{
    umask(022);

    /*
        Cleanup the environment. IFS is often a security hole
     */
    putenv("IFS=\t ");
    return 0;
}


int mprStartOsService()
{
    /* 
        Open a syslog connection
     */
#if SOLARIS
    openlog(mprGetAppName(), LOG_CONS, LOG_LOCAL0);
#else
    openlog(mprGetAppName(), LOG_CONS | LOG_PERROR, LOG_LOCAL0);
#endif
    return 0;
}


void mprStopOsService()
{
}


int mprGetRandomBytes(char *buf, int length, int block)
{
    int     fd, sofar, rc;

    fd = open((block) ? "/dev/random" : "/dev/urandom", O_RDONLY, 0666);
    if (fd < 0) {
        return MPR_ERR_CANT_OPEN;
    }

    sofar = 0;
    do {
        rc = read(fd, &buf[sofar], length);
        if (rc < 0) {
            mprAssert(0);
            return MPR_ERR_CANT_READ;
        }
        length -= rc;
        sofar += rc;
    } while (length > 0);
    close(fd);
    return 0;
}


#if BLD_CC_DYN_LOAD
int mprLoadNativeModule(MprModule *mp)
{
    MprModuleEntry  fn;
    void            *handle;

    mprAssert(mp);

    if ((handle = dlopen(mp->path, RTLD_LAZY | RTLD_GLOBAL)) == 0) {
        mprError("Can't load module %s\nReason: \"%s\"", mp->path, dlerror());
        return MPR_ERR_CANT_OPEN;
    } 
    mp->handle = handle;

    if (mp->entry) {
        if ((fn = (MprModuleEntry) dlsym(handle, mp->entry)) != 0) {
            if ((fn)(mp->moduleData, mp) < 0) {
                mprError("Initialization for module %s failed", mp->name);
                dlclose(handle);
                return MPR_ERR_CANT_INITIALIZE;
            }
        } else {
            mprError("Can't load module %s\nReason: can't find function \"%s\"", mp->path, mp->entry);
            dlclose(handle);
            return MPR_ERR_CANT_READ;
        }
    }
    return 0;
}


int mprUnloadNativeModule(MprModule *mp)
{
    return dlclose(mp->handle);
}
#endif


void mprSleep(MprTime timeout)
{
    MprTime         remaining, mark;
    struct timespec t;
    int             rc;

    mprAssert(timeout >= 0);
    
    mark = mprGetTime();
    remaining = timeout;
    do {
        /* MAC OS X corrupts the timeout if using the 2nd paramater, so recalc each time */
        t.tv_sec = remaining / 1000;
        t.tv_nsec = (remaining % 1000) * 1000000;
        rc = nanosleep(&t, NULL);
        remaining = mprGetRemainingTime(mark, timeout);
    } while (rc < 0 && errno == EINTR && remaining > 0);
}


/*  
    Write a message in the O/S native log (syslog in the case of linux)
 */
void mprWriteToOsLog(cchar *message, int flags, int level)
{
    char    *msg;
    int     sflag;

    if (flags & MPR_FATAL_SRC) {
        msg = "fatal error: ";
        sflag = LOG_ERR;

    } else if (flags & MPR_ASSERT_SRC) {
        msg = "program assertion error: ";
        sflag = LOG_WARNING;

    } else {
        msg = "error: ";
        sflag = LOG_WARNING;
    }
    syslog(sflag, "%s", message);
}


int mprInitWindow()
{
    return 0;
}

#else
void stubMprUnix() {}
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
