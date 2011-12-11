/**
    mprPrintf.c - Printf routines safe for embedded programming

    This module provides safe replacements for the standard printf formatting routines. Most routines in this file 
    are not thread-safe. It is the callers responsibility to perform all thread synchronization.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/*********************************** Defines **********************************/
/*
    Class definitions
 */
#define CLASS_NORMAL    0               /* [All other]      Normal characters */
#define CLASS_PERCENT   1               /* [%]              Begin format */
#define CLASS_MODIFIER  2               /* [-+ #,]          Modifiers */
#define CLASS_ZERO      3               /* [0]              Special modifier - zero pad */
#define CLASS_STAR      4               /* [*]              Width supplied by arg */
#define CLASS_DIGIT     5               /* [1-9]            Field widths */
#define CLASS_DOT       6               /* [.]              Introduce precision */
#define CLASS_BITS      7               /* [hlL]            Length bits */
#define CLASS_TYPE      8               /* [cdefginopsSuxX] Type specifiers */

#define STATE_NORMAL    0               /* Normal chars in format string */
#define STATE_PERCENT   1               /* "%" */
#define STATE_MODIFIER  2               /* -+ #,*/
#define STATE_WIDTH     3               /* Width spec */
#define STATE_DOT       4               /* "." */
#define STATE_PRECISION 5               /* Precision spec */
#define STATE_BITS      6               /* Size spec */
#define STATE_TYPE      7               /* Data type */
#define STATE_COUNT     8

/*
    Format:         %[modifier][width][precision][bits][type]

    [-+ #,]         Modifiers
    [hlL]           Length bits
 */


/*
    Flags
 */
#define SPRINTF_LEFT        0x1         /* Left align */
#define SPRINTF_SIGN        0x2         /* Always sign the result */
#define SPRINTF_LEAD_SPACE  0x4         /* put leading space for +ve numbers */
#define SPRINTF_ALTERNATE   0x8         /* Alternate format */
#define SPRINTF_LEAD_ZERO   0x10        /* Zero pad */
#define SPRINTF_SHORT       0x20        /* 16-bit */
#define SPRINTF_LONG        0x40        /* 32-bit */
#define SPRINTF_INT64       0x80        /* 64-bit */
#define SPRINTF_COMMA       0x100       /* Thousand comma separators */
#define SPRINTF_UPPER_CASE  0x200       /* As the name says for numbers */

typedef struct Format {
    uchar   *buf;
    uchar   *endbuf;
    uchar   *start;
    uchar   *end;
    ssize   growBy;
    ssize   maxsize;
    int     precision;
    int     radix;
    int     width;
    int     flags;
    int     len;
} Format;

#define BPUT(fmt, c) \
    if (1) { \
        /* Less one to allow room for the null */ \
        if ((fmt)->end >= ((fmt)->endbuf - sizeof(char))) { \
            if (growBuf(fmt) > 0) { \
                *(fmt)->end++ = (c); \
            } \
        } else { \
            *(fmt)->end++ = (c); \
        } \
    } else

#define BPUTNULL(fmt) \
    if (1) { \
        if ((fmt)->end > (fmt)->endbuf) { \
            if (growBuf(fmt) > 0) { \
                *(fmt)->end = '\0'; \
            } \
        } else { \
            *(fmt)->end = '\0'; \
        } \
    } else 

/*
    Just for Ejscript to be able to do %N and %S. THIS MUST MATCH EjsString in ejs.h
 */
typedef struct MprEjsString {
    void            *xtype;
#if BLD_DEBUG
    char            *kind;
    void            *type;
    MprMem          *mem;
#endif
    void            *next;
    void            *prev;
    ssize           length;
    MprChar         value[0];
} MprEjsString;

typedef struct MprEjsName {
    MprEjsString    *name;
    MprEjsString    *space;
} MprEjsName;

/***************************** Forward Declarations ***************************/

static int  getState(char c, int state);
static int  growBuf(Format *fmt);
static char *sprintfCore(char *buf, ssize maxsize, cchar *fmt, va_list arg);
static void outNum(Format *fmt, cchar *prefix, uint64 val);
static void outString(Format *fmt, cchar *str, ssize len);
#if BLD_CHAR_LEN > 1
static void outWideString(Format *fmt, MprChar *str, ssize len);
#endif
#if BLD_FEATURE_FLOAT
static void outFloat(Format *fmt, char specChar, double value);
#endif

