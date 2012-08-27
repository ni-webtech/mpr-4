/**
    testMpr.c - program for the MPR unit tests

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/****************************** Test Definitions ******************************/

extern MprTestDef testAlloc;
extern MprTestDef testArgv;
extern MprTestDef testBuf;
extern MprTestDef testEvent;
extern MprTestDef testCmd;
extern MprTestDef testFile;
extern MprTestDef testHash;
extern MprTestDef testList;
extern MprTestDef testPath;
extern MprTestDef testSocket;
extern MprTestDef testSprintf;
extern MprTestDef testTime;
extern MprTestDef testCond;
extern MprTestDef testLock;
extern MprTestDef testWorker;

static MprTestDef *testGroups[] = 
{
    &testAlloc,
    &testArgv,
    &testBuf,
    &testCond,
    &testCmd,
    &testEvent,
    &testFile,
    &testPath,
    &testHash,
    &testList,
    &testLock,
    &testWorker,
    &testSocket,
    &testSprintf,
    &testTime,
    0
};
 
static MprTestDef master = {
    "api",
    testGroups,
    0, 0, 
    { { 0 } },
};

/************************************* Code ***********************************/

MAIN(testMain, int argc, char **argv, char **envp)
{
    Mpr             *mpr;
    MprTestService  *ts;
    int             rc;

    mpr = mprCreate(argc, argv, MPR_USER_EVENTS_THREAD);
    mprAddStandardSignals();

    if ((ts = mprCreateTestService(mpr)) == 0) {
        mprError("Can't create test service");
        exit(2);
    }
    if (mprParseTestArgs(ts, argc, argv, 0) < 0) {
        exit(3);
    }  
    if (mprAddTestGroup(ts, &master) == 0) {
        exit(4);
    }
    if (mprStart(mpr)) {
        mprError("Can't start mpr services");
        exit(4);
    }
    /*
        Run the tests and return zero if 100% success
     */
    rc = mprRunTests(ts);
    mprReportTestResults(ts);
    mprDestroy(MPR_EXIT_DEFAULT);
    return (rc == 0) ? 0 : 6;
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
