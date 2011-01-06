/**
    testCond.c - Unit tests for mprCond
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mprTest.h"

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
