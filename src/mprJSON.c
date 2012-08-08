/**
    mprJSON.c - A JSON parser and serializer. 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */
/********************************** Includes **********************************/

#include    "mpr.h"

/****************************** Forward Declarations **************************/

static MprObj *deserialize(MprJson *jp);
static char advanceToken(MprJson *jp);
static cchar *findEndKeyword(MprJson *jp, cchar *str);
static cchar *findQuote(cchar *tok, int quote);
static MprObj *makeObj(MprJson *jp, bool list);
static cchar *parseComment(MprJson *jp);
static void jsonParseError(MprJson *jp, cchar *msg);
static cchar *parseName(MprJson *jp);
static cchar *parseValue(MprJson *jp);
static int setValue(MprJson *jp, MprObj *obj, int index, cchar *name, cchar *value, int type);

/************************************ Code ************************************/

MprObj *mprDeserializeCustom(cchar *str, MprJsonCallback callback, void *data)
{
    MprJson     jp;

    /*
        There is no need for GC management as this routine does not yield
     */
    memset(&jp, 0, sizeof(jp));
    jp.lineNumber = 1;
    jp.tok = str;
    jp.callback = callback;
    jp.data = data;
    return deserialize(&jp);
}


/*
    Deserialize a JSON string into an MprHash object. Objects and lists "[]" are stored in hashes. 
 */
MprObj *mprDeserialize(cchar *str)
{
    MprJsonCallback cb;

    cb.checkState = 0;
    cb.makeObj = makeObj;
    cb.parseError = jsonParseError;
    cb.setValue = setValue;
    return mprDeserializeCustom(str, cb, 0); 
}


static MprObj *deserialize(MprJson *jp)
{
    cvoid   *value;
    MprObj  *obj;
    cchar   *name;
    int     token, rc, index, valueType;

    if ((token = advanceToken(jp)) == '[') {
        obj = jp->callback.makeObj(jp, 1);
        index = 0;
    } else if (token == '{') {
        obj = jp->callback.makeObj(jp, 0);
        index = -1;
    } else {
        return (MprObj*) parseValue(jp);
    }
    jp->tok++;

    while (*jp->tok) {
        switch (advanceToken(jp)) {
        case '\0':
            break;

        case ',':
            if (index >= 0) {
                index++;
            }
            jp->tok++;
            continue;

        case '/':
            if (jp->tok[1] == '/' || jp->tok[1] == '*') {
                jp->tok = parseComment(jp);
            } else {
                mprJsonParseError(jp, "Unexpected character '%c'", *jp->tok);
                return 0;
            }
            continue;

        case '}':
        case ']':
            /* End of object or array */
            if (jp->callback.checkState && jp->callback.checkState(jp, NULL) < 0) {
                return 0;
            }
            jp->tok++;
            return obj;
            
        default:
            /*
                Value: String, "{" or "]"
             */
            value = 0;
            if (index < 0) {
                if ((name = parseName(jp)) == 0) {
                    return 0;
                }
                if ((token = advanceToken(jp)) != ':') {
                    if (token == ',' || token == '}' || token == ']') {
                        valueType = MPR_JSON_STRING;
                        value = name;
                    } else {
                        mprJsonParseError(jp, "Bad separator '%c'", *jp->tok);
                        return 0;
                    }
                }
                jp->tok++;
            } else {
                name = 0;
            }
            if (!value) {
                advanceToken(jp);
                if (jp->callback.checkState && jp->callback.checkState(jp, name) < 0) {
                    return 0;
                }
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
                if (!value) {
                    /* Error already reported */
                    return 0;
                }
            }
            if ((rc = jp->callback.setValue(jp, obj, index, name, value, valueType)) < 0) {
                return 0;
            }
        }
    }
    return obj;
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
        mprJsonParseError(jp, "Missing closing quote");
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
    char    token;

    token = advanceToken(jp);
    if (token == '"' || token == '\'') {
        return parseQuotedName(jp);
    } else {
        return parseUnquotedName(jp);
    }
}


static cchar *parseValue(MprJson *jp)
{
    cchar   *etok, *value;
    int     quote;

    value = 0;
    if (*jp->tok == '"' || *jp->tok == '\'') {
        quote = *jp->tok;
        if ((etok = findQuote(++jp->tok, quote)) == 0) {
            mprJsonParseError(jp, "Missing closing quote");
            return 0;
        }
        value = snclone(jp->tok, etok - jp->tok);
        jp->tok = etok + 1;

    } else {
        etok = findEndKeyword(jp, jp->tok);
        value = snclone(jp->tok, etok - jp->tok);
        jp->tok = etok;
    }
    return value;
}


