/**
    testUnicode.c - Unit tests for Unicode

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Code ************************************/

static void testBasicUnicode(MprTestGroup *gp)
{
    char    buf[256];
    char    *str;
    int     count;

    mprSprintf(buf, sizeof(buf), "%d", 12345678);
    assert(strlen(buf) == 8);
    assert(strcmp(buf, "12345678") == 0);

    mprSprintf(buf, sizeof(buf), "%d", -12345678);
    assert(strlen(buf) == 9);
    assert(strcmp(buf, "-12345678") == 0);

    str = sfmt("%d", 12345678);
    count = (int) strlen(str);
    assert(count == 8);
    assert(strcmp(str, "12345678") == 0);
}


MprTestDef testUnicode = {
    "unicode", 0, 0, 0,
    {
        MPR_TEST(0, testBasicUnicode),
        MPR_TEST(0, 0),
    },
};


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
