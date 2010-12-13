/**
    mprString.c - String routines safe for embedded programming

    This module provides safe replacements for the standard string library. 
    Most routines in this file are not thread-safe. It is the callers responsibility to perform all thread synchronization.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Code ************************************/
/*
    Format a number as a string. Support radix 10 and 16.
 */
char *itos(char *buf, int count, int64 value, int radix)
{
    char    numBuf[32];
    char    *cp, *dp, *endp;
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


char *schr(cchar *s, int c)
{
    return strchr(s, c);
}


/*
    Case insensitive string comparison. Limited by length
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
    return sncasecmp(s1, s2, max(strlen(s1), strlen(s2)));
}


size_t scopy(char *dest, size_t destMax, cchar *src)
{
    size_t      len;

    mprAssert(src);
    mprAssert(dest);
    mprAssert(0 < dest && destMax < MAXINT);

    len = strlen(src);
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
    size_t  size, len;

    if (str == NULL) {
        str = "";
    }
    len = strlen(str);
    size = len + 1;
    if ((ptr = mprAlloc(size)) != NULL) {
        memcpy(ptr, str, len);
    }
    ptr[len] = '\0';
    return ptr;
}


int scmp(cchar *s1, cchar *s2)
{
    if (s1 == 0 || s2 == 0) {
        return -1;
    } else if (s1 == 0) {
        return -1;
    } else if (s2 == 0) {
        return 1;
    }
    return sncmp(s1, s2, max(strlen(s1), strlen(s2)));
}


int sends(cchar *str, cchar *suffix)
{
    if (str == NULL || suffix == NULL) {
        return 0;
    }
    if (strcmp(&str[strlen(str) - strlen(suffix) - 1], suffix) == 0) {
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
uint shash(cchar *cname, size_t len)
{
    uchar   *name;
    uint    hash, rem, tmp;

    mprAssert(cname);
    mprAssert(0 <= len && len < MAXINT);

    if (cname == NULL) {
        return 0;
    }
    hash = len;
    rem = len & 3;
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
uint shashlower(cchar *cname, size_t len)
{
    uchar   *name;
    uint    hash, rem, tmp;

    mprAssert(cname);
    mprAssert(0 <= len && len < MAXINT);

    if (cname == NULL) {
        return 0;
    }
    hash = len;
    rem = len & 3;
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

    mprAssert(str);

    va_start(ap, str);
    result = sjoinv(str, ap);
    va_end(ap);
    return result;
}


char *sjoinv(cchar *buf, va_list args)
{
    va_list     ap;
    char        *dest, *str, *dp;
    int         required;

    mprAssert(buf);

    va_copy(ap, args);
    required = 1;
    if (buf) {
        required += strlen(buf);
    }
    str = va_arg(ap, char*);
    while (str) {
        required += strlen(str);
        str = va_arg(ap, char*);
    }
    if ((dest = mprAlloc(required)) == 0) {
        return 0;
    }
    dp = dest;
    if (buf) {
        strcpy(dp, buf);
        dp += strlen(buf);
    }
    va_copy(ap, args);
    str = va_arg(ap, char*);
    while (str) {
        strcpy(dp, str);
        dp += strlen(str);
        str = va_arg(ap, char*);
    }
    *dp = '\0';
    return dest;
}


size_t slen(cchar *s)
{
    return s ? strlen(s) : 0;
}


/*  
    Map a string to lower case (overwrites original string)
 */
char *slower(char *str)
{
    char    *cp;

    mprAssert(str);

    if (str) {
        for (cp = str; *cp; cp++) {
            if (isupper((int) *cp)) {
                *cp = (char) tolower((int) *cp);
            }
        }
    }
    return str;
}


int sncasecmp(cchar *s1, cchar *s2, size_t n)
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
    Case sensitive string comparison. Limited by length
 */
int sncmp(cchar *s1, cchar *s2, size_t n)
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
size_t sncopy(char *dest, size_t destMax, cchar *src, size_t count)
{
    size_t      len;

    mprAssert(dest);
    mprAssert(src);
    mprAssert(src != dest);
    mprAssert(0 <= count && count < MAXINT);
    mprAssert(0 < destMax && destMax < MAXINT);

    len = strlen(src);
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


char *spbrk(cchar *str, cchar *set)
{
    cchar       *sp;
    int         count;

    if (str == NULL || set == NULL) {
        return 0;
    }
    for (count = 0; *str; count++) {
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
    int         len, required;

    va_copy(ap, args);
    len = slen(buf);
    required = len + 1;
    str = va_arg(ap, char*);
    while (str) {
        required += strlen(str);
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
        dp += strlen(str);
        str = va_arg(ap, char*);
    }
    *dp = '\0';
    return dest;
}


size_t sspn(cchar *str, cchar *set)
{
#if KEEP
    cchar       *sp;
    int         count;

    if (str == NULL || set == NULL) {
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
    if (str == NULL || set == NULL) {
        return 0;
    }
    return strspn(str, set);
#endif
}
 

int sstarts(cchar *str, cchar *prefix)
{
    if (str == NULL || prefix == NULL) {
        return 0;
    }
    if (strncmp(str, prefix, strlen(prefix)) == 0) {
        return 1;
    }
    return 0;
}


char *scontains(cchar *str, cchar *pattern, size_t limit)
{
    cchar   *cp, *s1, *s2;
    size_t  lim;

    mprAssert(0 <= limit && limit < MAXINT);
    
    if (str == 0) {
        return 0;
    }
    if (pattern == 0 || *pattern == '\0') {
        return 0;
    }
    for (cp = str; *cp; cp++) {
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


/*
    Parse a number and check for parse errors. Supports radix 8, 10 or 16. 
    If radix is <= 0, then the radix is sleuthed from the input.
    Supports formats:
        [(+|-)][0][OCTAL_DIGITS]
        [(+|-)][0][(x|X)][HEX_DIGITS]
        [(+|-)][DIGITS]

 */
int64 stoi(cchar *str, int radix, int *err)
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
 */
char *stok(char *str, cchar *delim, char **last)
{
    char    *start, *end;
    int     i;

    start = str ? str : *last;

    if (start == 0) {
        *last = 0;
        return 0;
    }
    i = (int) strspn(start, delim);
    start += i;
    if (*start == '\0') {
        *last = 0;
        return 0;
    }
    end = strpbrk(start, delim);
    if (end) {
        *end++ = '\0';
        i = (int) strspn(end, delim);
        end += i;
    }
    *last = end;
    return start;
}


char *ssub(char *str, size_t offset, size_t len)
{
    char    *result;
    size_t  size;

    mprAssert(str);
    mprAssert(offset >= 0);
    mprAssert(0 <= len && len < MAXINT);

    if (str == NULL) {
        return NULL;
    }
    size = len + 1;
    if ((result = mprAlloc(size)) == NULL) {
        return NULL;
    }
    sncopy(result, size, &str[offset], len);
    return result;
}


char *strim(char *str, cchar *set, int where)
{
    int     len, i;

    if (str == NULL || set == NULL) {
        return str;
    }
    if (where & MPR_TRIM_START) {
        i = (int) strspn(str, set);
    } else {
        i = 0;
    }
    str += i;
    if (where & MPR_TRIM_END) {
        len = strlen(str);
        while (len > 0 && strspn(&str[len - 1], set) > 0) {
            str[len - 1] = '\0';
            len--;
        }
    }
    return str;
}



/*  
    Map a string to upper case (overwrites buffer)
 */
char *supper(char *str)
{
    char    *cp;

    mprAssert(str);
    if (str) {
        for (cp = str; *cp; cp++) {
            if (islower((int) *cp)) {
                *cp = (char) toupper((int) *cp);
            }
        }
    }
    return str;
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