/************************************* Code ***********************************/

ssize mprPrintf(cchar *fmt, ...)
{
    va_list     ap;
    char        *buf;
    ssize       len;

    /* No asserts here as this is used as part of assert reporting */

    va_start(ap, fmt);
    buf = mprAsprintfv(fmt, ap);
    va_end(ap);
    if (buf != 0 && MPR->stdOutput) {
        len = mprWriteFileString(MPR->stdOutput, buf);
    } else {
        len = -1;
    }
    return len;
}


ssize mprPrintfError(cchar *fmt, ...)
{
    va_list     ap;
    ssize       len;
    char        *buf;

    /* No asserts here as this is used as part of assert reporting */

    va_start(ap, fmt);
    buf = mprAsprintfv(fmt, ap);
    va_end(ap);
    if (buf && MPR->stdError) {
        len = mprWriteFileString(MPR->stdError, buf);
    } else {
        len = -1;
    }
    return len;
}


ssize mprFprintf(MprFile *file, cchar *fmt, ...)
{
    ssize       len;
    va_list     ap;
    char        *buf;

    if (file == 0) {
        return MPR_ERR_BAD_HANDLE;
    }
    va_start(ap, fmt);
    buf = mprAsprintfv(fmt, ap);
    va_end(ap);
    if (buf) {
        len = mprWriteFileString(file, buf);
    } else {
        len = -1;
    }
    return len;
}


#if FUTURE
/*
    Printf with a static buffer. Used internally only. WILL NOT MALLOC.
 */
int mprStaticPrintf(cchar *fmt, ...)
{
    MprFileSystem   *fs;
    va_list         ap;
    char            buf[MPR_MAX_STRING];

    fs = mprLookupFileSystem(NULL, "/");

    va_start(ap, fmt);
    sprintfCore(buf, MPR_MAX_STRING, fmt, ap);
    va_end(ap);
    return mprWriteFile(fs->stdOutput, buf, slen(buf));
}


/*
    Printf with a static buffer. Used internally only. WILL NOT MALLOC.
 */
int mprStaticPrintfError(cchar *fmt, ...)
{
    MprFileSystem   *fs;
    va_list         ap;
    char            buf[MPR_MAX_STRING];

    fs = mprLookupFileSystem(NULL, "/");

    va_start(ap, fmt);
    sprintfCore(buf, MPR_MAX_STRING, fmt, ap);
    va_end(ap);
    return mprWriteFile(fs->stdError, buf, slen(buf));
}
#endif


char *mprSprintf(char *buf, ssize bufsize, cchar *fmt, ...)
{
    va_list     ap;
    char        *result;

    mprAssert(buf);
    mprAssert(fmt);
    mprAssert(bufsize > 0);

    va_start(ap, fmt);
    result = sprintfCore(buf, bufsize, fmt, ap);
    va_end(ap);
    return result;
}


char *mprSprintfv(char *buf, ssize bufsize, cchar *fmt, va_list arg)
{
    mprAssert(buf);
    mprAssert(fmt);
    mprAssert(bufsize > 0);

    return sprintfCore(buf, bufsize, fmt, arg);
}


char *mprAsprintf(cchar *fmt, ...)
{
    va_list     ap;
    char        *buf;

    mprAssert(fmt);

    va_start(ap, fmt);
    buf = sprintfCore(NULL, -1, fmt, ap);
    va_end(ap);
    return buf;
}


char *mprAsprintfv(cchar *fmt, va_list arg)
{
    mprAssert(fmt);
    return sprintfCore(NULL, -1, fmt, arg);
}


