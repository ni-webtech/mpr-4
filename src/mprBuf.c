/**
    mprBuf.c - Dynamic buffer module

    This module is not thread-safe for performance. Callers must do their own locking.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/********************************** Forwards **********************************/

static void manageBuf(MprBuf *buf, int flags);

/*********************************** Code *************************************/
/*
    Create a new buffer. "maxsize" is the limit to which the buffer can ever grow. -1 means no limit. "initialSize" is 
    used to define the amount to increase the size of the buffer each time if it becomes full. (Note: mprGrowBuf() will 
    exponentially increase this number for performance.)
 */
MprBuf *mprCreateBuf(ssize initialSize, ssize maxSize)
{
    MprBuf      *bp;
    
    if (initialSize <= 0) {
        initialSize = MPR_DEFAULT_ALLOC;
    }
    if ((bp = mprAllocObj(MprBuf, manageBuf)) == 0) {
        return 0;
    }
    bp->growBy = MPR_BUFSIZE;
    mprSetBufSize(bp, initialSize, maxSize);
    return bp;
}


static void manageBuf(MprBuf *bp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(bp->data);
        mprMark(bp->refillArg);
    } 
}


MprBuf *mprCloneBuf(MprBuf *orig)
{
    MprBuf      *bp;
    ssize       len;

    if ((bp = mprCreateBuf(orig->growBy, orig->maxsize)) == 0) {
        return 0;
    }
    bp->refillProc = orig->refillProc;
    bp->refillArg = orig->refillArg;
    if ((len = mprGetBufLength(orig)) > 0) {
        memcpy(bp->data, orig->data, len);
    }
    return bp;
}


char *mprGet(MprBuf *bp)
{
    return (char*) bp->start;
}


/*
    Set the current buffer size and maximum size limit.
 */
