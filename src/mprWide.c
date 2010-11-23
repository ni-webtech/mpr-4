/**
    mprUnicode.c - Memory Allocator and Garbage Collector. 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if BLD_CHAR_LEN > 1
/************************************ Code ************************************/
/*
    Format a number as a string. Support radix 10 and 16.
 */
MprChar *itow(MprChar *buf, size_t count, int64 value, int radix)
{
    MprChar     numBuf[32];
    MprChar     *cp, *dp, *endp;
    char        digits[] = "0123456789ABCDEF";
    int         negative;

    if (radix != 10 && radix != 16) {
        return 0;
    }
    cp = &numBuf[sizeof(numBuf)];
    *--cp = '\0';

    if (value < 0) {
        negative = 1;
        value = -value;
        count--;
    } else {
        negative = 0;
    }
    do {
        *--cp = digits[value % radix];
        value /= radix;
    } while (value > 0);

    if (negative) {
        *--cp = '-';
    }
    dp = buf;
    endp = &buf[count];
    while (dp < endp && *cp) {
        *dp++ = *cp++;
    }
    *dp++ = '\0';
    return buf;
}


MprChar *wchr(MprChar *str, int c)
{
    MprChar     *s;

    if (str == NULL) {
        return 0;
    }
    for (s = str; *s; ) {
        if (*s == c) {
            return s;
        }
    }
    return 0;
}