static int getState(char c, int state)
{
    /*
     *  Declared here for Brew which can't handle globals.
     */
    char stateMap[] = {
    /*     STATES:  Normal Percent Modifier Width  Dot  Prec Bits Type */
    /* CLASS           0      1       2       3     4     5    6    7  */
    /* Normal   0 */   0,     0,      0,      0,    0,    0,   0,   0,
    /* Percent  1 */   1,     0,      1,      1,    1,    1,   1,   1,
    /* Modifier 2 */   0,     2,      2,      0,    0,    0,   0,   0,
    /* Zero     3 */   0,     2,      2,      3,    5,    5,   0,   0,
    /* Star     4 */   0,     3,      3,      0,    5,    0,   0,   0,
    /* Digit    5 */   0,     3,      3,      3,    5,    5,   0,   0,
    /* Dot      6 */   0,     4,      4,      4,    0,    0,   0,   0,
    /* Bits     7 */   0,     6,      6,      6,    6,    6,   6,   0,
    /* Types    8 */   0,     7,      7,      7,    7,    7,   7,   0,
    };

    /*
     *  Format:         %[modifier][width][precision][bits][type]
     *
     *  The Class map will map from a specifier letter to a state.
     */
    char classMap[] = {
        /*   0  ' '    !     "     #     $     %     &     ' */
                 2,    0,    0,    2,    0,    1,    0,    0,
        /*  07   (     )     *     +     ,     -     .     / */
                 0,    0,    4,    2,    2,    2,    6,    0,
        /*  10   0     1     2     3     4     5     6     7 */
                 3,    5,    5,    5,    5,    5,    5,    5,
        /*  17   8     9     :     ;     <     =     >     ? */
                 5,    5,    0,    0,    0,    0,    0,    0,
        /*  20   @     A     B     C     D     E     F     G */
                 8,    0,    0,    0,    0,    0,    0,    0,
        /*  27   H     I     J     K     L     M     N     O */
                 0,    0,    0,    0,    7,    0,    8,    0,
        /*  30   P     Q     R     S     T     U     V     W */
                 0,    0,    0,    8,    0,    0,    0,    0,
        /*  37   X     Y     Z     [     \     ]     ^     _ */
                 8,    0,    0,    0,    0,    0,    0,    0,
        /*  40   '     a     b     c     d     e     f     g */
                 0,    0,    0,    8,    8,    8,    8,    8,
        /*  47   h     i     j     k     l     m     n     o */
                 7,    8,    0,    0,    7,    0,    8,    8,
        /*  50   p     q     r     s     t     u     v     w */
                 8,    0,    0,    8,    0,    8,    0,    8,
        /*  57   x     y     z  */
                 8,    0,    0,
    };

    int     chrClass;

    if (c < ' ' || c > 'z') {
        chrClass = CLASS_NORMAL;
    } else {
        mprAssert((c - ' ') < (int) sizeof(classMap));
        chrClass = classMap[(c - ' ')];
    }
    mprAssert((chrClass * STATE_COUNT + state) < (int) sizeof(stateMap));
    state = stateMap[chrClass * STATE_COUNT + state];
    return state;
}


