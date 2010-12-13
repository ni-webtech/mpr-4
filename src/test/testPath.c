/**
    testPath.c - Unit tests for the mprPath module
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mprTest.h"

/*********************************** Locals **********************************/

#define DIR1        "/tmp/testTempDir"
#define DIR2        "/testSubDir"
#define DIRMODE     0755

typedef struct MprTestPath {
    char            *dir1;
    char            *dir2;
} MprTestPath;

/************************************ Code ************************************/
/*
    Make a unique filename for a given thread
 */
static char *makePath(cchar *name)
{
    return mprAsprintf("%s-%d-%s", name, getpid(), mprGetCurrentThreadName());
}


/*
    Initialization for this test module
 */
static int initPath(MprTestGroup *gp)
{
    MprTestPath     *ts;

    gp->data = mprAllocZeroed(sizeof(MprTestPath));
    if (gp->data == 0) {
        return MPR_ERR_MEMORY;
    }
    ts = (MprTestPath*) gp->data;

    ts->dir1 = makePath(DIR1);
    ts->dir2 = mprAsprintf("%s%s", ts->dir1, DIR2);
    if (ts->dir1 == 0 || ts->dir2 == 0) {
        mprFree(gp->data);
        gp->data = 0;
        return MPR_ERR_MEMORY;
    }

    /*
        Don't mind if these fail. We are just making sure they don't exist before we start the tests.
     */
    rmdir(ts->dir2);
    rmdir(ts->dir1);
    return 0;
}


static int termPath(MprTestGroup *gp)
{
    mprFree(gp->data);
    gp->data = 0;
    return 0;
}


static void testCopyPath(MprTestGroup *gp)
{
    MprTestPath     *ts;
    MprFile         *file;
    char            *from, *to;

    ts = (MprTestPath*) gp->data;

    from = mprAsprintf("copyTest-%s.tmp", mprGetCurrentThreadName(gp));
    assert(from != 0);
    file = mprOpen(from, O_CREAT | O_TRUNC | O_WRONLY, 0664);
    assert(file != 0);
    mprWriteString(file, "Hello World");
    mprFree(file);

    to = mprAsprintf("newTest-%s.tmp", mprGetCurrentThreadName(gp));
    assert(mprPathExists(from, F_OK));
    assert(!mprPathExists(to, F_OK));
    mprCopyPath(from, to, 0664);
    assert(mprPathExists(to, F_OK));
    mprDeletePath(from);
    mprDeletePath(to);
}


static void testAbsPath(MprTestGroup *gp)
{
    char        *path;

#if MANUAL_TESTING
    path = mprGetNormalizedPath("/");
    path = mprGetNormalizedPath("C:/");
    path = mprGetNormalizedPath("C:/abc");
    path = mprGetNormalizedPath("");
    path = mprGetNormalizedPath("c:abc");
    path = mprGetNormalizedPath("abc");

    path = mprGetAbsPath("/");
    path = mprGetAbsPath("C:/");
    path = mprGetAbsPath("C:/abc");
    path = mprGetAbsPath("");
    path = mprGetAbsPath("c:abc");
    path = mprGetAbsPath("abc");
#endif

    path = mprGetAbsPath("");
    assert(mprIsAbsPath(path));
    
    path = mprGetAbsPath("/");
    assert(mprIsAbsPath(path));
    
    path = mprGetAbsPath(".../../../../../../../../../../..");
    assert(mprIsAbsPath(path));
    assert(mprIsAbsPath(mprGetAbsPath("Makefile")));

    /*
        Manually check incase mprIsAbs gets it wrong
     */
    path = mprGetAbsPath("Makefile");
    assert(path && *path);
    assert(mprIsAbsPath(path));
#if BLD_WIN_LIKE
    assert(isalpha(path[0]));
    assert(path[1] == ':' && path[2] == '\\');
#elif BLD_UNIX_LIKE
    assert(path[0] == '/' && path[1] != '/');
#endif
    assert(strcmp(mprGetPathBase(path), "Makefile") == 0);
}


