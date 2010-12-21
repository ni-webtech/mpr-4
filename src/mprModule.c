/**
    mprModule.c - Dynamic module loading support.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

/********************************** Forwards **********************************/

static void manageModuleService(MprModuleService *ms, int flags);

/************************************* Code ***********************************/
/*
    Open the module service
 */
MprModuleService *mprCreateModuleService()
{
    MprModuleService    *ms;
    cchar               *searchPath;

    ms = mprAllocObj(MprModuleService, manageModuleService);
    if (ms == 0) {
        return 0;
    }
    ms->modules = mprCreateList();

    /*
        Define the default module search path
     */
    if (ms->searchPath == 0) {
#if BLD_DEBUG
        /*
            Put the mod prefix here incase running an installed debug build
         */
        searchPath = ".:" BLD_MOD_NAME ":../" BLD_MOD_NAME ":../../" BLD_MOD_NAME ":../../../" BLD_MOD_NAME ":" \
            BLD_MOD_PREFIX;
#else
        searchPath = BLD_MOD_PREFIX ":.";
#endif
    } else {
        searchPath = ms->searchPath;
    }
    ms->searchPath = sclone((searchPath) ? searchPath : (cchar*) ".");
    ms->mutex = mprCreateLock();
    return ms;
}


static void manageModuleService(MprModuleService *ms, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMarkList(ms->modules);
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

    ms = mprGetMpr()->moduleService;
    mprAssert(ms);

    for (next = 0; (mp = mprGetNextItem(ms->modules, &next)) != 0; ) {
        if (mp->start && mp->start(mp) < 0) {
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
#if VXWORKS && BLD_DEBUG && SYM_SYNC_INCLUDED
    symSyncLibInit();
#endif
    return 0;
}


/*
    Stop all modules
 */
void mprStopModuleService()
{
    MprModuleService    *ms;
    MprModule           *mp;
    int                 next;

    ms = mprGetMpr()->moduleService;
    mprAssert(ms);
    mprLock(ms->mutex);
    for (next = 0; (mp = mprGetNextItem(ms->modules, &next)) != 0; ) {
        if (mp->stop) {
            mp->stop(mp);
        }
    }
    mprUnlock(ms->mutex);
}


/*
    Create a new module
 */
MprModule *mprCreateModule(cchar *name, void *data)
{
    MprModuleService    *ms;
    MprModule           *mp;
    Mpr                 *mpr;
    int                 index;

    mpr = mprGetMpr();
    ms = mpr->moduleService;
    mprAssert(ms);

    if ((mp = mprAllocObj(MprModule, NULL)) == 0) {
        return 0;
    }
    index = mprAddItem(ms->modules, mp);
    mp->name = sclone(name);
    mp->moduleData = data;
    mp->handle = 0;
    mp->start = 0;
    mp->stop = 0;

    if (index < 0 || mp->name == 0) {
        mprFree(mp);
        return 0;
    }
    return mp;
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

    ms = mprGetMpr()->moduleService;
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


void mprSetModuleSearchPath(char *searchPath)
{
    MprModuleService    *ms;
    Mpr                 *mpr;

    mprAssert(searchPath && *searchPath);

    mpr = mprGetMpr();
    mprAssert(mpr);
    ms = mpr->moduleService;

    mprFree(ms->searchPath);
    ms->searchPath = sclone(searchPath);

#if BLD_WIN_LIKE && !WINCE
    {
        char    *path;

        /*
            So dependent DLLs can be loaded by LoadLibrary
         */
        path = sjoin("PATH=", searchPath, ";", getenv("PATH"), NULL);
        mprMapSeparators(path, '\\');
        putenv(path);
        mprFree(path);
    }
#endif
}


cchar *mprGetModuleSearchPath()
{
    MprModuleService    *ms;
    Mpr                 *mpr;

    mpr = mprGetMpr();
    mprAssert(mpr);
    ms = mpr->moduleService;

    return ms->searchPath;
}


#if BLD_CC_DYN_LOAD
/*
    Return true if the shared library in "file" can be found. Return the actual path in *path. The filename
    may not have a shared library extension which is typical so calling code can be cross platform.
 */
static int probe(cchar *filename, char **pathp)
{
    char    *path;

    mprAssert(filename && *filename);
    mprAssert(pathp);

    *pathp = 0;
    mprLog(6, "Probe for native module %s", filename);
    if (mprPathExists(filename, R_OK)) {
        *pathp = sclone(filename);
        return 1;
    }

    if (strstr(filename, BLD_SHOBJ) == 0) {
        path = sjoin(filename, BLD_SHOBJ, NULL);
        mprLog(6, "Probe for native module %s", path);
        if (mprPathExists(path, R_OK)) {
            *pathp = path;
            return 1;
        }
        mprFree(path);
    }
    return 0;
}


/*
    Search for a module in the modulePath.
 */
int mprSearchForModule(cchar *name, char **path)
{
    char    *fileName, *searchPath, *dir, *tok;

    /*
        Search for path directly
     */
    if (probe(name, path)) {
        mprLog(6, "Found native module %s at %s", name, *path);
        return 0;
    }

    /*
        Search in the searchPath
     */
    searchPath = sclone(mprGetModuleSearchPath());

    tok = 0;
    dir = stok(searchPath, MPR_SEARCH_SEP, &tok);
    while (dir && *dir) {
        fileName = mprJoinPath(dir, name);
        if (probe(fileName, path)) {
            mprFree(fileName);
            mprLog(6, "Found native module %s at %s", name, *path);
            return 0;
        }
        mprFree(fileName);
        dir = stok(0, MPR_SEARCH_SEP, &tok);
    }
    mprFree(searchPath);
    return MPR_ERR_CANT_FIND;
}
#endif


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
