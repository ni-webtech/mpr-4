/**
    testSocket.c - Unit tests for mprSocket

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mprTest.h"

/*********************************** Locals ***********************************/

typedef struct MprTestSocket {
    MprTestGroup    *gp;                        /* Test group reference */
    MprSocket       *sock;                      /* Server listen socket */
    MprSocket       *client;                    /* Client socket */
    MprBuf          *inBuf;                     /* Input buffer */
    MprWaitHandler  *listenHandler;             /* Listen wait handler for the server */
    MprWaitHandler  *clientHandler;             /* Wait handler for the client connection */
    int             port;                       /* Server port */
    int             accepted;                   /* Accept */
} MprTestSocket;

static int warnNoInternet = 0;
static int bufsize = 16 * 1024;

/***************************** Forward Declarations ***************************/

static int acceptFn(MprTestGroup *gp, MprEvent *event);
static void manageTestSocket(MprTestSocket *ts, int flags);
static MprTestSocket *openServer(MprTestGroup *gp, cchar *host);
static int readEvent(MprTestGroup *gp, MprEvent *event);

/************************************ Code ************************************/
/*
    Initialize the TestSocket structure and find a free server port to listen on.
    Also determine if we have an internet connection. 
    This is called per group.
 */
static int initSocket(MprTestGroup *gp)
{
    MprSocket       *sock;
    MprTestSocket   *sp;

    if (getenv("NO_INTERNET")) {
        warnNoInternet = 1;
    } else {
        /*
            See if we have an internet connection
         */
        sock = mprCreateSocket(NULL);
        if (mprOpenClientSocket(sock, "www.google.com", 80, 0) >= 0) {
            gp->hasInternet = 1;
        }
        mprCloseSocket(sock, 0);

        /*
            Check for IPv6 support
         */
        if ((sp = openServer(gp, "::1")) != 0) {
            gp->hasIPv6 = 1;
            mprFree(sp);
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
static MprTestSocket *openServer(MprTestGroup *gp, cchar *host)
{
    MprSocket       *listen;
    MprTestSocket   *ts;
    int             port;
    
    listen = mprCreateSocket(NULL);
    if (listen == 0) {
        return 0;
    }
    for (port = 9175; port < 9250; port++) {
        if (mprOpenServerSocket(listen, host, port, MPR_SOCKET_NODELAY | MPR_SOCKET_THREAD) >= 0) {
            if (mprListenOnSocket(listen) < 0) {
                continue;
            }
            ts = mprAllocObj(MprTestSocket, manageTestSocket);
            if (ts == 0) {
                return 0;
            }
            ts->gp = gp;
            ts->sock = listen;
            ts->inBuf = mprCreateBuf(0, 0);
            ts->port = port;

            ts->listenHandler = mprCreateWaitHandler(listen->fd, MPR_SOCKET_READABLE, NULL, (MprEventProc) acceptFn, gp);
            return ts;
        }
    }
    return 0;
}


static void manageTestSocket(MprTestSocket *ts, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ts->sock);
        mprMark(ts->client);
        mprMark(ts->inBuf);
        mprMark(ts->listenHandler);
        mprMark(ts->clientHandler);

    } else if (flags & MPR_MANAGE_FREE) {
        if (ts->listenHandler) {
            printf("CLOSE LISTEN HANDLER\n");
        }
        mprRemoveWaitHandler(ts->clientHandler);
        mprRemoveWaitHandler(ts->listenHandler);
        if (ts->sock) {
            printf("CLOSE SOCK %d\n", ts->sock->fd);
        }
        mprCloseSocket(ts->sock, 0);
        if (ts->client) {
            mprAssert(ts->client->fd < 0);
        }
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
    MprTestSocket   *ts;

    ts = (MprTestSocket*) gp->data;
    sp = mprAcceptSocket(ts->sock);
    assert(sp);
    if (sp) {
        mprAssert(sp->fd >= 0);
        ts->client = sp;
        ts->accepted = 1;        
        ts->clientHandler = mprCreateWaitHandler(sp->fd, MPR_READABLE, NULL, (MprEventProc) readEvent, (void*) gp);
        assert(ts->clientHandler);
        mprSignalTestComplete(gp);
    }
    return 0;
}


/*
    Read incoming data. Expect to read about 5K worth of data.
 */
static int readEvent(MprTestGroup *gp, MprEvent *event)
{
    MprTestSocket   *ts;
    MprSocket       *sp;
    char            *buf;
    int             rc, space, nbytes, len;

    ts = (MprTestSocket*) gp->data;
    sp = ts->client;

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
    mprEnableWaitEvents(ts->clientHandler, MPR_READABLE);
    return 0;
}


static void testCreateSocket(MprTestGroup *gp)
{
    MprSocket       *sp;

    sp = mprCreateSocket(NULL);
    assert(sp != 0);
    mprFree(sp);
}


static void testClient(MprTestGroup *gp)
{
    MprSocket       *sp;
    int             rc;

    if (gp->hasInternet) {
        sp = mprCreateSocket(NULL);
        assert(sp != 0);
    
        rc = mprOpenClientSocket(sp, "www.google.com", 80, 0);
        assert(rc >= 0);
        mprFree(sp);
        
    } else if (warnNoInternet++ == 0) {
        mprPrintf("\n%12s Skipping test %s.testClient: no internet connection.\n", "[Notice]", gp->fullName);
    }
}


static void testClientServer(MprTestGroup *gp, cchar *host)
{
    MprSocket       *client;
    MprTestSocket   *ts;
    MprDispatcher   *dispatcher;
    MprTime         mark;
    ssize           len;
    char            *buf, *thisBuf;
    int             i, rc, thisLen, sofar, nbytes, count;

    dispatcher = mprGetDispatcher(gp);

    gp->data = ts = openServer(gp, host);
    assert(ts != NULL);
    if (ts == 0) {
        return;
    }
    client = mprCreateSocket(NULL);
    assert(client != 0);
    assert(!ts->accepted);

    /*
        Open client connection
     */
    rc = mprOpenClientSocket(client, host, ts->port, 0);
    assert(rc >= 0);

    mprWaitForTestToComplete(gp, MPR_TEST_SLEEP);
    /*  Set in acceptFn() */
    assert(ts->accepted);

    buf = "01234567890123456789012345678901234567890123456789\r\n";
    len = strlen(buf);

    /*
        Write a set of lines to the client. Server should receive. Use blocking mode. This writes about 5K of data.
     */
    mprSetSocketBlockingMode(client, 1);
    sofar = 0;
    count = 100;
    for (i = 0; i < count; i++) {
        /*
            Non-blocking I/O may return a short-write
         */
        thisBuf = buf;
        for (thisLen = len; thisLen > 0; ) {
            nbytes = mprWriteSocket(client, thisBuf, thisLen);
            assert(nbytes >= 0);
            thisLen -= nbytes;
            thisBuf += nbytes;
#if UNUSED
            if (nbytes == 0) {
                mprServiceEvents(NULL, 50, MPR_SERVICE_ONE_THING | MPR_SERVICE_NO_GC);
            }
#endif
        }
    }
    mprCloseSocket(client, 1);

    mark = mprGetTime(gp);
    do {
#if UNUSED
        mprServiceEvents(NULL, 50, MPR_SERVICE_ONE_THING | MPR_SERVICE_NO_GC);
#endif
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
    mprFree(client); 
    gp->data = 0;
    mprFree(ts); 
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
        if (mprHasSecureSockets(gp)) {
            sp = mprCreateSocket(NULL);
            assert(sp != 0);
            assert(sp->provider != 0);
            rc = mprOpenClientSocket(sp, "www.google.com", 443, 0);
            assert(rc >= 0);
            mprFree(sp);
        }
    } else if (warnNoInternet++ == 0) {
        mprPrintf("\n%12s Skipping test %s.testClientSslv4: no internet connection.\n", "[Notice]", gp->fullName);
    }
}


MprTestDef testSocket = {
    "socket", 0, initSocket, termSocket,
    {
#if 1 || UNUSED
        MPR_TEST(0, testCreateSocket),
        MPR_TEST(0, testClient),
#endif
#if !WIN
        MPR_TEST(0, testClientServerIPv4),
        MPR_TEST(0, testClientServerIPv6),
#endif
#if 1 || UNUSED
        MPR_TEST(0, testClientSslv4),
#endif
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
