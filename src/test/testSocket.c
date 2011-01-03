/**
    testSocket.c - Unit tests for mprSocket

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mprTest.h"

/*********************************** Locals ***********************************/

typedef struct TestSocket {
    MprTestGroup    *gp;                        /* Test group reference */
    MprSocket       *server;                    /* Server listen socket */
    MprSocket       *accepted;                  /* Server-side accepted client socket */
    MprSocket       *client;                    /* Client socket */
    MprBuf          *inBuf;                     /* Input buffer */
    int             port;                       /* Server port */
} TestSocket;

static int warnNoInternet = 0;
static int bufsize = 16 * 1024;

/***************************** Forward Declarations ***************************/

static int acceptFn(MprTestGroup *gp, MprEvent *event);
static void manageTestSocket(TestSocket *ts, int flags);
static MprSocket *openServer(MprTestGroup *gp, cchar *host);
static int readEvent(MprTestGroup *gp, MprEvent *event);

/************************************ Code ************************************/
/*
    Initialize the TestSocket structure and find a free server port to listen on.
    Also determine if we have an internet connection. 
    This is called per group.
 */
static int initSocket(MprTestGroup *gp)
{
    TestSocket      *ts;
    MprSocket       *sp;

    if ((ts = mprAllocObj(TestSocket, manageTestSocket)) == 0) {
        return 0;
    }
    ts->inBuf = mprCreateBuf(0, 0);
    gp->data = ts;

    if (getenv("NO_INTERNET")) {
        warnNoInternet = 1;
    } else {
        /*
            See if we have an internet connection
         */
        sp = mprCreateSocket(NULL);
        if (mprConnectSocket(sp, "www.google.com", 80, 0) >= 0) {
            gp->hasInternet = 1;
        }
        mprCloseSocket(sp, 0);

        /*
            Check for IPv6 support
         */
        if ((sp = openServer(gp, "::1")) != 0) {
            gp->hasIPv6 = 1;
            mprCloseSocket(sp, 0);
        }
    }
    return 0;
}


static int termSocket(MprTestGroup *gp)
{
    return 0;
}


/*
    Open a server on a free port.
 */
static MprSocket *openServer(MprTestGroup *gp, cchar *host)
{
    TestSocket      *ts;
    MprSocket       *sp;
    int             port;
    
    ts = gp->data;
    if ((sp = mprCreateSocket(NULL)) == 0) {
        return 0;
    }
    for (port = 9175; port < 9250; port++) {
        if (mprListenOnSocket(sp, host, port, MPR_SOCKET_NODELAY | MPR_SOCKET_THREAD) >= 0) {
            ts->port = port;
            mprAddSocketHandler(sp, MPR_SOCKET_READABLE, NULL, (MprEventProc) acceptFn, gp);
            return sp;
        }
    }
    return 0;
}


static void manageTestSocket(TestSocket *ts, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ts->server);
        mprMark(ts->accepted);
        mprMark(ts->client);
        mprMark(ts->inBuf);

    } else if (flags & MPR_MANAGE_FREE) {
        mprCloseSocket(ts->accepted, 0);
        mprCloseSocket(ts->client, 0);
        mprCloseSocket(ts->server, 0);
    }
}


/*
    Test:
        - IPv4
        - IPv6
        - blocking / non-blocking mode
        - client & server
        - broadcast, datagram, 
        - options: reuse, nodelay
 */
static int acceptFn(MprTestGroup *gp, MprEvent *event)
{
    MprSocket       *sp;
    TestSocket      *ts;

    ts = (TestSocket*) gp->data;
    sp = mprAcceptSocket(ts->server);
    assert(sp != NULL);
    if (sp) {
        mprAssert(sp->fd >= 0);
        ts->accepted = sp;
        mprAddSocketHandler(sp, MPR_READABLE, NULL, (MprEventProc) readEvent, (void*) gp);
        mprSignalTestComplete(gp);
    }
    return 0;
}


/*
    Read incoming data. Expect to read about 5K worth of data.
 */
static int readEvent(MprTestGroup *gp, MprEvent *event)
{
    TestSocket      *ts;
    MprSocket       *sp;
    ssize           len, nbytes, space;
    char            *buf;
    int             rc;

    ts = (TestSocket*) gp->data;
    sp = ts->accepted;

    len = mprGetBufLength(ts->inBuf);
    space = mprGetBufSpace(ts->inBuf);
    if (space < (bufsize / 2)) {
        rc = mprGrowBuf(ts->inBuf, bufsize - space);
        assert(rc == 0);
    }
    buf = mprGetBufEnd(ts->inBuf);
    nbytes = mprReadSocket(sp, buf, mprGetBufSpace(ts->inBuf));

    if (nbytes < 0) {
        mprCloseSocket(sp, 1);
        mprSignalTestComplete(gp);
        return 1;

    } else if (nbytes == 0) {
        if (mprIsSocketEof(sp)) {
            mprCloseSocket(sp, 1);
            mprSignalTestComplete(gp);
            return 1;
        }

    } else {
        mprAdjustBufEnd(ts->inBuf, nbytes);
    }
    mprEnableSocketEvents(sp, MPR_READABLE);
    return 0;
}


