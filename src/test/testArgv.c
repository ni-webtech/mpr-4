/**
    testArgv.c - Unit tests for mprMakeArgv
 
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Code ************************************/

static void testMakeArgv(MprTestGroup *gp)
{
    int     argc;
    char    **argv;

    //  Simple arg parsing
    mprMakeArgv("app a b", &argc, &argv, 0);
    assert(argc == 3);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "a") == 0);
    assert(strcmp(argv[2], "b") == 0);

    //  Quoted arg parsing
    mprMakeArgv("app \"a\" b", &argc, &argv, 0);
    assert(argc == 3);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "a") == 0);
    assert(strcmp(argv[2], "b") == 0);

    //  Embedded spaces in quoted args
    mprMakeArgv("app \"a b\"", &argc, &argv, 0);
    assert(argc == 2);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "a b") == 0);

    //  Single quotes
    mprMakeArgv("app 'a b'", &argc, &argv, 0);
    assert(argc == 2);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "a b") == 0);

#if !BLD_WIN_LIKE
    //  Windows does not support back quoting
    //  Backquoting (need double to get past cc compiler)
    mprMakeArgv("\\\"a\\\"", &argc, &argv, 0);
    assert(argc == 1);
    assert(strcmp(argv[0], "\"a\"") == 0);

    mprMakeArgv("app \\\"a b\\\"", &argc, &argv, 0);
    assert(argc == 3);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "\"a") == 0);
    assert(strcmp(argv[2], "b\"") == 0);

    //  Backquote at the end (preserved)
    mprMakeArgv("a\\", &argc, &argv, 0);
    assert(argc == 1);
    assert(strcmp(argv[0], "a\\") == 0);
#endif
}


MprTestDef testArgv = {
    "makeArgv", 0, 0, 0,
    {
        MPR_TEST(0, testMakeArgv),
        MPR_TEST(0, 0),
    },
};

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
    details at: http://www.embedthis.com/downloads/gplLicense.html
    
    This program is distributed WITHOUT ANY WARRANTY; without even the 
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
    
    This GPL license does NOT permit incorporating this software into 
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses 
    for this software and support services are available from Embedthis 
    Software at http://www.embedthis.com 
    
    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
