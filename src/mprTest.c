/*
    mprTest.c - Embedthis Unit Test Framework

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/***************************** Forward Declarations ***************************/

static void     adjustFailedCount(MprTestService *sp, int adj);
static void     adjustThreadCount(MprTestService *sp, int adj);
static void     buildFullNames(MprTestGroup *gp, cchar *runName);
static MprList  *copyGroups(MprTestService *sp, MprList *groups);
static MprTestFailure *createFailure(MprTestGroup *gp, cchar *loc, cchar *message);
static MprTestGroup *createTestGroup(MprTestService *sp, MprTestDef *def, MprTestGroup *parent);
static bool     filterTestGroup(MprTestGroup *gp);
static bool     filterTestCast(MprTestGroup *gp, MprTestCase *tc);
static char     *getErrorMessage(MprTestGroup *gp);
static int      loadModule(MprTestService *sp, cchar *fileName);
static void     manageTestService(MprTestService *ts, int flags);
static int      parseFilter(MprTestService *sp, cchar *str);
static void     runInit(MprTestGroup *parent);
static void     runTerm(MprTestGroup *parent);
static void     runTestGroup(MprTestGroup *gp);
static void     runTestProc(MprTestGroup *gp, MprTestCase *test);
static void     runTestThread(MprList *groups, MprThread *tp);
static int      setLogging(char *logSpec);

/******************************************************************************/

MprTestService *mprCreateTestService()
{
    MprTestService      *sp;

    if ((sp = mprAllocObj(MprTestService, manageTestService)) == 0) {
        return 0;
    }
    MPR->testService = sp;
    sp->iterations = 1;
    sp->numThreads = 1;
    sp->workers = 0;
    sp->testFilter = mprCreateList(-1, 0);
    sp->groups = mprCreateList(-1, 0);
    sp->threadData = mprCreateList(-1, 0);
    sp->start = mprGetTime();
    sp->mutex = mprCreateLock();
    return sp;
}


static void manageTestService(MprTestService *ts, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ts->commandLine);
        mprMark(ts->groups);
        mprMark(ts->threadData);
        mprMark(ts->name);
        mprMark(ts->testFilter);
        mprMark(ts->mutex);
    }
}