static void testJoinPath(MprTestGroup *gp)
{
#if BLD_WIN_LIKE
    assert(strcmp(mprJoinPath("\\tmp", "Makefile"), "\\tmp\\Makefile") == 0);
    assert(strcmp(mprJoinPath("\\tmp", "\\Makefile"), "\\Makefile") == 0);
    assert(strcmp(mprJoinPath("\\tmp", NULL), "\\tmp") == 0);
    assert(strcmp(mprJoinPath("\\tmp", "."), "\\tmp") == 0);
    assert(strcmp(mprJoinPath("\\\\\\tmp\\\\\\", "Makefile\\\\\\"), "\\tmp\\Makefile") == 0);
#else
    assert(strcmp(mprJoinPath("/tmp", "Makefile"), "/tmp/Makefile") == 0);
    assert(strcmp(mprJoinPath("/tmp", "/Makefile"), "/Makefile") == 0);
    assert(strcmp(mprJoinPath("/tmp", NULL), "/tmp") == 0);
    assert(strcmp(mprJoinPath("/tmp", "."), "/tmp") == 0);
    assert(strcmp(mprJoinPath("///tmp///", "Makefile///"), "/tmp/Makefile") == 0);
#endif
    assert(strcmp(mprJoinPath("", "Makefile"), "Makefile") == 0);

    assert(strcmp(mprJoinPathExt("/abc", ".exe"), "/abc.exe") == 0);
    assert(strcmp(mprJoinPathExt("/abc.bat", ".exe"), "/abc.bat") == 0);
    assert(strcmp(mprJoinPathExt("/abc", ""), "/abc") == 0);
    assert(strcmp(mprJoinPathExt("ejs.web/file", ".mod"), "ejs.web/file.mod") == 0);
}


static void testMakeDir(MprTestGroup *gp)
{
    MprTestPath     *ts;
    int             rc;
    
    ts = (MprTestPath*) gp->data;

    mprGlobalLock(gp);
    rc = mprMakeDir(ts->dir1, DIRMODE, 1);
    assert(rc == 0);
    assert(access(ts->dir1, X_OK) == 0);

    rmdir(ts->dir1);
    assert(access(ts->dir1, X_OK) < 0);

    rmdir(ts->dir2);
    rmdir(ts->dir1);

    /*
        Dir should not exist before test
     */
    assert(access(ts->dir1, X_OK) < 0);

    /*
        Make directory path
     */
    rc = mprMakeDir(ts->dir2, DIRMODE, 1);
    assert(rc == 0);
    assert(access(ts->dir2, X_OK) == 0);

    rc = mprDeletePath(ts->dir2);
    assert(rc == 0);
    rc = mprDeletePath(ts->dir1);
    assert(rc == 0);
    
    assert(access(ts->dir1, X_OK) < 0);
    assert(access(ts->dir2, X_OK) < 0);
    mprGlobalUnlock(gp);
}


static void testNormalize(MprTestGroup *gp)
{
    char        *path;

    assert(strcmp(mprGetNormalizedPath(""), "") == 0);
    assert(strcmp(mprGetNormalizedPath("/"), "/") == 0);
    assert(strcmp(mprGetNormalizedPath("."), ".") == 0);
    assert(strcmp(mprGetNormalizedPath("abc/"), "abc") == 0);
    assert(strcmp(mprGetNormalizedPath("./"), ".") == 0);
    assert(strcmp(mprGetNormalizedPath("../"), "..") == 0);
    assert(strcmp(mprGetNormalizedPath("/.."), "/") == 0);
    assert(strcmp(mprGetNormalizedPath("./.."), "..") == 0);
    assert(strcmp(mprGetNormalizedPath("//.."), "/") == 0);
    assert(strcmp(mprGetNormalizedPath("/abc/.."), "/") == 0);
    assert(strcmp(mprGetNormalizedPath("abc/.."), ".") == 0);
    assert(strcmp(mprGetNormalizedPath("/abc/../def"), "/def") == 0);
    assert(strcmp(mprGetNormalizedPath("/abc/../def/../xyz"), "/xyz") == 0);
    assert(strcmp(mprGetNormalizedPath("/abc/def/../.."), "/") == 0);
    assert(strcmp(mprGetNormalizedPath("/abc/def/../../xyz"), "/xyz") == 0);
    assert(strcmp(mprGetNormalizedPath("/abc/def/.././../xyz"), "/xyz") == 0);
    assert(strcmp(mprGetNormalizedPath("//a//b//./././c/d/e/f/../../g"), "/a/b/c/d/g") == 0);
    assert(strcmp(mprGetNormalizedPath("../../modules/ejs.mod"), "../../modules/ejs.mod") == 0);

    path = mprGetNormalizedPath("//a//b//./././c/d/e/f/../../g");
    mprAssert(strcmp(path, "/a/b/c/d/g") == 0);

#if VXWORKS || BLD_WIN_LIKE
    path = mprGetNormalizedPath("\\\\a\\\\b\\\\.\\.\\.\\c\\d\\e\\f\\..\\..\\g");
    mprAssert(strcmp(path, "\\a\\b\\c\\d\\g") == 0);
    assert(strcmp(mprGetNormalizedPath("host:"), "host:.") == 0);
    assert(strcmp(mprGetNormalizedPath("host:/"), "host:/") == 0);
    assert(strcmp(mprGetNormalizedPath("host:////"), "host:/") == 0);
    assert(strcmp(mprGetNormalizedPath("host:abc"), "host:abc") == 0);
    assert(strcmp(mprGetNormalizedPath("c:abc"), "c:abc") == 0);
#endif
}


