/**
    mprString.c - String routines safe for embedded programming

    This module provides safe replacements for the standard string library. 
    Most routines in this file are not thread-safe. It is the callers responsibility to perform all thread synchronization.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Code ************************************/

char *itos(int64 value)
{
    return itosradix(value, 10);
}

/*
    Format a number as a string. Support radix 10 and 16.
 */
char *itosradix(int64 value, int radix)
{
    char    numBuf[32];
    char    *cp;
    char    digits[] = "0123456789ABCDEF";
    int     negative;

    if (radix != 10 && radix != 16) {
        return 0;
    }
    cp = &numBuf[sizeof(numBuf)];
    *--cp = '\0';

    if (value < 0) {
        negative = 1;
        value = -value;
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
    return sclone(cp);
}


char *itosbuf(char *buf, ssize size, int64 value, int radix)
{
    char    *cp, *end;
    char    digits[] = "0123456789ABCDEF";
    int     negative;

    if ((radix != 10 && radix != 16) || size < 2) {
        return 0;
    }
    end = cp = &buf[size];
    *--cp = '\0';

    if (value < 0) {
        negative = 1;
        value = -value;
        size--;
    } else {
        negative = 0;
    }
    do {
        *--cp = digits[value % radix];
        value /= radix;
    } while (value > 0 && cp > buf);

    if (negative) {
        if (cp <= buf) {
            return 0;
        }
        *--cp = '-';
    }
    if (buf < cp) {
        /* Move the null too */
        memmove(buf, cp, end - cp + 1);
    }
    return buf;
}


char *scamel(cchar *str)
{
    char    *ptr;
    ssize   size, len;

    if (str == 0) {
        str = "";
    }
    len = slen(str);
    size = len + 1;
    if ((ptr = mprAlloc(size)) != 0) {
        memcpy(ptr, str, len);
        ptr[len] = '\0';
    }
    ptr[0] = (char) tolower((int) ptr[0]);
    return ptr;
}


/*
    Case insensitive string comparison. Limited by length
    MOB TODO - name is not great. scaselesscmp, sncaselesscmp
 */
int scasecmp(cchar *s1, cchar *s2)
{
    if (s1 == 0 || s2 == 0) {
        return -1;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    return sncasecmp(s1, s2, max(slen(s1), slen(s2)));
}


bool scasematch(cchar *s1, cchar *s2)
{
    return scasecmp(s1, s2) == 0;
}


char *schr(cchar *s, int c)
{
    if (s == 0) {
        return 0;
    }
    return strchr(s, c);
}


char *scontains(cchar *str, cchar *pattern, ssize limit)
{
    cchar   *cp, *s1, *s2;
    ssize   lim;

    if (limit < 0) {
        limit = MAXINT;
    }
    if (str == 0) {
        return 0;
    }
    if (pattern == 0 || *pattern == '\0') {
        return 0;
    }
    for (cp = str; *cp && limit > 0; cp++, limit--) {
        s1 = cp;
        s2 = pattern;
        for (lim = limit; *s1 && *s2 && (*s1 == *s2) && lim > 0; lim--) {
            s1++;
            s2++;
        }
        if (*s2 == '\0') {
            return (char*) cp;
        }
    }
    return 0;
}


ssize scopy(char *dest, ssize destMax, cchar *src)
{
    ssize      len;

    mprAssert(src);
    mprAssert(dest);
    mprAssert(0 < dest && destMax < MAXINT);

    len = slen(src);
    if (destMax <= len) {
        mprAssert(!MPR_ERR_WONT_FIT);
        return MPR_ERR_WONT_FIT;
    }
    strcpy(dest, src);
    return len;
}


char *sclone(cchar *str)
{
    char    *ptr;
    ssize   size, len;

    if (str == 0) {
        str = "";
    }
    len = slen(str);
    size = len + 1;
    if ((ptr = mprAlloc(size)) != 0) {
        memcpy(ptr, str, len);
        ptr[len] = '\0';
    }
    return ptr;
}


int scmp(cchar *s1, cchar *s2)
{
    if (s1 == s2) {
        return 0;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    return sncmp(s1, s2, max(slen(s1), slen(s2)));
}


//  MOB should return bool
int sends(cchar *str, cchar *suffix)
{
    if (str == 0 || suffix == 0) {
        return 0;
    }
    if (strcmp(&str[slen(str) - slen(suffix)], suffix) == 0) {
        return 1;
    }
    return 0;
}


char *sfmt(cchar *fmt, ...)
{
    va_list     ap;
    char        *buf;

    mprAssert(fmt);

    va_start(ap, fmt);
    buf = mprAsprintfv(fmt, ap);
    va_end(ap);
    return buf;
}


char *sfmtv(cchar *fmt, va_list arg)
{
    mprAssert(fmt);
    return mprAsprintfv(fmt, arg);
}


/*
    Compute a hash for a C string
    (Based on work by Paul Hsieh (c) 2004-2008, see http://www.azillionmonkeys.com/qed/hash.html)
 */
uint shash(cchar *cname, ssize len)
{
    uchar   *name;
    uint    hash, rem, tmp;

    mprAssert(cname);
    mprAssert(0 <= len && len < MAXINT);

    if (cname == 0) {
        return 0;
    }
    hash = (uint) len;
    rem = (int) (len & 3);
    name = (uchar*) cname;
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


/*
    Hash the lower case name
 */
uint shashlower(cchar *cname, ssize len)
{
    uchar   *name;
    uint    hash, rem, tmp;

    mprAssert(cname);
    mprAssert(0 <= len && len < MAXINT);

    if (cname == 0) {
        return 0;
    }
    hash = (uint) len;
    rem = (int) (len & 3);
    name = (uchar*) cname;

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
        hash += tolower(name[0]) + tolower((name[1]) << 8);
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


char *sjoin(cchar *str, ...)
{
    va_list     ap;
    char        *result;

    va_start(ap, str);
    result = sjoinv(str, ap);
    va_end(ap);
    return result;
}


char *sjoinv(cchar *buf, va_list args)
{
    va_list     ap;
    char        *dest, *str, *dp;
    ssize       required;

    va_copy(ap, args);
    required = 1;
    if (buf) {
        required += slen(buf);
    }
    str = va_arg(ap, char*);
    while (str) {
        required += slen(str);
        str = va_arg(ap, char*);
    }
    if ((dest = mprAlloc(required)) == 0) {
        return 0;
    }
    dp = dest;
    if (buf) {
        strcpy(dp, buf);
        dp += slen(buf);
    }
    va_copy(ap, args);
    str = va_arg(ap, char*);
    while (str) {
        strcpy(dp, str);
        dp += slen(str);
        str = va_arg(ap, char*);
    }
    *dp = '\0';
    return dest;
}


ssize slen(cchar *s)
{
    return s ? strlen(s) : 0;
}


/*  
    Map a string to lower case. Allocates a new string.
 */
char *slower(cchar *str)
{
    char    *cp, *s;

    mprAssert(str);

    if (str) {
        s = sclone(str);
        for (cp = s; *cp; cp++) {
            if (isupper((int) *cp)) {
                *cp = (char) tolower((int) *cp);
            }
        }
        str = s;
    }
    return (char*) str;
}


bool smatch(cchar *s1, cchar *s2)
{
    return scmp(s1, s2) == 0;
}


int sncasecmp(cchar *s1, cchar *s2, ssize n)
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


/*
    Clone a sub-string of a specified length. The null is added after the length. The given len can be longer than the
    source string.
 */
char *snclone(cchar *str, ssize len)
{
    char    *ptr;
    ssize   size, l;

    if (str == 0) {
        str = "";
    }
    l = slen(str);
    len = min(l, len);
    size = len + 1;
    if ((ptr = mprAlloc(size)) != 0) {
        memcpy(ptr, str, len);
        ptr[len] = '\0';
    }
    return ptr;
}


/*
    Case sensitive string comparison. Limited by length
 */
int sncmp(cchar *s1, cchar *s2, ssize n)
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
 */
ssize sncopy(char *dest, ssize destMax, cchar *src, ssize count)
{
    ssize      len;

    mprAssert(dest);
    mprAssert(src);
    mprAssert(src != dest);
    mprAssert(0 <= count && count < MAXINT);
    mprAssert(0 < destMax && destMax < MAXINT);

    len = slen(src);
    len = min(len, count);
    if (destMax <= len) {
        mprAssert(!MPR_ERR_WONT_FIT);
        return MPR_ERR_WONT_FIT;
    }
    if (len > 0) {
        memcpy(dest, src, len);
        dest[len] = '\0';
    } else {
        *dest = '\0';
        len = 0;
    } 
    return len;
}


bool snumber(cchar *s)
{
    return s && *s && strspn(s, "1234567890") == strlen(s);
} 


char *spascal(cchar *str)
{
    char    *ptr;
    ssize   size, len;

    if (str == 0) {
        str = "";
    }
    len = slen(str);
    size = len + 1;
    if ((ptr = mprAlloc(size)) != 0) {
        memcpy(ptr, str, len);
        ptr[len] = '\0';
    }
    ptr[0] = (char) toupper((int) ptr[0]);
    return ptr;
}


char *spbrk(cchar *str, cchar *set)
{
    cchar       *sp;
    int         count;

    if (str == 0 || set == 0) {
        return 0;
    }
    for (count = 0; *str; count++, str++) {
        for (sp = set; *sp; sp++) {
            if (*str == *sp) {
                return (char*) str;
            }
        }
    }
    return 0;
}


char *srchr(cchar *s, int c)
{
    if (s == 0) {
        return 0;
    }
    return strrchr(s, c);
}


char *srejoin(char *buf, ...)
{
    va_list     args;
    char        *result;

    va_start(args, buf);
    result = srejoinv(buf, args);
    va_end(args);
    return result;
}


char *srejoinv(char *buf, va_list args)
{
    va_list     ap;
    char        *dest, *str, *dp;
    ssize       len, required;

    va_copy(ap, args);
    len = slen(buf);
    required = len + 1;
    str = va_arg(ap, char*);
    while (str) {
        required += slen(str);
        str = va_arg(ap, char*);
    }
    if ((dest = mprRealloc(buf, required)) == 0) {
        return 0;
    }
    dp = &dest[len];
    va_copy(ap, args);
    str = va_arg(ap, char*);
    while (str) {
        strcpy(dp, str);
        dp += slen(str);
        str = va_arg(ap, char*);
    }
    *dp = '\0';
    return dest;
}


char *sreplace(cchar *str, cchar *pattern, cchar *replacement)
{
    MprBuf      *buf;
    cchar       *s;
    ssize       plen;

    buf = mprCreateBuf(-1, -1);
    if (pattern && *pattern && replacement) {
        plen = slen(pattern);
        for (s = str; *s; s++) {
            if (sncmp(s, pattern, plen) == 0) {
                mprPutStringToBuf(buf, replacement);
                s += plen - 1;
            } else {
                mprPutCharToBuf(buf, *s);
            }
        }
    }
    mprAddNullToBuf(buf);
    return sclone(mprGetBufStart(buf));
}


ssize sspn(cchar *str, cchar *set)
{
#if KEEP
    cchar       *sp;
    int         count;

    if (str == 0 || set == 0) {
        return 0;
    }
    for (count = 0; *str; count++, str++) {
        for (sp = set; *sp; sp++) {
            if (*str == *sp) {
                break;
            }
        }
        if (*str != *sp) {
            break;
        }
    }
    return count;
#else
    if (str == 0 || set == 0) {
        return 0;
    }
    return strspn(str, set);
#endif
}
 

bool sstarts(cchar *str, cchar *prefix)
{
    if (str == 0 || prefix == 0) {
        return 0;
    }
    if (strncmp(str, prefix, slen(prefix)) == 0) {
        return 1;
    }
    return 0;
}


int64 stoi(cchar *str)
{
    return stoiradix(str, 10, NULL);
}


/*
    Parse a number and check for parse errors. Supports radix 8, 10 or 16. 
    If radix is <= 0, then the radix is sleuthed from the input.
    Supports formats:
        [(+|-)][0][OCTAL_DIGITS]
        [(+|-)][0][(x|X)][HEX_DIGITS]
        [(+|-)][DIGITS]

 */
int64 stoiradix(cchar *str, int radix, int *err)
{
    cchar   *start;
    int64   val;
    int     n, c, negative;

    if (err) {
        *err = 0;
    }
    if (str == 0) {
        if (err) {
            *err = MPR_ERR_BAD_SYNTAX;
        }
        return 0;
    }
    while (isspace((int) *str)) {
        str++;
    }
    val = 0;
    if (*str == '-') {
        negative = 1;
        str++;
    } else {
        negative = 0;
    }
    start = str;
    if (radix <= 0) {
        radix = 10;
        if (*str == '0') {
            if (tolower((int) str[1]) == 'x') {
                radix = 16;
                str += 2;
            } else {
                radix = 8;
                str++;
            }
        }

    } else if (radix == 16) {
        if (*str == '0' && tolower((int) str[1]) == 'x') {
            str += 2;
        }

    } else if (radix > 10) {
        radix = 10;
    }
    if (radix == 16) {
        while (*str) {
            c = tolower((int) *str);
            if (isdigit(c)) {
                val = (val * radix) + c - '0';
            } else if (c >= 'a' && c <= 'f') {
                val = (val * radix) + c - 'a' + 10;
            } else {
                break;
            }
            str++;
        }
    } else {
        while (*str && isdigit((int) *str)) {
            n = *str - '0';
            if (n >= radix) {
                break;
            }
            val = (val * radix) + n;
            str++;
        }
    }
    if (str == start) {
        /* No data */
        if (err) {
            *err = MPR_ERR_BAD_SYNTAX;
        }
        return 0;
    }
    return (negative) ? -val: val;
}


/*
    Note "str" is modifed as per strtok()
    MOB - warning this does not allocate - should it?
 */
char *stok(char *str, cchar *delim, char **last)
{
    char    *start, *end;
    ssize   i;

    start = str ? str : *last;

    if (start == 0) {
        *last = 0;
        return 0;
    }
    i = strspn(start, delim);
    start += i;
    if (*start == '\0') {
        *last = 0;
        return 0;
    }
    end = strpbrk(start, delim);
    if (end) {
        *end++ = '\0';
        i = strspn(end, delim);
        end += i;
    }
    *last = end;
    return start;
}


char *ssub(cchar *str, ssize offset, ssize len)
{
    char    *result;
    ssize   size;

    mprAssert(str);
    mprAssert(offset >= 0);
    mprAssert(0 <= len && len < MAXINT);

    if (str == 0) {
        return 0;
    }
    size = len + 1;
    if ((result = mprAlloc(size)) == 0) {
        return 0;
    }
    sncopy(result, size, &str[offset], len);
    return result;
}


/*
    Trim characters from the given set. Returns a newly allocated string.
 */
char *strim(cchar *str, cchar *set, int where)
{
    char    *s;
    ssize   len, i;

    if (str == 0 || set == 0) {
        return 0;
    }
    if (where & MPR_TRIM_START) {
        i = strspn(str, set);
    } else {
        i = 0;
    }
    s = sclone(&str[i]);
    if (where & MPR_TRIM_END) {
        len = slen(s);
        while (len > 0 && strspn(&s[len - 1], set) > 0) {
            s[len - 1] = '\0';
            len--;
        }
    }
    return s;
}



/*  
    Map a string to upper case
 */
char *supper(cchar *str)
{
    char    *cp, *s;

    mprAssert(str);
    if (str) {
        s = sclone(str);
        for (cp = s; *cp; cp++) {
            if (islower((int) *cp)) {
                *cp = (char) toupper((int) *cp);
            }
        }
        str = s;
    }
    return (char*) str;
}


/*
    Expand ${token} references in a path or string.
    Currently support DOCUMENT_ROOT, SERVER_ROOT and PRODUCT, OS and VERSION.
 */
char *stemplate(cchar *str, MprHash *keys)
{
    MprBuf      *buf;
    char        *src, *result, *cp, *tok, *value;

    if (str) {
        if (schr(str, '$') == 0) {
            return sclone(str);
        }
        buf = mprCreateBuf(0, 0);
        for (src = (char*) str; *src; ) {
            if (*src == '$') {
                if (*++src == '{') {
                    for (cp = ++src; *cp && *cp != '}'; cp++) ;
                    tok = snclone(src, cp - src);
                } else {
                    for (cp = src; *cp && (isalnum((int) *cp) || *cp == '_'); cp++) ;
                    tok = snclone(src, cp - src);
                }
                if ((value = mprLookupKey(keys, tok)) != 0) {
                    mprPutStringToBuf(buf, value);
                    if (src > str && src[-1] == '{') {
                        src = cp + 1;
                    } else {
                        src = cp;
                    }
                } else {
                    mprPutCharToBuf(buf, '$');
                    if (src > str && src[-1] == '{') {
                        mprPutCharToBuf(buf, '{');
                    }
                    mprPutCharToBuf(buf, *src++);
                }
            } else {
                mprPutCharToBuf(buf, *src++);
            }
        }
        mprAddNullToBuf(buf);
        result = sclone(mprGetBufStart(buf));
    } else {
        result = MPR->emptyString;
    }
    return result;
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