int mprParseTestArgs(MprTestService *sp, int argc, char *argv[])
{
    cchar       *programName;
    char        *argp;
    int         err, i, depth, nextArg, outputVersion;

    i = 0;
    err = 0;
    outputVersion = 0;

    programName = mprGetPathBase(argv[0]);
    sp->name = sclone(BLD_PRODUCT);

    /*
        Save the command line
     */
    sp->commandLine = sclone(mprGetPathBase(argv[i++]));
    for (; i < argc; i++) {
        sp->commandLine = sjoin(sp->commandLine, " ", argv[i], NULL);
    }

    for (nextArg = 1; nextArg < argc; nextArg++) {
        argp = argv[nextArg];

        if (strcmp(argp, "--continue") == 0) {
            sp->continueOnFailures = 1; 

        } else if (strcmp(argp, "--depth") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                depth = atoi(argv[++nextArg]);
                if (depth < 0 || depth > 10) {
                    mprError("Bad test depth %d, (range 0-9)", depth);
                    err++;
                } else {
                    sp->testDepth = depth;
                }
            }

        } else if (strcmp(argp, "--debugger") == 0 || strcmp(argp, "-D") == 0) {
            mprSetDebugMode(1);
            sp->debugOnFailures = 1;

        } else if (strcmp(argp, "--echo") == 0) {
            sp->echoCmdLine = 1;

        } else if (strcmp(argp, "--filter") == 0 || strcmp(argp, "-f") == 0) {
            //  MOB DEPRECATE
            if (nextArg >= argc) {
                err++;
            } else {
                if (parseFilter(sp, argv[++nextArg]) < 0) {
                    err++;
                }
            }

        } else if (strcmp(argp, "--iterations") == 0 || (strcmp(argp, "-i") == 0)) {
            if (nextArg >= argc) {
                err++;
            } else {
                sp->iterations = atoi(argv[++nextArg]);
            }

        } else if (strcmp(argp, "--log") == 0 || strcmp(argp, "-l") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                setLogging(argv[++nextArg]);
            }

        } else if (strcmp(argp, "--module") == 0) {
            if (nextArg >= argc) {
                err++;
            } else if (loadModule(sp, argv[++nextArg]) < 0) {
                return MPR_ERR_CANT_OPEN;
            }

        } else if (strcmp(argp, "--name") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                sp->name = sclone(argv[++nextArg]);
            }

        } else if (strcmp(argp, "--step") == 0 || strcmp(argp, "-s") == 0) {
            sp->singleStep = 1; 

        } else if (strcmp(argp, "--threads") == 0 || strcmp(argp, "-t") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                i = atoi(argv[++nextArg]);
                if (i <= 0 || i > 100) {
                    mprError("%s: Bad number of threads (1-100)", programName);
                    return MPR_ERR_BAD_ARGS;
                }
                sp->numThreads = i;
            }

        } else if (strcmp(argp, "--verbose") == 0 || strcmp(argp, "-v") == 0) {
            sp->verbose++;

        } else if (strcmp(argp, "--version") == 0 || strcmp(argp, "-V") == 0) {
            outputVersion++;

        } else if (strcmp(argp, "--workers") == 0 || strcmp(argp, "-w") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                i = atoi(argv[++nextArg]);
                if (i < 0 || i > 100) {
                    mprError("%s: Bad number of worker threads (0-100)", programName);
                    return MPR_ERR_BAD_ARGS;
                }
                sp->workers = i;
            }

        } else if (strcmp(argp, "-?") == 0 || (strcmp(argp, "--help") == 0 || strcmp(argp, "--?") == 0)) {
            err++;

        } else if (*argp != '-') {
            break;

        } else {
            /* Ignore unknown args */
        }
    }

    if (sp->workers == 0) {
        sp->workers = 2 + sp->numThreads * 2;
    }
    if (nextArg < argc) {
        if (parseFilter(sp, argv[nextArg++]) < 0) {
            err++;
        }
    }
    if (err) {
        mprPrintfError("usage: %s [options] [filter paths]\n"
        "    --continue            # Continue on errors\n"
        "    --depth number        # Zero == basic, 1 == throrough, 2 extensive\n"
        "    --debug               # Run in debug mode\n"
        "    --echo                # Echo the command line\n"
        "    --iterations count    # Number of iterations to run the test\n"
        "    --log logFile:level   # Log to file file at verbosity level\n"
        "    --name testName       # Set test name\n"
        "    --step                # Single step tests\n"
        "    --threads count       # Number of test threads\n"
        "    --verbose             # Verbose mode\n"
        "    --version             # Output version information\n"
        "    --workers count       # Set maximum worker threads\n\n",
        programName);
        return MPR_ERR_BAD_ARGS;
    }
    if (outputVersion) {
        mprPrintfError("%s: Version: %s\n", BLD_NAME, BLD_VERSION);
        return MPR_ERR_BAD_ARGS;
    }
    sp->argc = argc;
    sp->argv = argv;
    sp->firstArg = nextArg;

    mprSetMaxWorkers(sp->workers);
    return 0;
}


static int parseFilter(MprTestService *sp, cchar *filter)
{
    char    *str, *word, *tok;

    mprAssert(filter);
    if (filter == 0 || *filter == '\0') {
        return 0;
    }

    tok = 0;
    str = sclone(filter);
    word = stok(str, " \t\r\n", &tok);
    while (word) {
        if (mprAddItem(sp->testFilter, sclone(word)) < 0) {
            mprAssert(!MPR_ERR_MEMORY);
            return MPR_ERR_MEMORY;
        }
        word = stok(0, " \t\r\n", &tok);
    }
    return 0;
}