static char *sprintfCore(char *buf, ssize maxsize, cchar *spec, va_list arg)
{
    Format        fmt;
    MprEjsString  *es;
    MprEjsName    qname;
    ssize         len;
    int64         iValue;
    uint64        uValue;
    int           state;
    char          c, *safe;

    if (spec == 0) {
        spec = "";
    }
    if (buf != 0) {
        mprAssert(maxsize > 0);
        fmt.buf = (uchar*) buf;
        fmt.endbuf = &fmt.buf[maxsize];
        fmt.growBy = -1;
    } else {
        if (maxsize <= 0) {
            maxsize = MAXINT;
        }
        len = min(MPR_DEFAULT_ALLOC, maxsize);
        buf = mprAlloc(len);
        if (buf == 0) {
            return 0;
        }
        fmt.buf = (uchar*) buf;
        fmt.endbuf = &fmt.buf[len];
        fmt.growBy = min(MPR_DEFAULT_ALLOC * 2, maxsize - len);
    }
    fmt.maxsize = maxsize;
    fmt.start = fmt.buf;
    fmt.end = fmt.buf;
    fmt.len = 0;
    *fmt.start = '\0';

    state = STATE_NORMAL;

    while ((c = *spec++) != '\0') {
        state = getState(c, state);

        switch (state) {
        case STATE_NORMAL:
            BPUT(&fmt, c);
            break;

        case STATE_PERCENT:
            fmt.precision = -1;
            fmt.width = 0;
            fmt.flags = 0;
            break;

        case STATE_MODIFIER:
            switch (c) {
            case '+':
                fmt.flags |= SPRINTF_SIGN;
                break;
            case '-':
                fmt.flags |= SPRINTF_LEFT;
                break;
            case '#':
                fmt.flags |= SPRINTF_ALTERNATE;
                break;
            case '0':
                fmt.flags |= SPRINTF_LEAD_ZERO;
                break;
            case ' ':
                fmt.flags |= SPRINTF_LEAD_SPACE;
                break;
            case ',':
                fmt.flags |= SPRINTF_COMMA;
                break;
            }
            break;

        case STATE_WIDTH:
            if (c == '*') {
                fmt.width = va_arg(arg, int);
                if (fmt.width < 0) {
                    fmt.width = -fmt.width;
                    fmt.flags |= SPRINTF_LEFT;
                }
            } else {
                while (isdigit((int) c)) {
                    fmt.width = fmt.width * 10 + (c - '0');
                    c = *spec++;
                }
                spec--;
            }
            break;

        case STATE_DOT:
            fmt.precision = 0;
            break;

        case STATE_PRECISION:
            if (c == '*') {
                fmt.precision = va_arg(arg, int);
            } else {
                while (isdigit((int) c)) {
                    fmt.precision = fmt.precision * 10 + (c - '0');
                    c = *spec++;
                }
                spec--;
            }
            break;

        case STATE_BITS:
            switch (c) {
            case 'L':
                fmt.flags |= SPRINTF_INT64;
                break;

            case 'l':
                fmt.flags |= SPRINTF_LONG;
                break;

            case 'h':
                fmt.flags |= SPRINTF_SHORT;
                break;
            }
            break;

        case STATE_TYPE:
            switch (c) {
            case 'e':
#if BLD_FEATURE_FLOAT
            case 'g':
            case 'f':
                fmt.radix = 10;
                outFloat(&fmt, c, (double) va_arg(arg, double));
                break;
#endif /* BLD_FEATURE_FLOAT */

            case 'c':
                BPUT(&fmt, (char) va_arg(arg, int));
                break;

            case 'N':
                /* Name */
                qname = va_arg(arg, MprEjsName);
                if (qname.name) {
#if BLD_CHAR_LEN == 1
                    outString(&fmt, qname.space->value, qname.space->length);
                    BPUT(&fmt, ':');
                    BPUT(&fmt, ':');
                    outString(&fmt, qname.name->value, qname.name->length);
#else
                    outWideString(&fmt, qname.space->value, qname.space->length);
                    BPUT(&fmt, ':');
                    BPUT(&fmt, ':');
                    outWideString(&fmt, qname.name->value, qname.name->length);
#endif
                } else {
                    outString(&fmt, NULL, 0);
                }
                break;

            case 'S':
                /* Safe string */
#if BLD_CHAR_LEN > 1
                if (fmt.flags & SPRINTF_LONG) {
                    safe = mprEscapeHtml(va_arg(arg, MprChar*));
                    outWideString(&fmt, safe, -1);
                } else
#endif
                {
                    safe = mprEscapeHtml(va_arg(arg, MprChar*));
                    outString(&fmt, safe, -1);
                }
                break;

            case '@':
                /* MprEjsString */
                es = va_arg(arg, MprEjsString*);
                if (es) {
#if BLD_CHAR_LEN == 1
                    outString(&fmt, es->value, es->length);
#else
                    outWideString(&fmt, es->value, es->length);
#endif
                } else {
                    outString(&fmt, NULL, 0);
                }
                break;

            case 'w':
                /* Wide string of MprChar characters (Same as %ls"). Null terminated. */
#if BLD_CHAR_LEN > 1
                outWideString(&fmt, va_arg(arg, MprChar*), -1);
                break;
#else
                /* Fall through */
#endif

            case 's':
                /* Standard string */
#if BLD_CHAR_LEN > 1
                if (fmt.flags & SPRINTF_LONG) {
                    outWideString(&fmt, va_arg(arg, MprChar*), -1);
                } else
#endif
                    outString(&fmt, va_arg(arg, char*), -1);
                break;

            case 'i':
                ;

            case 'd':
                fmt.radix = 10;
                if (fmt.flags & SPRINTF_SHORT) {
                    iValue = (short) va_arg(arg, int);
                } else if (fmt.flags & SPRINTF_LONG) {
                    iValue = (long) va_arg(arg, long);
                } else if (fmt.flags & SPRINTF_INT64) {
                    iValue = (int64) va_arg(arg, int64);
                } else {
                    iValue = (int) va_arg(arg, int);
                }
                if (iValue >= 0) {
                    if (fmt.flags & SPRINTF_LEAD_SPACE) {
                        outNum(&fmt, " ", iValue);
                    } else if (fmt.flags & SPRINTF_SIGN) {
                        outNum(&fmt, "+", iValue);
                    } else {
                        outNum(&fmt, 0, iValue);
                    }
                } else {
                    outNum(&fmt, "-", -iValue);
                }
                break;

            case 'X':
                fmt.flags |= SPRINTF_UPPER_CASE;
#if MPR_64_BIT
                fmt.flags &= ~(SPRINTF_SHORT|SPRINTF_LONG);
                fmt.flags |= SPRINTF_INT64;
#else
                fmt.flags &= ~(SPRINTF_INT64);
#endif
                /*  Fall through  */
            case 'o':
            case 'x':
            case 'u':
                if (fmt.flags & SPRINTF_SHORT) {
                    uValue = (ushort) va_arg(arg, uint);
                } else if (fmt.flags & SPRINTF_LONG) {
                    uValue = (ulong) va_arg(arg, ulong);
                } else if (fmt.flags & SPRINTF_INT64) {
                    uValue = (uint64) va_arg(arg, uint64);
                } else {
                    uValue = va_arg(arg, uint);
                }
                if (c == 'u') {
                    fmt.radix = 10;
                    outNum(&fmt, 0, uValue);
                } else if (c == 'o') {
                    fmt.radix = 8;
                    if (fmt.flags & SPRINTF_ALTERNATE && uValue != 0) {
                        outNum(&fmt, "0", uValue);
                    } else {
                        outNum(&fmt, 0, uValue);
                    }
                } else {
                    fmt.radix = 16;
                    if (fmt.flags & SPRINTF_ALTERNATE && uValue != 0) {
                        if (c == 'X') {
                            outNum(&fmt, "0X", uValue);
                        } else {
                            outNum(&fmt, "0x", uValue);
                        }
                    } else {
                        outNum(&fmt, 0, uValue);
                    }
                }
                break;

            case 'n':       /* Count of chars seen thus far */
                if (fmt.flags & SPRINTF_SHORT) {
                    short *count = va_arg(arg, short*);
                    *count = (int) (fmt.end - fmt.start);
                } else if (fmt.flags & SPRINTF_LONG) {
                    long *count = va_arg(arg, long*);
                    *count = (int) (fmt.end - fmt.start);
                } else {
                    int *count = va_arg(arg, int *);
                    *count = (int) (fmt.end - fmt.start);
                }
                break;

            case 'p':       /* Pointer */
#if MPR_64_BIT
                uValue = (uint64) va_arg(arg, void*);
#else
                uValue = (uint) PTOI(va_arg(arg, void*));
#endif
                fmt.radix = 16;
                outNum(&fmt, "0x", uValue);
                break;

            default:
                BPUT(&fmt, c);
            }
        }
    }
    BPUTNULL(&fmt);
    return (char*) fmt.buf;
}


