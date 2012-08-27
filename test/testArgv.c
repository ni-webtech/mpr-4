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
    cchar   **argv;

    //  Simple arg parsing
    argc = mprMakeArgv("app a b", &argv, 0);
    assert(argc == 3);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "a") == 0);
    assert(strcmp(argv[2], "b") == 0);

    //  Quoted arg parsing
    argc = mprMakeArgv("app \"a\" b", &argv, 0);
    assert(argc == 3);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "a") == 0);
    assert(strcmp(argv[2], "b") == 0);

    //  Embedded spaces in quoted args
    argc = mprMakeArgv("app \"a b\"", &argv, 0);
    assert(argc == 2);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "a b") == 0);

    //  Single quotes
    argc = mprMakeArgv("app 'a b'", &argv, 0);
    assert(argc == 2);
    assert(strcmp(argv[0], "app") == 0);
    assert(strcmp(argv[1], "a b") == 0);

    //  Backquoting a double quote (need double to get past cc compiler): \"a
    argc = mprMakeArgv("\\\"a", &argv, 0);
    assert(argc == 1);
    assert(strcmp(argv[0], "\"a") == 0);

    //  Backquoting a single quote (need double to get past cc compiler): \'a
    argc = mprMakeArgv("\\'a", &argv, 0);
    assert(argc == 1);
    assert(strcmp(argv[0], "'a") == 0);

    //  Backquoting normal chars preserves the backquote \a
    argc = mprMakeArgv("\\a", &argv, 0);
    assert(argc == 1);
    assert(strcmp(argv[0], "\\a") == 0);

    //  Backquoted path
    argc = mprMakeArgv("\\a\\b\\c", &argv, 0);
    assert(argc == 1);
    assert(strcmp(argv[0], "\\a\\b\\c") == 0);

    //  Backquote at the end (preserved)
    argc = mprMakeArgv("a\\", &argv, 0);
    assert(argc == 1);
    assert(strcmp(argv[0], "a\\") == 0);

}


static void testArgvRegressions(MprTestGroup *gp)
{
    int     argc;
    cchar   **argv;

    //  "\bin\sh" "-c" "c:/home/mob/ejs/out/bin/ejs.exe ./args \"a b\" c"
    argc = mprMakeArgv("\"\\bin\\sh\" \"-c\" \"c:/home/mob/ejs/out/bin/ejs.exe ./args \\\"a b\\\" c\"", &argv, 0);
    assert(argc == 3);
    assert(strcmp(argv[0], "\\bin\\sh") == 0);
    assert(strcmp(argv[1], "-c") == 0);
    assert(strcmp(argv[2], "c:/home/mob/ejs/out/bin/ejs.exe ./args \"a b\" c") == 0);

    //  "\bin\sh" "-c" "/bin/ejs ./args \"a b\" c"
    argc = mprMakeArgv("\"\\bin\\sh\" \"-c\" \"/bin/ejs ./args \\\"a b\\\" c\"", &argv, 0);
    assert(argc == 3);
    assert(strcmp(argv[0], "\\bin\\sh") == 0);
    assert(strcmp(argv[1], "-c") == 0);
    assert(strcmp(argv[2], "/bin/ejs ./args \"a b\" c") == 0);
}


MprTestDef testArgv = {
    "makeArgv", 0, 0, 0,
    {
        MPR_TEST(0, testMakeArgv),
        MPR_TEST(0, testArgvRegressions),
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