static int loadModule(MprTestService *sp, cchar *fileName)
{
    MprModule   *mp;
    char        *cp, *base, entry[MPR_MAX_FNAME], path[MPR_MAX_FNAME];

    mprAssert(fileName && *fileName);

    base = mprGetPathBase(fileName);
    mprAssert(base);
    if ((cp = strrchr(base, '.')) != 0) {
        *cp = '\0';
    }
    if (mprLookupModule(base)) {
        return 0;
    }
    mprSprintf(entry, sizeof(entry), "%sInit", base);
    if (fileName[0] == '/' || (*fileName && fileName[1] == ':')) {
        mprSprintf(path, sizeof(path), "%s%s", fileName, BLD_SHOBJ);
    } else {
        mprSprintf(path, sizeof(path), "./%s%s", fileName, BLD_SHOBJ);
    }
    if ((mp = mprCreateModule(base, path, entry, sp)) == 0) {
        mprError("Can't create module %s", path);
        return -1;
    }
    if (mprLoadModule(mp) < 0) {
        mprError("Can't load module %s", path);
        return -1;
    }
    return 0;
}


int mprRunTests(MprTestService *sp)
{
    MprTestGroup    *gp;
    MprThread       *tp;
    MprList         *lp;
    char            tName[64];
    int             next, i;

    /*
        Build the full names for all groups
     */
    next = 0; 
    while ((gp = mprGetNextItem(sp->groups, &next)) != 0) {
        buildFullNames(gp, gp->name);
    }
    sp->activeThreadCount = sp->numThreads;

    if (sp->echoCmdLine) {
        mprPrintf("%12s %s ... ", "[Test]", sp->commandLine);
        if (sp->verbose) {
            mprPrintf("\n");
        }
    }
    sp->start = mprGetTime();

    /*
        Create worker threads for each test thread. 
     */
    for (i = 0; i < sp->numThreads; i++) {
        mprSprintf(tName, sizeof(tName), "test.%d", i);
        if ((lp = copyGroups(sp, sp->groups)) == 0) {
            mprAssert(!MPR_ERR_MEMORY);
            return MPR_ERR_MEMORY;
        }
        if (mprAddItem(sp->threadData, lp) < 0) {
            mprAssert(!MPR_ERR_MEMORY);
            return MPR_ERR_MEMORY;
        }
        /*
            Build the full names for all groups
         */
        next = 0; 
        while ((gp = mprGetNextItem(lp, &next)) != 0) {
            buildFullNames(gp, gp->name);
        }
        tp = mprCreateThread(tName, (MprThreadProc) runTestThread, (void*) lp, 0);
        if (tp == 0) {
            mprAssert(!MPR_ERR_MEMORY);
            return MPR_ERR_MEMORY;
        }
        if (mprStartThread(tp) < 0) {
            mprError("Can't start thread %d", i);
            return MPR_ERR_CANT_INITIALIZE;
        }
    }
    mprServiceEvents(-1, 0);
    return (sp->totalFailedCount == 0) ? 0 : 1;
}


static MprList *copyGroups(MprTestService *sp, MprList *groups)
{
    MprTestGroup    *gp, *newGp;
    MprList         *lp;
    int             next;

    if ((lp = mprCreateList(0, 0)) == NULL) {
        return 0;
    }
    next = 0; 
    while ((gp = mprGetNextItem(groups, &next)) != 0) {
        newGp = createTestGroup(sp, gp->def, NULL);
        if (newGp == 0) {
            return 0;
        }
        if (mprAddItem(lp, newGp) < 0) {
            return 0;
        }
    }
    return lp;
}


/*
    Run the test groups. One invocation per thread. Used even if not multithreaded.
 */
