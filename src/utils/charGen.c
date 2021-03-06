/*
    charGen.c - Generate the character lookup tables.
    These are used in uri.c  for escape / descape routines.
    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

/*********************************** Code *************************************/

int main(int argc, char *argv[])
{
    uchar    flags;
    uint     c;

    mprCreate(argc, argv, 0);

    mprPrintf("static uchar charMatch[256] = {\n\t0x00,");

    for (c = 1; c < 256; ++c) {
        flags = 0;
        if (c % 16 == 0)
            mprPrintf("\n\t");
#if BIT_WIN_LIKE
        if (strchr("&;`'\"|*?~<>^()[]{}$\\\n\r%", c)) {
            flags |= MPR_ENCODE_SHELL;
        }
#else
        if (strchr("&;`\'\"|*?~<>^()[]{}$\\\n", c)) {
            flags |= MPR_ENCODE_SHELL;
        }
#endif

        if (isalnum(c) || strchr("-_.~", c)) {
            /* Acceptable */
        } else if (strchr("+", c)) {
            flags |= MPR_ENCODE_URI_COMPONENT | MPR_ENCODE_JS_URI_COMPONENT;
        } else if (strchr("#;,/?:@&=+$", c)) {
            flags |= MPR_ENCODE_URI_COMPONENT | MPR_ENCODE_JS_URI_COMPONENT;
        } else if (strchr("!'()*", c)) {
            flags |= MPR_ENCODE_URI | MPR_ENCODE_URI_COMPONENT;
        } else if (strchr("[]", c)) {
            flags |= MPR_ENCODE_JS_URI | MPR_ENCODE_URI_COMPONENT | MPR_ENCODE_JS_URI_COMPONENT;
        } else {
            flags |= MPR_ENCODE_URI | MPR_ENCODE_URI_COMPONENT | MPR_ENCODE_JS_URI | MPR_ENCODE_JS_URI_COMPONENT;
        }
        if (strchr("<>&\"'", c) != 0) {
            flags |= MPR_ENCODE_HTML;
        }
        mprPrintf("0x%02x%c", flags, (c < 255) ? ',' : ' ');
    }
    mprPrintf("\n};\n");
    return 0;
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
