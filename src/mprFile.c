/**
    mprFile.c - File services.

    This modules provides a simple cross platform file I/O abstraction. It uses the MprFileSystem to provide I/O services.
    This module is not thread safe.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/****************************** Forward Declarations **************************/

static ssize fillBuf(MprFile *file);
static void manageFile(MprFile *file, int flags);

/************************************ Code ************************************/

MprFile *mprAttachFileFd(int fd, cchar *name, int omode)
{
    MprFileSystem   *fs;
    MprFile         *file;

    fs = mprLookupFileSystem("/");

    if ((file = mprAllocObj(MprFile, manageFile)) != 0) {
        file->fd = fd;
        file->fileSystem = fs;
        file->path = sclone(name);
        file->mode = omode;
        file->attached = 1;
    }
    return file;
}


static void manageFile(MprFile *file, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(file->buf);
        mprMark(file->path);

    } else if (flags & MPR_MANAGE_FREE) {
        if (!file->attached) {
            mprCloseFile(file);
        }
    }
}


int mprFlushFile(MprFile *file)
{
    MprFileSystem   *fs;
    MprBuf          *bp;
    ssize           len, rc;

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
                return (int) rc;
            }
            mprAdjustBufStart(bp, rc);
        }
        mprFlushBuf(bp);
    }
    return 0;
}


//  MOB - naming vs mprSeekFile or mprSetFilePosition or mprTellFile
MprOff mprGetFilePosition(MprFile *file)
{
    return file->pos;
}


MprOff mprGetFileSize(MprFile *file)
{
    return file->size;
}


MprFile *mprGetStderr()
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(NULL);
    return fs->stdError;
}


MprFile *mprGetStdin()
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(NULL);
    return fs->stdInput;
}


MprFile *mprGetStdout()
{
    MprFileSystem   *fs;

    fs = mprLookupFileSystem(NULL);
    return fs->stdOutput;
}


/*
    Get a character from the file. This will put the file into buffered mode.
 */
