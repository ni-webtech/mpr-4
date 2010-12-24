/**
    mprFileSystem.c - File system services.

    This module provides a simple cross platform file system abstraction. File systems provide a file system switch and 
    underneath a file system provider that implements actual I/O.
    This module is not thread-safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Code ************************************/

MprFileSystem *mprCreateFileSystem(cchar *path)
{
    MprFileSystem   *fs;
    char            *cp;

    /*
        TODO - evolve this to support multiple file systems in a single system
     */
#if BLD_FEATURE_ROMFS
    fs = (MprFileSystem*) mprCreateRomFileSystem(path);
#else
    fs = (MprFileSystem*) mprCreateDiskFileSystem(path);
#endif

#if BLD_WIN_LIKE
    fs->separators = sclone("\\/");
    fs->newline = sclone("\r\n");
#else
    fs->separators = sclone("/");
    fs->newline = sclone("\n");
#endif

#if BLD_WIN_LIKE || MACOSX
    fs->caseSensitive = 0;
#else
    fs->caseSensitive = 1;
#endif

#if BLD_WIN_LIKE || VXWORKS
    fs->hasDriveSpecs = 1;
#endif

    if (MPR->fileSystem == NULL) {
        MPR->fileSystem = fs;
    }
    fs->root = mprGetAbsPath(path);
    if ((cp = strpbrk(fs->root, fs->separators)) != 0) {
        *++cp = '\0';
    }
#if BLD_WIN_LIKE && FUTURE
    mprReadRegistry(&fs->cygdrive, MPR_BUFSIZE, "HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygnus Solutions\\Cygwin\\mounts v2",
        "cygdrive prefix");
#endif
    return fs;
}


void mprAddFileSystem(MprFileSystem *fs)
{
    mprAssert(fs);
    
    //  TODO - this does not currently add a file system. It merely replaces the existing.
    MPR->fileSystem = fs;
}


/*
    Note: path can be null
 */
MprFileSystem *mprLookupFileSystem(cchar *path)
{
    return MPR->fileSystem;
}


cchar *mprGetPathNewline(cchar *path)
{
    MprFileSystem   *fs;

    mprAssert(path);

    fs = mprLookupFileSystem(path);
    return fs->newline;
}


cchar *mprGetPathSeparators(cchar *path)
{
    MprFileSystem   *fs;

    mprAssert(path);

    fs = mprLookupFileSystem(path);
    return fs->separators;
}


void mprSetPathSeparators(cchar *path, cchar *separators)
{
    MprFileSystem   *fs;

    mprAssert(path);
    mprAssert(separators);
    
    fs = mprLookupFileSystem(path);
    fs->separators = sclone(separators);
}


void mprSetPathNewline(cchar *path, cchar *newline)
{
    MprFileSystem   *fs;
    
    mprAssert(path);
    mprAssert(newline);
    
    fs = mprLookupFileSystem(path);
    fs->newline = sclone(newline);
}


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
