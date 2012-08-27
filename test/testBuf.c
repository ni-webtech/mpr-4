/**
    testBuf.c - Unit tests for the Buf class
 *
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/*********************************** Locals ***********************************/

typedef struct TestBuf {
    MprBuf      *buf;
} TestBuf;

static void manageTestBuf(TestBuf *tb, int flags);

/************************************ Code ************************************/

static int initBuf(MprTestGroup *gp)
{
    TestBuf     *tb;

    gp->data = tb = mprAllocObj(TestBuf, manageTestBuf);
    return 0;
}


static void manageTestBuf(TestBuf *tb, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(tb->buf);
    }
}


static void testCreateBuf(MprTestGroup *gp)
{
    MprBuf  *bp;

    bp = mprCreateBuf(512, 512);
    assert(bp != 0);
}


static void testIsBufEmpty(MprTestGroup *gp)
{
    MprBuf  *bp;
    ssize  size;

    size = 512;
    bp = mprCreateBuf(size, -1);

    assert(bp != 0);
    assert(mprGetBufLength(bp) == 0);
    assert(mprGetBufSize(bp) >= size);
    assert(mprGetBufSpace(bp) >= (size - 1));
    assert(mprGetBufLength(bp) == 0);
    assert(mprGetBufStart(bp) == mprGetBufEnd(bp));
}


static void testPutAndGetToBuf(MprTestGroup *gp)
{
    MprBuf  *bp;
    int     i, size, rc, c, bytes;

    size = 512;
    bp = mprCreateBuf(size, -1);

    bytes = size / 2;
    for (i = 0; i < bytes; i++) {
        rc = mprPutCharToBuf(bp, 'c');
        assert(rc == 1);
    }
    assert(mprGetBufLength(bp) == bytes);

    for (i = 0; i < bytes; i++) {
        c = mprGetCharFromBuf(bp);
        assert(c == 'c');
    }
    c = mprGetCharFromBuf(bp);
    assert(c == -1);
}


static void testFlushBuf(MprTestGroup *gp)
{
    MprBuf      *bp;
    ssize       rc;
    char        buf[512];
    int         size, i;

    size = 512;
    bp = mprCreateBuf(size, size);
    assert(bp != 0);

    /*
        Do multiple times to test that flush resets the buffer pointers correctly
     */
    for (i = 0; i < 100; i++) {
        rc = mprPutStringToBuf(bp, "Hello World");
        assert(rc == 11);
        assert(mprGetBufLength(bp) == 11);

        mprFlushBuf(bp);
        assert(mprGetBufLength(bp) == 0);
        assert(mprGetCharFromBuf(bp) == -1);
        assert(mprGetBlockFromBuf(bp, buf, sizeof(buf)) == 0);
    }
}


static void testGrowBuf(MprTestGroup *gp)
{
    MprBuf      *bp;
    ssize       size, rc, bytes;
    int         i, c;

    /*
        Put more data than the initial size to force the buffer to grow
     */
    size = 512;
    bp = mprCreateBuf(size, -1);
    bytes = size * 10;
    for (i = 0; i < bytes; i++) {
        rc = mprPutCharToBuf(bp, 'c');
        assert(rc == 1);
    }
    rc = mprGetBufSize(bp);
    rc = mprGetBufLength(bp);
    assert(mprGetBufSize(bp) > size);
    assert(mprGetBufSize(bp) >= (ssize) bytes);
    assert(mprGetBufLength(bp) == bytes);

    for (i = 0; i < bytes; i++) {
        c = mprGetCharFromBuf(bp);
        assert(c == 'c');
    }
    c = mprGetCharFromBuf(bp);
    assert(c == -1);

    /*
        Test a fixed buffer (should not grow) 
     */
    size = 2;                       /* Can only ever fit in 1 byte */
    bp = mprCreateBuf(size, size);
    assert(bp != 0);

    rc = mprPutCharToBuf(bp, 'c');
    assert(rc == 1);

    rc = mprPutCharToBuf(bp, 'c');
    assert(rc == 1);
    
    rc = mprPutCharToBuf(bp, 'c');
    assert(rc == -1);
}


