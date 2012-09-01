/*
    mprRomFile.c - ROM File system

    ROM support for systems without disk or flash based file systems. This module provides read-only file retrieval 
    from compiled file images. Use the mprRomComp program to compile files into C code and then link them into your 
    application. This module uses a hashed symbol table for fast file lookup.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if BIT_ROM 
/****************************** Forward Declarations **************************/

static void manageRomFile(MprFile *file, int flags);
static int getPathInfo(MprRomFileSystem *rfs, cchar *path, MprPath *info);
static MprRomInode *lookup(MprRomFileSystem *rfs, cchar *path);

/*********************************** Code *************************************/

static MprFile *openFile(MprFileSystem *fileSystem, cchar *path, int flags, int omode)
{
    MprRomFileSystem    *rfs;
    MprFile             *file;
    
    mprAssert(path && *path);

    rfs = (MprRomFileSystem*) fileSystem;
    file = mprAllocObj(MprFile, manageRomFile);
    file->fileSystem = fileSystem;
    file->mode = omode;
    file->fd = -1;
    file->path = sclone(path);
    if ((file->inode = lookup(rfs, path)) == 0) {
        return 0;
    }
    return file;
}


static void manageRomFile(MprFile *file, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(file->fileSystem);
        mprMark(file->path);
        mprMark(file->buf);
        mprMark(file->inode);
    }
}


static int closeFile(MprFile *file)
{
    return 0;
}


static ssize readFile(MprFile *file, void *buf, ssize size)
{
    MprRomInode     *inode;
    ssize           len;

    mprAssert(buf);

    if (file->fd == 0) {
        return read(file->fd, buf, size);
    }
    inode = file->inode;
    len = min(inode->size - file->iopos, size);
    mprAssert(len >= 0);
    memcpy(buf, &inode->data[file->iopos], len);
    file->iopos += len;
    return len;
}


static ssize writeFile(MprFile *file, cvoid *buf, ssize size)
{
    if (file->fd == 1 || file->fd == 2) {
        return write(file->fd, buf, size);
    }
    return MPR_ERR_CANT_WRITE;
}


static long seekFile(MprFile *file, int seekType, long distance)
{
    MprRomInode     *inode;

    mprAssert(seekType == SEEK_SET || seekType == SEEK_CUR || seekType == SEEK_END);

    inode = file->inode;

    switch (seekType) {
    case SEEK_CUR:
        file->iopos += distance;
        break;

    case SEEK_END:
        file->iopos = inode->size + distance;
        break;

    default:
        file->iopos = distance;
        break;
    }
    if (file->iopos < 0) {
        errno = EBADF;
        return MPR_ERR_BAD_STATE;
    }
    return file->iopos;
}


static bool accessPath(MprRomFileSystem *fileSystem, cchar *path, int omode)
{
    MprPath     info;

    return getPathInfo(fileSystem, path, &info) == 0 ? 1 : 0;
}


static int deletePath(MprRomFileSystem *fileSystem, cchar *path)
{
    return MPR_ERR_CANT_WRITE;
}
 

static int makeDir(MprRomFileSystem *fileSystem, cchar *path, int perms, int owner, int group)
{
    return MPR_ERR_CANT_WRITE;
}


static int makeLink(MprRomFileSystem *fileSystem, cchar *path, cchar *target, int hard)
{
    return MPR_ERR_CANT_WRITE;
}


static int getPathInfo(MprRomFileSystem *rfs, cchar *path, MprPath *info)
{
    MprRomInode *ri;

    mprAssert(path && *path);

    info->checked = 1;

    if ((ri = (MprRomInode*) lookup(rfs, path)) == 0) {
        return MPR_ERR_CANT_FIND;
    }
    memset(info, 0, sizeof(MprPath));

    info->valid = 1;
    info->size = ri->size;
    info->mtime = 0;
    info->inode = ri->num;

    if (ri->data == 0) {
        info->isDir = 1;
        info->isReg = 0;
    } else {
        info->isReg = 1;
        info->isDir = 0;
    }
    return 0;
}


static int getPathLink(MprRomFileSystem *rfs, cchar *path)
{
    /* Links not supported on ROMfs */
    return -1;
}


static MprRomInode *lookup(MprRomFileSystem *rfs, cchar *path)
{
    if (path == 0) {
        return 0;
    }
    /*
        Remove "./" segments
     */
    while (*path == '.') {
        if (path[1] == '\0') {
            path++;
        } else if (path[1] == '/') {
            path += 2;
        } else {
            break;
        }
    }
    /*
        Skip over the leading "/"
     */
    if (*path == '/') {
        path++;
    }
    return (MprRomInode*) mprLookupKey(rfs->fileIndex, path);
}