static void outString(Format *fmt, cchar *str, ssize len)
{
    cchar   *cp;
    ssize   i;

    if (str == NULL) {
        str = "null";
        len = 4;
    } else if (fmt->flags & SPRINTF_ALTERNATE) {
        str++;
        len = (ssize) *str;
    } else if (fmt->precision >= 0) {
        for (cp = str, len = 0; len < fmt->precision; len++) {
            if (*cp++ == '\0') {
                break;
            }
        }
    } else if (len < 0) {
        len = slen(str);
    }
    if (!(fmt->flags & SPRINTF_LEFT)) {
        for (i = len; i < fmt->width; i++) {
            BPUT(fmt, (char) ' ');
        }
    }
    for (i = 0; i < len && *str; i++) {
        BPUT(fmt, *str++);
    }
    if (fmt->flags & SPRINTF_LEFT) {
        for (i = len; i < fmt->width; i++) {
            BPUT(fmt, (char) ' ');
        }
    }
}


#if BLD_CHAR_LEN > 1
static void outWideString(Format *fmt, MprChar *str, ssize len)
{
    MprChar     *cp;
    int         i;

    if (str == 0) {
        BPUT(fmt, (char) 'n');
        BPUT(fmt, (char) 'u');
        BPUT(fmt, (char) 'l');
        BPUT(fmt, (char) 'l');
        return;
    } else if (fmt->flags & SPRINTF_ALTERNATE) {
        str++;
        len = (ssize) *str;
    } else if (fmt->precision >= 0) {
        for (cp = str, len = 0; len < fmt->precision; len++) {
            if (*cp++ == 0) {
                break;
            }
        }
    } else if (len < 0) {
        for (cp = str, len = 0; *cp++ == 0; len++) ;
    }
    if (!(fmt->flags & SPRINTF_LEFT)) {
        for (i = len; i < fmt->width; i++) {
            BPUT(fmt, (char) ' ');
        }
    }
    for (i = 0; i < len && *str; i++) {
        BPUT(fmt, *str++);
    }
    if (fmt->flags & SPRINTF_LEFT) {
        for (i = len; i < fmt->width; i++) {
            BPUT(fmt, (char) ' ');
        }
    }
}
#endif