int mprGetFileChar(MprFile *file)
{
    MprBuf      *bp;
    ssize     len;

    mprAssert(file);

    if (file == 0) {
        return MPR_ERR;
    }
    if (file->buf == 0) {
        file->buf = mprCreateBuf(MPR_BUFSIZE, MPR_BUFSIZE);
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


static char *findNewline(cchar *str, cchar *newline, ssize len, ssize *nlen)
{
    char    *start, *best;
    ssize   newlines;
    int     i;

    mprAssert(str);
    mprAssert(newline);
    mprAssert(nlen);
    mprAssert(len > 0);

    if (str == NULL || newline == NULL) {
        return NULL;
    }
    newlines = slen(newline);
    mprAssert(newlines == 1 || newlines == 2);

    start = best = NULL;
    *nlen = 0;
    for (i = 0; i < newlines; i++) {
        if ((start = memchr(str, newline[i], len)) != 0) {
            if (best == NULL || start < best) {
                best = start;
                *nlen = 1;
                if (newlines == 2 && best[1] == newline[!i]) {
                    (*nlen)++;
                }
            }
        }
    }
    return best;
}


//  MOB -- rename mprReadFileLine
/*
    Get a string from the file. This will put the file into buffered mode.
    Return NULL on eof.
 */
char *mprGetFileString(MprFile *file, ssize maxline, ssize *lenp)
{
    MprBuf          *bp;
    MprFileSystem   *fs;
    ssize           size, len, nlen, consumed;
    cchar           *eol, *newline, *start;
    char            *result;

    mprAssert(file);

    if (file == 0) {
        return NULL;
    }
    if (lenp) {
        *lenp = 0;
    }
    if (maxline <= 0) {
        maxline = MPR_BUFSIZE;
    }
    fs = file->fileSystem;
    newline = fs->newline;
    if (file->buf == 0) {
        file->buf = mprCreateBuf(maxline, maxline);
    }
    bp = file->buf;

    result = NULL;
    size = 0;
    do {
        if (mprGetBufLength(bp) == 0) {
            if (fillBuf(file) <= 0) {
                return result;
            }
        }
        start = mprGetBufStart(bp);
        len = mprGetBufLength(bp);
        if ((eol = findNewline(start, newline, len, &nlen)) != 0) {
            len = eol - start;
            consumed = len + nlen;
        } else {
            consumed = len;
        }
        file->pos += (MprOff) consumed;
        if (lenp) {
            *lenp += len;
        }
        if ((result = mprRealloc(result, size + len + 1)) == 0) {
            return NULL;
        }
        memcpy(&result[size], start, len);
        size += len;
        result[size] = '\0';
        mprAdjustBufStart(bp, consumed);
    } while (!eol);

    return result;
}


MprFile *mprOpenFile(cchar *path, int omode, int perms)
{
    MprFileSystem   *fs;
    MprFile         *file;
    MprPath         info;

    fs = mprLookupFileSystem(path);

    file = fs->openFile(fs, path, omode, perms);
    if (file) {
        file->fileSystem = fs;
        file->path = sclone(path);
        if (omode & (O_WRONLY | O_RDWR)) {
            /*
                OPT. Should compute this lazily.
             */
            fs->getPathInfo(fs, path, &info);
            file->size = (MprOff) info.size;
        }
        file->mode = omode;
        file->perms = perms;
    }
    return file;
}


int mprCloseFile(MprFile *file)
{
    MprFileSystem   *fs;

    if (file == 0) {
        return MPR_ERR_CANT_ACCESS;
    }
    fs = mprLookupFileSystem(file->path);
    return fs->closeFile(file);
}


/*
    Put a string to the file. This will put the file into buffered mode.
 */
ssize mprPutFileString(MprFile *file, cchar *str)
{
    MprBuf  *bp;
    ssize   total, bytes, count;
    char    *buf;

    mprAssert(file);
    count = slen(str);

    /*
        Buffer output and flush when full.
     */
    if (file->buf == 0) {
        file->buf = mprCreateBuf(MPR_BUFSIZE, 0);
        if (file->buf == 0) {
            return MPR_ERR_CANT_ALLOCATE;
        }
    }
    bp = file->buf;

    if (mprGetBufLength(bp) > 0 && mprGetBufSpace(bp) < count) {
        mprFlushFile(file);
    }
    total = 0;
    buf = (char*) str;

    while (count > 0) {
        bytes = mprPutBlockToBuf(bp, buf, count);
        if (bytes < 0) {
            return MPR_ERR_CANT_ALLOCATE;

        } else if (bytes == 0) {
            if (mprFlushFile(file) < 0) {
                return MPR_ERR_CANT_WRITE;
            }
            continue;
        }
        count -= bytes;
        buf += bytes;
        total += bytes;
        file->pos += (MprOff) bytes;
    }
    return total;
}


/*
    Peek at a character from the file without disturbing the read position. This will put the file into buffered mode.
 */
int mprPeekFileChar(MprFile *file)
{
    MprBuf      *bp;
    ssize       len;

    mprAssert(file);

    if (file == 0) {
        return MPR_ERR;
    }
    if (file->buf == 0) {
        file->buf = mprCreateBuf(MPR_BUFSIZE, MPR_BUFSIZE);
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
ssize mprPutFileChar(MprFile *file, int c)
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
    return mprWriteFile(file, &c, 1);
}


ssize mprReadFile(MprFile *file, void *buf, ssize size)
{
    MprFileSystem   *fs;
    MprBuf          *bp;
    ssize           bytes, totalRead;
    void            *bufStart;

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
    file->pos += (MprOff) totalRead;
    return totalRead;
}


MprOff mprSeekFile(MprFile *file, int seekType, MprOff pos)
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
                if (mprFlushFile(file) < 0) {
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


int mprTruncateFile(cchar *path, MprOff size)
{
    MprFileSystem   *fs;

    mprAssert(path && *path);

    if ((fs = mprLookupFileSystem(path)) == 0) {
        return MPR_ERR_CANT_OPEN;
    }
    return fs->truncateFile(fs, path, size);
}


ssize mprWriteFile(MprFile *file, cvoid *buf, ssize count)
{
    MprFileSystem   *fs;
    MprBuf          *bp;
    ssize           bytes, written;

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
                mprFlushFile(file);
            }
            count -= bytes;
            written += bytes;
            buf = (char*) buf + bytes;
        }
    }
    file->pos += (MprOff) written;
    if (file->pos > file->size) {
        file->size = file->pos;
    }
    return written;
}


ssize mprWriteFileString(MprFile *file, cchar *str)
{
    return mprWriteFile(file, str, slen(str));
}


//  MOB - rethink name
ssize mprWriteFileFormat(MprFile *file, cchar *fmt, ...)
{
    va_list     ap;
    char        *buf;
    ssize       rc;

    rc = -1;
    va_start(ap, fmt);
    if ((buf = sfmtv(fmt, ap)) != NULL) {
        rc = mprWriteFileString(file, buf);
    }
    va_end(ap);
    return rc;
}


/*
    Fill the read buffer. Return the new buffer length. Only called when the buffer is empty.
 */
static ssize fillBuf(MprFile *file)
{
    MprFileSystem   *fs;
    MprBuf          *bp;
    ssize           len;

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
int mprEnableFileBuffering(MprFile *file, ssize initialSize, ssize maxSize)
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
        file->buf = mprCreateBuf(initialSize, maxSize);
    }
    return 0;
}


void mprDisableFileBuffering(MprFile *file)
{
    mprFlushFile(file);
    file->buf = 0;
}


int mprGetFileFd(MprFile *file)
{
    return file->fd;
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