static void testMiscBuf(MprTestGroup *gp)
{
    MprBuf      *bp;
    ssize       size, rc;
    int         c;

    size = 512;
    bp = mprCreateBuf(size, 0);
    assert(bp != 0);

    /*
        Test insert char
     */
    rc = mprPutStringToBuf(bp, " big dog");
    assert(rc == 8);
    assert(mprGetBufLength(bp) == 8);

    /*
        Test add null
     */
    mprFlushBuf(bp);
    assert(mprGetBufLength(bp) == 0);

    rc = mprPutCharToBuf(bp, 'A');
    assert(rc == 1);
    
    rc = mprPutCharToBuf(bp, 'B');
    assert(rc == 1);
    assert(mprGetBufLength(bp) == 2);

    mprAddNullToBuf(bp);
    assert(mprGetBufLength(bp) == 2);
    assert(strcmp(mprGetBufStart(bp), "AB") == 0);

    c = mprLookAtNextCharInBuf(bp);
    assert(c == 'A');
}


static void testBufLoad(MprTestGroup *gp)
{
    TestBuf     *tb;
    MprBuf      *bp;
    char        obuf[512], ibuf[512];
    ssize       rc, count, bytes, sofar, size, len;
    int         i, j;

    tb = gp->data;

    /*
        Pick an odd size to guarantee put blocks are sometimes partial.
     */
    len = 981;
    tb->buf = bp = mprCreateBuf(len, 0);
    assert(bp != 0);
    mprYield(MPR_YIELD_STICKY);

    for (i = 0; i < (int) sizeof(ibuf); i++) {
        ibuf[i] = 'A' + (i % 26);
    }
    for (j = 0; j < 1000; j++) {
        rc = mprPutBlockToBuf(bp, (char*) ibuf, sizeof(ibuf));
        assert(rc == sizeof(ibuf));

        count = 0;
        while (mprGetBufLength(bp) > 0) {
            size = 0xFFFF & (int) mprGetTime();
            bytes = (int) ((size % sizeof(obuf)) / 9) + 1;
            bytes = min(bytes, ((int) sizeof(obuf) - count));
            rc = mprGetBlockFromBuf(bp, &obuf[count], bytes);
            assert(rc > 0);
            count += rc;
        }
        assert(count == sizeof(ibuf));
        for (i = 0; i < (int) sizeof(ibuf); i++) {
            assert(obuf[i] == ('A' + (i % 26)));
        }
        mprFlushBuf(bp);
    }

    /*
        Now do a similar load test but using the start / end pointer directly 
     */
    for (j = 0; j < 1000; j++) {
        bytes = sizeof(ibuf);
        sofar = 0;
        do {
            len = mprGetBufSpace(bp);
            len = min(len, bytes);
            memcpy(mprGetBufEnd(bp), &ibuf[sofar], len);
            sofar += len;
            bytes -= len;
            mprAdjustBufEnd(bp, len);
        } while (bytes > 0);
        assert(sofar == sizeof(ibuf));

        sofar = 0;
        while (mprGetBufLength(bp) > 0) {
            len = min(mprGetBufLength(bp), ((ssize) (sizeof(obuf) - sofar)));
            memcpy(&obuf[sofar], mprGetBufStart(bp), len);
            sofar += len;
            mprAdjustBufStart(bp, len);
        }
        assert(sofar == sizeof(ibuf));
        for (i = 0; i < (int) sizeof(obuf); i++) {
            assert(obuf[i] == ('A' + (i % 26)));
        }
        mprFlushBuf(bp);
    }
    mprResetYield();
}


/*
    TODO -- missing explicit thread interlock tests
    Missing:
        - length tests
        - Fmt String tests
        - Buf refill
        - Copy down
        - Adjust start / end
 */

MprTestDef testBuf = {
    "buf", 0, initBuf, 0,
    {
        MPR_TEST(0, testCreateBuf),
        MPR_TEST(0, testIsBufEmpty),
        MPR_TEST(0, testPutAndGetToBuf),
        MPR_TEST(0, testFlushBuf),
        MPR_TEST(0, testGrowBuf),
        MPR_TEST(0, testMiscBuf),
        MPR_TEST(0, testBufLoad),
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
