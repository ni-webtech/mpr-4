/**
    mprVxworks.c - Vxworks specific adaptions

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

#if VXWORKS
/********************************* Includes ***********************************/

#include    "mpr.h"

/*********************************** Code *************************************/

int mprCreateOsService()
{
    return 0;
}


int mprStartOsService()
{
    return 0;
}


void mprStopOsService()
{
}


int access(const char *path, int mode)
{
    struct stat sbuf;
    return stat((char*) path, &sbuf);
}


int mprGetRandomBytes(char *buf, int length, bool block)
{
    int     i;

    for (i = 0; i < length; i++) {
        buf[i] = (char) (mprGetTime() >> i);
    }
    return 0;
}


int mprLoadNativeModule(MprModule *mp)
{
    MprModuleEntry  fn;
    SYM_TYPE        symType;
    MprPath         info;
    char            *at;
    void            *handle;
    int             fd;

    mprAssert(mp);
    fn = 0;
    handle = 0;

    if (!mp->entry || symFindByName(sysSymTbl, mp->entry, (char**) &fn, &symType) == -1) {
        if ((at = mprSearchForModule(mp->path)) == 0) {
            mprError("Can't find module \"%s\", cwd: \"%s\", search path \"%s\"", mp->path, mprGetCurrentPath(),
                mprGetModuleSearchPath());
            return 0;
        }
        mp->path = at;
        mprGetPathInfo(mp->path, &info);
        mp->modified = info.mtime;

        mprLog(2, "Loading native module %s", mp->name);
        if ((fd = open(mp->path, O_RDONLY, 0664)) < 0) {
            mprError("Can't open module \"%s\"", mp->path);
            return MPR_ERR_CANT_OPEN;
        }
        errno = 0;
        handle = loadModule(fd, LOAD_GLOBAL_SYMBOLS);
        if (handle == 0 || errno != 0) {
            close(fd);
            if (handle) {
                unldByModuleId(handle, 0);
            }
            mprError("Can't load module %s", mp->path);
            return MPR_ERR_CANT_READ;
        }
        close(fd);
        mp->handle = handle;

    } else if (mp->entry) {
        mprLog(2, "Activating module %s", mp->name);
    }
    if (mp->entry) {
        if (symFindByName(sysSymTbl, mp->entry, (char**) &fn, &symType) == -1) {
            mprError("Can't find symbol %s when loading %s", mp->entry, mp->path);
            return MPR_ERR_CANT_READ;
        }
        if ((fn)(mp->moduleData, mp) < 0) {
            mprError("Initialization for %s failed.", mp->path);
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
    return 0;
}


int mprUnloadNativeModule(MprModule *mp)
{
    unldByModuleId((MODULE_ID) mp->handle, 0);
    return 0;
}


void mprNap(MprTime milliseconds)
{
    struct timespec timeout;
    int             rc;

    mprAssert(milliseconds >= 0);
    timeout.tv_sec = milliseconds / 1000;
    timeout.tv_nsec = (milliseconds % 1000) * 1000000;
    do {
        rc = nanosleep(&timeout, &timeout);
    } while (rc < 0 && errno == EINTR);
}


void mprSleep(MprTime timeout)
{
    mprYield(MPR_YIELD_STICKY);
    mprNap(timeout);
    mprResetYield();
}


void mprWriteToOsLog(cchar *message, int flags, int level)
{
}


uint mprGetpid(void) {
    return taskIdSelf();
}


int fsync(int fd) { 
    return 0; 
}


int ftruncate(int fd, off_t offset) { 
    return 0; 
}


int usleep(uint msec)
{
    struct timespec     timeout;
    int                 rc;

    timeout.tv_sec = msec / (1000 * 1000);
    timeout.tv_nsec = msec % (1000 * 1000) * 1000;
    do {
        rc = nanosleep(&timeout, &timeout);
    } while (rc < 0 && errno == EINTR);
    return 0;
}


int mprInitWindow()
{
    return 0;
}


//  TODO - is this still needed?
/*
    Create a routine to pull in the GCC support routines for double and int64 manipulations for some platforms. Do this
    incase modules reference these routines. Without this, the modules have to reference them. Which leads to multiple 
    defines if two modules include them. (Code to pull in moddi3, udivdi3, umoddi3)
 */
double  __mpr_floating_point_resolution(double a, double b, int64 c, int64 d, uint64 e, uint64 f) {
    a = a / b; a = a * b; c = c / d; c = c % d; e = e / f; e = e % f;
    c = (int64) a; d = (uint64) a; a = (double) c; a = (double) e;
    return (a == b) ? a : b;
}


#else
void stubMprVxWorks() {}
#endif /* VXWORKS */

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
