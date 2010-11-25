/**
    mprMixed.c - Mixed mode strings. Unicode results with ascii args.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

#if BLD_CHAR_LEN > 1
/********************************** Forwards **********************************/

int mcasecmp(MprChar *str1, cchar *str2)
{
    return mncasecmp(str1, str2, -1);
}


int mcmp(MprChar *s1, cchar *s2)
{
    return mncmp(s1, s2, -1);
}


MprChar *mcontains(MprChar *str, cchar *pattern, size_t limit)
{
    MprChar     *cp, *s1;
    cchar       *s2;
    size_t      lim;

    mprAssert(0 <= limit && limit < MAXSIZE);

    if (str == 0) {
        return 0;
    }
    if (pattern == 0 || *pattern == '\0') {
        return (MprChar*) str;
    }
    for (cp = str; *cp; cp++) {
        s1 = cp;
        s2 = pattern;
        for (lim = limit; *s1 && *s2 && (*s1 == (uchar) *s2) && lim > 0; lim--) {
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
size_t mcopy(MprChar *dest, cchar *src)
{
    size_t      len;

    mprAssert(src);
    mprAssert(dest);
    mprAssert(0 < destMax && destMax < MAXINT);

    len = strlen(src);
    if (destMax <= len) {
        mprAssert(!MPR_ERR_WONT_FIT);
        return MPR_ERR_WONT_FIT;
    }
    return mtow(dest, len + 1, src, len);
}


int mends(MprChar *str, cchar *suffix)
{
    MprChar     *cp;
    cchar       *sp;

    if (str == NULL || suffix == NULL) {
        return 0;
    }
    cp = &str[wlen(str) - 1];
    sp = &suffix[strlen(suffix) - 1];
    for (; cp > str && sp > suffix; ) {
        if (*cp-- != *sp--) {
            return 0;
        }
    }
    if (sp > suffix) {
        return 0;
    }
    return 1;
}


MprChar *mfmt(cchar *fmt, ...)
{
    MprChar     *result;
    va_list     ap;
    char        *mresult;

    mprAssert(fmt);

    va_start(ap, fmt);
    mresult = mprAsprintfv(fmt, ap);
    va_end(ap);
    result = amtow(mresult, NULL);
    mprFree(mresult);
    return result;
}


MprChar *mfmtv(cchar *fmt, va_list arg)
{
    MprChar     *result;
    char        *mresult;

    mprAssert(fmt);
    mresult = mprAsprintfv(fmt, arg);
    result = amtow(mresult, NULL);
    mprFree(mresult);
    return result;
}


/*
    Sep is ascii, args are MprChar
 */
MprChar *mjoin(cchar *sep, ...)
{
    MprChar     *result;
    va_list     ap;

    va_start(ap, sep);
    result = mrejoinv(NULL, sep, ap);
    va_end(ap);
    return result;
}


MprChar *mjoinv(cchar *sep, va_list args)
{
    return mrejoinv(NULL, sep, args);
}


/*
    Case insensitive string comparison. Limited by length
 */
int mncasecmp(MprChar *s1, cchar *s2, size_t n)
{
    int     rc;

    mprAssert(0 <= n && n < MAXSIZE);

    if (s1 == 0 || s2 == 0) {
        return -1;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    for (rc = 0; n > 0 && *s1 && rc == 0; s1++, s2++, n--) {
        rc = tolower((int) *s1) - tolower((int) (uchar) *s2);
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



int mncmp(MprChar *s1, cchar *s2, size_t n)
{
    mprAssert(0 <= n && n < MAXSIZE);

    if (s1 == 0 && s2 == 0) {
        return 0;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    for (rc = 0; n > 0 && *s1 && rc == 0; s1++, s2++, n--) {
        rc = *s1 - (uchar) *s2;
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


size_t mncopy(MprChar *dest, size_t destMax, cchar *src, size_t len)
{
    mprAssert(0 <= len && len < MAXSIZE);
    mprAssert(0 < destMax && destMax < MAXSIZE);

    return mtow(dest, destMax, src, len);
}


MprChar *mpbrk(MprChar *str, cchar *set)
{
    cchar   *sp;
    int     count;

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


/*
    Sep is ascii, args are MprChar
 */
MprChar *mrejoin(MprChar *buf, cchar *sep, ...)
{
    MprChar     *result;
    va_list     ap;

    va_start(ap, sep);
    result = mrejoinv(buf, sep, ap);
    va_end(ap);
    return result;
}


MprChar *mrejoinv(MprChar *buf, cchar *sep, va_list args)
{
    va_list     ap;
    MprChar     *dest, *str, *dp, *wsep;
    int         required, seplen, len;

    if (sep == 0) {
        sep = "";
    } 
    wsep = amtow(sep, NULL);
    seplen = strlen(sep);
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
    if ((dest = mprRealloc(buf, required)) == 0) {
        mprFree(wsep);
        return 0;
    }
    dp = dest;
    va_copy(ap, args);
    str = va_arg(ap, MprChar*);
    if (str && *buf) {
        wcopy(dp, required, wsep);
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
            wcopy(dp, required, wsep);
            dp += seplen;
            required -= seplen;
        }
    }
    *dp = '\0';
    mprFree(wsep);
    return dest;
}


size_t mspn(MprChar *str, cchar *set)
{
    cchar   *sp;
    int     count;

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
            break;
        }
    }
    return count;
}
 

int mstarts(MprChar *str, cchar *prefix)
{
    if (str == NULL || prefix == NULL) {
        return 0;
    }
    if (mncmp(str, prefix, strlen(prefix)) == 0) {
        return 1;
    }
    return 0;
}


MprChar *mtok(MprChar *str, cchar *delim, MprChar **last)
{
    MprChar    *start, *end;
    size_t     i;

    start = str ? str : *last;

    if (start == 0) {
        *last = 0;
        return 0;
    }
    i = mspn(start, delim);
    start += i;
    if (*start == '\0') {
        *last = 0;
        return 0;
    }
    end = mpbrk(start, delim);
    if (end) {
        *end++ = '\0';
        i = mspn(end, delim);
        end += i;
    }
    *last = end;
    return start;
}


MprChar *mtrim(MprChar *str, cchar *set, int where)
{
    size_t  len, i;

    if (str == NULL || set == NULL) {
        return str;
    }
    if (where & MPR_TRIM_START) {
        i = mspn(str, set);
    } else {
        i = 0;
    }
    str += i;
    if (where & MPR_TRIM_END) {
        len = wlen(str);
        while (len > 0 && mspn(&str[len - 1], set) > 0) {
            str[len - 1] = '\0';
            len--;
        }
    }
    return str;
}

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

