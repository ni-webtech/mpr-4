/**
    mprModule.c - Dynamic module loading support.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

/********************************** Forwards **********************************/

static void manageModule(MprModule *mp, int flags);
static void manageModuleService(MprModuleService *ms, int flags);

/************************************* Code ***********************************/
/*
    Open the module service
 */
MprModuleService *mprCreateModuleService()
{
    MprModuleService    *ms;

    if ((ms = mprAllocObj(MprModuleService, manageModuleService)) == 0) {
        return 0;
    }
    ms->modules = mprCreateList(-1, 0);
    ms->mutex = mprCreateLock();
    MPR->moduleService = ms;
    mprSetModuleSearchPath(NULL);
    return ms;
}


static void manageModuleService(MprModuleService *ms, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ms->modules);
        mprMark(ms->searchPath);
        mprMark(ms->mutex);
    }
}


/*
    Call the start routine for each module
 */
int mprStartModuleService()
{
    MprModuleService    *ms;
    MprModule           *mp;
    int                 next;

    ms = MPR->moduleService;
    mprAssert(ms);

    for (next = 0; (mp = mprGetNextItem(ms->modules, &next)) != 0; ) {
        if (mprStartModule(mp) < 0) {
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
#if VXWORKS && BLD_DEBUG && SYM_SYNC_INCLUDED
    symSyncLibInit();
#endif
    return 0;
}


void mprStopModuleService()
{
    MprModuleService    *ms;
    MprModule           *mp;
    int                 next;

    ms = MPR->moduleService;
    mprAssert(ms);
    mprLock(ms->mutex);
    for (next = 0; (mp = mprGetNextItem(ms->modules, &next)) != 0; ) {
        mprStopModule(mp);
    }
    mprUnlock(ms->mutex);
}


MprModule *mprCreateModule(cchar *name, cchar *path, cchar *entry, void *data)
{
    MprModuleService    *ms;
    MprModule           *mp;
    int                 index;

    ms = MPR->moduleService;
    mprAssert(ms);

#if UNUSED
    MprPath             info;
    char                *at;
    if (path) {
        if ((at = mprSearchForModule(path)) == 0) {
            mprError("Can't find module \"%s\", cwd: \"%s\", search path \"%s\"", path, mprGetCurrentPath(),
                mprGetModuleSearchPath());
            return 0;
        }
        path = at;
        mprGetPathInfo(path, &info);
    }
#endif
    if ((mp = mprAllocObj(MprModule, manageModule)) == 0) {
        return 0;
    }
    mp->name = sclone(name);
    mp->path = sclone(path);
    mp->entry = sclone(entry);
    mp->moduleData = data;
    mp->lastActivity = mprGetTime();
    index = mprAddItem(ms->modules, mp);
    if (index < 0 || mp->name == 0) {
        return 0;
    }
    return mp;
}


static void manageModule(MprModule *mp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(mp->entry);
        mprMark(mp->name);
        mprMark(mp->path);
        mprMark(mp->moduleData);
    }
}


int mprStartModule(MprModule *mp)
{
    mprAssert(mp);

    if (mp->start && !(mp->flags & MPR_MODULE_STARTED)) {
        if (mp->start(mp) < 0) {
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
    mp->flags |= MPR_MODULE_STARTED;
    return 0;
}


int mprStopModule(MprModule *mp)
{
    mprAssert(mp);

    if (mp->stop && (mp->flags & MPR_MODULE_STARTED) && !(mp->flags & MPR_MODULE_STOPPED)) {
        if (mp->stop(mp) < 0) {
            return MPR_ERR_NOT_READY;
        }
        mp->flags |= MPR_MODULE_STOPPED;
    }
    return 0;
}


/*
    See if a module is already loaded
 */
MprModule *mprLookupModule(cchar *name)
{
    MprModuleService    *ms;
    MprModule           *mp;
    int                 next;

    mprAssert(name && name);

    ms = MPR->moduleService;
    mprAssert(ms);

    for (next = 0; (mp = mprGetNextItem(ms->modules, &next)) != 0; ) {
        mprAssert(mp->name);
        if (mp && strcmp(mp->name, name) == 0) {
            return mp;
        }
    }
    return 0;
}


void *mprLookupModuleData(cchar *name)
{
    MprModule   *module;

    if ((module = mprLookupModule(name)) == NULL) {
        return NULL;
    }
    return module->moduleData;
}


void mprSetModuleTimeout(MprModule *module, MprTime timeout)
{
    module->timeout = timeout;
}


//  MOB - rename SetModuleStop
void mprSetModuleFinalizer(MprModule *module, MprModuleProc stop)
{
    module->stop = stop;
}


void mprSetModuleSearchPath(char *searchPath)
{
    MprModuleService    *ms;
    cchar               *libdir;

    ms = MPR->moduleService;

    if (searchPath == 0) {
        libdir = mprJoinPath(mprGetPathParent(mprGetAppDir()), mprGetPathBase(BLD_LIB_NAME));
        ms->searchPath = sjoin(mprGetAppDir(), MPR_SEARCH_SEP, libdir, MPR_SEARCH_SEP, BLD_LIB_PREFIX, NULL);
    } else {
        ms->searchPath = sclone(searchPath);
    }

#if UNUSED && KEEP
#if BLD_WIN_LIKE && !WINCE
    {
        /*
            Set PATH so dependent DLLs can be loaded by LoadLibrary
            WARNING: this will leak if called too much.
         */
        char *path = sjoin("PATH=", searchPath, ";", getenv("PATH"), NULL);
        mprMapSeparators(path, '\\');
        mprHold(path);
        putenv(path);
    }
#endif
#endif
}


cchar *mprGetModuleSearchPath()
{
    return MPR->moduleService->searchPath;
}


/*
    Load a module. The module is located by searching for the filename by optionally using the module search path.
 */
int mprLoadModule(MprModule *mp)
{
#if BLD_CC_DYN_LOAD
    mprAssert(mp);

    if (mprLoadNativeModule(mp) < 0) {
        return MPR_ERR_CANT_READ;
    }
    mprStartModule(mp);
    return 0;
#else
    mprError("Product built without the ability to load modules dynamically");
    return MPR_ERR_BAD_STATE;
#endif
}


int mprUnloadModule(MprModule *mp)
{
    mprLog(6, "Unloading native module %s from %s", mp->name, mp->path);
    if (mprStopModule(mp) < 0) {
        return MPR_ERR_NOT_READY;
    }
#if BLD_CC_DYN_LOAD
    if (mp->handle) {
        if (mprUnloadNativeModule(mp) != 0) {
            mprError("Can't unload module %s", mp->name);
        }
        mp->handle = 0;
    }
#endif
    mprRemoveItem(MPR->moduleService->modules, mp);
    return 0;
}


#if BLD_CC_DYN_LOAD
/*
    Return true if the shared library in "file" can be found. Return the actual path in *path. The filename
    may not have a shared library extension which is typical so calling code can be cross platform.
 */
static char *probe(cchar *filename)
{
    char    *path;

    mprAssert(filename && *filename);

    mprLog(7, "Probe for native module %s", filename);
    if (mprPathExists(filename, R_OK)) {
        return sclone(filename);
    }

    if (strstr(filename, BLD_SHOBJ) == 0) {
        path = sjoin(filename, BLD_SHOBJ, NULL);
        mprLog(7, "Probe for native module %s", path);
        if (mprPathExists(path, R_OK)) {
            return path;
        }
    }
    return 0;
}
#endif


/*
    Search for a module "filename" in the modulePath. Return the result in "result"
 */
char *mprSearchForModule(cchar *filename)
{
#if BLD_CC_DYN_LOAD
    char    *path, *f, *searchPath, *dir, *tok;

    filename = mprNormalizePath(filename);

    /*
        Search for the path directly
     */
    if ((path = probe(filename)) != 0) {
        mprLog(6, "Found native module %s at %s", filename, path);
        return path;
    }

    /*
        Search in the searchPath
     */
    searchPath = sclone(mprGetModuleSearchPath());
    tok = 0;
    dir = stok(searchPath, MPR_SEARCH_SEP, &tok);
    while (dir && *dir) {
        f = mprJoinPath(dir, filename);
        if ((path = probe(f)) != 0) {
            mprLog(6, "Found native module %s at %s", filename, path);
            return path;
        }
        dir = stok(0, MPR_SEARCH_SEP, &tok);
    }
#endif /* BLD_CC_DYN_LOAD */
    return 0;
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