static void outNum(Format *fmt, cchar *prefix, uint64 value)
{
    char    numBuf[64];
    char    *cp;
    char    *endp;
    char    c;
    int     letter, len, leadingZeros, i, fill;

    endp = &numBuf[sizeof(numBuf) - 1];
    *endp = '\0';
    cp = endp;

    /*
     *  Convert to ascii
     */
    if (fmt->radix == 16) {
        do {
            letter = (int) (value % fmt->radix);
            if (letter > 9) {
                if (fmt->flags & SPRINTF_UPPER_CASE) {
                    letter = 'A' + letter - 10;
                } else {
                    letter = 'a' + letter - 10;
                }
            } else {
                letter += '0';
            }
            *--cp = letter;
            value /= fmt->radix;
        } while (value > 0);

    } else if (fmt->flags & SPRINTF_COMMA) {
        i = 1;
        do {
            *--cp = '0' + (int) (value % fmt->radix);
            value /= fmt->radix;
            if ((i++ % 3) == 0 && value > 0) {
                *--cp = ',';
            }
        } while (value > 0);
    } else {
        do {
            *--cp = '0' + (int) (value % fmt->radix);
            value /= fmt->radix;
        } while (value > 0);
    }

    len = (int) (endp - cp);
    fill = fmt->width - len;

    if (prefix != 0) {
        fill -= (int) slen(prefix);
    }
    leadingZeros = (fmt->precision > len) ? fmt->precision - len : 0;
    fill -= leadingZeros;

    if (!(fmt->flags & SPRINTF_LEFT)) {
        c = (fmt->flags & SPRINTF_LEAD_ZERO) ? '0': ' ';
        for (i = 0; i < fill; i++) {
            BPUT(fmt, c);
        }
    }
    if (prefix != 0) {
        while (*prefix) {
            BPUT(fmt, *prefix++);
        }
    }
    for (i = 0; i < leadingZeros; i++) {
        BPUT(fmt, '0');
    }
    while (*cp) {
        BPUT(fmt, *cp);
        cp++;
    }
    if (fmt->flags & SPRINTF_LEFT) {
        for (i = 0; i < fill; i++) {
            BPUT(fmt, ' ');
        }
    }
}


