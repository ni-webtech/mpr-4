/**
    testEvent.c - Unit tests for Events

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/*********************************** Locals ***********************************/

typedef struct TestEvent {
    MprEvent    *event;
} TestEvent;

static void manageTestEvent(TestEvent *te, int flags);

/************************************ Code ************************************/

static int initEvent(MprTestGroup *gp)
{
    TestEvent   *te;

    if ((te = mprAllocObj(TestEvent, manageTestEvent)) == 0) {
        return 0;
    }
    gp->data = te;
    return 0;
}


static void manageTestEvent(TestEvent *te, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(te->event);
    }
}


/*
    Event callback used by a few unit tests. This signals completion of the unit test.
 */
static void eventCallback(void *data, MprEvent *event)
{
    mprSignalTestComplete((MprTestGroup*) data);
}


static void testCreateEvent(MprTestGroup *gp)
{
    TestEvent   *te;

    te = gp->data;

    /*
        Without spawning a thread
     */
    te->event = mprCreateEvent(NULL, "testCreateEvent", 0, eventCallback, (void*) gp, MPR_EVENT_QUICK);
    assert(te->event != 0);
    assert(mprWaitForTestToComplete(gp, MPR_TEST_SLEEP));

    /*
        Run event callback on a separate thread
     */
    te->event = mprCreateEvent(NULL, "testCreateEvent", 0, eventCallback, (void*) gp, 0);
    assert(te->event != 0);
    assert(mprWaitForTestToComplete(gp, MPR_TEST_SLEEP));
    te->event = 0;
}


static void testCancelEvent(MprTestGroup *gp)
{
    TestEvent   *te;

    te = gp->data;

    te->event = mprCreateEvent(NULL, "testCancelEvent", 20000, eventCallback, (void*) gp, MPR_EVENT_QUICK);
    assert(te->event != 0);
    mprRemoveEvent(te->event);
    te->event = 0;
}


static void testReschedEvent(MprTestGroup *gp)
{
    TestEvent   *te;

    te = gp->data;

    te->event = mprCreateEvent(NULL, "testReschedEvent", 50000000, eventCallback, (void*) gp, MPR_EVENT_QUICK);
    assert(te->event != 0);
    
    mprRescheduleEvent(te->event, 20);
    assert(mprWaitForTestToComplete(gp, MPR_TEST_SLEEP));
    te->event = 0;
}


MprTestDef testEvent = {
    "event", 0, initEvent, 0,
    {
        MPR_TEST(0, testCreateEvent),
        MPR_TEST(0, testCancelEvent),
        MPR_TEST(0, testReschedEvent),
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