static void testRelPath(MprTestGroup *gp)
{
    char    *path, *absPath;
    
    path = mprGetRelPath("Makefile");
    assert(strcmp(path, "Makefile") == 0);
    
    path = mprGetNormalizedPath("../a.b");
    assert(strcmp(path, "../a.b") == 0);

    path = mprGetRelPath("/");
    assert(mprIsRelPath(path));
    assert(strncmp(path, "../", 3) == 0);
    
    path = mprGetRelPath("//");
    assert(mprIsRelPath(path));
    assert(strncmp(path, "../", 3) == 0);
    
    path = mprGetRelPath("/tmp");
    assert(mprIsRelPath(path));
    assert(strncmp(path, "../", 3) == 0);

    path = mprGetRelPath("/Unknown/someone/junk");
    assert(mprIsRelPath(path));
    assert(strncmp(path, "../", 3) == 0);
           
    path = mprGetRelPath("/Users/mob/junk");
    assert(mprIsRelPath(path));
    assert(strncmp(path, "../", 3) == 0);
    
    path = mprGetRelPath("/Users/mob/././../mob/junk");
    assert(mprIsRelPath(path));
    assert(strncmp(path, "../", 3) == 0);
    
    path = mprGetRelPath(".");
    assert(strcmp(path, ".") == 0);

    path = mprGetRelPath("..");
    assert(strcmp(path, "..") == 0);

    absPath = mprGetAbsPath("Makefile");
    assert(mprIsAbsPath(absPath));
    path = mprGetRelPath(absPath);
    assert(!mprIsAbsPath(path));
    assert(strcmp(path, "Makefile") == 0);
}


static void testSame(MprTestGroup *gp)
{
    /* Assumes running in test/utest/api */
    assert(mprSamePath("testFile", "./testFile"));
#if AVOID
    /*
        This forces the tests to be run in a specific directory
     */
    assert(mprSamePath("testFile", "../test/api/testFile"));
    assert(mprSamePath(mprGetAbsPath("testFile"), "../test/api/testFile"));
    assert(mprSamePath("testFile", mprGetAbsPath("../test/api/testFile")));
#endif
}


static void testSearch(MprTestGroup *gp)
{
#if !DEBUG_IDE
    assert(mprSearchPath("Makefile", 0, "../.." MPR_SEARCH_SEP ".", NULL) != 0);
    assert(mprSearchPath("Makefile", 0, "../..", ".", NULL) != 0);
#endif
    assert(!mprSearchPath("Unfounded", 0, "../..", ".", "/", NULL) != 0);
}


static void testTemp(MprTestGroup *gp)
{
    char    *path;

    path = mprGetTempPath(NULL);
    assert(path && *path);
    assert(mprIsAbsPath(path));
    assert(mprPathExists(path, F_OK));
    mprDeletePath(path);
    mprFree(path);
}


static void testTransform(MprTestGroup *gp)
{
    char    *path;

    path = mprGetTransformedPath("/", MPR_PATH_ABS);
    assert(mprIsAbsPath(path));

    path = mprGetTransformedPath("/", MPR_PATH_REL);
    assert(mprIsRelPath(path));
    assert(path[0] == '.');

    path = mprGetTransformedPath("/", 0);
    assert(strcmp(path, "/") == 0);

#if BLD_WIN_LIKE 
    path = mprGetTransformedPath("/", MPR_PATH_ABS);
    mprAssert(mprIsAbsPath(path));

#if FUTURE && CYGWIN
    path = mprGetTransformedPath("c:/cygdrive/c/tmp/a.txt", 0);
    mprAssert(strcmp(path, "/tmp/a.txt"));
    path = mprGetTransformedPath("c:/cygdrive/c/tmp/a.txt", MPR_PATH_CYGWIN);
    mprAssert(strcmp(path, "/cygdrive/c/tmp/a.txt"));
#endif

    /* Test MapSeparators */
    path = sclone("\\a\\b\\c\\d");
    mprMapSeparators(path, '/');
    assert(*path == '/');
    assert(strchr(path, '\\') == 0);

    /* Test PortablePath */
    path = sclone("\\a\\b\\c\\d");
    path = mprGetPortablePath(path);
    mprAssert(*path == '/');
    assert(strchr(path, '\\') == 0);
    assert(mprIsAbsPath(path));
#endif
}

    
MprTestDef testPath = {
    "path", 0, initPath, termPath,
    {
        MPR_TEST(0, testCopyPath),
        MPR_TEST(0, testAbsPath),
        MPR_TEST(0, testJoinPath),
        MPR_TEST(0, testMakeDir),
        MPR_TEST(0, testNormalize),
        MPR_TEST(0, testRelPath),
        MPR_TEST(0, testSame),
        MPR_TEST(0, testSearch),
        MPR_TEST(0, testTransform),
        MPR_TEST(0, testTemp),
        MPR_TEST(0, 0),
    },
};


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
