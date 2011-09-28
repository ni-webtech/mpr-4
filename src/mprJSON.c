/**
    mprJSON.c - A JSON parser and serializer. 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/****************************** Forward Declarations **************************/

static MprObj *deserialize(MprJson *jp);
static cchar *eatSpace(cchar *tok);
static cchar *findEndKeyword(MprJson *jp, cchar *str);
static cchar *findQuote(cchar *tok, int quote);
static MprObj *makeObj(MprJson *jp, bool list);
static cchar *parseComment(MprJson *jp);
static void parseError(MprJson *jp, cchar *fmt, ...);
static cchar *parseName(MprJson *jp);
static cchar *parseValue(MprJson *jp);
static int peekSep(MprJson *jp);
static int setItem(MprObj *obj, cchar *value, int type, int index);
static int setKey(MprObj *obj, cchar *name, cchar *value, int type);

/************************************ Code ************************************/

MprObj *mprDeserializeCustom(cchar *str, MprSetItem setItem, MprSetKey setKey, MprMakeObj makeObj)
{
    MprJson         jp;

    memset(&jp, 0, sizeof(jp));
    jp.lineNumber = 1;
    jp.tok = str;
    jp.setItem = setItem;
    jp.setKey = setKey;
    jp.makeObj = makeObj;
    return deserialize(&jp);
}


/*
    Deserialize a JSON string into an MprHash object. Objects and lists "[]" are stored in hashes. 
 */
MprObj *mprDeserialize(cchar *str)
{
    return mprDeserializeCustom(str, setItem, setKey, makeObj); 
}


static MprObj *deserialize(MprJson *jp)
{
    cvoid   *value;
    void    *obj;
    cchar   *name;
    int     rc, sep, isArray, index, valueType;

    obj = 0;
    isArray = 0;
    index = 0;

    while (*jp->tok) {
        jp->tok = eatSpace(jp->tok);
        switch (*jp->tok) {
        case '\0':
            return obj;

        case '\n':
            jp->lineNumber++;
            jp->tok++;
            break;

        case ',':
            jp->tok++;
            index++;
            continue;

        case '/':
            if (jp->tok[1] == '/' || jp->tok[1] == '*') {
                jp->tok = parseComment(jp);
            } else {
                parseError(jp, "Unexpected character'%c'", *jp->tok);
                return 0;
            }
            continue;

        case '{':
            obj = jp->makeObj(jp, 0);
            isArray = 0;
            ++jp->tok;
            break;

        case '[':
            obj = jp->makeObj(jp, 1);
            isArray = 1;
            ++jp->tok;
            break;

        case '}':
        case ']':
            /* End of object or array */
            jp->tok++;
            return obj;
            
        default:
            if (obj == 0) {
                parseError(jp, "Bad format");
                return 0;
            }
            if ((name = parseName(jp)) == 0) {
                return 0;
            }
            if (jp->check && jp->check(jp, name) < 0) {
                parseError(jp, "Check state failed for '%s'", name);
                return 0;
            }
            if ((sep = peekSep(jp)) < 0) {
                /* Already reported */
                return 0;
            }
            if (sep == ':') {
                if (isArray) {
                    parseError(jp, "Bad separator '%c' in list", sep);
                    return 0;
                }
                jp->tok = eatSpace(jp->tok + 1);
                if (*jp->tok == '{') {
                    value = deserialize(jp);
                    valueType = MPR_JSON_OBJ;

                } else if (*jp->tok == '[') {
                    value = deserialize(jp);
                    valueType = MPR_JSON_ARRAY;

                } else {
                    value = parseValue(jp);
                    valueType = MPR_JSON_STRING;
                }
                if (value == 0) {
                    /* Error already reported */
                    return 0;
                }
                if ((rc = jp->setKey(obj, name, value, valueType)) < 0) {
                    parseError(jp, "Can't set key '%s' value '%s'", name, value);
                    return 0;
                }
            } else if (sep == ',' || sep == ']') {
                if (isArray) {
                    if ((rc = jp->setItem(obj, name, valueType, index)) < 0) {
                        parseError(jp, "Can't set item '%s'", name);
                        return 0;
                    }
                } else {
                    parseError(jp, "Bad separator '%c' in properties", sep);
                    return 0;
                }
            } else {
                parseError(jp, "Bad separator '%c'", sep);
                return 0;
            }
        }
    }
    return 0;
}


static cchar *parseComment(MprJson *jp)
{
    cchar   *tok;

    tok = jp->tok;
    if (*tok == '/') {
        for (tok++; *tok && *tok != '\n'; tok++) ;

    } else if (*jp->tok == '*') {
        tok++;
        for (tok++; tok[0] && (tok[0] != '*' || tok[1] != '/'); tok++) {
            if (*tok == '\n') {
                jp->lineNumber++;
            }
        }
    }
    return tok - 1;
}


static cchar *parseQuotedName(MprJson *jp)
{
    cchar    *etok, *name;
    int      quote;

    quote = *jp->tok;
    if ((etok = findQuote(++jp->tok, quote)) == 0) {
        parseError(jp, "Missing closing quote");
        return 0;
    }
    name = snclone(jp->tok, etok - jp->tok);
    jp->tok = ++etok;
    return name;
}


static cchar *parseUnquotedName(MprJson *jp)
{
    cchar    *etok, *name;

    etok = findEndKeyword(jp, jp->tok);
    name = snclone(jp->tok, etok - jp->tok);
    jp->tok = etok;
    return name;
}


