/**
    mprDisk.c - File services for systems with a (disk) based file system.

    This module is not thread safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

#if !BLD_FEATURE_ROMFS
/********************************** Forwards **********************************/

static void manageDiskFile(MprFile *file, int flags);
static int getPathInfo(MprDiskFileSystem *fileSystem, cchar *path, MprPath *info);

/************************************ Code ************************************/

static MprFile *openFile(MprFileSystem *fileSystem, cchar *path, int omode, int perms)
{
    MprDiskFileSystem   *dfs;
    MprFile             *file;
    
    mprAssert(path);

    dfs = (MprDiskFileSystem*) fileSystem;
    file = mprAllocObj(MprFile, manageDiskFile);
    if (file == 0) {
        return 0;
    }
    file->path = sclone(path);
    file->fd = open(path, omode, perms);
    if (file->fd < 0) {
        mprFree(file);
        return 0;
    }
    return file;
}


static void manageDiskFile(MprFile *file, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(file->buf);
        mprMark(file->path);

    } else if (flags & MPR_MANAGE_FREE) {
        mprCloseFile(file);
    }
}


int mprCloseFile(MprFile *file)
{
    MprBuf  *bp;

    mprAssert(file);

    if (file == 0) {
        return 0;
    }
    bp = file->buf;
    if (bp && (file->mode & (O_WRONLY | O_RDWR))) {
        mprFlush(file);
    }
    if (file->fd >= 0) {
        close(file->fd);
        file->fd = -1;
    }
    return 0;
}


static ssize readFile(MprFile *file, void *buf, ssize size)
{
    mprAssert(file);
    mprAssert(buf);

    return read(file->fd, buf, (uint) size);
}


static ssize writeFile(MprFile *file, cvoid *buf, ssize count)
{
    mprAssert(file);
    mprAssert(buf);

#if VXWORKS
    return write(file->fd, (void*) buf, count);
#else
    return write(file->fd, buf, (uint) count);
#endif
}


static MprOffset seekFile(MprFile *file, int seekType, MprOffset distance)
{
    mprAssert(file);

    if (file == 0) {
        return MPR_ERR_BAD_HANDLE;
    }
    return (MprOffset) lseek(file->fd, distance, seekType);
}


static bool accessPath(MprDiskFileSystem *fileSystem, cchar *path, int omode)
{
    return access(path, omode) == 0;
}


static int deletePath(MprDiskFileSystem *fileSystem, cchar *path)
{
    MprPath     info;

    if (getPathInfo(fileSystem, path, &info) == 0 && info.isDir) {
        return rmdir((char*) path);
    }
#if WIN
{
    int i, rc;
    for (i = 0; i < 100; i++) {
        rc = DeleteFile((char*) path);
        if (rc != 0) {
            rc = 0;
            break;
        }
        mprSleep(10);
    }
    return rc;
}
#else
    return unlink((char*) path);
#endif
}
 

static int makeDir(MprDiskFileSystem *fileSystem, cchar *path, int perms)
{
#if VXWORKS
    return mkdir((char*) path);
#else
    return mkdir(path, perms);
#endif
}


static int makeLink(MprDiskFileSystem *fileSystem, cchar *path, cchar *target, int hard)
{
#if BLD_UNIX_LIKE
    if (hard) {
        return link(target, path);
    } else {
        return symlink(target, path);
    }
#else
    return MPR_ERR_BAD_STATE;
#endif
}