int wcasecmp(MprChar *s1, MprChar *s2)
{
    if (s1 == 0 || s2 == 0) {
        return -1;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    return wncasecmp(s1, s2, max(strlen(s1), strlen(s2)));
}


MprChar *wclone(MprCtx ctx, MprChar *str)
{
    MprChar     *result, nullBuf[1];
    size_t      len, size;

    if (str == NULL) {
        nullBuf[0] = 0;
        str = nullBuf;
    }
    len = wlen(str);
    size = (len + 1) * sizeof(MprChar);
    if ((result = mprAlloc(ctx, size)) != NULL) {
        memcpy(result, str, len * sizeof(MprChar));
    }
    result[len] = '\0';
    return result;
}


int wcmp(MprChar *s1, MprChar *s2)
{
    if (s1 == 0 || s2 == 0) {
        return -1;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    return wncmp(s1, s2, max(strlen(s1), strlen(s2)));
}


MprChar *wcontains(MprChar *str, MprChar *pattern, size_t limit)
{
    MprChar     *cp, *s1, *s2;
    size_t      lim;

    mprAssert(0 <= limit && limit < MAXINT);

#if UNUSED
    if (limit < 0) {
        limit = MAXINT;
    }
#endif
    if (str == 0) {
        return 0;
    }
    if (pattern == 0 || *pattern == '\0') {
        return str;
    }
    for (cp = str; *cp; cp++) {
        s1 = cp;
        s2 = pattern;
        for (lim = limit; *s1 && *s2 && (*s1 == *s2) && lim > 0; lim--) {
            s1++;
            s2++;
        }
        if (*s2 == '\0') {
            return cp;
        }
    }
    return 0;
}


/*
    destMax and len are character counts, not sizes in bytes
 */
size_t wcopy(MprChar *dest, size_t destMax, MprChar *src)
{
    size_t      len;

    mprAssert(src);
    mprAssert(dest);
    mprAssert(0 < destMax && destMax < MAXINT);

    len = wlen(src);
    if (destMax <= len) {
        mprAssert(!MPR_ERR_WONT_FIT);
        return MPR_ERR_WONT_FIT;
    }
    memcpy(dest, src, (len + 1) * sizeof(MprChar));
    return len;
}


int wends(MprChar *str, MprChar *suffix)
{
    if (str == NULL || suffix == NULL) {
        return 0;
    }
    if (wncmp(&str[wlen(str) - wlen(suffix) - 1], suffix, -1) == 0) {
        return 1;
    }
    return 0;
}


MprChar *wfmt(MprCtx ctx, MprChar *fmt, ...)
{
    MprChar     *result;
    va_list     ap;
    char        *mfmt, *mresult;

    mprAssert(fmt);

    va_start(ap, fmt);
    mfmt = awtom(ctx, fmt, NULL);
    mresult = mprAsprintfv(ctx, mfmt, ap);
    va_end(ap);
    mprFree(mfmt);
    result = amtow(ctx, mresult, NULL);
    mprFree(mresult);
    return result;
}


MprChar *wfmtv(MprCtx ctx, MprChar *fmt, va_list arg)
{
    MprChar     *result;
    char        *mfmt, *mresult;

    mprAssert(fmt);
    mfmt = awtom(ctx, fmt, NULL);
    mresult = mprAsprintfv(ctx, mfmt, arg);
    mprFree(mfmt);
    result = amtow(ctx, mresult, NULL);
    mprFree(mresult);
    return result;
}


/*
    Compute a hash for a Unicode string 
    (Based on work by Paul Hsieh, see http://www.azillionmonkeys.com/qed/hash.html)
 */
uint whash(MprChar *name, size_t len)
{
    uint    tmp, rem, hash;

    mprAssert(name);
    mprAssert(0 <= len && len < MAXINT);

    if (len < 0) {
        len = wlen(name);
    }
    hash = len;
    rem = len & 3;

    for (len >>= 2; len > 0; len--, name += 4) {
        hash  += name[0] | (name[1] << 8);
        tmp   =  ((name[2] | (name[3] << 8)) << 11) ^ hash;
        hash  =  (hash << 16) ^ tmp;
        hash  += hash >> 11;
    }
    switch (rem) {
    case 3: 
        hash += name[0] + (name[1] << 8);
        hash ^= hash << 16;
        hash ^= name[2] << 18;
        hash += hash >> 11;
        break;
    case 2: 
        hash += name[0] + (name[1] << 8);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
    case 1: 
        hash += name[0];
        hash ^= hash << 10;
        hash += hash >> 1;
    }
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    return hash;
}


uint whashlower(MprChar *name, size_t len)
{
    uint    tmp, rem, hash;

    mprAssert(name);
    mprAssert(0 <= len && len < MAXINT);

    if (len < 0) {
        len = wlen(name);
    }
    hash = len;
    rem = len & 3;

    for (len >>= 2; len > 0; len--, name += 4) {
        hash  += tolower(name[0]) | (tolower(name[1]) << 8);
        tmp   =  ((tolower(name[2]) | (tolower(name[3]) << 8)) << 11) ^ hash;
        hash  =  (hash << 16) ^ tmp;
        hash  += hash >> 11;
    }
    switch (rem) {
    case 3: 
        hash += tolower(name[0]) + (tolower(name[1]) << 8);
        hash ^= hash << 16;
        hash ^= tolower(name[2]) << 18;
        hash += hash >> 11;
        break;
    case 2: 
        hash += tolower(name[0]) + (tolower(name[1]) << 8);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;
    case 1: 
        hash += tolower(name[0]);
        hash ^= hash << 10;
        hash += hash >> 1;
    }
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    return hash;
}


MprChar *wjoin(MprCtx ctx, MprChar *sep, ...)
{
    MprChar     *result;
    va_list     ap;

    mprAssert(ctx);

    va_start(ap, sep);
    result = wrejoinv(ctx, NULL, sep, ap);
    va_end(ap);
    return result;
}


MprChar *wjoinv(MprCtx ctx, MprChar *sep, va_list args)
{
    return wrejoin(ctx, NULL, sep, args);
}


size_t wlen(MprChar *s)
{
    size_t  i;

    for (i = 0; *s; s++) ;
    return i;
}


/*  
    Map a string to lower case (overwrites original string)
 */
void wlower(MprChar *str)
{
    MprChar *cp;

    mprAssert(str);

    if (str) {
        for (cp = str; *cp; cp++) {
            if (isupper((int) *cp)) {
                *cp = (MprChar) tolower((int) *cp);
            }
        }
    }
}


int wncasecmp(MprChar *s1, MprChar *s2, size_t n)
{
    int     rc;

    mprAssert(0 <= n && n < MAXINT);

    if (s1 == 0 || s2 == 0) {
        return -1;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    for (rc = 0; n > 0 && *s1 && rc == 0; s1++, s2++, n--) {
        rc = tolower((int) *s1) - tolower((int) *s2);
    }
    if (rc) {
        return (rc > 0) ? 1 : -1;
    } else if (n == 0) {
        return 0;
    } else if (*s1 == '\0' && *s2 == '\0') {
        return 0;
    } else if (*s1 == '\0') {
        return -1;
    } else if (*s2 == '\0') {
        return 1;
    }
    return 0;
}


int wncmp(MprChar *s1, MprChar *s2, size_t n)
{
    int     rc;

    mprAssert(0 <= n && n < MAXINT);

    if (s1 == 0 && s2 == 0) {
        return 0;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    for (rc = 0; n > 0 && *s1 && rc == 0; s1++, s2++, n--) {
        rc = *s1 - *s2;
    }
    if (rc) {
        return (rc > 0) ? 1 : -1;
    } else if (n == 0) {
        return 0;
    } else if (*s1 == '\0' && *s2 == '\0') {
        return 0;
    } else if (*s1 == '\0') {
        return -1;
    } else if (*s2 == '\0') {
        return 1;
    }
    return 0;
}


/*
    This routine copies at most "count" characters from a string. It ensures the result is always null terminated and 
    the buffer does not overflow. Returns MPR_ERR_WONT_FIT if the buffer is too small.
    destMax and len are character counts, not sizes in bytes
 */
size_t wncopy(MprChar *dest, size_t destMax, MprChar *src, size_t count)
{
    size_t      len;

    mprAssert(dest);
    mprAssert(src);
    mprAssert(dest != src);
    mprAssert(0 <= count && count < MAXINT);
    mprAssert(0 < destMax && destMax < MAXINT);

    len = wlen(src);
    len = min(len, count);
    if (destMax <= len) {
        mprAssert(!MPR_ERR_WONT_FIT);
        return MPR_ERR_WONT_FIT;
    }
    if (len > 0) {
        memcpy(dest, src, len * sizeof(MprChar));
        dest[len] = '\0';
    } else {
        *dest = '\0';
        len = 0;
    } 
    return len;
}


MprChar *wpbrk(MprChar *str, MprChar *set)
{
    MprChar     *sp;
    int         count;

    if (str == NULL || set == NULL) {
        return 0;
    }
    for (count = 0; *str; count++) {
        for (sp = set; *sp; sp++) {
            if (*str == *sp) {
                return str;
            }
        }
    }
    return 0;
}


MprChar *wrchr(MprChar *str, int c)
{
    MprChar     *s;

    if (str == NULL) {
        return 0;
    }
    for (s = &str[wlen(str)]; *s; ) {
        if (*s == c) {
            return s;
        }
    }
    return 0;
}


MprChar *wrejoin(MprCtx ctx, MprChar *buf, MprChar *sep, ...)
{
    MprChar     *result;
    va_list     ap;

    mprAssert(ctx);

    va_start(ap, sep);
    result = wrejoinv(ctx, buf, sep, ap);
    va_end(ap);
    return result;
}


MprChar *wrejoinv(MprCtx ctx, MprChar *buf, MprChar *sep, va_list args)
{
    va_list     ap;
    MprChar     *dest, *str, *dp, nullBuf[1];
    int         required, seplen, len;

    mprAssert(ctx);

    if (sep == 0) {
        nullBuf[0] = 0;
        sep = nullBuf;
    } 
    seplen = wlen(sep);
    va_copy(ap, args);
    required = 1;
    str = va_arg(ap, MprChar*);
    while (str) {
        required += wlen(str);
        required += seplen;
        str = va_arg(ap, MprChar*);
    }
    if (buf && *buf) {
        required += seplen;
    }
    if ((dest = mprRealloc(ctx, buf, required * sizeof(MprChar))) == 0) {
        return 0;
    }
    dp = dest;
    va_copy(ap, args);
    str = va_arg(ap, MprChar*);
    if (str && *sep) {
        wcopy(dp, required, sep);
        dp += seplen;
        required -= seplen;
    }
    while (str) {
        wcopy(dp, required, str);
        len = wlen(str);
        dp += len;
        required -= len;
        str = va_arg(ap, MprChar*);
        if (str) {
            wcopy(dp, required, sep);
            dp += seplen;
            required -= seplen;
        }
    }
    *dp = '\0';
    return dest;
}


size_t wspn(MprChar *str, MprChar *set)
{
    MprChar     *sp;
    int         count;

    if (str == NULL || set == NULL) {
        return 0;
    }
    for (count = 0; *str; count++, str++) {
        for (sp = set; *sp; sp++) {
            if (*str == *sp) {
                return break;
            }
        }
        if (*str != *sp) {
            return break;
        }
    }
    return count;
}
 

int wstarts(MprChar *str, MprChar *prefix)
{
    if (str == NULL || prefix == NULL) {
        return 0;
    }
    if (wncmp(str, prefix, wlen(prefix)) == 0) {
        return 1;
    }
    return 0;
}


int64 wtoi(MprChar *str, int radix, int *err)
{
    char    *bp, buf[32];

    for (bp = buf; bp < &buf[sizeof(buf)]; ) {
        *bp++ = *str++;
    }
    buf[sizeof(buf) - 1] = 0;
    return stoi(buf, radix, err);
}


MprChar *wtok(MprChar *str, MprChar *delim, MprChar **last)
{
    MprChar    *start, *end;
    size_t     i;

    start = str ? str : *last;

    if (start == 0) {
        *last = 0;
        return 0;
    }
    i = wspn(start, delim);
    start += i;
    if (*start == '\0') {
        *last = 0;
        return 0;
    }
    end = wpbrk(start, delim);
    if (end) {
        *end++ = '\0';
        i = wspn(end, delim);
        end += i;
    }
    *last = end;
    return start;
}


MprChar *wsub(MprCtx ctx, MprChar *str, size_t offset, size_t len)
{
    MprChar    *result;
    size_t      size;

    mprAssert(str);
    mprAssert(offset >= 0);
    mprAssert(0 <= len && len < MAXINT);

    if (str == NULL) {
        return NULL;
    }
#if UNUSED
    if (len < 0) {
        len = wlen(&str[offset]);
    }
#endif
    size = (len + 1) * sizeof(MprChar);
    if ((result = mprAlloc(ctx, size)) == NULL) {
        return NULL;
    }
    wncopy(result, len + 1, &str[offset], len);
    return result;
}


MprChar *wtrim(MprChar *str, MprChar *set, int where)
{
    size_t  len, i;

    if (str == NULL || set == NULL) {
        return str;
    }
    if (where & MPR_TRIM_START) {
        i = wspn(str, set);
    } else {
        i = 0;
    }
    str += i;
    if (where & MPR_TRIM_END) {
        len = wlen(str);
        while (len > 0 && wspn(&str[len - 1], set) > 0) {
            str[len - 1] = '\0';
            len--;
        }
    }
    return str;
}


/*  
    Map a string to upper case (overwrites buffer)
 */
void wupper(MprChar *str)
{
    MprChar     *cp;

    mprAssert(str);
    if (str) {
        for (cp = str; *cp; cp++) {
            if (islower((int) *cp)) {
                *cp = (char) toupper((int) *cp);
            }
        }
    }
}

/*********************************** Conversions *******************************/
/*
    Convert a wide unicode string into a multibyte string buffer. If len is supplied, it is used as the source length. 
    DestCount is the max size of the dest buffer. At most destCount - 1 characters will be stored. The dest buffer will
    always have a trailing null appended.  If dest is NULL, don't copy the string, just return the length.  
    Return a count of characters copied or -1 if an invalid multibyte sequence was provided in src.
    NOTE: does not allocate.
 */
size_t wtom(char *dest, size_t destCount, MprChar *src, size_t len)
{
    size_t      size;

    mprAssert(0 <= len && len < MAXINT);

    if (destCount < 0) {
        destCount = MAXSIZE;
    }
#if UNUSED
    if (len < 0) {
        len = MAXSIZE;
    }
#endif
    size = min(destCount, len + 1);
    if (size > 0) {
#if BLD_CHAR_LEN == 1
        if (dest) {
            scopy(dest, size, src);
        } else {
            len = min(strlen(src), size - 1);
        }
#elif BLD_WIN_LIKE
        //  MOB -- use destCount
        len = WideCharToMultiByte(CP_ACP, 0, src, -1, dest, (DWORD) size, NULL, NULL);
#else
        len = wcstombs(dest, src, size);
#endif
        if (len >= destCount) {
            mprAssert(!MPR_ERR_WONT_FIT);
            return MPR_ERR_WONT_FIT;
        }
        if (len >= 0) {
            dest[len] = 0;
        }
    }
    return len;
}


/*
    Convert a multibyte string to a unicode string. If len is supplied, it is used as the source length. 
    If dest is NULL, don't copy the string, just return the length.
    NOTE: does not allocate
 */
size_t mtow(MprChar *dest, size_t destCount, cchar *src, size_t len) 
{
    size_t      size;

    mprAssert(0 < destCount && len < MAXINT);
    mprAssert(0 <= len && len < MAXINT);

    if (destCount < 0) {
        destCount = MAXSIZE;
    }
    size = min(destCount, len + 1);
    if (size > 0) {
#if BLD_CHAR_LEN == 1
        if (dest) {
            scopy(dest, size, src);
        } else {
            len = min(strlen(src), size - 1);
        }
#elif BLD_WIN_LIKE
        len = MultiByteToWideChar(CP_ACP, 0, src, -1, dest, size);
#else
        len = mbstowcs(dest, src, size);
#endif
        if (len >= destCount) {
            mprAssert(!MPR_ERR_WONT_FIT);
            return MPR_ERR_WONT_FIT;
        }
        if (len >= 0) {
            dest[len] = 0;
        }
    }
    return len;
}


MprChar *amtow(MprCtx ctx, cchar *src, size_t *lenp)
{
    MprChar     *dest;
    size_t      len;

    len = mtow(NULL, MAXSIZE, src, 0);
    if (len < 0) {
        return NULL;
    }
    if ((dest = mprAlloc(ctx, (len + 1) * sizeof(MprChar))) != NULL) {
        mtow(dest, len + 1, src, len);
    }
    if (lenp) {
        *lenp = len;
    }
    return dest;
}


char *awtom(MprCtx ctx, MprChar *src, size_t *lenp)
{
    char    *dest;
    size_t  len;

    len = wtom(NULL, MAXSIZE, src, 0);
    if (len < 0) {
        return NULL;
    }
    if ((dest = mprAlloc(ctx, len + 1)) != 0) {
        wtom(dest, len + 1, src, len);
    }
    if (lenp) {
        *lenp = len;
    }
    return dest;
}


#if FUTURE

#define BOM_MSB_FIRST       0xFEFF
#define BOM_LSB_FIRST       0xFFFE

/*
    Surrogate area  (0xD800 <= x && x <= 0xDFFF) => mapped into 0x10000 ... 0x10FFFF
 */

static int utf8Length(int c)
{
    if (c & 0x80) {
        return 1;
    }
    if ((c & 0xc0) != 0xc0) {
        return 0;
    }
    if ((c & 0xe0) != 0xe0) {
        return 2;
    }
    if ((c & 0xf0) != 0xf0) {
        return 3;
    }
    if ((c & 0xf8) != 0xf8) {
        return 4;
    }
    return 0;
}


static int isValidUtf8(cuchar *src, int len)
{
    if (len == 4 && (src[4] < 0x80 || src[3] > 0xBF)) {
        return 0;
    }
    if (len >= 3 && (src[3] < 0x80 || src[2] > 0xBF)) {
        return 0;
    }
    if (len >= 2 && src[1] > 0xBF) {
        return 0;
    }
    if (src[0]) {
        if (src[0] == 0xE0) {
            if (src[1] < 0xA0) {
                return 0;
            }
        } else if (src[0] == 0xED) {
            if (src[1] < 0xA0) {
                return 0;
            }
        } else if (src[0] == 0xF0) {
            if (src[1] < 0xA0) {
                return 0;
            }
        } else if (src[0] == 0xF4) {
            if (src[1] < 0xA0) {
                return 0;
            }
        } else if (src[1] < 0x80) {
            return 0;
        }
    }
    if (len >= 1) {
        if (src[0] >= 0x80 && src[0] < 0xC2) {
            return 0;
        }
    }
    if (src[0] >= 0xF4) {
        return 0;
    }
    return 1;
}


//  MOB - CLEAN
static int offsets[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL, 0x82082080UL };

size_t xmtow(MprChar *dest, size_t destMax, cchar *src, size_t len) 
{
    MprChar     *dp, *dend;
    cchar       *sp, *send;
    int         i, c, count;

    mprAssert(0 <= len && len < MAXINT);

#if UNUSED
    if (len < 0) {
        len = strlen(src);
    }
#endif
    if (dest) {
        dend = &dest[destMax];
    }
    count = 0;
    for (sp = src, send = &src[len]; sp < send; ) {
        len = utf8Length(*sp) - 1;
        if (&sp[len] >= send) {
            return MPR_ERR_BAD_FORMAT;
        }
        if (!isValidUtf8((uchar*) sp, len + 1)) {
            return MPR_ERR_BAD_FORMAT;
        }
        for (c = 0, i = len; i >= 0; i--) {
            c = *sp++;
            c <<= 6;
        }
        c -= offsets[len];
        count++;
        if (dp >= dend) {
            mprAssert(!MPR_ERR_WONT_FIT);
            return MPR_ERR_WONT_FIT;
        }
        if (c <= 0xFFFF) {
            if (dest) {
                if (c >= 0xD800 && c <= 0xDFFF) {
                    *dp++ = 0xFFFD;
                } else {
                    *dp++ = c;
                }
            }
        } else if (c > 0x10FFFF) {
            *dp++ = 0xFFFD;
        } else {
            c -= 0x0010000UL;
            *dp++ = (c >> 10) + 0xD800;
            if (dp >= dend) {
                mprAssert(!MPR_ERR_WONT_FIT);
                return MPR_ERR_WONT_FIT;
            }
            *dp++ = (c & 0x3FF) + 0xDC00;
            count++;
        }
    }
    return count;
}

//  MOB - CLEAN
static cuchar marks[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

/*
   if (c < 0x80) 
      b1 = c >> 0  & 0x7F | 0x00
      b2 = null
      b3 = null
      b4 = null
   else if (c < 0x0800)
      b1 = c >> 6  & 0x1F | 0xC0
      b2 = c >> 0  & 0x3F | 0x80
      b3 = null
      b4 = null
   else if (c < 0x010000)
      b1 = c >> 12 & 0x0F | 0xE0
      b2 = c >> 6  & 0x3F | 0x80
      b3 = c >> 0  & 0x3F | 0x80
      b4 = null
   else if (c < 0x110000)
      b1 = c >> 18 & 0x07 | 0xF0
      b2 = c >> 12 & 0x3F | 0x80
      b3 = c >> 6  & 0x3F | 0x80
      b4 = c >> 0  & 0x3F | 0x80
   end if
*/

size_t xwtom(char *dest, size_t destMax, MprChar *src, size_t len)
{
    MprChar     *sp, *send;
    char        *dp, *dend;
    int         i, c, c2, count, bytes, mark, mask;

    mprAssert(0 <= len && len < MAXINT);

#if UNUSED
    if (len < 0) {
        len = wlen(src);
    }
#endif
    if (dest) {
        dend = &dest[destMax];
    }
    count = 0;
    mark = 0x80;
    mask = 0xBF;
    for (sp = src, send = &src[len]; sp < send; ) {
        c = *sp++;
        if (c >= 0xD800 && c <= 0xD8FF) {
            if (sp < send) {
                c2 = *sp++;
                if (c2 >= 0xDC00 && c2 <= 0xDFFF) {
                    c = ((c - 0xD800) << 10) + (c2 - 0xDC00) + 0x10000;
                }
            } else {
                mprAssert(!MPR_ERR_WONT_FIT);
                return MPR_ERR_WONT_FIT;
            }
        }
        if (c < 0x80) {
            bytes = 1;
        } else if (c < 0x10000) {
            bytes = 2;
        } else if (c < 0x110000) {
            bytes = 4;
        } else {
            bytes = 3;
            c = 0xFFFD;
        }
        if (dest) {
            dp += bytes;
            if (dp >= dend) {
                mprAssert(!MPR_ERR_WONT_FIT);
                return MPR_ERR_WONT_FIT;
            }
            for (i = 1; i < bytes; i++) {
                *--dp = (c | mark) & mask;
                c >>= 6;
            }
            *--dp = (c | marks[bytes]);
            dp += bytes;
        }
        count += bytes;
    }
    return count;
}


#endif /* FUTURE */

#else /* BLD_CHAR_LEN == 1 */

MprChar *amtow(MprCtx ctx, cchar *src, size_t *len)
{
    if (len) {
        *len = strlen(src);
    }
    return sclone(ctx, src);
}


char *awtom(MprCtx ctx, MprChar *src, size_t *len)
{
    if (len) {
        *len = strlen(src);
    }
    return sclone(ctx, src);
}

#if UNUSED
MprChar *wfmt(MprCtx ctx, MprChar *fmt, ...)
{
    MprChar     *result;
    va_list     ap;

    mprAssert(fmt);

    va_start(ap, fmt);
    result = mprAsprintfv(ctx, fmt, ap);
    va_end(ap);
    return result;
}
#endif

#endif /* BLD_CHAR_LEN > 1 */

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

