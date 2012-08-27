/**
    testCond.c - Unit tests for mprCond
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/*********************************** Locals ***********************************/

typedef struct TestCond {
    MprCond     *cond;
    MprEvent    *event;
} TestCond;

static void manageTestCond(TestCond *tc, int flags);

/************************************ Code ************************************/

static int initCond(MprTestGroup *gp)
{
    gp->data = mprAllocObj(TestCond, manageTestCond);
    return 0;
}


static void manageTestCond(TestCond *tc, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(tc->cond);
        mprMark(tc->event);
    }
}


static void callback(void *data, MprEvent *event)
{
    mprSignalCond((MprCond*) data);
}


static void testCriticalSection(MprTestGroup *gp)
{
    TestCond        *tc;
    int             rc, delay;

    tc = gp->data;
    tc->cond = mprCreateCond(gp);
    assert(tc->cond != 0);
    mprAssert(tc->cond->triggered == 0);

    tc->event = mprCreateEvent(NULL, "testCriticalSection", 0, callback, tc->cond, MPR_EVENT_QUICK);
    assert(tc->event != 0);

    delay = MPR_TEST_TIMEOUT + (mprGetDebugMode() * 1200 * 1000);
    mprYield(MPR_YIELD_STICKY);

    rc = mprWaitForCond(tc->cond, delay);
    assert(rc == 0);
    mprResetYield();

    tc->cond = 0;
    tc->event = 0;
    //  TODO - add test with longer event delay to catch when wait runs first
}


MprTestDef testCond = {
    "cond", 0, initCond, 0,
    {
        MPR_TEST(0, testCriticalSection),
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