void runTestThread(MprList *groups, MprThread *tp)
{
    MprTestService  *sp;
    MprTestGroup    *gp;
    int             next, i, count;

    /*
        Get the service pointer
     */
    gp = mprGetFirstItem(groups);
    if (gp == 0) {
        return;
    }
    sp = gp->service;
    mprAssert(sp);

    for (next = 0; (gp = mprGetNextItem(groups, &next)) != 0; ) {
        runInit(gp);
    }
    count = 0;
    for (i = (sp->iterations + sp->numThreads - 1) / sp->numThreads; i >= 0; i--) {
        if (sp->totalFailedCount > 0 && !sp->continueOnFailures) {
            break;
        }
        next = 0; 
        while ((gp = mprGetNextItem(groups, &next)) != 0) {
            runTestGroup(gp);
        }
        mprPrintf("%12s Iteration %d complete (%s)\n", "[Notice]", count++, tp->name);
    }
    for (next = 0; (gp = mprGetNextItem(groups, &next)) != 0; ) {
        runTerm(gp);
    }
    if (tp) {
        adjustThreadCount(sp, -1);
    }
}


void mprReportTestResults(MprTestService *sp)
{
    if (sp->totalFailedCount == 0 && sp->verbose >= 1) {
        mprPrintf("%12s All tests PASSED for \"%s\"\n", "[REPORT]", sp->name);
    }
    if (sp->totalFailedCount > 0 || sp->verbose >= 2) {
        double  elapsed;
        elapsed = ((mprGetTime() - sp->start) * 1.0 / 1000.0);
        mprPrintf("%12s %d tests completed, %d test(s) failed.\n", 
            "[DETAILS]", sp->totalTestCount, sp->totalFailedCount);
        mprPrintf("%12s Elapsed time: %5.2f seconds.\n", "[BENCHMARK]", elapsed);
    }
}


static void buildFullNames(MprTestGroup *gp, cchar *name)
{
    MprTestGroup    *np;
    char            *nameBuf;
    cchar           *nameStack[MPR_TEST_MAX_STACK];
    int             tos, nextItem;

    tos = 0;

    /*
        Build the full name for this case
     */
    nameStack[tos++] = name;
    for (np = gp->parent; np && np != np->parent && tos < MPR_TEST_MAX_STACK;  np = np->parent) {
        nameStack[tos++] = np->name;
    }
    nameBuf = sclone(gp->service->name);
    while (--tos >= 0) {
        nameBuf = sjoin(nameBuf, ".", nameStack[tos], NULL);
    }
    mprAssert(gp->fullName == 0);
    gp->fullName = sclone(nameBuf);

    /*
        Recurse for all test case groups
     */
    nextItem = 0;
    np = mprGetNextItem(gp->groups, &nextItem);
    while (np) {
        buildFullNames(np, np->name);
        np = mprGetNextItem(gp->groups, &nextItem);
    }
}


/*
    Used by main program to add the top level test group(s)
 */
MprTestGroup *mprAddTestGroup(MprTestService *sp, MprTestDef *def)
{
    MprTestGroup    *gp;

    gp = createTestGroup(sp, def, NULL);
    if (gp == 0) {
        return 0;
    }
    if (mprAddItem(sp->groups, gp) < 0) {
        return 0;
    }
    return gp;
}


static void manageTestGroup(MprTestGroup *gp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(gp->name);
        mprMark(gp->fullName);
        mprMark(gp->failures);
        mprMark(gp->service);
        mprMark(gp->dispatcher);
        mprMark(gp->parent);
        mprMark(gp->root);
        mprMark(gp->groups);
        mprMark(gp->cases);
#if UNUSED
        mprMark(gp->def);
#endif
        mprMark(gp->http);
        mprMark(gp->conn);
        mprMark(gp->content);
        mprMark(gp->data);
        mprMark(gp->mutex);
    }
}