int mprSetRomFileSystem(MprRomInode *inodeList)
{
    MprRomFileSystem    *rfs;
    MprRomInode         *ri;

    rfs = (MprRomFileSystem*) MPR->fileSystem;
    rfs->romInodes = inodeList;
    rfs->fileIndex = mprCreateHash(MPR_FILES_HASH_SIZE, MPR_HASH_STATIC_KEYS | MPR_HASH_STATIC_VALUES);

    for (ri = inodeList; ri->path; ri++) {
        if (mprAddKey(rfs->fileIndex, ri->path, ri) < 0) {
            mprAssert(!MPR_ERR_MEMORY);
            return MPR_ERR_MEMORY;
        }
    }
    return 0;
}


void manageRomFileSystem(MprRomFileSystem *rfs, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
#if !WINCE
        MprFileSystem *fs = (MprFileSystem*) rfs;
        mprMark(fs->separators);
        mprMark(fs->newline);
        mprMark(fs->root);
#if BIT_WIN_LIKE || CYGWIN
        mprMark(fs->cygdrive);
        mprMark(fs->cygwin);
#endif
        mprMark(rfs->fileIndex);
        mprMark(rfs->romInodes);
#endif
    }
}


MprRomFileSystem *mprCreateRomFileSystem(cchar *path)
{
    MprFileSystem      *fs;
    MprRomFileSystem   *rfs;

    if ((rfs = mprAllocObj(MprRomFileSystem, manageRomFileSystem)) == 0) {
        return rfs;
    }
    fs = &rfs->fileSystem;
    fs->accessPath = (MprAccessFileProc) accessPath;
    fs->deletePath = (MprDeleteFileProc) deletePath;
    fs->getPathInfo = (MprGetPathInfoProc) getPathInfo;
    fs->getPathLink = (MprGetPathLinkProc) getPathLink;
    fs->makeDir = (MprMakeDirProc) makeDir;
    fs->makeLink = (MprMakeLinkProc) makeLink;
    fs->openFile = (MprOpenFileProc) openFile;
    fs->closeFile = (MprCloseFileProc) closeFile;
    fs->readFile = (MprReadFileProc) readFile;
    fs->seekFile = (MprSeekFileProc) seekFile;
    fs->writeFile = (MprWriteFileProc) writeFile;

    if ((MPR->stdError = mprAllocStruct(MprFile)) == 0) {
        return NULL;
    }
    mprSetName(MPR->stdError, "stderr");
    MPR->stdError->fd = 2;
    MPR->stdError->fileSystem = fs;
    MPR->stdError->mode = O_WRONLY;

    if ((MPR->stdInput = mprAllocStruct(MprFile)) == 0) {
        return NULL;
    }
    mprSetName(MPR->stdInput, "stdin");
    MPR->stdInput->fd = 0;
    MPR->stdInput->fileSystem = fs;
    MPR->stdInput->mode = O_RDONLY;

    if ((MPR->stdOutput = mprAllocStruct(MprFile)) == 0) {
        return NULL;
    }
    mprSetName(MPR->stdOutput, "stdout");
    MPR->stdOutput->fd = 1;
    MPR->stdOutput->fileSystem = fs;
    MPR->stdOutput->mode = O_WRONLY;

#if UNUSED
    fs->stdError = mprAllocZeroed(sizeof(MprFile));
    if (fs->stdError == 0) {
        return NULL;
    }
    fs->stdError->fd = 2;
    fs->stdError->fileSystem = fs;
    fs->stdError->mode = O_WRONLY;

    fs->stdInput = mprAllocZeroed(sizeof(MprFile));
    if (fs->stdInput == 0) {
        return NULL;
    }
    fs->stdInput->fd = 0;
    fs->stdInput->fileSystem = fs;
    fs->stdInput->mode = O_RDONLY;

    fs->stdOutput = mprAllocZeroed(sizeof(MprFile));
    if (fs->stdOutput == 0) {
        return NULL;
    }
    fs->stdOutput->fd = 1;
    fs->stdOutput->fileSystem = fs;
    fs->stdOutput->mode = O_WRONLY;
#endif
    return rfs;
}


#else /* BIT_ROM */
void stubRomfs() {}
#endif /* BIT_ROM */

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
