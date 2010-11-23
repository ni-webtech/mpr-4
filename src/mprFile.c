/**
    mprFile.c - File services.

    This modules provides a simple cross platform file I/O abstraction. It uses the MprFileSystem to provide I/O services.
    This module is not thread safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/****************************** Forward Declarations **************************/

static int fillBuf(MprFile *file);
static void manageFile(MprFile *file, int flags);

/************************************ Code ************************************/

MprFile *mprAttachFd(MprCtx ctx, int fd, cchar *name, int omode)
{
    MprFileSystem   *fs;
    MprFile         *file;

    fs = mprLookupFileSystem(ctx, "/");

    file = mprAllocObj(ctx, MprFile, manageFile);
    if (file) {
        file->fd = fd;
        file->fileSystem = fs;
        file->path = sclone(file, name);
        file->mode = omode;
    }
    return file;
}


static void manageFile(MprFile *file, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(file->buf);
        mprMark(file->path);

    } else if (flags & MPR_MANAGE_FREE) {
        mprCloseFile(file);
    }
}


int mprFlush(MprFile *file)
{
    MprFileSystem   *fs;
    MprBuf          *bp;
    int             len, rc;

    mprAssert(file);
    if (file == 0) {
        return MPR_ERR_BAD_HANDLE;
    }
    if (file->buf == 0) {
        return 0;
    }

    if (file->mode & (O_WRONLY | O_RDWR)) {
        fs = file->fileSystem;
        bp = file->buf;
        while (mprGetBufLength(bp) > 0) {
            len = mprGetBufLength(bp);
            rc = fs->writeFile(file, mprGetBufStart(bp), len);
            if (rc < 0) {
                return rc;
            }
            mprAdjustBufStart(bp, rc);
        }
        mprFlushBuf(bp);
    }
    return 0;
}


MprOffset mprGetFilePosition(MprFile *file)
{
    return file->pos;
}


MprOffset mprGetFileSize(MprFile *file)
{
    return file->size;
}


MprFile *mprGetStderr(MprCtx ctx)
{
    MprFileSystem   *fs;
    fs = mprLookupFileSystem(ctx, NULL);
    return fs->stdError;
}


MprFile *mprGetStdin(MprCtx ctx)
{
    MprFileSystem   *fs;
    fs = mprLookupFileSystem(ctx, NULL);
    return fs->stdInput;
}


MprFile *mprGetStdout(MprCtx ctx)
{
    MprFileSystem   *fs;
    fs = mprLookupFileSystem(ctx, NULL);
    return fs->stdOutput;
}


/*
    Get a character from the file. This will put the file into buffered mode.
 */
int mprGetc(MprFile *file)
{
    MprBuf  *bp;
    int     len;

    mprAssert(file);

    if (file == 0) {
        return MPR_ERR;
    }
    if (file->buf == 0) {
        file->buf = mprCreateBuf(file, MPR_BUFSIZE, MPR_MAX_STRING);
    }
    bp = file->buf;

    if (mprGetBufLength(bp) == 0) {
        len = fillBuf(file);
        if (len <= 0) {
            return -1;
        }
    }
    if (mprGetBufLength(bp) == 0) {
        return 0;
    }
    file->pos++;
    return mprGetCharFromBuf(bp);
}


/*
    Get a string from the file. This will put the file into buffered mode.
    Return NULL on eof.
 */
char *mprGets(MprFile *file, size_t size, int *lenp)
{
    MprBuf          *bp;
    MprFileSystem   *fs;
    cchar           *eol, *newline;
    size_t          len;
    int             count;

    mprAssert(file);
    if (file == 0) {
        return NULL;
    }
    if (size == 0) {
        size = MAXSIZE;
    }
    fs = file->fileSystem;
    newline = fs->newline;
    if (file->buf == 0) {
        file->buf = mprCreateBuf(file, size, size);
    }
    bp = file->buf;
    /*
        Must leave room for null
     */
    count = 0;
    while (--size > 0) {
        if (mprGetBufLength(bp) == 0) {
            if (fillBuf(file) <= 0) {
                return NULL;
            }
        }
        if ((eol = scontains(mprGetBufStart(bp), newline, mprGetBufLength(bp))) != 0) {
            len = eol - mprGetBufStart(bp);
            if (lenp) {
                *lenp = len;
            }
            file->pos += len + slen(newline);
            return ssub(NULL, mprGetBufStart(bp), 0, len);
        }
    }
    file->pos += size;
    return ssub(NULL, mprGetBufStart(bp), 0, size);
}