static MprTestGroup *createTestGroup(MprTestService *sp, MprTestDef *def, MprTestGroup *parent)
{
    MprTestGroup    *gp, *child;
    MprTestDef      **dp;
    MprTestCase     *tc;
    char            name[80];
    static int      counter = 0;

    mprAssert(sp);
    mprAssert(def);

    gp = mprAllocObj(MprTestGroup, manageTestGroup);
    if (gp == 0) {
        return 0;
    }
    gp->service = sp;
    if (parent) {
        gp->dispatcher = parent->dispatcher;
    } else {
        mprSprintf(name, sizeof(name), "Test-%d", counter++);
        gp->dispatcher = mprCreateDispatcher(name, 1);
    }

    gp->failures = mprCreateList(0, 0);
    if (gp->failures == 0) {
        return 0;
    }
    gp->cases = mprCreateList(0, MPR_LIST_STATIC_VALUES);
    if (gp->cases == 0) {
        return 0;
    }
    gp->groups = mprCreateList(0, 0);
    if (gp->groups == 0) {
        return 0;
    }
    gp->def = def;
    gp->name = sclone(def->name);
    gp->success = 1;

    for (tc = def->caseDefs; tc->proc; tc++) {
        if (mprAddItem(gp->cases, tc) < 0) {
            return 0;
        }
    }
    if (def->groupDefs) {
        for (dp = &def->groupDefs[0]; *dp && (*dp)->name; dp++) {
            child = createTestGroup(sp, *dp, gp);
            if (child == 0) {
                return 0;
            }
            if (mprAddItem(gp->groups, child) < 0) {
                return 0;
            }
            child->parent = gp;
            child->root = gp->parent;
        }
    }
    return gp;
}


void mprResetTestGroup(MprTestGroup *gp)
{
    gp->success = 1;
    gp->mutex = mprCreateLock();
}


static void runInit(MprTestGroup *parent)
{
    MprTestGroup    *gp;
    int             next;

    next = 0; 
    while ((gp = mprGetNextItem(parent->groups, &next)) != 0) {
        if (! filterTestGroup(gp)) {
            continue;
        }
        if (gp->def->init && (*gp->def->init)(gp) < 0) {
            gp->failedCount++;
            if (!gp->service->continueOnFailures) {
                break;
            }
        }
        runInit(gp);
    }
}


static void runTerm(MprTestGroup *parent)
{
    MprTestGroup    *gp;
    int             next;

    next = 0; 
    while ((gp = mprGetNextItem(parent->groups, &next)) != 0) {
        if (! filterTestGroup(gp)) {
            continue;
        }
        if (gp->def->term && (*gp->def->term)(gp) < 0) {
            gp->failedCount++;
            if (!gp->service->continueOnFailures) {
                break;
            }
        }
        runInit(gp);
    }
}


static void runTestGroup(MprTestGroup *parent)
{
    MprTestService  *sp;
    MprTestGroup    *gp, *nextGroup;
    MprTestCase     *tc;
    int             count, nextItem;

    sp = parent->service;

    /*
        Recurse over sub groups
     */
    nextItem = 0;
    gp = mprGetNextItem(parent->groups, &nextItem);
    while (gp && (parent->success || sp->continueOnFailures)) {
        nextGroup = mprGetNextItem(parent->groups, &nextItem);
        if (gp->testDepth > sp->testDepth) {
            gp = nextGroup;
            continue;
        }

        /*
            See if this group has been filtered for execution
         */
        if (! filterTestGroup(gp)) {
            gp = nextGroup;
            continue;
        }
        count = sp->totalFailedCount;
        if (count > 0 && !sp->continueOnFailures) {
            return;
        }

        /*
            Recurse over all tests in this group
         */
        runTestGroup(gp);
        gp->testCount++;

        if (! gp->success) {
            /*  Propagate the failures up the parent chain */
            parent->failedCount++;
            parent->success = 0;
        }
        gp = nextGroup;
    }

    /*
        Run test cases for this group
     */
    nextItem = 0;
    tc = mprGetNextItem(parent->cases, &nextItem);
    while (tc && (parent->success || sp->continueOnFailures)) {
        if (parent->testDepth <= sp->testDepth) {
            if (filterTestCast(parent, tc)) {
                runTestProc(parent, tc);
            }
        }
        tc = mprGetNextItem(parent->cases, &nextItem);
    }
}


/*
    Return true if we are to run the test group
 */