static cchar *parseName(MprJson *jp)
{
    jp->tok = eatSpace(jp->tok);
    if (*jp->tok == '"' || *jp->tok == '\'') {
        return parseQuotedName(jp);
    } else {
        return parseUnquotedName(jp);
    }
}


static int peekSep(MprJson *jp)
{
    int     sep;

    jp->tok = eatSpace(jp->tok);
    sep = *jp->tok;
    if (sep != ':' && sep != ',' && sep != ']') {
        parseError(jp, "Missing ':', ',' or ']' in input");
        return MPR_ERR_BAD_FORMAT;
    } 
    return sep;
}


static cchar *parseValue(MprJson *jp)
{
    cchar   *etok, *value;
    int     quote;

    value = 0;
    if (*jp->tok == '"' || *jp->tok == '\'') {
        quote = *jp->tok;
        if ((etok = findQuote(++jp->tok, quote)) == 0) {
            parseError(jp, "Missing closing quote");
            return 0;
        }
        value = snclone(jp->tok, etok - jp->tok);

    } else {
        etok = findEndKeyword(jp, jp->tok);
        value = snclone(jp->tok, etok - jp->tok);
    }
    jp->tok = etok + 1;
    return value;
}


static int setItem(MprObj *obj, cchar *value, int type, int index)
{
    MprKey  *kp;
    char    ibuf[32];

    itosbuf(ibuf, sizeof(ibuf), index, 10);
    if ((kp = mprAddKey(obj, ibuf, value)) == 0) {
        return MPR_ERR_MEMORY;
    }
    kp->type = type;
    return index;
}


static int setKey(MprObj *obj, cchar *key, cchar *value, int type)
{
    MprKey  *kp;

    if ((kp = mprAddKey(obj, key, value)) == 0) {
        return MPR_ERR_MEMORY;
    }
    kp->type = type;
    return 0;
}


/*
    Supports hashes where properties are strings or hashes of strings. N-level nest is supported.
 */
static cchar *objToString(MprBuf *buf, MprObj *obj, int type, int pretty)
{
    MprKey  *kp;
    void    *item;
    int     next;

    if (type == MPR_JSON_ARRAY) {
        mprPutCharToBuf(buf, '[');
        if (pretty) mprPutCharToBuf(buf, '\n');
        for (ITERATE_ITEMS(obj, item, next)) {
            if (pretty) mprPutStringToBuf(buf, "    ");
            if (kp->type != MPR_JSON_STRING) {
                objToString(buf, (MprObj*) kp->data, kp->type, pretty);
            } else {
                mprPutStringToBuf(buf, kp->data);
            }
            mprPutCharToBuf(buf, ',');
            if (pretty) mprPutCharToBuf(buf, '\n');
        }
        mprPutCharToBuf(buf, ']');

    } else if (type == MPR_JSON_OBJ) {
        mprPutCharToBuf(buf, '{');
        if (pretty) mprPutCharToBuf(buf, '\n');
        for (ITERATE_KEYS(obj, kp)) {
            if (pretty) mprPutStringToBuf(buf, "    ");
            mprPutStringToBuf(buf, kp->key);
            mprPutStringToBuf(buf, ": ");
            if (kp->type != MPR_JSON_STRING) {
                objToString(buf, (MprObj*) kp->data, kp->type, pretty);
            } else {
                mprPutStringToBuf(buf, kp->data);
            }
            mprPutCharToBuf(buf, ',');
            if (pretty) mprPutCharToBuf(buf, '\n');
        }
        mprPutCharToBuf(buf, '}');
    }
    if (pretty) mprPutCharToBuf(buf, '\n');
    return sclone(mprGetBufStart(buf));
}


/*
    Serialize into JSON format.
 */
cchar *mprSerialize(MprObj *obj, int flags)
{
    MprBuf  *buf;
    int     pretty;

    pretty = (flags & MPR_JSON_PRETTY);
    if ((buf = mprCreateBuf(0, 0)) == 0) {
        return 0;
    }
    objToString(buf, obj, MPR_JSON_OBJ, pretty);
    return mprGetBuf(buf);
}


static cchar *eatSpace(cchar *tok)
{
    while (isspace((int) *tok) && *tok != '\n') {
        tok++;
    }
    return tok;
}


static cchar *findQuote(cchar *tok, int quote)
{
    cchar   *cp;

    mprAssert(tok);
    for (cp = tok; *cp; cp++) {
        if (*cp == quote && (cp == tok || *cp != '\\')) {
            return cp;
        }
    }
    return 0;
}


static cchar *findEndKeyword(MprJson *jp, cchar *str)
{
    cchar   *cp, *etok;

    mprAssert(str);
    for (cp = jp->tok; *cp; cp++) {
        if ((etok = strpbrk(cp, " \t\n\r:,")) != 0) {
            if (etok == jp->tok || *etok != '\\') {
                return etok;
            }
        }
    }
    return &str[strlen(str)];
}


static void parseError(MprJson *jp, cchar *fmt, ...)
{
    va_list     args;
    cchar       *msg;

    va_start(args, fmt);
    msg = sfmtv(fmt, args);
#if UNUSED
    mprError("%s\nIn file '%s' at line %d", msg, jp->path, jp->lineNumber);
#else
    mprError("%s\nAt line %d", msg, jp->lineNumber);
#endif
    va_end(args);
}


//  MOB - remove jp arg
static MprObj *makeObj(MprJson *jp, bool list)
{
    if (list) {
        return (MprObj*) mprCreateList(0, 0);
    }
    return (MprObj*) mprCreateHash(0, 0);
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