MprFile *mprOpen(MprCtx ctx, cchar *path, int omode, int perms)
{
    MprFileSystem   *fs;
    MprFile         *file;
    MprPath         info;

    fs = mprLookupFileSystem(ctx, path);

    file = fs->openFile(ctx, fs, path, omode, perms);
    if (file) {
        file->fileSystem = fs;
        file->path = sclone(file, path);
        if (omode & (O_WRONLY | O_RDWR)) {
            /*
                OPT. Should compute this lazily.
             */
            fs->getPathInfo(fs, path, &info);
            file->size = (MprOffset) info.size;
        }
        file->mode = omode;
        file->perms = perms;
    }
    return file;
}


/*
    Put a string to the file. This will put the file into buffered mode.
 */
int mprPuts(MprFile *file, cchar *str)
{
    MprBuf  *bp;
    char    *buf;
    size_t  total, bytes, count;

    mprAssert(file);
    count = strlen(str);

    /*
        Buffer output and flush when full.
     */
    if (file->buf == 0) {
        file->buf = mprCreateBuf(file, MPR_BUFSIZE, 0);
        if (file->buf == 0) {
            return MPR_ERR_CANT_ALLOCATE;
        }
    }
    bp = file->buf;

    if (mprGetBufLength(bp) > 0 && mprGetBufSpace(bp) < count) {
        mprFlush(file);
    }
    total = 0;
    buf = (char*) str;

    while (count > 0) {
        bytes = mprPutBlockToBuf(bp, buf, count);
        if (bytes < 0) {
            return MPR_ERR_CANT_ALLOCATE;

        } else if (bytes == 0) {
            if (mprFlush(file) < 0) {
                return MPR_ERR_CANT_WRITE;
            }
            continue;
        }
        count -= bytes;
        buf += bytes;
        total += bytes;
        file->pos += bytes;
    }
    return total;
}


/*
    Peek at a character from the file without disturbing the read position. This will put the file into buffered mode.
 */
int mprPeekc(MprFile *file)
{
    MprBuf  *bp;
    int     len;

    mprAssert(file);

    if (file == 0) {
        return MPR_ERR;
    }

    if (file->buf == 0) {
        file->buf = mprCreateBuf(file, MPR_BUFSIZE, MPR_MAX_STRING);
    }
    bp = file->buf;

    if (mprGetBufLength(bp) == 0) {
        len = fillBuf(file);
        if (len <= 0) {
            return -1;
        }
    }
    if (mprGetBufLength(bp) == 0) {
        return 0;
    }
    return ((uchar*) mprGetBufStart(bp))[0];
}


/*
    Put a character to the file. This will put the file into buffered mode.
 */
int mprPutc(MprFile *file, int c)
{
    mprAssert(file);

    if (file == 0) {
        return -1;
    }
    if (file->buf) {
        if (mprPutCharToBuf(file->buf, c) != 1) {
            return MPR_ERR_CANT_WRITE;
        }
        file->pos++;
        return 1;

    }
    return mprWrite(file, &c, 1);
}


int mprRead(MprFile *file, void *buf, size_t size)
{
    MprFileSystem   *fs;
    MprBuf          *bp;
    void            *bufStart;
    size_t          bytes, totalRead;

    mprAssert(file);
    if (file == 0) {
        return MPR_ERR_BAD_HANDLE;
    }
    fs = file->fileSystem;
    bp = file->buf;
    if (bp == 0) {
        totalRead = fs->readFile(file, buf, size);

    } else {
        bufStart = buf;
        while (size > 0) {
            if (mprGetBufLength(bp) == 0) {
                bytes = fillBuf(file);
                if (bytes <= 0) {
                    return -1;
                }
            }
            bytes = min(size, mprGetBufLength(bp));
            memcpy(buf, mprGetBufStart(bp), bytes);
            mprAdjustBufStart(bp, bytes);
            buf = (void*) (((char*) buf) + bytes);
            size -= bytes;
        }
        totalRead = ((char*) buf - (char*) bufStart);
    }
    file->pos += totalRead;
    return totalRead;
}


