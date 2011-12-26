/**
    testTime.c - Unit tests for Time and Date routines
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Code ************************************/

static void testTimeBasics(MprTestGroup *gp)
{
    MprTime     mark, now, remaining, elapsed;

    mark = mprGetTime();
    assert(mark != 0);
    
    remaining = mprGetRemainingTime(mark, 30000);
    assert(0 <= remaining && remaining <= 30000);

    elapsed = mprGetElapsedTime(mark);
    assert(0 <= elapsed && elapsed < 30000);

    mprSleep(21);
    now = mprGetTime();
    assert(mprCompareTime(mark, now) <= 0);
}


static void testZones(MprTestGroup *gp)
{
    MprTime     now;
    struct  tm  tm;

    /* TODO - need some tests here */
    now = mprGetTime();
    mprDecodeLocalTime(&tm, now);
    mprDecodeUniversalTime(&tm, now);
}


static void testFormatTime(MprTestGroup *gp)
{
    MprTime     now;
    struct  tm  tm;
    char        *str;

    /* TODO - need some tests here */
    now = mprGetTime();
    mprDecodeLocalTime(&tm, now);
    str = mprFormatTm(NULL, &tm);
    assert(str && *str);
}


static void testParseTime(MprTestGroup *gp)
{
    MprTime     when;

    when = 0;
    assert(mprParseTime(&when, "today", MPR_LOCAL_TIMEZONE, NULL) == 0);
    assert(mprParseTime(&when, "tomorrow", MPR_LOCAL_TIMEZONE, NULL) == 0);
    assert(mprParseTime(&when, "12:00", MPR_LOCAL_TIMEZONE, NULL) == 0);
    assert(mprParseTime(&when, "12:30 pm", MPR_LOCAL_TIMEZONE, NULL) == 0);
    assert(mprParseTime(&when, "1/31/99", MPR_LOCAL_TIMEZONE, NULL) == 0);
    assert(mprParseTime(&when, "Jan 17 2012", MPR_LOCAL_TIMEZONE, NULL) == 0);
    assert(mprParseTime(&when, "March 17 2012", MPR_LOCAL_TIMEZONE, NULL) == 0);
    assert(when != 0);
}


MprTestDef testTime = {
    "time", 0, 0, 0,
    {
        MPR_TEST(0, testTimeBasics),
        MPR_TEST(0, testZones),
        MPR_TEST(0, testFormatTime),
        MPR_TEST(0, testParseTime),
        MPR_TEST(0, 0),
    },
};

/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
    
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
