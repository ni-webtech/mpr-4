/**
    testPath.c - Unit tests for the mprPath module
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/*********************************** Locals **********************************/

#define DIR1        "/tmp/testTempDir"
#define DIR2        "/testSubDir"
#define DIRMODE     0755

typedef struct TestPath {
    char    *dir1;
    char    *dir2;
} TestPath;

static char *makePath(cchar *name);
static void manageTestPath(TestPath *tp, int flags);

/************************************ Code ************************************/
/*
    Initialization for this test module
 */
static int initPath(MprTestGroup *gp)
{
    TestPath     *ts;

    gp->data = mprAllocObj(TestPath, manageTestPath);
    if (gp->data == 0) {
        return MPR_ERR_MEMORY;
    }
    ts = (TestPath*) gp->data;

    ts->dir1 = makePath(DIR1);
    ts->dir2 = sfmt("%s%s", ts->dir1, DIR2);
    if (ts->dir1 == 0 || ts->dir2 == 0) {
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


static void manageTestPath(TestPath *tp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(tp->dir1);
        mprMark(tp->dir2);
    }
}


static int termPath(MprTestGroup *gp)
{
    gp->data = 0;
    return 0;
}


static void testCopyPath(MprTestGroup *gp)
{
    MprFile     *file;
    char        *from, *to;

    from = sfmt("copyTest-%s.tmp", mprGetCurrentThreadName(gp));
    assert(from != 0);
    file = mprOpenFile(from, O_CREAT | O_TRUNC | O_WRONLY, 0664);
    assert(file != 0);
    mprWriteFileString(file, "Hello World");
    mprCloseFile(file);

    to = sfmt("newTest-%s.tmp", mprGetCurrentThreadName(gp));
    assert(mprPathExists(from, F_OK));
    mprCopyPath(from, to, 0664);
    assert(mprPathExists(to, F_OK));
    mprDeletePath(from);
    mprDeletePath(to);
}


static void testAbsPath(MprTestGroup *gp)
{
    char        *path;

#if MANUAL_TESTING
    path = mprNormalizePath("/");
    path = mprNormalizePath("C:/");
    path = mprNormalizePath("C:/abc");
    path = mprNormalizePath("");
    path = mprNormalizePath("c:abc");
    path = mprNormalizePath("abc");

    path = mprGetAbsPath("/");
    path = mprGetAbsPath("C:/");
    path = mprGetAbsPath("C:/abc");
    path = mprGetAbsPath("");
    path = mprGetAbsPath("c:abc");
    path = mprGetAbsPath("abc");
#endif

    path = mprGetAbsPath("");
    assert(mprIsPathAbs(path));
    
    path = mprGetAbsPath("/");
    assert(mprIsPathAbs(path));
    
    path = mprGetAbsPath("../../../../../../../../../../..");
    assert(mprIsPathAbs(path));
    assert(mprIsPathAbs(mprGetAbsPath("Makefile")));

    /*
        Manually check incase mprIsAbs gets it wrong
     */
    path = mprGetAbsPath("Makefile");
    assert(path && *path);
    assert(mprIsPathAbs(path));
#if BIT_WIN_LIKE
    assert(isalpha(path[0]));
    assert(path[1] == ':' && path[2] == '\\');
#elif BIT_UNIX_LIKE
    assert(path[0] == '/' && path[1] != '/');
#endif
    assert(strcmp(mprGetPathBase(path), "Makefile") == 0);
}


static void testJoinPath(MprTestGroup *gp)
{
#if BIT_WIN_LIKE
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
    TestPath    *ts;
    int         rc;
    
    ts = (TestPath*) gp->data;

    mprGlobalLock(gp);
    rc = mprMakeDir(ts->dir1, DIRMODE, -1, -1, 1);
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
    rc = mprMakeDir(ts->dir2, DIRMODE, -1, -1, 1);
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

    assert(strcmp(mprNormalizePath(""), "") == 0);
    assert(strcmp(mprNormalizePath("/"), "/") == 0);
    assert(strcmp(mprNormalizePath("."), ".") == 0);
    assert(strcmp(mprNormalizePath("abc/"), "abc") == 0);
    assert(strcmp(mprNormalizePath("./"), ".") == 0);
    assert(strcmp(mprNormalizePath("../"), "..") == 0);
    assert(strcmp(mprNormalizePath("/.."), "/") == 0);
    assert(strcmp(mprNormalizePath("./.."), "..") == 0);
    assert(strcmp(mprNormalizePath("//.."), "/") == 0);
    assert(strcmp(mprNormalizePath("/abc/.."), "/") == 0);
    assert(strcmp(mprNormalizePath("abc/.."), ".") == 0);
    assert(strcmp(mprNormalizePath("/abc/../def"), "/def") == 0);
    assert(strcmp(mprNormalizePath("/abc/../def/../xyz"), "/xyz") == 0);
    assert(strcmp(mprNormalizePath("/abc/def/../.."), "/") == 0);
    assert(strcmp(mprNormalizePath("/abc/def/../../xyz"), "/xyz") == 0);
    assert(strcmp(mprNormalizePath("/abc/def/.././../xyz"), "/xyz") == 0);
    assert(strcmp(mprNormalizePath("//a//b//./././c/d/e/f/../../g"), "/a/b/c/d/g") == 0);
    assert(strcmp(mprNormalizePath("../../modules/ejs.mod"), "../../modules/ejs.mod") == 0);

    path = mprNormalizePath("//a//b//./././c/d/e/f/../../g");
    assert(strcmp(path, "/a/b/c/d/g") == 0);

#if VXWORKS || BIT_WIN_LIKE
    path = mprNormalizePath("\\\\a\\\\b\\\\.\\.\\.\\c\\d\\e\\f\\..\\..\\g");
    mprAssert(strcmp(path, "\\a\\b\\c\\d\\g") == 0);
    assert(strcmp(mprNormalizePath("host:"), "host:.") == 0);
    assert(strcmp(mprNormalizePath("host:/"), "host:/") == 0);
    assert(strcmp(mprNormalizePath("host:////"), "host:/") == 0);
    assert(strcmp(mprNormalizePath("host:abc"), "host:abc") == 0);
    assert(strcmp(mprNormalizePath("c:abc"), "c:abc") == 0);
#endif
}


static void testRelPath(MprTestGroup *gp)
{
    char    *path, *absPath;
    
    path = mprGetRelPath("Makefile", 0);
    assert(strcmp(path, "Makefile") == 0);
    
    path = mprNormalizePath("../a.b");
    assert(strcmp(path, "../a.b") == 0);

    path = mprGetRelPath("/", 0);
    assert(mprIsPathRel(path));
    assert(strncmp(path, "../", 3) == 0);
    
    path = mprGetRelPath("//", 0);
    assert(mprIsPathRel(path));
    assert(strncmp(path, "../", 3) == 0);
    
    path = mprGetRelPath("/tmp", 0);
    assert(mprIsPathRel(path));
    assert(strncmp(path, "../", 3) == 0);

    path = mprGetRelPath("/Unknown/someone/junk", 0);
    assert(mprIsPathRel(path));
    assert(strncmp(path, "../", 3) == 0);
           
    path = mprGetRelPath("/Users/mob/junk", 0);
    assert(mprIsPathRel(path));
    assert(strncmp(path, "../", 3) == 0);
    
    path = mprGetRelPath("/Users/mob/././../mob/junk", 0);
    assert(mprIsPathRel(path));
    assert(strncmp(path, "../", 3) == 0);
    
    path = mprGetRelPath(".", 0);
    assert(strcmp(path, ".") == 0);

    path = mprGetRelPath("..", 0);
    assert(strcmp(path, "..") == 0);

    path = mprGetRelPath("/Users/mob/github/admin", 0);
    assert(sstarts(path, ".."));

    path = mprGetRelPath("/Users/mob/git", 0);
    path = mprGetRelPath("/Users/mob/git/mpr/test", 0);
    /* Can't really test the result of this */

    absPath = mprGetAbsPath("Makefile");
    assert(mprIsPathAbs(absPath));
    path = mprGetRelPath(absPath, 0);
    assert(!mprIsPathAbs(path));
    assert(strcmp(path, "Makefile") == 0);

#if FUTURE
    //  MOB - problem in that we don't know the cwd when testMpr runs
    //  Test relative to an origin

    out = mprGetAbsPath("../../out");
    cwd = mprGetCurrentPath();
    assert(smatch(mprGetRelPath(cwd, out), "../src/test"));
#endif
}


static void testResolvePath(MprTestGroup *gp)
{
#if FUTURE
    char    *path, *absPath, *out;
    
    out = mprGetAbsPath("../out");
    assert(smatch(mprResolvePath(out, mprGetCurrentPath()), "../src/test"));
#endif
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


static void testTemp(MprTestGroup *gp)
{
    char    *path;

    path = mprGetTempPath(NULL);
    assert(path && *path);
    assert(mprIsPathAbs(path));
    assert(mprPathExists(path, F_OK));
    mprDeletePath(path);
}


static void testTransform(MprTestGroup *gp)
{
    char    *path;

    path = mprTransformPath("/", MPR_PATH_ABS);
    assert(mprIsPathAbs(path));

    path = mprTransformPath("/", MPR_PATH_REL);
    assert(mprIsPathRel(path));
    assert(path[0] == '.');

    path = mprTransformPath("/", 0);
    assert(strcmp(path, "/") == 0);

    path = mprTransformPath("/", MPR_PATH_ABS);
    mprAssert(mprIsPathAbs(path));

#if BIT_WIN_LIKE || CYGWIN
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
    assert(mprIsPathAbs(path));
#endif

#if CYGWIN
    path = mprGetAbsPath("c:/a/b");
    assert(smatch(path, "/cygdrive/c/a/b"));
    path = mprGetAbsPath("/a/b");
    assert(smatch(path, "/a/b"));
    path = mprGetAbsPath("c:/cygwin/a/b");
    assert(smatch(path, "/a/b"));
    path = mprGetWinPath("c:/a/b");
    assert(smatch(path, "C:\\a\\b"));
    path = mprGetWinPath("/cygdrive/c/a/b");
    assert(smatch(path, "C:\\a\\b"));
#endif
}

    
/*
    Make a unique filename for a given thread
 */
static char *makePath(cchar *name)
{
    return sfmt("%s-%d-%s", name, getpid(), mprGetCurrentThreadName());
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
        MPR_TEST(0, testResolvePath),
        MPR_TEST(0, testSame),
        MPR_TEST(0, testTransform),
        MPR_TEST(0, testTemp),
        MPR_TEST(0, 0),
    },
};


/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
    
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