MprOffset mprSeek(MprFile *file, int seekType, MprOffset pos)
{
    MprFileSystem   *fs;

    mprAssert(file);
    fs = file->fileSystem;

    if (file->buf) {
        if (! (seekType == SEEK_CUR && pos == 0)) {
            /*
                Discard buffering as we may be seeking outside the buffer.
                OPT. Could be smarter about this and preserve the buffer.
             */
            if (file->mode & (O_WRONLY | O_RDWR)) {
                if (mprFlush(file) < 0) {
                    return MPR_ERR_CANT_WRITE;
                }
            }
            if (file->buf) {
                mprFlushBuf(file->buf);
            }
        }
    }
    if (seekType == SEEK_SET) {
        file->pos = pos;
    } else if (seekType == SEEK_CUR) {
        file->pos += pos;
    } else {
        file->pos = fs->seekFile(file, SEEK_END, 0);
    }
    if (fs->seekFile(file, SEEK_SET, (long) file->pos) != (long) file->pos) {
        return MPR_ERR;
    }
    if (file->mode & (O_WRONLY | O_RDWR)) {
        if (file->pos > file->size) {
            file->size = file->pos;
        }
    }
    return file->pos;
}


int mprTruncate(cchar *path, MprOffset size)
{
    MprFileSystem   *fs;

    mprAssert(path && *path);

    if ((fs = mprLookupFileSystem(NULL, path)) == 0) {
        return MPR_ERR_CANT_OPEN;
    }
    return fs->truncateFile(fs, path, size);
}


int mprWrite(MprFile *file, cvoid *buf, size_t count)
{
    MprFileSystem   *fs;
    MprBuf          *bp;
    size_t          bytes, written;

    mprAssert(file);
    if (file == 0) {
        return MPR_ERR_BAD_HANDLE;
    }

    fs = file->fileSystem;
    bp = file->buf;
    if (bp == 0) {
        if ((written = fs->writeFile(file, buf, count)) < 0) {
            return written;
        }
    } else {
        written = 0;
        while (count > 0) {
            bytes = mprPutBlockToBuf(bp, buf, count);
            if (bytes < 0) {
                return bytes;
            } 
            if (bytes != count) {
                mprFlush(file);
            }
            count -= bytes;
            written += bytes;
            buf = (char*) buf + bytes;
        }
    }
    file->pos += written;
    if (file->pos > file->size) {
        file->size = file->pos;
    }
    return written;
}


int mprWriteString(MprFile *file, cchar *str)
{
    return mprWrite(file, str, strlen(str));
}


int mprWriteFormat(MprFile *file, cchar *fmt, ...)
{
    va_list     ap;
    char        *buf;
    int         rc;

    rc = -1;
    va_start(ap, fmt);
    if ((buf = mprAsprintfv(file, fmt, ap)) != NULL) {
        rc = mprWriteString(file, buf);
        mprFree(buf);
    }
    va_end(ap);
    return rc;
}


/*
    Fill the read buffer. Return the new buffer length. Only called when the buffer is empty.
 */
static int fillBuf(MprFile *file)
{
    MprFileSystem   *fs;
    MprBuf          *bp;
    int             len;

    bp = file->buf;
    fs = file->fileSystem;

    mprAssert(mprGetBufLength(bp) == 0);
    mprFlushBuf(bp);

    len = fs->readFile(file, mprGetBufStart(bp), mprGetBufSpace(bp));
    if (len <= 0) {
        return len;
    }
    mprAdjustBufEnd(bp, len);
    return len;
}


/*
    Enable and control file buffering
 */
int mprEnableFileBuffering(MprFile *file, int initialSize, int maxSize)
{
    mprAssert(file);

    if (file == 0) {
        return MPR_ERR_BAD_STATE;
    }
    if (initialSize <= 0) {
        initialSize = MPR_BUFSIZE;
    }
    if (maxSize <= 0) {
        maxSize = MPR_BUFSIZE;
    }
    if (maxSize <= initialSize) {
        maxSize = initialSize;
    }
    if (file->buf == 0) {
        file->buf = mprCreateBuf(file, initialSize, maxSize);
    }
    return 0;
}


void mprDisableFileBuffering(MprFile *file)
{
    mprFlush(file);
    mprFree(file->buf);
    file->buf = 0;
}


int mprGetFileFd(MprFile *file)
{
    return file->fd;
}

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
