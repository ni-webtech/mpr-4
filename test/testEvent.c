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
