/**
    testMem.c - Unit tests for the mprMem module

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */ 

/********************************** Includes **********************************/

#include    "mprTest.h"

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
    mprFree(0);
    cp = sclone(NULL);
    assert(cp != 0);
    assert(cp[0] == '\0');
}


#if UNUSED
static void allocManager(int **dp, int flags)
{
    mprAssert(flags & MPR_MANAGE_FREE);
    if (flags & MPR_MANAGE_FREE) {
        mprAssert(*dp);
        mprAssert(**dp == 0);
        **dp = 1;
    }
}


static void testManager(MprTestGroup *gp)
{
    int     done, **dp;

    done = 0;
    dp = mprAllocObj(int*, allocManager);
    assert(dp);
    assert(*dp == 0);
    *dp = &done;
    assert(!done);
    //  MOB - free does not invoke destructors immediately
    mprFree(dp);
    mprRequestGC(1);
    assert(done == 1);
}
#endif


static void testBigAlloc(MprTestGroup *gp)
{
    void    *mp;
    ssize  memsize, len;
    
    memsize = mprGetMem();
    len = 8 * 1024 * 1024;
    mp = mprAlloc(len);
    assert(mp != 0);
    memset(mp, 0, len);
    mprFree(mp);
    
    if (mprGetMem() > memsize) {
        assert((mprGetMem() - memsize) < (len * 2));
    }
}


static void testLotsOfAlloc(MprTestGroup *gp)
{
    void    *mp;
    ssize  memsize;
    int     i;

    for (i = 0; i < 10000; i++) {
        mp = mprAlloc(64);
        assert(mp != 0);
        mprFree(mp);
    }
    memsize = mprGetMem();
    for (i = 2; i < (1024 * 1024); i *= 2) {
        mp = mprAlloc(i);
        assert(mp != 0);
        mprFree(mp);
        mprRequestGC(0);
    }
    if (mprGetMem() > memsize) {
        assert((mprGetMem() - memsize) < (4 * 1024 * 1024));
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
        mprFree(blocks[i]);
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
        mprFree(blocks[i]);
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
        mprFree(blocks[i]);
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
    ssize       memsize, len, actual;
    uchar       *cp;
    int         i, j, index, blockSize, iterations, depth;
    
    /*
        Allocate blocks and store in a cache. The GC will mark blocks in the cache and preserve. Others will be deleted.
        Check memory does not grow unexpectedly.
     */
    depth = gp->service->testDepth;
    iterations = (depth * depth * 1024) + 1024;
    memsize = mprGetMem();
    blockSize = 16 * 1024;
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