static int setValue(MprJson *jp, MprObj *obj, int index, cchar *key, cchar *value, int type)
{
    MprKey  *kp;
    char    keybuf[32];

    if (index >= 0) {
        itosbuf(keybuf, sizeof(keybuf), index, 10);
        key = keybuf;
    }
    if ((kp = mprAddKey(obj, key, value)) == 0) {
        return MPR_ERR_MEMORY;
    }
    kp->type = type;
    return 0;
}


static MprObj *makeObj(MprJson *jp, bool list)
{
    MprHash     *hash;

    if ((hash = mprCreateHash(0, 0)) == 0) {
        return 0;
    }
    if (list) {
        hash->flags |= MPR_HASH_LIST;
    }
    return hash;
}


static void quoteValue(MprBuf *buf, cchar *str)
{
    cchar   *cp;

    mprPutCharToBuf(buf, '\'');
    for (cp = str; *cp; cp++) {
        if (*cp == '\'') {
            mprPutCharToBuf(buf, '\\');
        }
        mprPutCharToBuf(buf, *cp);
    }
    mprPutCharToBuf(buf, '\'');
}


/*
    Supports hashes where properties are strings or hashes of strings. N-level nest is supported.
 */
static cchar *objToString(MprBuf *buf, MprObj *obj, int type, int pretty)
{
    MprKey  *kp;
    char    numbuf[32];
    int     i, len;

    if (type == MPR_JSON_ARRAY) {
        mprPutCharToBuf(buf, '[');
        if (pretty) mprPutCharToBuf(buf, '\n');
        len = mprGetHashLength(obj);
        for (i = 0; i < len; i++) {
            itosbuf(numbuf, sizeof(numbuf), i, 10);
            if (pretty) mprPutStringToBuf(buf, "    ");
            if ((kp = mprLookupKeyEntry(obj, numbuf)) == 0) {
                mprAssert(kp);
                continue;
            }
            if (kp->type == MPR_JSON_ARRAY || kp->type == MPR_JSON_OBJ) {
                objToString(buf, (MprObj*) kp->data, kp->type, pretty);
            } else {
                quoteValue(buf, kp->data);
            }
            mprPutCharToBuf(buf, ',');
            if (pretty) mprPutCharToBuf(buf, '\n');
        }
        mprPutCharToBuf(buf, ']');

    } else if (type == MPR_JSON_OBJ) {
        mprPutCharToBuf(buf, '{');
        if (pretty) mprPutCharToBuf(buf, '\n');
        for (ITERATE_KEYS(obj, kp)) {
            if (kp->key == 0 || kp->data == 0) continue;
            if (pretty) mprPutStringToBuf(buf, "    ");
            mprPutStringToBuf(buf, kp->key);
            mprPutStringToBuf(buf, ": ");
            if (kp->type == MPR_JSON_ARRAY || kp->type == MPR_JSON_OBJ) {
                objToString(buf, (MprObj*) kp->data, kp->type, pretty);
            } else {
                quoteValue(buf, kp->data);
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


static char advanceToken(MprJson *jp)
{
    while (isspace((uchar) *jp->tok)) {
        if (*jp->tok == '\n') {
            jp->lineNumber++;
        }
        jp->tok++;
    }
    return *jp->tok;
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
        if ((etok = strpbrk(cp, " \t\n\r:,}]")) != 0) {
            if (etok == jp->tok || *etok != '\\') {
                return etok;
            }
        }
    }
    return &str[strlen(str)];
}


static void jsonParseError(MprJson *jp, cchar *msg)
{
    if (jp->path) {
        mprLog(4, "%s\nIn file '%s' at line %d", msg, jp->path, jp->lineNumber);
    } else {
        mprLog(4, "%s\nAt line %d", msg, jp->lineNumber);
    }
}


void mprJsonParseError(MprJson *jp, cchar *fmt, ...)
{
    va_list     args;
    cchar       *msg;

    va_start(args, fmt);
    msg = sfmtv(fmt, args);
    (jp->callback.parseError)(jp, msg);
    va_end(args);
}



/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
    
    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire 
    a commercial license from Embedthis Software. You agree to be fully bound 
    by the terms of either license. Consult the LICENSE.md distributed with 
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