static bool filterTestGroup(MprTestGroup *gp)
{
    MprTestService  *sp;
    MprList         *testFilter;
    char            *pattern;
    ssize           len;
    int             next;

    sp = gp->service;
    testFilter = sp->testFilter;

    if (testFilter == 0) {
        return 1;
    }

    /*
        See if this test has been filtered
     */
    if (mprGetListLength(testFilter) > 0) {
        next = 0;
        pattern = mprGetNextItem(testFilter, &next);
        while (pattern) {
            len = min(strlen(pattern), strlen(gp->fullName));
            if (sncasecmp(gp->fullName, pattern, len) == 0) {
                break;
            }
            pattern = mprGetNextItem(testFilter, &next);
        }
        if (pattern == 0) {
            return 0;
        }
    }
    return 1;
}


/*
    Return true if we are to run the test case
 */
static bool filterTestCast(MprTestGroup *gp, MprTestCase *tc)
{
    MprTestService  *sp;
    MprList         *testFilter;
    char            *pattern, *fullName;
    ssize           len;
    int             next;

    sp = gp->service;
    testFilter = sp->testFilter;

    if (testFilter == 0) {
        return 1;
    }

    /*
        See if this test has been filtered
     */
    if (mprGetListLength(testFilter) > 0) {
        fullName = mprAsprintf("%s.%s", gp->fullName, tc->name);
        next = 0;
        pattern = mprGetNextItem(testFilter, &next);
        while (pattern) {
            len = min(strlen(pattern), strlen(fullName));
            if (sncasecmp(fullName, pattern, len) == 0) {
                break;
            }
            pattern = mprGetNextItem(testFilter, &next);
        }
        if (pattern == 0) {
            return 0;
        }
    }
    return 1;
}


static void runTestProc(MprTestGroup *gp, MprTestCase *test)
{
    MprTestService      *sp;

    if (test->proc == 0) {
        return;
    }
    sp = gp->service;

    mprResetTestGroup(gp);

    if (sp->singleStep) {
        mprPrintf("%12s Run test \"%s.%s\", press <ENTER>: ", "[Test]", gp->fullName, test->name);
        getchar();

    } else if (sp->verbose) {
        mprPrintf("%12s Run test \"%s.%s\": ", "[Test]", gp->fullName, test->name);
    }
    if (gp->skip) {
        if (sp->verbose) {
            if (gp->skipWarned++ == 0) {
                mprPrintf("%12s Skipping test: \"%s.%s\": \n", "[Skip]", gp->fullName, test->name);
            }
        }
    } else {
        (test->proc)(gp);
        mprYield(0);
    
        mprLock(sp->mutex);
        if (gp->success) {
            ++sp->totalTestCount;
            if (sp->verbose) {
                mprPrintf("PASSED\n");
            }
        } else {
            mprPrintfError("FAILED test \"%s.%s\"\nDetails: %s\n", gp->fullName, test->name, getErrorMessage(gp));
        }
    }
    mprUnlock(sp->mutex);
}


static char *getErrorMessage(MprTestGroup *gp)
{
    MprTestFailure  *fp;
    char            msg[MPR_MAX_STRING], *errorMsg;
    int             nextItem;

    nextItem = 0;
    errorMsg = sclone("");
    fp = mprGetNextItem(gp->failures, &nextItem);
    while (fp) {
        mprSprintf(msg, sizeof(msg), "Failure in %s\nAssertion: \"%s\"\n", fp->loc, fp->message);
        if ((errorMsg = sjoin(errorMsg, msg, NULL)) == NULL) {
            break;
        }
        fp = mprGetNextItem(gp->failures, &nextItem);
    }
    return errorMsg;
}


static int addFailure(MprTestGroup *gp, cchar *loc, cchar *message)
{
    MprTestFailure  *fp;

    fp = createFailure(gp, loc, message);
    if (fp == 0) {
        mprAssert(fp);
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    mprAddItem(gp->failures, fp);
    return 0;
}


static void manageTestFailure(MprTestFailure *fp, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(fp->loc);
        mprMark(fp->message);
    }
}