static int getPathInfo(MprDiskFileSystem *fileSystem, cchar *path, MprPath *info)
{
    struct stat s;
#if BLD_WIN_LIKE
    cchar       *ext;
    char        *allocPath;

    mprAssert(path);
    mprAssert(info);

    allocPath = 0;
    info->checked = 1;
    info->valid = 0;

    if (stat(path, &s) < 0) {
        mprFree(allocPath);
        return -1;
    }
    info->valid = 1;
    info->size = s.st_size;
    info->atime = s.st_atime;
    info->ctime = s.st_ctime;
    info->mtime = s.st_mtime;
    info->inode = s.st_ino;
    info->isDir = (s.st_mode & S_IFDIR) != 0;
    info->isReg = (s.st_mode & S_IFREG) != 0;
    info->isLink = 0;
    ext = mprGetPathExtension(path);
    if (ext && strcmp(ext, "lnk") == 0) {
        info->isLink = 1;
    }

#if !WINCE
    /*
        Work hard on windows to determine if the file is a regular file.
     */
    if (info->isReg) {
        long    att;

        if ((att = GetFileAttributes(path)) == -1) {
            mprFree(allocPath);
            return -1;
        }
        if (att & (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_ENCRYPTED |
                FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_OFFLINE)) {
            /*
                Catch accesses to devices like CON, AUX, NUL, LPT etc att will be set to ENCRYPTED on Win9X and NT.
             */
            info->isReg = 0;
        }
        if (info->isReg) {
            HANDLE handle;
            handle = CreateFile(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
            if (handle == INVALID_HANDLE_VALUE) {
                info->isReg = 0;
            } else {
                long    fileType;
                fileType = GetFileType(handle);
                if (fileType == FILE_TYPE_CHAR || fileType == FILE_TYPE_PIPE) {
                    info->isReg = 0;
                }
                CloseHandle(handle);
            }
        }
    }
    if (strcmp(path, "nul") == 0) {
        info->isReg = 0;
    }

#endif
    mprFree(allocPath);
#else /* !BLD_WIN_LIKE */
    mprAssert(path);
    mprAssert(info);

    info->valid = 0;
    info->checked = 1;

#if VXWORKS
    if (stat((char*) path, &s) < 0) {
        return MPR_ERR_CANT_ACCESS;
    }
#else
    if (lstat((char*) path, &s) < 0) {
        return MPR_ERR_CANT_ACCESS;
    }
#endif
    info->valid = 1;
    info->size = s.st_size;
    info->atime = s.st_atime;
    info->ctime = s.st_ctime;
    info->mtime = s.st_mtime;
    info->inode = s.st_ino;
    info->isDir = S_ISDIR(s.st_mode);
    info->isReg = S_ISREG(s.st_mode);
    info->perms = s.st_mode & 07777;
#ifdef S_ISLNK
    info->isLink = S_ISLNK(s.st_mode);
    if (info->isLink) {
        struct stat realInfo;
        if (stat((char*) path, &realInfo) < 0) {
            return MPR_ERR_CANT_ACCESS;
        }
        info->size = realInfo.st_size;
    }
#endif
    if (strcmp(path, "/dev/null") == 0) {
        info->isReg = 0;
    }
#endif
    return 0;
}
 
static char *getPathLink(MprDiskFileSystem *fileSystem, cchar *path)
{
#if BLD_UNIX_LIKE
    char    pbuf[MPR_MAX_PATH];
    int     len;

    if ((len = readlink(path, pbuf, sizeof(pbuf) - 1)) < 0) {
        return NULL;
    }
    pbuf[len] = '\0';
    return sclone(pbuf);
#else
    return NULL;
#endif
}


static int truncateFile(MprDiskFileSystem *fileSystem, cchar *path, MprOffset size)
{
    if (!mprPathExists(path, F_OK)) {
        return MPR_ERR_CANT_ACCESS;
    }
#if BLD_WIN_LIKE
{
    HANDLE  h;

    h = CreateFile(path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    SetFilePointer(h, size, 0, FILE_BEGIN);
    if (h == INVALID_HANDLE_VALUE || SetEndOfFile(h) == 0) {
        CloseHandle(h);
        return MPR_ERR_CANT_WRITE;
    }
    CloseHandle(h);
}
#elif VXWORKS
{
#if FUTURE
    int     fd;

    fd = open(path, O_WRONLY, 0664);
    if (fd < 0 || ftruncate(fd, size) < 0) {
        return MPR_ERR_CANT_WRITE;
    }
    close(fd);
#endif
    return MPR_ERR_CANT_WRITE;
}
#else
    if (truncate(path, size) < 0) {
        return MPR_ERR_CANT_WRITE;
    }
#endif
    return 0;
}


static void manageDiskFileSystem(MprDiskFileSystem *dfs, int flags)
{
#if !WINCE
    if (flags & MPR_MANAGE_MARK) {
        mprMark(dfs->stdError);
        mprMark(dfs->stdInput);
        mprMark(dfs->stdOutput);
        mprMark(dfs->separators);
        mprMark(dfs->newline);
        mprMark(dfs->root);
    }
#endif
}


MprDiskFileSystem *mprCreateDiskFileSystem(cchar *path)
{
    MprFileSystem       *fs;
    MprDiskFileSystem   *dfs;

    if ((dfs = mprAllocObj(MprDiskFileSystem, manageDiskFileSystem)) == 0) {
        return 0;
    }

    /*
        Temporary
     */
    fs = (MprFileSystem*) dfs;

    dfs->accessPath = accessPath;
    dfs->deletePath = deletePath;
    dfs->getPathInfo = getPathInfo;
    dfs->getPathLink = getPathLink;
    dfs->makeDir = makeDir;
    dfs->makeLink = makeLink;
    dfs->openFile = openFile;
    dfs->closeFile = mprCloseFile;
    dfs->readFile = readFile;
    dfs->seekFile = seekFile;
    dfs->truncateFile = truncateFile;
    dfs->writeFile = writeFile;

#if !WINCE
    dfs->stdError = mprAllocObj(MprFile, NULL);
    if (dfs->stdError == 0) {
        mprFree(dfs);
    }
    mprSetName(dfs->stdError, "stderr");
    dfs->stdError->fd = 2;
    dfs->stdError->fileSystem = fs;
    dfs->stdError->mode = O_WRONLY;

    dfs->stdInput = mprAllocObj(MprFile, NULL);
    if (dfs->stdInput == 0) {
        mprFree(dfs);
    }
    mprSetName(dfs->stdInput, "stdin");
    dfs->stdInput->fd = 0;
    dfs->stdInput->fileSystem = fs;
    dfs->stdInput->mode = O_RDONLY;

    dfs->stdOutput = mprAllocObj(MprFile, NULL);
    if (dfs->stdOutput == 0) {
        mprFree(dfs);
    }
    mprSetName(dfs->stdOutput, "stdout");
    dfs->stdOutput->fd = 1;
    dfs->stdOutput->fileSystem = fs;
    dfs->stdOutput->mode = O_WRONLY;
#endif
    return dfs;
}
#endif /* !BLD_FEATURE_ROMFS */


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