static void testCreateSocket(MprTestGroup *gp)
{
    MprSocket       *sp;

    sp = mprCreateSocket(NULL);
    assert(sp != 0);
    mprCloseSocket(sp, 0);
}


static void testClient(MprTestGroup *gp)
{
    MprSocket       *sp;
    int             rc;

    if (gp->hasInternet) {
        if (gp->service->testDepth > 1 && mprHasSecureSockets(gp)) {
            sp = mprCreateSocket(NULL);
            assert(sp != 0);
            rc = mprConnectSocket(sp, "www.google.com", 80, 0);
            assert(rc >= 0);
            mprCloseSocket(sp, 0);
        }
    } else if (warnNoInternet++ == 0) {
        mprPrintf("\n%12s Skipping test %s.testClient: no internet connection.\n", "[Notice]", gp->fullName);
    }
}


static void testClientServer(MprTestGroup *gp, cchar *host)
{
    TestSocket      *ts;
    MprDispatcher   *dispatcher;
    MprTime         mark;
    ssize           len, thisLen, nbytes, sofar;
    char            *buf, *thisBuf;
    int             i, rc, count;

    dispatcher = mprGetDispatcher(gp);

    ts = gp->data;
    ts->accepted = 0;
    ts->server = openServer(gp, host);
    assert(ts->server != NULL);
    if (ts->server == 0) {
        return;
    }
    ts->client = mprCreateSocket(NULL);
    assert(ts->client != 0);
    assert(!ts->accepted);

    rc = mprConnectSocket(ts->client, host, ts->port, 0);
    assert(rc >= 0);

    mprWaitForTestToComplete(gp, MPR_TEST_SLEEP);
    /*  Set in acceptFn() */
    assert(ts->accepted != 0);

    buf = "01234567890123456789012345678901234567890123456789\r\n";
    len = strlen(buf);

    /*
        Write a set of lines to the client. Server should receive. Use blocking mode. This writes about 5K of data.
     */
    mprSetSocketBlockingMode(ts->client, 1);
    sofar = 0;
    count = 100;
    for (i = 0; i < count; i++) {
        /*
            Non-blocking I/O may return a short-write
         */
        thisBuf = buf;
        for (thisLen = len; thisLen > 0; ) {
            nbytes = mprWriteSocket(ts->client, thisBuf, thisLen);
            assert(nbytes >= 0);
            thisLen -= nbytes;
            thisBuf += nbytes;
        }
    }
    mprCloseSocket(ts->client, 1);
    ts->client = 0;

    mark = mprGetTime();
    do {
        if (mprWaitForTestToComplete(gp, MPR_TEST_SLEEP)) {
            break;
        }
    } while (mprGetRemainingTime(mark, MPR_TEST_SLEEP) > 0);

    if (mprGetBufLength(ts->inBuf) != (count * len)) {
        print("i %d count %d, remaining %d, buflen %d, cmp %d", i, count, thisLen, mprGetBufLength(ts->inBuf), count * len);
        print("ELAPSED %d", mprGetElapsedTime(mark));
    }
    assert(mprGetBufLength(ts->inBuf) == (count * len));
    mprFlushBuf(ts->inBuf);

    mprCloseSocket(ts->server, 0);
    ts->server = 0;
}


static void testClientServerIPv4(MprTestGroup *gp)
{
    testClientServer(gp, "127.0.0.1");
}


static void testClientServerIPv6(MprTestGroup *gp)
{
    if (gp->hasIPv6) {
        testClientServer(gp, "::1");
    }
}


static void testClientSslv4(MprTestGroup *gp)
{
    MprSocket       *sp;
    int             rc;

    if (gp->hasInternet) {
        if (gp->service->testDepth > 1 && mprHasSecureSockets(gp)) {
            sp = mprCreateSocket(NULL);
            assert(sp != 0);
            assert(sp->provider != 0);
            rc = mprConnectSocket(sp, "www.google.com", 443, 0);
            assert(rc >= 0);
            mprCloseSocket(sp, 0);
        }
    } else if (warnNoInternet++ == 0) {
        mprPrintf("\n%12s Skipping test %s.testClientSslv4: no internet connection.\n", "[Notice]", gp->fullName);
    }
}


MprTestDef testSocket = {
    "socket", 0, initSocket, termSocket,
    {
        MPR_TEST(0, testCreateSocket),
        MPR_TEST(0, testClient),
#if !WIN
        MPR_TEST(0, testClientServerIPv4),
        MPR_TEST(0, testClientServerIPv6),
#endif
        MPR_TEST(0, testClientSslv4),
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