static MprTestFailure *createFailure(MprTestGroup *gp, cchar *loc, cchar *message)
{
    MprTestFailure  *fp;

    if ((fp = mprAllocObj(MprTestFailure, manageTestFailure)) == 0) {
        return 0;
    }
    fp->loc = sclone(loc);
    fp->message = sclone(message);
    return fp;
}


bool assertTrue(MprTestGroup *gp, cchar *loc, bool isTrue, cchar *msg)
{
    if (! isTrue) {
        gp->success = isTrue;
    }
    if (! isTrue) {
        if (gp->service->debugOnFailures) {
            mprBreakpoint();
        }
        addFailure(gp, loc, msg);
        gp->failedCount++;
        adjustFailedCount(gp->service, 1);
    }
    return isTrue;
}


bool mprWaitForTestToComplete(MprTestGroup *gp, int timeout)
{
    MprTime     expires, remaining;
    int         rc;
    
    mprAssert(gp->dispatcher);
    mprAssert(timeout >= 0);

    if (mprGetDebugMode()) {
        timeout *= 100;
    }
    expires = mprGetTime() + timeout;
    remaining = timeout;
    do {
        mprWaitForEvent(gp->dispatcher, remaining);
        remaining = expires - mprGetTime();
    } while (!gp->testComplete && remaining > 0);
    rc = gp->testComplete;
    gp->testComplete = 0;
    return rc;
}


void mprSignalTestComplete(MprTestGroup *gp)
{
    gp->testComplete = 1;
    mprSignalDispatcher(gp->dispatcher);
}


static void adjustThreadCount(MprTestService *sp, int adj)
{
    mprLock(sp->mutex);
    sp->activeThreadCount += adj;
    if (sp->activeThreadCount <= 0) {
        mprTerminate(MPR_GRACEFUL);
    }
    mprUnlock(sp->mutex);
}


static void adjustFailedCount(MprTestService *sp, int adj)
{
    mprLock(sp->mutex);
    sp->totalFailedCount += adj;
    mprUnlock(sp->mutex);
}


static void logHandler(int flags, int level, cchar *msg)
{
    MprFile     *file;
    char        *prefix;

    file = (MprFile*) MPR->logData;
    prefix = MPR->name;

    while (*msg == '\n') {
        mprFprintf(file, "\n");
        msg++;
    }
    if (flags & MPR_LOG_SRC) {
        mprFprintf(file, "%s: %d: %s\n", prefix, level, msg);
    } else if (flags & MPR_ERROR_SRC) {
        mprFprintf(file, "%s: Error: %s\n", prefix, msg);
    } else if (flags & MPR_FATAL_SRC) {
        mprFprintf(file, "%s: Fatal: %s\n", prefix, msg);
    } else if (flags & MPR_RAW) {
        mprFprintf(file, "%s", msg);
    }
    if (flags & (MPR_ERROR_SRC | MPR_FATAL_SRC)) {
        mprBreakpoint();
    }
}


static int setLogging(char *logSpec)
{
    MprFile     *file;
    char        *levelSpec;
    int         level;

    level = 0;

    if ((levelSpec = strchr(logSpec, ':')) != 0) {
        *levelSpec++ = '\0';
        level = atoi(levelSpec);
    }

    if (strcmp(logSpec, "stdout") == 0) {
        file = MPR->fileSystem->stdOutput;

    } else if (strcmp(logSpec, "stderr") == 0) {
        file = MPR->fileSystem->stdError;

    } else {
        if ((file = mprOpenFile(logSpec, O_CREAT | O_WRONLY | O_TRUNC | O_TEXT, 0664)) == 0) {
            mprPrintfError("Can't open log file %s\n", logSpec);
            return MPR_ERR_CANT_OPEN;
        }
    }
    mprSetLogLevel(level);
    mprSetLogHandler(logHandler, (void*) file);
    return 0;
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
