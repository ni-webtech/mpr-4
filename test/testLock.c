/**
    testLock.c - Unit tests for mprLock
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/*********************************** Locals ***********************************/

static MprOsThread      critical[2048];

/*
    Single global mutex
 */
static MprMutex     *mutex;
static int          threadCount;    

/************************************ Code ************************************/

static int initLock(MprTestGroup *gp)
{
    mprGlobalLock(gp);
    if (mutex == 0) {
        mutex = mprCreateLock(gp);
        gp->data = mutex;
        mprAddRoot(mutex);
    }
    threadCount++;
    mprGlobalUnlock(gp);
    return 0;
}


static int termLock(MprTestGroup *gp)
{
    mprGlobalLock(gp);
    if (--threadCount == 0) {
        if (mutex) {
            mprRemoveRoot(mutex);
            mutex = 0;
        }
    }
    mprGlobalUnlock(gp);
    return 0;
}


static void testCriticalSection(MprTestGroup *gp)
{
    int     i, size;

    mprLock(mutex);
    assert(mutex != 0);
    size = sizeof(critical) / sizeof(MprOsThread);
    for (i = 0; i < size; i++) {
        critical[i] = mprGetCurrentOsThread();
    }
    for (i = 0; i < size; i++) {
        assert(critical[i] == mprGetCurrentOsThread());
    }
    mprUnlock(mutex);
    assert(mutex != 0);
}


MprTestDef testLock = {
    "lock", 0, initLock, termLock,
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
