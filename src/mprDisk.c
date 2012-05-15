/**
    mprDisk.c - File services for systems with a (disk) based file system.

    This module is not thread safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

#if !BIT_FEATURE_ROMFS
/*********************************** Defines **********************************/

#if WINDOWS
/*
    Open/Delete retries to circumvent windows pending delete problems
 */
#define RETRIES 40
#endif

/********************************** Forwards **********************************/

static int closeFile(MprFile *file);
static void manageDiskFile(MprFile *file, int flags);
static int getPathInfo(MprDiskFileSystem *fs, cchar *path, MprPath *info);

/************************************ Code ************************************/
#if FUTURE
/*
    Open a file with support for cygwin paths. Tries windows path first then under /cygwin.
 */
static int cygOpen(MprFileSystem *fs, cchar *path, int omode, int perms)
{
    int     fd;

    fd = open(path, omode, perms);
#if WINDOWS
    if (fd < 0) {
        if (*path == '/') {
            path = sjoin(fs->cygwin, path, NULL);
        }
        fd = open(path, omode, perms);
    }
#endif
    return fd;
}
#endif

static MprFile *openFile(MprFileSystem *fs, cchar *path, int omode, int perms)
{
    MprFile     *file;
    
    mprAssert(path);

    if ((file = mprAllocObj(MprFile, manageDiskFile)) == 0) {
        return NULL;
    }
    file->path = sclone(path);
    file->fd = open(path, omode, perms);
    if (file->fd < 0) {
#if WINDOWS
        /*
            Windows opens can fail of immediately following a delete. Windows uses pending deletes which prevent opens.
         */
        int i, err = GetLastError();
        if (err == ERROR_ACCESS_DENIED) {
            for (i = 0; i < RETRIES; i++) {
                file->fd = open(path, omode, perms);
                if (file->fd >= 0) {
                    break;
                }
                mprNap(10);
            }
            if (file->fd < 0) {
                file = NULL;
            }
        } else {
            file = NULL;
        }
#else
        file = NULL;
#endif
    }
    return file;
}


static void manageDiskFile(MprFile *file, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(file->path);
        mprMark(file->fileSystem);
        mprMark(file->buf);
#if BIT_FEATURE_ROMFS
        mprMark(file->inode);
#endif

    } else if (flags & MPR_MANAGE_FREE) {
        closeFile(file);
    }
}