#if BLD_FEATURE_FLOAT
static void outFloat(Format *fmt, char specChar, double value)
{
    char    result[MPR_MAX_STRING], *cp;
    int     c, fill, i, len, index;

    result[0] = '\0';
    if (specChar == 'f') {
        sprintf(result, "%.*f", fmt->precision, value);
        // result = mprDtoa(value, fmt->precision, MPR_DTOA_ALL_DIGITS, MPR_DTOA_FIXED_FORM);
        // sprintf(result, "%*.*f", fmt->width, fmt->precision, value);

    } else if (specChar == 'g') {
        sprintf(result, "%*.*g", fmt->width, fmt->precision, value);
        // sprintf(result, "%*.*g", fmt->width, fmt->precision, value);
        // result = mprDtoa(value, fmt->precision, 0, 0);

    } else if (specChar == 'e') {
        sprintf(result, "%*.*e", fmt->width, fmt->precision, value);
        // result = mprDtoa(value, fmt->precision, MPR_DTOA_N_DIGITS, MPR_DTOA_EXPONENT_FORM);
        // sprintf(result, "%*.*e", fmt->width, fmt->precision, value);
    }
    len = (int) slen(result);
    fill = fmt->width - len;
    if (fmt->flags & SPRINTF_COMMA) {
        if (((len - 1) / 3) > 0) {
            fill -= (len - 1) / 3;
        }
    }

    if (fmt->flags & SPRINTF_SIGN && value > 0) {
        BPUT(fmt, '+');
        fill--;
    }
    if (!(fmt->flags & SPRINTF_LEFT)) {
        c = (fmt->flags & SPRINTF_LEAD_ZERO) ? '0': ' ';
        for (i = 0; i < fill; i++) {
            BPUT(fmt, c);
        }
    }
    index = len;
    for (cp = result; *cp; cp++) {
        BPUT(fmt, *cp);
        if (fmt->flags & SPRINTF_COMMA) {
            if ((--index % 3) == 0 && index > 0) {
                BPUT(fmt, ',');
            }
        }
    }
    if (fmt->flags & SPRINTF_LEFT) {
        for (i = 0; i < fill; i++) {
            BPUT(fmt, ' ');
        }
    }
    BPUTNULL(fmt);
}

int mprIsNan(double value) {
#if WIN
    return _fpclass(value) & (_FPCLASS_SNAN | _FPCLASS_QNAN);
#elif VXWORKS
    return value == (0.0 / 0.0);
#else
    return fpclassify(value) == FP_NAN;
#endif
}


int mprIsInfinite(double value) {
#if WIN
    return _fpclass(value) & (_FPCLASS_PINF | _FPCLASS_NINF);
#elif VXWORKS
    return value == (1.0 / 0.0) || value == (-1.0 / 0.0);
#else
    return fpclassify(value) == FP_INFINITE;
#endif
}

int mprIsZero(double value) {
#if WIN
    return _fpclass(value) & (_FPCLASS_NZ | _FPCLASS_PZ);
#elif VXWORKS
    return value == 0.0 || value == -0.0;
#else
    return fpclassify(value) == FP_ZERO;
#endif
}

/*
    Convert a double to ascii. Caller must free the result. This uses the JavaScript ECMA-262 spec for formatting rules.

    function dtoa(double value, int mode, int ndigits, int *periodOffset, int *sign, char **end)
 */
