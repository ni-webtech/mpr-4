/**
    testThread.c - Threading Unit Tests 
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Code ************************************/

static void workerProc(void *data, MprWorker *thread)
{
    mprSignalTestComplete((MprTestGroup*) data);
}


static void testStartWorker(MprTestGroup *gp)
{
    int     rc;

    /*
        Can only run this test if the worker is greater than the number of threads.
     */
    if (mprGetMaxWorkers(gp) > gp->service->numThreads) {
        rc = mprStartWorker(workerProc, (void*) gp);
        assert(rc == 0);
        assert(mprWaitForTestToComplete(gp, MPR_TEST_SLEEP));
    }
}


MprTestDef testWorker = {
    "worker", 0, 0, 0,
    {
        MPR_TEST(0, testStartWorker),
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
