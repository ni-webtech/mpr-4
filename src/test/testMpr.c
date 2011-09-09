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

MAIN(testMain, int argc, char *argv[]) 
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
#if BLD_FEATURE_SSL && (BLD_FEATURE_MATRIXSSL || BLD_FEATURE_OPENSSL)
    if (!mprLoadSsl(0)) {
        exit(5);
    }
#endif
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
