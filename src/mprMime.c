/* 
    mprMime.c - Mime type handling

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

/*********************************** Code *************************************/
/*  
    Inbuilt mime type support
 */
static char *standardMimeTypes[] = {
    "ai",    "application/postscript",
    "asc",   "text/plain",
    "au",    "audio/basic",
    "avi",   "video/x-msvideo",
    "bin",   "application/octet-stream",
    "bmp",   "image/bmp",
    "class", "application/octet-stream",
    "css",   "text/css",
    "deb",   "application/octet-stream",
    "dll",   "application/octet-stream",
    "dmg",   "application/octet-stream",
    "doc",   "application/msword",
    "eps",   "application/postscript",
    "es",    "application/x-javascript",
    "exe",   "application/octet-stream",
    "gif",   "image/gif",
    "gz",    "application/x-gzip",
    "htm",   "text/html",
    "html",  "text/html",
    "ico",   "image/x-icon",
    "jar",   "application/octet-stream",
    "jpeg",  "image/jpeg",
    "jpg",   "image/jpeg",
    "js",    "application/javascript",
    "json",  "application/json",
    "mp3",   "audio/mpeg",
    "pdf",   "application/pdf",
    "php",   "application/x-php",
    "pl",    "application/x-perl",
    "png",   "image/png",
    "ppt",   "application/vnd.ms-powerpoint",
    "ps",    "application/postscript",
    "py",    "application/x-python",
    "py",    "application/x-python",
    "ra",    "audio/x-realaudio",
    "ram",   "audio/x-pn-realaudio",
    "rmm",   "audio/x-pn-realaudio",
    "rtf",   "text/rtf",
    "rv",    "video/vnd.rn-realvideo",
    "so",    "application/octet-stream",
    "swf",   "application/x-shockwave-flash",
    "tar",   "application/x-tar",
    "tgz",   "application/x-gzip",
    "tiff",  "image/tiff",
    "txt",   "text/plain",
    "wav",   "audio/x-wav",
    "xls",   "application/vnd.ms-excel",
    "zip",   "application/zip",
    0,       0,
};

/********************************** Forward ***********************************/

static void addStandardMimeTypes(MprHash *table);
static void manageMimeType(MprMime *mt, int flags);

/*********************************** Code *************************************/

MprHash *mprCreateMimeTypes(cchar *path)
{
    MprHash     *table;
    MprFile     *file;
    char        *buf, *tok, *ext, *type;
    int         line;

    if (path) {
        if ((file = mprOpenFile(path, O_RDONLY | O_TEXT, 0)) == 0) {
            return 0;
        }
        if ((table = mprCreateHash(MPR_DEFAULT_HASH_SIZE, 0)) == 0) {
            mprCloseFile(file);
            return 0;
        }
        line = 0;
        while ((buf = mprReadLine(file, 0, NULL)) != 0) {
            line++;
            if (buf[0] == '#' || isspace((uchar) buf[0])) {
                continue;
            }
            type = stok(buf, " \t\n\r", &tok);
            ext = stok(0, " \t\n\r", &tok);
            if (type == 0 || ext == 0) {
                mprError("Bad mime type in %s at line %d", path, line);
                continue;
            }
            while (ext) {
                mprAddMime(table, ext, type);
                ext = stok(0, " \t\n\r", &tok);
            }
        }
        mprCloseFile(file);

    } else {
        if ((table = mprCreateHash(59, 0)) == 0) {
            return 0;
        }
        addStandardMimeTypes(table);
    }
    return table;
}


static void addStandardMimeTypes(MprHash *table)
{
    char    **cp;

    for (cp = standardMimeTypes; cp[0]; cp += 2) {
        mprAddMime(table, cp[0], cp[1]);
    }
}


static void manageMimeType(MprMime *mt, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(mt->type);
        mprMark(mt->program);
    }
}


MprMime *mprAddMime(MprHash *table, cchar *ext, cchar *mimeType)
{
    MprMime  *mt;

    if ((mt = mprAllocObj(MprMime, manageMimeType)) == 0) {
        return 0;
    }
    mt->type = sclone(mimeType);
    if (*ext == '.') {
        ext++;
    }
    mprAddKey(table, ext, mt);
    return mt;
}


int mprSetMimeProgram(MprHash *table, cchar *mimeType, cchar *program)
{
    MprKey      *kp;
    MprMime     *mt;
    
    kp = 0;
    mt = 0;
    while ((kp = mprGetNextKey(table, kp)) != 0) {
        mt = (MprMime*) kp->data;
        if (mt->type[0] == mimeType[0] && strcmp(mt->type, mimeType) == 0) {
            break;
        }
    }
    if (mt == 0) {
        mprError("Can't find mime type %s for action program %s", mimeType, program);
        return MPR_ERR_CANT_FIND;
    }
    mt->program = sclone(program);
    return 0;
}


cchar *mprGetMimeProgram(MprHash *table, cchar *mimeType)
{
    MprMime      *mt;

    if (mimeType == 0 || *mimeType == '\0') {
        return 0;
    }
    if ((mt = mprLookupKey(table, mimeType)) == 0) {
        return 0;
    }
    return mt->program;
}


cchar *mprLookupMime(MprHash *table, cchar *ext)
{
    MprMime     *mt;
    cchar       *ep;

    if (ext == 0 || *ext == '\0') {
        return "";
    }
    if ((ep = strrchr(ext, '.')) != 0) {
        ext = &ep[1];
    }
    if (table == 0) {
        table = MPR->mimeTypes;
    }
    if ((mt = mprLookupKey(table, ext)) == 0) {
        return 0;
    }
    return mt->type;
}


/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the Embedthis Open Source license or you may acquire a 
    commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
