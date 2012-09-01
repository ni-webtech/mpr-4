/**
    testMem.c - Unit tests for the mprMem module

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */ 

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Code ************************************/

static void testBasicAlloc(MprTestGroup *gp)
{
    char    *cp;
    int     size;

    size = 16;
    cp = mprAlloc(size);
    assert(cp != 0);
    memset(cp, 0x77, size);

    cp = mprAlloc(size);
    assert(cp != 0);
    memset(cp, 0x77, size);
    cp = mprRealloc(cp, size * 2);
    assert(cp != 0);

    cp = sclone("Hello World");
    assert(cp != 0);
    assert(strcmp(cp, "Hello World") == 0);

    /*
        Test special MPR allowances
     */
    cp = sclone(NULL);
    assert(cp != 0);
    assert(cp[0] == '\0');
}

static int when = 0;

static void testBigAlloc(MprTestGroup *gp)
{
    void    *mp;
    ssize  len;
    
if (when) {
    mprPrintMem("Before big alloc", 1);
}
    len = 8 * 1024 * 1024;
    mp = mprAlloc(len);
    assert(mp != 0);
    memset(mp, 0, len);    
}


static void testLotsOfAlloc(MprTestGroup *gp)
{
    void    *mp;
    int     i, maxblock, count;

    count = (gp->service->testDepth * 10 * 1024) + 1024;
    for (i = 0; i < count; i++) {
        mp = mprAlloc(64);
        assert(mp != 0);
    }
    maxblock = (gp->service->testDepth * 1024 * 1024) + 1024;
    for (i = 2; i < maxblock; i *= 2) {
        mp = mprAlloc(i);
        assert(mp != 0);
        mprRequestGC(0);
    }
}


static void testAllocIntegrityChecks(MprTestGroup *gp)
{
    void    *blocks[256];
    uchar   *cp;
    int     i, j, size, count;

    /*
        Basic integrity test. Allocate blocks of 64 bytes and fill and test each block
     */
    size = 64;
    count = sizeof(blocks) / sizeof(void*);
    for (i = 0; i < count; i++) {
        blocks[i] = mprAlloc(size);
        assert(blocks[i] != 0);
        memset(blocks[i], i % 0xff, size);
    }
    /*
        Memory can't be freed without our consent (GC waits for a sync point before advancing generations).
        So this memory is safe and wont be freed yet.
     */
    for (i = 0; i < count; i++) {
        cp = (uchar*) blocks[i];
        for (j = 0; j < size; j++) {
            assert(cp[j] == (i % 0xff));
        }
    }

    /*
        Now do with bigger blocks and also free some before testing
     */
    count = sizeof(blocks) / sizeof(void*);
    for (i = 1; i < count; i++) {
        size = 1 << ((i + 6) / 100);
        blocks[i] = mprAlloc(size);
        assert(blocks[i] != 0);
        memset(blocks[i], i % 0xff, size);
    }
    for (i = 1; i < count; i += 3) {
        blocks[i] = 0;
    }
    for (i = 1; i < count; i++) {
        if (blocks[i] == 0) {
            continue;
        }
        cp = (uchar*) blocks[i];
        size = 1 << ((i + 6) / 100);
        for (j = 0; j < size; j++) {
            assert(cp[j] == (i % 0xff));
        }
    }
}


#define CACHE_MAX 256

typedef struct Cache {
    void    *blocks[CACHE_MAX];
} Cache;


static void cacheManager(Cache *cache, int flags) 
{
    int     i;
    
    if (flags & MPR_MANAGE_MARK) {
        for (i = 0; i < CACHE_MAX; i++) {
            mprMark(cache->blocks[i]);
        }
    }
}


static void testAllocLongevity(MprTestGroup *gp)
{
    Cache       *cache;
    ssize       len, actual;
    uchar       *cp;
    int         i, j, index, blockSize, iterations, depth;
    
    /*
        Allocate blocks and store in a cache. The GC will mark blocks in the cache and preserve. Others will be deleted.
        Check memory does not grow unexpectedly.
     */
    depth = gp->service->testDepth;
    iterations = (depth * depth * 512) + 64;
    blockSize = 1024;
    cache = mprAllocObj(Cache, cacheManager);
    assert(cache != 0);
    mprAddRoot(cache);

    for (i = 0; i < iterations; i++) {
        index = mprRandom() % CACHE_MAX;
        if ((cp = cache->blocks[index]) != NULL) {
            len = mprGetBlockSize(cp);
            for (j = 0; j < len; j++) {
                assert(cp[j] == index);
            }
        }
        len = mprRandom() % blockSize;
        cp = cache->blocks[index] = mprAlloc(len);
        actual = mprGetBlockSize(cp);
        assert(actual >= len);        
        memset(cp, index, actual);
    }
    mprRemoveRoot(cache);
}


/*
    TODO missing tests for:
    - triggering memoryFailure callbacks
    - Memory redline limits
    - Slab memory usage
 */
MprTestDef testAlloc = {
    "alloc", 0, 0, 0,
    {
        MPR_TEST(0, testBasicAlloc),
        MPR_TEST(0, testBigAlloc),
        MPR_TEST(0, testLotsOfAlloc),
        MPR_TEST(0, testAllocIntegrityChecks),
        MPR_TEST(0, testAllocLongevity),
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
