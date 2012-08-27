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
        FUTURE: evolve this to support multiple file systems in a single system
     */
#if BIT_ROM
    fs = (MprFileSystem*) mprCreateRomFileSystem(path);
#else
    fs = (MprFileSystem*) mprCreateDiskFileSystem(path);
#endif

#if BIT_WIN_LIKE
    fs->separators = sclone("\\/");
    fs->newline = sclone("\r\n");
#elif CYGWIN
    fs->separators = sclone("/\\");
    fs->newline = sclone("\n");
#else
    fs->separators = sclone("/");
    fs->newline = sclone("\n");
#endif

#if BIT_WIN_LIKE || MACOSX || CYGWIN
    fs->caseSensitive = 0;
#else
    fs->caseSensitive = 1;
#endif

#if BIT_WIN_LIKE || VXWORKS || CYGWIN
    fs->hasDriveSpecs = 1;
#endif

    if (MPR->fileSystem == NULL) {
        MPR->fileSystem = fs;
    }
    fs->root = mprGetAbsPath(path);
    if ((cp = strpbrk(fs->root, fs->separators)) != 0) {
        *++cp = '\0';
    }
#if BIT_WIN_LIKE || CYGWIN
    fs->cygwin = mprReadRegistry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Cygwin\\setup", "rootdir");
    fs->cygdrive = sclone("/cygdrive");
#endif
    return fs;
}


void mprAddFileSystem(MprFileSystem *fs)
{
    mprAssert(fs);
    
    /* NOTE: this does not currently add a file system. It merely replaces the existing file system. */
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

    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the Embedthis Open Source license or you may acquire a 
    commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