char *mprDtoa(double value, int ndigits, int mode, int flags)
{
    MprBuf  *buf;
    char    *intermediate, *ip;
    int     period, sign, len, exponentForm, fixedForm, exponent, count, totalDigits, npad;

    buf = mprCreateBuf(64, -1);
    intermediate = 0;
    exponentForm = 0;
    fixedForm = 0;

    if (mprIsNan(value)) {
        mprPutStringToBuf(buf, "NaN");

    } else if (mprIsInfinite(value)) {
        if (value < 0) {
            mprPutStringToBuf(buf, "-Infinity");
        } else {
            mprPutStringToBuf(buf, "Infinity");
        }
    } else if (value == 0) {
        mprPutCharToBuf(buf, '0');

    } else {
        if (ndigits <= 0) {
            if (!(flags & MPR_DTOA_FIXED_FORM)) {
                mode = MPR_DTOA_ALL_DIGITS;
            }
            ndigits = 0;

        } else if (mode == MPR_DTOA_ALL_DIGITS) {
            mode = MPR_DTOA_N_DIGITS;
        }
        if (flags & MPR_DTOA_EXPONENT_FORM) {
            exponentForm = 1;
            if (ndigits > 0) {
                ndigits++;
            } else {
                ndigits = 0;
                mode = MPR_DTOA_ALL_DIGITS;
            }
        } else if (flags & MPR_DTOA_FIXED_FORM) {
            fixedForm = 1;
        }

        /*
            Convert to an intermediate string representation. Period is the offset of the decimal point. NOTE: the
            intermediate representation may have less digits than period.
            Note: ndigits < 0 seems to trim N digits from the end with rounding.
         */
        ip = intermediate = dtoa(value, mode, ndigits, &period, &sign, NULL);
        len = (int) slen(intermediate);
        exponent = period - 1;

        if (mode == MPR_DTOA_ALL_DIGITS && ndigits == 0) {
            ndigits = len;
        }
        if (!fixedForm) {
            if (period <= -6 || period > 21) {
                exponentForm = 1;
            }
        }
        if (sign) {
            mprPutCharToBuf(buf, '-');
        }
        if (exponentForm) {
            mprPutCharToBuf(buf, ip[0] ? ip[0] : '0');
            if (len > 1) {
                mprPutCharToBuf(buf, '.');
                mprPutSubStringToBuf(buf, &ip[1], (ndigits == 0) ? len - 1: ndigits);
            }
            mprPutCharToBuf(buf, 'e');
            mprPutCharToBuf(buf, (period < 0) ? '-' : '+');
            mprPutFmtToBuf(buf, "%d", (exponent < 0) ? -exponent: exponent);

        } else {
            if (mode == MPR_DTOA_N_FRACTION_DIGITS) {
                /* Count of digits */
                if (period <= 0) {
                    /* Leading fractional zeros required */
                    mprPutStringToBuf(buf, "0.");
                    mprPutPadToBuf(buf, '0', -period);
                    mprPutStringToBuf(buf, ip);
                    npad = ndigits - len + period;
                    if (npad > 0) {
                        mprPutPadToBuf(buf, '0', npad);
                    }

                } else {
                    count = min(len, period);
                    /* Leading integral digits */
                    mprPutSubStringToBuf(buf, ip, count);
                    /* Trailing zero pad */
                    if (period > len) {
                        mprPutPadToBuf(buf, '0', period - len);
                    }
                    totalDigits = count + ndigits;
                    if (period < totalDigits) {
                        count = totalDigits + sign - (int) mprGetBufLength(buf);
                        mprPutCharToBuf(buf, '.');
                        mprPutSubStringToBuf(buf, &ip[period], count);
                        mprPutPadToBuf(buf, '0', count - slen(&ip[period]));
                    }
                }

            } else if (len <= period && period <= 21) {
                /* data shorter than period */
                mprPutStringToBuf(buf, ip);
                mprPutPadToBuf(buf, '0', period - len);

            } else if (0 < period && period <= 21) {
                /* Period shorter than data */
                mprPutSubStringToBuf(buf, ip, period);
                mprPutCharToBuf(buf, '.');
                mprPutStringToBuf(buf, &ip[period]);

            } else if (-6 < period && period <= 0) {
                /* Small negative exponent */
                mprPutStringToBuf(buf, "0.");
                mprPutPadToBuf(buf, '0', -period);
                mprPutStringToBuf(buf, ip);

            } else {
                mprAssert(0);
            }
        }
    }
    mprAddNullToBuf(buf);
    if (intermediate) {
        freedtoa(intermediate);
    }
    return sclone(mprGetBufStart(buf));
}
#endif /* BLD_FEATURE_FLOAT */


/*
    Grow the buffer to fit new data. Return 1 if the buffer can grow. 
    Grow using the growBy size specified when creating the buffer. 
 */
static int growBuf(Format *fmt)
{
    uchar   *newbuf;
    ssize   buflen;

    buflen = (int) (fmt->endbuf - fmt->buf);
    if (fmt->maxsize >= 0 && buflen >= fmt->maxsize) {
        return 0;
    }
    if (fmt->growBy <= 0) {
        /*
            User supplied buffer
         */
        return 0;
    }
    newbuf = mprAlloc(buflen + fmt->growBy);
    if (newbuf == 0) {
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    if (fmt->buf) {
        memcpy(newbuf, fmt->buf, buflen);
    }
    buflen += fmt->growBy;
    fmt->end = newbuf + (fmt->end - fmt->buf);
    fmt->start = newbuf + (fmt->start - fmt->buf);
    fmt->buf = newbuf;
    fmt->endbuf = &fmt->buf[buflen];

    /*
        Increase growBy to reduce overhead
     */
    if ((buflen + (fmt->growBy * 2)) < fmt->maxsize) {
        fmt->growBy *= 2;
    }
    return 1;
}


/*
    For easy debug trace
 */
int print(cchar *fmt, ...)
{
    int             len;
    va_list         ap;
    va_start(ap, fmt);
    len = vprintf(fmt, ap);
    va_end(ap);
    return len;
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