static int closeFile(MprFile *file)
{
    MprBuf  *bp;

    mprAssert(file);

    if (file == 0) {
        return 0;
    }
    bp = file->buf;
    if (bp && (file->mode & (O_WRONLY | O_RDWR))) {
        mprFlushFile(file);
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


static MprOff seekFile(MprFile *file, int seekType, MprOff distance)
{
    mprAssert(file);

    if (file == 0) {
        return MPR_ERR_BAD_HANDLE;
    }
#if BIT_WIN_LIKE
    return (MprOff) _lseeki64(file->fd, (int64) distance, seekType);
#elif HAS_OFF64
    return (MprOff) lseek64(file->fd, (off64_t) distance, seekType);
#else
    return (MprOff) lseek(file->fd, (off_t) distance, seekType);
#endif
}


static bool accessPath(MprDiskFileSystem *fs, cchar *path, int omode)
{
#if BIT_WIN && FUTURE
    if (access(path, omode) < 0) {
        if (*path == '/') {
            path = sjoin(fs->cygwin, path, NULL);
        }
    }
#endif
    return access(path, omode) == 0;
}


static int deletePath(MprDiskFileSystem *fs, cchar *path)
{
    MprPath     info;

    if (getPathInfo(fs, path, &info) == 0 && info.isDir) {
        return rmdir((char*) path);
    }
#if WINDOWS
{
    /*
        NOTE: Windows delete makes a file pending delete which prevents immediate recreation. Rename and then delete.
     */
    int i, err;
    for (i = 0; i < RETRIES; i++) {
        if (DeleteFile((char*) path) != 0) {
            return 0;
        }
        err = GetLastError();
        if (err != ERROR_SHARING_VIOLATION) {
            break;
        }
        mprNap(10);
    }
    return MPR_ERR_CANT_DELETE;
}
#else
    return unlink((char*) path);
#endif
}
 

static int makeDir(MprDiskFileSystem *fs, cchar *path, int perms, int owner, int group)
{
    int     rc;

#if VXWORKS
    rc = mkdir((char*) path);
#else
    rc = mkdir(path, perms);
#endif
    if (rc < 0) {
        return MPR_ERR_CANT_CREATE;
    }
#if BIT_UNIX_LIKE
    if ((owner != -1 || group != -1) && chown(path, owner, group) < 0) {
        rmdir(path);
        return MPR_ERR_CANT_COMPLETE;
    }
#endif
    return 0;
}


static int makeLink(MprDiskFileSystem *fs, cchar *path, cchar *target, int hard)
{
#if BIT_UNIX_LIKE
    if (hard) {
        return link(target, path);
    } else {
        return symlink(target, path);
    }
#else
    return MPR_ERR_BAD_STATE;
#endif
}


static int getPathInfo(MprDiskFileSystem *fs, cchar *path, MprPath *info)
{
#if WINCE
    struct stat s;
    cchar       *ext;

    mprAssert(path);
    mprAssert(info);

    info->checked = 1;
    info->valid = 0;
    info->isReg = 0;
    info->isDir = 0;

    if (_stat64(path, &s) < 0) {
        return -1;
    }
    info->valid = 1;
    info->size = s.st_size;
    info->atime = s.st_atime;
    info->ctime = s.st_ctime;
    info->mtime = s.st_mtime;
    info->perms = s.st_mode & 07777;
    info->owner = s.st_uid;
    info->group = s.st_gid;
    info->inode = s.st_ino;
    info->isDir = (s.st_mode & S_IFDIR) != 0;
    info->isReg = (s.st_mode & S_IFREG) != 0;
    info->isLink = 0;
    ext = mprGetPathExt(path);
    if (ext && strcmp(ext, "lnk") == 0) {
        info->isLink = 1;
    }

#elif BIT_WIN_LIKE
    struct __stat64     s;
    cchar               *ext;

    mprAssert(path);
    mprAssert(info);
    info->checked = 1;
    info->valid = 0;
    info->isReg = 0;
    info->isDir = 0;
    if (_stat64(path, &s) < 0) {
#if BIT_WIN && FUTURE
        /*
            Try under /cygwin
         */
        if (*path == '/') {
            path = sjoin(fs->cygwin, path, NULL);
        }
        if (_stat64(path, &s) < 0) {
            return -1;
        }
#else
        return -1;
#endif
    }
    ext = mprGetPathExt(path);
    info->valid = 1;
    info->size = s.st_size;
    info->atime = s.st_atime;
    info->ctime = s.st_ctime;
    info->mtime = s.st_mtime;
    info->perms = s.st_mode & 07777;
    info->owner = s.st_uid;
    info->group = s.st_gid;
    info->inode = s.st_ino;
    info->isDir = (s.st_mode & S_IFDIR) != 0;
    info->isReg = (s.st_mode & S_IFREG) != 0;
    info->isLink = 0;
    if (ext) {
        if (strcmp(ext, "lnk") == 0) {
            info->isLink = 1;
        } else if (strcmp(ext, "dll") == 0) {
            info->perms |= 111;
        }
    }
    /*
        Work hard on windows to determine if the file is a regular file.
     */
    if (info->isReg) {
        long    att;

        if ((att = GetFileAttributes(path)) == -1) {
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

#elif VXWORKS
    struct stat s;
    info->valid = 0;
    info->isReg = 0;
    info->isDir = 0;
    info->checked = 1;
    if (stat((char*) path, &s) < 0) {
        return MPR_ERR_CANT_ACCESS;
    }
    info->valid = 1;
    info->size = s.st_size;
    info->atime = s.st_atime;
    info->ctime = s.st_ctime;
    info->mtime = s.st_mtime;
    info->inode = s.st_ino;
    info->isDir = S_ISDIR(s.st_mode);
    info->isReg = S_ISREG(s.st_mode);
    info->perms = s.st_mode & 07777;
    info->owner = s.st_uid;
    info->group = s.st_gid;
#else
    struct stat s;
    info->valid = 0;
    info->isReg = 0;
    info->isDir = 0;
    info->checked = 1;
    if (lstat((char*) path, &s) < 0) {
        return MPR_ERR_CANT_ACCESS;
    }
    #ifdef S_ISLNK
        info->isLink = S_ISLNK(s.st_mode);
        if (info->isLink) {
            if (stat((char*) path, &s) < 0) {
                return MPR_ERR_CANT_ACCESS;
            }
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
    info->owner = s.st_uid;
    info->group = s.st_gid;
    if (strcmp(path, "/dev/null") == 0) {
        info->isReg = 0;
    }
#endif
    return 0;
}
 
static char *getPathLink(MprDiskFileSystem *fs, cchar *path)
{
#if BIT_UNIX_LIKE
    char    pbuf[MPR_MAX_PATH];
    ssize   len;

    if ((len = readlink(path, pbuf, sizeof(pbuf) - 1)) < 0) {
        return NULL;
    }
    pbuf[len] = '\0';
    return sclone(pbuf);
#else
    return NULL;
#endif
}


static int truncateFile(MprDiskFileSystem *fs, cchar *path, MprOff size)
{
    if (!mprPathExists(path, F_OK)) {
#if BIT_WIN_LIKE && FUTURE
        /*
            Try under /cygwin
         */
        if (*path == '/') {
            path = sjoin(fs->cygwin, path, NULL);
        }
        if (!mprPathExists(path, F_OK))
#endif
        return MPR_ERR_CANT_ACCESS;
    }
#if BIT_WIN_LIKE
{
    HANDLE  h;

    h = CreateFile(path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    SetFilePointer(h, (LONG) size, 0, FILE_BEGIN);
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
        mprMark(dfs->separators);
        mprMark(dfs->newline);
        mprMark(dfs->root);
#if BIT_WIN_LIKE || CYGWIN
        mprMark(dfs->cygdrive);
        mprMark(dfs->cygwin);
#endif
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
    dfs->closeFile = closeFile;
    dfs->readFile = readFile;
    dfs->seekFile = seekFile;
    dfs->truncateFile = truncateFile;
    dfs->writeFile = writeFile;

#if !WINCE
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
#endif
    return dfs;
}
#endif /* !BIT_FEATURE_ROMFS */


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

