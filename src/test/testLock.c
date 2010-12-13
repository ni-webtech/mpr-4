/**
    testLock.c - Unit tests for mprLock
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mprTest.h"

/*********************************** Locals ***********************************/

static MprOsThread      critical[2048];
static MprMutex         *mutex;

/************************************ Code ************************************/

static int initLock(MprTestGroup *gp)
{
    mprGlobalLock(gp);
    if (mutex == 0) {
        mutex = mprCreateLock(gp);
        mprAddRoot(mutex);
    }
    mprGlobalUnlock(gp);
    return 0;
}


static int termLock(MprTestGroup *gp)
{
    mprGlobalLock(gp);
    if (mutex) {
        mprRemoveRoot(mutex);
        mprFree(mutex);
        mutex = 0;
    }
    mprGlobalUnlock(gp);
    return 0;
}


static void testCriticalSection(MprTestGroup *gp)
{
    int     i, size;

    mprLock(mutex);
    size = sizeof(critical) / sizeof(MprThread*);
    for (i = 0; i < size; i++) {
        critical[i] = mprGetCurrentOsThread(gp);
    }
    for (i = 0; i < size; i++) {
        assert(critical[i] == mprGetCurrentOsThread(gp));
    }
    mprUnlock(mutex);
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
    
    Copyright (c) Embedthis Software LLC, 2003-2010. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2010. All Rights Reserved.
    
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
    
    @end
 */
