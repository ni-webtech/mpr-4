/**
    mprModule.c - Dynamic module loading support.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

/************************************* Code ***********************************/
/*
    Open the module service
 */
MprModuleService *mprCreateModuleService(MprCtx ctx)
{
    MprModuleService    *ms;
    cchar               *searchPath;

    ms = mprAllocObj(ctx, MprModuleService, NULL);
    if (ms == 0) {
        return 0;
    }
    ms->modules = mprCreateList(ms);

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
    ms->searchPath = mprStrdup(ms, (searchPath) ? searchPath : (cchar*) ".");
    ms->mutex = mprCreateLock(ms);
    return ms;
}


/*
    Call the start routine for each module
 */
int mprStartModuleService(MprModuleService *ms)
{
    MprModule       *mp;
    int             next;

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
void mprStopModuleService(MprModuleService *ms)
{
    MprModule       *mp;
    int             next;

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
MprModule *mprCreateModule(MprCtx ctx, cchar *name, void *data)
{
    MprModuleService    *ms;
    MprModule           *mp;
    Mpr                 *mpr;
    int                 index;

    mpr = mprGetMpr(ctx);
    ms = mpr->moduleService;
    mprAssert(ms);

    if ((mp = mprAllocObj(mpr, MprModule, NULL)) == 0) {
        return 0;
    }
    index = mprAddItem(ms->modules, mp);
    mp->name = mprStrdup(mp, name);
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
MprModule *mprLookupModule(MprCtx ctx, cchar *name)
{
    MprModuleService    *ms;
    MprModule           *mp;
    int                 next;

    mprAssert(name && name);

    ms = mprGetMpr(ctx)->moduleService;
    mprAssert(ms);

    for (next = 0; (mp = mprGetNextItem(ms->modules, &next)) != 0; ) {
        mprAssert(mp->name);
        if (mp && strcmp(mp->name, name) == 0) {
            return mp;
        }
    }
    return 0;
}


/*
    Update the module search path
 */
void mprSetModuleSearchPath(MprCtx ctx, char *searchPath)
{
    MprModuleService    *ms;
    Mpr                 *mpr;

    mprAssert(ctx);
    mprAssert(searchPath && *searchPath);

    mpr = mprGetMpr(ctx);
    mprAssert(mpr);
    ms = mpr->moduleService;

    mprFree(ms->searchPath);
    ms->searchPath = mprStrdup(ms, searchPath);

#if BLD_WIN_LIKE && !WINCE
    {
        char    *path;

        /*
            So dependent DLLs can be loaded by LoadLibrary
         */
        path = mprStrcat(mpr, -1, "PATH=", searchPath, ";", getenv("PATH"), NULL);
        mprMapSeparators(mpr, path, '\\');
        putenv(path);
        mprFree(path);
    }
#endif
}


cchar *mprGetModuleSearchPath(MprCtx ctx)
{
    MprModuleService    *ms;
    Mpr                 *mpr;

    mprAssert(ctx);

    mpr = mprGetMpr(ctx);
    mprAssert(mpr);
    ms = mpr->moduleService;

    return ms->searchPath;
}


#if BLD_CC_DYN_LOAD
/*
    Return true if the shared library in "file" can be found. Return the actual path in *path. The filename
    may not have a shared library extension which is typical so calling code can be cross platform.
 */
static int probe(MprCtx ctx, cchar *filename, char **pathp)
{
    char    *path;

    mprAssert(ctx);
    mprAssert(filename && *filename);
    mprAssert(pathp);

    *pathp = 0;
    mprLog(ctx, 6, "Probe for native module %s", filename);
    if (mprPathExists(ctx, filename, R_OK)) {
        *pathp = mprStrdup(ctx, filename);
        return 1;
    }

    if (strstr(filename, BLD_SHOBJ) == 0) {
        path = mprStrcat(ctx, -1, filename, BLD_SHOBJ, NULL);
        mprLog(ctx, 6, "Probe for native module %s", path);
        if (mprPathExists(ctx, path, R_OK)) {
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
int mprSearchForModule(MprCtx ctx, cchar *name, char **path)
{
    char    *fileName, *searchPath, *dir, *tok;

    /*
        Search for path directly
     */
    if (probe(ctx, name, path)) {
        mprLog(ctx, 6, "Found native module %s at %s", name, *path);
        return 0;
    }

    /*
        Search in the searchPath
     */
    searchPath = mprStrdup(ctx, mprGetModuleSearchPath(ctx));

    tok = 0;
    dir = mprStrTok(searchPath, MPR_SEARCH_SEP, &tok);
    while (dir && *dir) {
        fileName = mprJoinPath(ctx, dir, name);
        if (probe(ctx, fileName, path)) {
            mprFree(fileName);
            mprLog(ctx, 6, "Found native module %s at %s", name, *path);
            return 0;
        }
        mprFree(fileName);
        dir = mprStrTok(0, MPR_SEARCH_SEP, &tok);
    }
    mprFree(searchPath);
    return MPR_ERR_NOT_FOUND;
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

    @end
 */