int mprSetBufSize(MprBuf *bp, ssize initialSize, ssize maxSize)
{
    mprAssert(bp);

    if (initialSize <= 0) {
        if (maxSize > 0) {
            bp->maxsize = maxSize;
        }
        return 0;
    }
    if (maxSize > 0 && initialSize > maxSize) {
        initialSize = maxSize;
    }
    mprAssert(initialSize > 0);

    if (bp->data) {
        /*
            Buffer already exists
         */
        if (bp->buflen < initialSize) {
            if (mprGrowBuf(bp, initialSize - bp->buflen) < 0) {
                return MPR_ERR_MEMORY;
            }
        }
        bp->maxsize = maxSize;
        return 0;
    }
    if ((bp->data = mprAlloc(initialSize)) == 0) {
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    bp->growBy = initialSize;
    bp->maxsize = maxSize;
    bp->buflen = initialSize;
    bp->endbuf = &bp->data[bp->buflen];
    bp->start = bp->data;
    bp->end = bp->data;
    *bp->start = '\0';
    return 0;
}


void mprSetBufMax(MprBuf *bp, ssize max)
{
    bp->maxsize = max;
}


/*
    This appends a silent null. It does not count as one of the actual bytes in the buffer
 */
void mprAddNullToBuf(MprBuf *bp)
{
    ssize      space;

    space = bp->endbuf - bp->end;
    if (space < sizeof(char)) {
        if (mprGrowBuf(bp, 1) < 0) {
            return;
        }
    }
    mprAssert(bp->end < bp->endbuf);
    if (bp->end < bp->endbuf) {
        *((char*) bp->end) = (char) '\0';
    }
}


void mprAdjustBufEnd(MprBuf *bp, ssize size)
{
    mprAssert(bp->buflen == (bp->endbuf - bp->data));
    mprAssert(size <= bp->buflen);
    mprAssert((bp->end + size) >= bp->data);
    mprAssert((bp->end + size) <= bp->endbuf);

    bp->end += size;
    if (bp->end > bp->endbuf) {
        mprAssert(bp->end <= bp->endbuf);
        bp->end = bp->endbuf;
    }
    if (bp->end < bp->data) {
        bp->end = bp->data;
    }
}


/*
    Adjust the start pointer after a user copy. Note: size can be negative.
 */
void mprAdjustBufStart(MprBuf *bp, ssize size)
{
    mprAssert(bp->buflen == (bp->endbuf - bp->data));
    mprAssert(size <= bp->buflen);
    mprAssert((bp->start + size) >= bp->data);
    mprAssert((bp->start + size) <= bp->end);

    bp->start += size;
    if (bp->start > bp->end) {
        bp->start = bp->end;
    }
    if (bp->start <= bp->data) {
        bp->start = bp->data;
    }
}


void mprFlushBuf(MprBuf *bp)
{
    bp->start = bp->data;
    bp->end = bp->data;
}


int mprGetCharFromBuf(MprBuf *bp)
{
    if (bp->start == bp->end) {
        return -1;
    }
    return (uchar) *bp->start++;
}


ssize mprGetBlockFromBuf(MprBuf *bp, char *buf, ssize size)
{
    ssize     thisLen, bytesRead;

    mprAssert(buf);
    mprAssert(size >= 0);
    mprAssert(bp->buflen == (bp->endbuf - bp->data));

    /*
        Get the max bytes in a straight copy
     */
    bytesRead = 0;
    while (size > 0) {
        thisLen = mprGetBufLength(bp);
        thisLen = min(thisLen, size);
        if (thisLen <= 0) {
            break;
        }

        memcpy(buf, bp->start, thisLen);
        buf += thisLen;
        bp->start += thisLen;
        size -= thisLen;
        bytesRead += thisLen;
    }
    return bytesRead;
}


ssize mprGetBufLength(MprBuf *bp)
{
    return (bp->end - bp->start);
}


ssize mprGetBufSize(MprBuf *bp)
{
    return bp->buflen;
}


ssize mprGetBufSpace(MprBuf *bp)
{
    return (bp->endbuf - bp->end);
}


char *mprGetBuf(MprBuf *bp)
{
    return (char*) bp->data;
}


char *mprGetBufStart(MprBuf *bp)
{
    return (char*) bp->start;
}


char *mprGetBufEnd(MprBuf *bp)
{
    return (char*) bp->end;
}


//  TODO - rename mprPutbackCharToBuf as it really can't insert if the buffer is empty

int mprInsertCharToBuf(MprBuf *bp, int c)
{
    if (bp->start == bp->data) {
        return MPR_ERR_BAD_STATE;
    }
    *--bp->start = c;
    return 0;
}


int mprLookAtNextCharInBuf(MprBuf *bp)
{
    if (bp->start == bp->end) {
        return -1;
    }
    return *bp->start;
}


int mprLookAtLastCharInBuf(MprBuf *bp)
{
    if (bp->start == bp->end) {
        return -1;
    }
    return bp->end[-1];
}


int mprPutCharToBuf(MprBuf *bp, int c)
{
    char       *cp;
    ssize      space;

    mprAssert(bp->buflen == (bp->endbuf - bp->data));

    space = bp->buflen - mprGetBufLength(bp);
    if (space < sizeof(char)) {
        if (mprGrowBuf(bp, 1) < 0) {
            return -1;
        }
    }
    cp = (char*) bp->end;
    *cp++ = (char) c;
    bp->end = (char*) cp;

    if (bp->end < bp->endbuf) {
        *((char*) bp->end) = (char) '\0';
    }
    return 1;
}


/*
    Return the number of bytes written to the buffer. If no more bytes will fit, may return less than size.
    Never returns < 0.
 */
ssize mprPutBlockToBuf(MprBuf *bp, cchar *str, ssize size)
{
    ssize      thisLen, bytes, space;

    mprAssert(str);
    mprAssert(size >= 0);
    mprAssert(size < MAXINT);

    bytes = 0;
    while (size > 0) {
        space = mprGetBufSpace(bp);
        thisLen = min(space, size);
        if (thisLen <= 0) {
            if (mprGrowBuf(bp, size) < 0) {
                break;
            }
            space = mprGetBufSpace(bp);
            thisLen = min(space, size);
        }
        memcpy(bp->end, str, thisLen);
        str += thisLen;
        bp->end += thisLen;
        size -= thisLen;
        bytes += thisLen;
    }
    if (bp->end < bp->endbuf) {
        *((char*) bp->end) = (char) '\0';
    }
    return bytes;
}


ssize mprPutStringToBuf(MprBuf *bp, cchar *str)
{
    if (str) {
        return mprPutBlockToBuf(bp, str, slen(str));
    }
    return 0;
}


ssize mprPutSubStringToBuf(MprBuf *bp, cchar *str, ssize count)
{
    ssize     len;

    if (str) {
        len = slen(str);
        len = min(len, count);
        if (len > 0) {
            return mprPutBlockToBuf(bp, str, len);
        }
    }
    return 0;
}


ssize mprPutPadToBuf(MprBuf *bp, int c, ssize count)
{
    mprAssert(count < MAXINT);

    while (count-- > 0) {
        if (mprPutCharToBuf(bp, c) < 0) {
            return -1;
        }
    }
    return count;
}


ssize mprPutFmtToBuf(MprBuf *bp, cchar *fmt, ...)
{
    va_list     ap;
    char        *buf;

    if (fmt == 0) {
        return 0;
    }
    va_start(ap, fmt);
    buf = sfmtv(fmt, ap);
    va_end(ap);
    return mprPutStringToBuf(bp, buf);
}


/*
    Grow the buffer. Return 0 if the buffer grows. Increase by the growBy size specified when creating the buffer. 
 */
int mprGrowBuf(MprBuf *bp, ssize need)
{
    char    *newbuf;
    ssize   growBy;

    if (bp->maxsize > 0 && bp->buflen >= bp->maxsize) {
        return MPR_ERR_TOO_MANY;
    }
    if (bp->start > bp->end) {
        mprCompactBuf(bp);
    }
    if (need > 0) {
        growBy = max(bp->growBy, need);
    } else {
        growBy = bp->growBy;
    }
    if ((newbuf = mprAlloc(bp->buflen + growBy)) == 0) {
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    if (bp->data) {
        memcpy(newbuf, bp->data, bp->buflen);
    }
    bp->buflen += growBy;
    bp->end = newbuf + (bp->end - bp->data);
    bp->start = newbuf + (bp->start - bp->data);
    bp->data = newbuf;
    bp->endbuf = &bp->data[bp->buflen];

    /*
        Increase growBy to reduce overhead
     */
    if (bp->maxsize > 0) {
        if ((bp->buflen + (bp->growBy * 2)) > bp->maxsize) {
            bp->growBy = min(bp->maxsize - bp->buflen, bp->growBy * 2);
        }
    } else {
        if ((bp->buflen + (bp->growBy * 2)) > bp->maxsize) {
            bp->growBy = min(bp->buflen, bp->growBy * 2);
        }
    }
    return 0;
}


/*
    Add a number to the buffer (always null terminated).
 */
ssize mprPutIntToBuf(MprBuf *bp, int64 i)
{
    ssize       rc;

    rc = mprPutStringToBuf(bp, itos(i));
    if (bp->end < bp->endbuf) {
        *((char*) bp->end) = (char) '\0';
    }
    return rc;
}


void mprCompactBuf(MprBuf *bp)
{
    if (mprGetBufLength(bp) == 0) {
        mprFlushBuf(bp);
        return;
    }
    if (bp->start > bp->data) {
        memmove(bp->data, bp->start, (bp->end - bp->start));
        bp->end -= (bp->start - bp->data);
        bp->start = bp->data;
    }
}


MprBufProc mprGetBufRefillProc(MprBuf *bp) 
{
    return bp->refillProc;
}


void mprSetBufRefillProc(MprBuf *bp, MprBufProc fn, void *arg)
{ 
    bp->refillProc = fn; 
    bp->refillArg = arg; 
}


int mprRefillBuf(MprBuf *bp) 
{ 
    return (bp->refillProc) ? (bp->refillProc)(bp, bp->refillArg) : 0; 
}


void mprResetBufIfEmpty(MprBuf *bp)
{
    if (mprGetBufLength(bp) == 0) {
        mprFlushBuf(bp);
    }
}


#if BLD_CHAR_LEN > 1
void mprAddNullToWideBuf(MprBuf *bp)
{
    ssize      space;

    space = bp->endbuf - bp->end;
    if (space < sizeof(MprChar)) {
        if (mprGrowBuf(bp, sizeof(MprChar)) < 0) {
            return;
        }
    }
    mprAssert(bp->end < bp->endbuf);
    if (bp->end < bp->endbuf) {
        *((MprChar*) bp->end) = (char) '\0';
    }
}


int mprPutCharToWideBuf(MprBuf *bp, int c)
{
    MprChar *cp;
    int     space;

    mprAssert(bp->buflen == (bp->endbuf - bp->data));

    space = bp->buflen - mprGetBufLength(bp);
    if (space < (sizeof(MprChar) * 2)) {
        if (mprGrowBuf(bp, sizeof(MprChar) * 2) < 0) {
            return -1;
        }
    }
    cp = (MprChar*) bp->end;
    *cp++ = (MprChar) c;
    bp->end = (char*) cp;

    if (bp->end < bp->endbuf) {
        *((MprChar*) bp->end) = (char) '\0';
    }
    return 1;
}


int mprPutFmtToWideBuf(MprBuf *bp, cchar *fmt, ...)
{
    va_list     ap;
    MprChar     *wbuf;
    char        *buf;
    ssize       len;
    int         rc, space;

    if (fmt == 0) {
        return 0;
    }
    va_start(ap, fmt);
    space = mprGetBufSpace(bp);
    space += (bp->maxsize - bp->buflen);
    buf = sfmtv(fmt, ap);
    wbuf = amtow(bp, buf, &len);
    rc = mprPutBlockToBuf(bp, (char*) wbuf, len * sizeof(MprChar));
    va_end(ap);
    return rc;
}


int mprPutStringToWideBuf(MprBuf *bp, cchar *str)
{
    MprChar     *wstr;
    ssize       len;

    if (str) {
        wstr = amtow(bp, str, &len);
        return mprPutBlockToBuf(bp, (char*) wstr, len);
    }
    return 0;
}

#endif /* BLD_CHAR_LEN > 1 */

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
