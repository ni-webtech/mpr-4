/**
    mprMem.c - Memory Allocator and Garbage Collector. 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

/******************************* Local Defines ********************************/
#if BLD_MEMORY_DEBUG
/*
    Set this address to break when this address is allocated or freed
 */
static MprMem *stopAlloc = 0;
static int stopSeqno = -1;
#endif

#define GET_MEM(ptr)            ((MprMem*) (((char*) (ptr)) - sizeof(MprMem)))
#define GET_PTR(mp)             ((char*) (((char*) (mp)) + sizeof(MprMem)))
#define GET_NEXT(mp)            ((mp)->last) ? NULL : ((MprMem*) ((char*) mp + mp->size))
#define GET_REGION(mp)          ((MprRegion*) (((char*) mp) - MPR_ALLOC_ALIGN(sizeof(MprRegion))))
#define GET_USIZE(mp)           ((ssize) (mp->size - sizeof(MprMem) - (mp->hasManager * sizeof(void*))))

/*
    Trailing block. This is optional and will look like:
        Manager
        Trailer
 */
#define PAD_PTR(mp, offset)     ((void*) (((char*) mp) + mp->size - ((offset) * sizeof(void*))))

#define MANAGER_SIZE            1
#define MANAGER_OFFSET          1
#define GET_MANAGER(mp)         ((MprManager) (*(void**) ((PAD_PTR(mp, MANAGER_OFFSET)))))
#define SET_MANAGER(mp, fn)     if (1) { *((MprManager*) PAD_PTR(mp, MANAGER_OFFSET)) = fn; } else

#if BLD_MEMORY_DEBUG
#define BREAKPOINT(mp)          breakpoint(mp);
#define CHECK(mp)               mprCheckBlock((MprMem*) mp)
#define CHECK_PTR(ptr)          CHECK(GET_MEM(ptr))
#define RESET_MEM(mp)           if (mp != GET_MEM(MPR)) { \
                                    memset((char*) mp + sizeof(MprFreeMem), 0xFE, mp->size - sizeof(MprFreeMem)); \
                                } else
#define SET_MAGIC(mp)           if (1) { (mp)->magic = MPR_ALLOC_MAGIC; } else
#define SET_SEQ(mp)             if (1) { (mp)->seqno = heap->nextSeqno++; } else
#define INIT_BLK(mp, len)       if (1) { \
                                    SET_MAGIC(mp); \
                                    SET_SEQ(mp); \
                                    mp->size = len; \
                                    mp->name = NULL; \
                                    mp->gen = heap->active; \
                                    mp->mark = heap->eternal; \
                                } else
#define VALID_BLK(mp)           validBlk(mp)

#else /* Release mode */
#define BREAKPOINT(mp)
#define CHECK(mp)           
#define CHECK_PTR(mp)           
#define RESET_MEM(mp)           
#define SET_MAGIC(mp)
#define SET_SEQ(mp)           
#define INIT_BLK(mp, len)       if (1) { mp->size = len; mp->gen = heap->active ; mp->mark = heap->eternal ; } else
#define VALID_BLK(mp)           1
#endif

#if BLD_MEMORY_STATS
    #define INC(field)          if (1) { heap->stats.field++; } else 
#else
    #define INC(field)
#endif

#define lockHeap()              mprSpinLock(&heap->heapLock);
#define unlockHeap()            mprSpinUnlock(&heap->heapLock);

#define percent(a,b) ((int) ((a) * 100 / (b)))

#if !MACOSX && !FREEBSD
    #define NEED_FFSL 1
    #if WIN
    #elif BLD_HOST_CPU_ARCH == MPR_CPU_IX86 || BLD_HOST_CPU_ARCH == MPR_CPU_IX64
        #define USE_FFSL_ASM_X86 1
    #endif
    static MPR_INLINE int ffsl(ulong word);
    static MPR_INLINE int flsl(ulong word);
#elif BSD_EMULATION
    #define ffsl FFSL
    #define flsl FLSL
    #define NEED_FFSL 1
    #define USE_FFSL_ASM_X86 1
    static MPR_INLINE int ffsl(ulong word);
    static MPR_INLINE int flsl(ulong word);
#endif

/********************************** Data **************************************/

#undef          MPR
Mpr             *MPR;
static MprHeap  *heap;
static int      padding[] = { 0, MANAGER_SIZE };

/***************************** Forward Declarations ***************************/

static void allocException(ssize size, bool granted);
static MprMem *freeBlock(MprMem *mp);
static void *getNextRoot(int *indexp);
static int getQueueIndex(ssize size, int roundup);
static void getSystemInfo();
static MprMem *growHeap(ssize size);
static int initFree();
static void initGen();
static void linkBlock(MprMem *mp); 
static void mark();
static void marker(void *unused, MprThread *tp);
static void markRoots();
static int memoryNotifier(int flags, ssize size);
static void nextGen();
static MprMem *searchFree(ssize size);
static void sweep();
static void sweeper(void *unused, MprThread *tp);
static void synchronize();
static void unlinkBlock(MprFreeMem *fp);

#if BLD_WIN_LIKE
    static int winPageModes(int flags);
#endif
#if BLD_MEMORY_DEBUG
    static void breakpoint(MprMem *mp);
    static int validBlk(MprMem *mp);
#endif
#if BLD_MEMORY_STATS
    static MprFreeMem *getQueue(ssize size);
    static void printQueueStats();
    static void printGCStats();
#endif

/************************************* Code ***********************************/
/*
    Initialize the memory subsystem
 */
Mpr *mprCreateMemService(MprManager manager, int flags)
{
    MprRegion   *region;
    MprHeap     initHeap;
    MprMem      *mp, *spare;
    ssize       regionSize, size, mprSize;

    heap = &initHeap;
    memset(heap, 0, sizeof(MprHeap));
    heap->stats.maxMemory = MAXINT;
    heap->stats.redLine = MAXINT / 100 * 99;
    mprInitSpinLock(&heap->heapLock);
    initGen();

    /*
        Hand-craft the Mpr structure this include the MprRegion and MprMem headers
     */
    mprSize = MPR_ALLOC_ALIGN(sizeof(MprMem) + sizeof(Mpr) + (MANAGER_SIZE * sizeof(void*)));
    regionSize = MPR_ALLOC_ALIGN(sizeof(MprRegion));
    size = max(mprSize + regionSize, MPR_REGION_MIN_SIZE);

    if ((region = mprVirtAlloc(size, MPR_MAP_READ | MPR_MAP_WRITE)) == NULL) {
        return NULL;
    }
    mp = region->start = (MprMem*) (((char*) region) + regionSize);
    region->size = size;

    MPR = (Mpr*) GET_PTR(mp);
    INIT_BLK(mp, mprSize);
    mp->hasManager = 1;
    SET_MANAGER(mp, manager);
    mprSetName(MPR, "Mpr");

    spare = (MprMem*) (((char*) mp) + mprSize);
    INIT_BLK(spare, size - regionSize - mprSize);
    spare->last = 1;
    spare->prior = mp;

    heap = &MPR->heap;
    heap->notifier = (MprMemNotifier) memoryNotifier;
    heap->nextSeqno = 1;
    heap->chunkSize = MPR_REGION_MIN_SIZE;
    heap->stats.maxMemory = MAXINT;
    heap->stats.redLine = MAXINT / 100 * 99;
    heap->newQuota = MPR_NEW_QUOTA;
    heap->regions = region;
    heap->enabled = 1;
    heap->flags |= MPR_THREAD_PATTERN;
    heap->stats.bytesAllocated += size;
    INC(allocs);

    heap->stats.bytesAllocated += size;
    mprInitSpinLock(&heap->heapLock);
    mprInitSpinLock(&heap->heapLock2);
    mprInitSpinLock(&heap->rootLock);
    getSystemInfo();

    initFree();
    initGen();
    linkBlock(spare);

    heap->markerCond = mprCreateCond();
    heap->roots = mprCreateList();
    mprAddRoot(MPR);
    return MPR;
}


void mprDestroyMemService()
{
    MprRegion   *region;
    MprMem      *mp, *next;

    if (heap->destroying) {
        return;
    }
    heap->destroying = 1;
    for (region = heap->regions; region; region = region->next) {
        for (mp = region->start; mp; mp = next) {
            next = GET_NEXT(mp);
            if (unlikely(mp->hasManager)) {
                (GET_MANAGER(mp))(GET_PTR(mp), MPR_MANAGE_FREE);
            }
        }
    }
#if BLD_MEMORY_STATS && 0
    if (heap->enabled) {
        printGCStats();
    }
#endif
}


void *mprAllocBlock(ssize usize, int flags)
{
    MprMem      *mp;
    void        *ptr;
    ssize       size;
    int         padWords;

    int grew = 0;
    usize = max(usize, sizeof(MprFreeMem) - sizeof(MprMem));
    padWords = padding[flags & MPR_ALLOC_PAD_MASK];
    size = MPR_ALLOC_ALIGN(usize + sizeof(MprMem) + (padWords * sizeof(void*)));
    
    if ((mp = searchFree(size)) == NULL) {
        grew = 1;
        if ((mp = growHeap(size)) == NULL) {
            return NULL;
        }
    }
    BREAKPOINT(mp);
    mprAssert(mp->size >= size);
    ptr = GET_PTR(mp);
    if (flags & MPR_ALLOC_ZERO) {
        memset(ptr, 0, usize);
    }
    if (flags & MPR_ALLOC_MANAGER) {
        mp->hasManager = 1;
        *((MprManager*) PAD_PTR(mp, padWords)) = NULL;
    }
    mp->gen = heap->active;
    SET_SEQ(mp);
    CHECK(mp);
    BREAKPOINT(mp);
    return ptr;
}


void mprFree(void *ptr)
{
    MprMem  *mp;

    if (likely(ptr)) {
        mp = GET_MEM(ptr);
        CHECK(mp);
        mprAssert(!mp->free);
        if (unlikely(mp->isRoot)) {
            mprRemoveRoot(mp);
        }
        mp->gen = heap->stale;
        if (mp->size >= MPR_ALLOC_BIG) {
            heap->newCount += getQueueIndex(mp->size, 0);
        }
    }
}


void *mprRealloc(void *ptr, ssize usize)
{
    MprMem      *mp, *newb;
    void        *newptr;
    int         flags;

    mprAssert(usize > 0);

    if (ptr == 0) {
        return mprAllocBlock(usize, 0);
    }
    mp = GET_MEM(ptr);
    CHECK(mp);
    if (usize <= GET_USIZE(mp)) {
        return ptr;
    }
    flags = mp->hasManager ? MPR_ALLOC_MANAGER : 0;
    if ((newptr = mprAllocBlock(usize, flags)) == NULL) {
        return 0;
    }
    newb = GET_MEM(newptr);
    if (mp->hasManager) {
        SET_MANAGER(newb, GET_MANAGER(mp));
    }
    memcpy(newptr, ptr, mp->size - sizeof(MprMem));
    /* Preserves mprHold() */
    newb->gen = mp->gen;
#if UNUSED
    freeBlock(mp);
#endif
    return newptr;
}


void *mprMemdup(cvoid *ptr, ssize usize)
{
    char    *newp;

    if ((newp = mprAllocBlock(usize, 0)) != 0) {
        memcpy(newp, ptr, usize);
    }
    return newp;
}


int mprMemcmp(cvoid *s1, ssize s1Len, cvoid *s2, ssize s2Len)
{
    ssize       len;
    int         rc;

    mprAssert(s1);
    mprAssert(s2);
    mprAssert(s1Len >= 0);
    mprAssert(s2Len >= 0);

    len = min(s1Len, s2Len);

    rc = memcmp(s1, s2, len);
    if (rc == 0) {
        if (s1Len < s2Len) {
            return -1;
        } else if (s1Len > s2Len) {
            return 1;
        }
    }
    return rc;
}


/*
    Supports insitu copy where src and destination overlap
 */
ssize mprMemcpy(void *dest, ssize destMax, cvoid *src, ssize nbytes)
{
    mprAssert(dest);
    mprAssert(destMax <= 0 || destMax >= nbytes);
    mprAssert(src);
    mprAssert(nbytes >= 0);

    if (destMax > 0 && nbytes > destMax) {
        mprAssert(!MPR_ERR_WONT_FIT);
        return MPR_ERR_WONT_FIT;
    }
    if (nbytes > 0) {
        memmove(dest, src, nbytes);
        return nbytes;
    } else {
        return 0;
    }
}


/*
    Initialize the free space map and queues.

    The free map is a two dimensional array of free queues. The first dimension is indexed by
    the most significant bit (MSB) set in the requested block size. The second dimension is the next 
    MPR_ALLOC_BUCKET_SHIFT (4) bits below the MSB.

    +-------------------------------+
    |       |MSB|  Bucket   | rest  |
    +-------------------------------+
    | 0 | 0 | 1 | 1 | 1 | 1 | X | X |
    +-------------------------------+
 */
static int initFree() 
{
    MprFreeMem  *freeq;
#if BLD_MEMORY_STATS
    ssize       bit, size, groupBits, bucketBits;
    int         index, group, bucket;
#endif
    
    heap->freeEnd = &heap->freeq[MPR_ALLOC_NUM_GROUPS * MPR_ALLOC_NUM_BUCKETS];
    for (freeq = heap->freeq; freeq != heap->freeEnd; freeq++) {
#if BLD_MEMORY_STATS
        /*
            NOTE: skip the buckets with MSB == 0 (round up)
         */
        index = (int) (freeq - heap->freeq);
        group = index / MPR_ALLOC_NUM_BUCKETS;
        bucket = index % MPR_ALLOC_NUM_BUCKETS;

        bit = (group != 0);
        groupBits = bit << (group + MPR_ALLOC_BUCKET_SHIFT - 1);
        bucketBits = ((ssize) bucket) << (max(0, group - 1));

        size = groupBits | bucketBits;
        freeq->info.stats.minSize = (int) (size << MPR_ALIGN_SHIFT);
#endif
        freeq->next = freeq->prev = freeq;
    }
    return 0;
}


static void initGen()
{
    heap->eternal = MPR_GEN_ETERNAL;
    heap->active = heap->eternal - 1;
    heap->stale = heap->active - 1;
    heap->dead = heap->stale - 1;
}


static void nextGen() 
{
    int     active;

    active = (heap->active + 1) % MPR_MAX_GEN;
    heap->active = active;
    heap->stale = (active - 1 + MPR_MAX_GEN) % MPR_MAX_GEN;
    heap->dead = (active - 2 + MPR_MAX_GEN) % MPR_MAX_GEN;
}


static int getQueueIndex(ssize size, int roundup)
{   
    ssize       usize, asize;
    int         aligned, bucket, group, index, msb;
    
    mprAssert(MPR_ALLOC_ALIGN(size) == size);

    /*
        Allocate based on user sizes (sans header). This permits block searches to avoid scanning the next 
        highest queue for common block sizes: eg. 1K.
     */
    usize = (size - sizeof(MprMem));
    asize = usize >> MPR_ALIGN_SHIFT;

    /* Zero based most significant bit */
    msb = (flsl((int) asize) - 1);

    group = max(0, msb - MPR_ALLOC_BUCKET_SHIFT + 1);
    mprAssert(group < MPR_ALLOC_NUM_GROUPS);

    bucket = (asize >> max(0, group - 1)) & (MPR_ALLOC_NUM_BUCKETS - 1);
    mprAssert(bucket < MPR_ALLOC_NUM_BUCKETS);

    index = (group * MPR_ALLOC_NUM_BUCKETS) + bucket;
    mprAssert(index < (heap->freeEnd - heap->freeq));
    
#if BLD_MEMORY_STATS
    mprAssert(heap->freeq[index].info.stats.minSize <= (int) usize && 
        (int) usize < heap->freeq[index + 1].info.stats.minSize);
#endif
    
    if (roundup) {
        /*
            Good-fit strategy: check if the requested size is the smallest possible size in a queue. If not the smallest,
            must look at the next queue higher up to guarantee a block of sufficient size.
            Blocks of of size <= 512 bytes (0x20 shifted) are mapped directly to queues. ie. There is only one block size
            per queue. Otherwise, get a mask of the bits below the group and bucket bits. If any are set, then not the 
            lowest size in the queue.
         */
        if (asize > 0x20) {
            ssize mask = (((ssize) 1) << (msb - MPR_ALLOC_BUCKET_SHIFT)) - 1;
            aligned = (asize & mask) == 0;
            if (!aligned) {
                index++;
            }
        }
    }
    return index;
}


#if BLD_MEMORY_STATS
static MprFreeMem *getQueue(ssize size)
{   
    MprFreeMem  *freeq;
    int         index;
    
    index = getQueueIndex(size, 0);
    freeq = &heap->freeq[index];
    return freeq;
}
#endif


static MprMem *searchFree(ssize required)
{
    MprFreeMem  *freeq, *fp;
    MprMem      *mp, *after, *spare;
//  MOB
MprMem *next, *prev;    
    ssize       size, maxBlock;
    ulong       groupMap, bucketMap;
    int         bucket, baseGroup, group, index;
    
    index = getQueueIndex(required, 1);
    baseGroup = index / MPR_ALLOC_NUM_BUCKETS;
    bucket = index % MPR_ALLOC_NUM_BUCKETS;
    heap->newCount += index;

    lockHeap();
    INC(requests);
    
    /* Mask groups lower than the base group */
    groupMap = heap->groupMap & ~((((ssize) 1) << baseGroup) - 1);
    while (groupMap) {
        group = (int) (ffsl(groupMap) - 1);
        if (groupMap & ((((ssize) 1) << group))) {
            bucketMap = heap->bucketMap[group];
            if (baseGroup == group) {
                bucketMap &= ~((((ssize) 1) << bucket) - 1);
            }
            while (bucketMap) {
                bucket = (int) (ffsl(bucketMap) - 1);
                index = (group * MPR_ALLOC_NUM_BUCKETS) + bucket;
                freeq = &heap->freeq[index];
                if (freeq->next != freeq) {
                    fp = freeq->next;
                    mp = (MprMem*) fp;
mprAssert(mp->free);
                    unlinkBlock(fp);
mprAssert(!mp->free);
                    INC(reuse);
mprAssert(mp->gen == heap->eternal);
                    CHECK(mp);
                    
                    if (mp->size >= (ssize) (required + MPR_ALLOC_MIN_SPLIT)) {
                        //  MOB -- what is this trying to do?
                        maxBlock = (((ssize) 1 ) << group | (((ssize) bucket) << (max(0, group - 1)))) << MPR_ALIGN_SHIFT;
                        maxBlock += sizeof(MprMem);

                        if (mp->size > maxBlock) {
                            size = mp->size;
                            spare = (MprMem*) ((char*) mp + required);
                            INIT_BLK(spare, size - required);
                            spare->last = mp->last;
                            spare->prior = mp;
                            if ((after = GET_NEXT(spare)) != NULL) {
                                after->prior = spare;
                            }
                            mp->last = 0;
                            mp->size = required;
                            INC(splits);
mprAssert(spare->gen != heap->dead);
                            linkBlock(spare);
                            
                            mp = (MprMem*) fp;
                            next = GET_NEXT(mp);
                            prev = mp->prior;
                            mprAssert(next->free);
                            mprAssert(prev == 0 || !prev->free);
                            
                        } else {
                            
                            MprMem *mp, *next, *prev;    
                            mp = (MprMem*) fp;
                            next = GET_NEXT(mp);
                            prev = mp->prior;
                            mprAssert(next == 0 || !next->free);
                            mprAssert(prev == 0 || !prev->free);
                        }
                    } else {
                        
                        MprMem *mp, *next, *prev;    
                        mp = (MprMem*) fp;
                        next = GET_NEXT(mp);
                        prev = mp->prior;
                        mprAssert(next == 0 || !next->free);
                        mprAssert(prev == 0 || !prev->free);
                    }
                    unlockHeap();
                    return mp;
                }
                bucketMap &= ~(((ssize) 1) << bucket);
                heap->bucketMap[group] &= ~(((ssize) 1) << bucket);
            }
            groupMap &= ~(((ssize) 1) << group);
            heap->groupMap &= ~(((ssize) 1) << group);
            /*
                Put GC here so it is no in the common path above where the block can be satisfied from the free list
             */
            unlockHeap();
            if (!heap->requested && heap->newCount > heap->newQuota) {
                heap->requested = 1;
                mprSignalCond(heap->markerCond);
            }
            lockHeap();
        }
    }
    unlockHeap();
    if (!heap->requested && heap->newCount > heap->newQuota) {
        heap->requested = 1;
        mprSignalCond(heap->markerCond);
    }
    return NULL;
}


#if BLD_CC_MMU && BLD_MEMORY_DEBUG
static int isFirst(MprMem *mp)
{
    MprRegion   *region;

    for (region = heap->regions; region; region = region->next) {
        if (region->start == mp) {
            return 1;
        }
    }
    return 0;
}
#endif


/*
    Called by the sweeper only
 */
static MprMem *freeBlock(MprMem *mp)
{
    MprMem      *prev, *next, *after;
    ssize       size;
#if BLD_CC_MMU
    MprRegion   *region;
#endif
    int a, b;
    
    a = b = 0;
    BREAKPOINT(mp);

    size = mp->size;
    after = next = prev = NULL;
    
    /*
        Coalesce with next if it is free
     */
    lockHeap();
    next = GET_NEXT(mp);
    if (next && next->free) {
        a = 1;
        BREAKPOINT(next);
        unlinkBlock((MprFreeMem*) next);
        if ((after = GET_NEXT(next)) != NULL) {
            mprAssert(after->prior == next);
            after->prior = mp;
        } else {
            mp->last = 1;
        }
        size += next->size;
        mp->size = size;
        // printf("  JOIN NEXT %x size %d\n", next, size);
        INC(joins);
        next = GET_NEXT(mp);
        if (next) CHECK(next);
        mprAssert(next == 0 || !next->free);
    }
    /*
        Coalesce with previous if it is free
     */
    prev = mp->prior;
    if (prev && prev->free) {
        b = 1;
        BREAKPOINT(prev);
        unlinkBlock((MprFreeMem*) prev);
        if ((after = GET_NEXT(mp)) != NULL) {
            mprAssert(after->prior == mp);
            after->prior = prev;
        } else {
            prev->last = 1;
        }
        size += prev->size;
        prev->size = size;
        // printf("  JOIN PREV %x size %d\n", prev, size);
        mp = prev;
        INC(joins);
        prev = mp->prior;
        if (prev) CHECK(prev);
        mprAssert(prev == 0 || !prev->free);
    }
    next = GET_NEXT(mp);

#if BLD_MEMORY_DEBUG
    if (mp->prior == NULL) {
        mprAssert(isFirst(mp) || mp->seqno == 0);
    }
    /* Next block must never be free - should all be coalesced */
    mprAssert(next == 0 || !next->free);
#endif

    /*
        Release entire regions back to the O/S. (Blocks equal to Empty regions have no prior and are last)
     */
#if BLD_CC_MMU
    if (mp->prior == NULL && mp->last && heap->stats.bytesFree > (MPR_REGION_MIN_SIZE * 4)) {
        INC(unpins);
        unlockHeap();
        region = GET_REGION(mp);
        region->freeable = 1;
    } else
#endif
    {
        linkBlock(mp);
        // RESET_MEM(mp);
        unlockHeap();
    }
    /*
        WARN: there is a race here. Another thread may allocate and split the block just freed. So next will be
        pessimistic and there may be newly created intervening blocks.
     */
    return next;
}


/*
    Grow the heap and return a block of the required size (unqueued)
 */
static MprMem *growHeap(ssize required)
{
    MprRegion   *region;
    MprMem      *mp, *spare;
//  MOB
MprMem *next, *prev;    
    ssize       size, rsize;

    mprAssert(required > 0);

    rsize = MPR_ALLOC_ALIGN(sizeof(MprRegion));
    size = max(required + rsize, (ssize) heap->chunkSize);
    size = MPR_PAGE_ALIGN(size, heap->pageSize);
#if FUTURE && KEEP && MOB
    if (size >= MPR_ALLOC_MAX_BLOCK) {
        mprAssert(size < MPR_ALLOC_MAX_BLOCK);
        return 0;
    }
#endif
    if ((region = mprVirtAlloc(size, MPR_MAP_READ | MPR_MAP_WRITE)) == NULL) {
        return 0;
    }
    mprInitSpinLock(&region->lock);
    region->size = size;
    region->start = (MprMem*) (((char*) region) + rsize);
    mp = (MprMem*) region->start;
    INIT_BLK(mp, required);
    CHECK(mp);

    spare = (MprMem*) ((char*) mp + required);
    INIT_BLK(spare, size - required - rsize);
    spare->last = 1;
    spare->prior = mp;
    CHECK(spare);

    /*
        RACE: Put region onto the front of the regions list. Racing with sweep().
     */
    lockHeap();
    region->next = heap->regions;
    heap->regions = region;
    heap->stats.bytesAllocated += size;
    INC(allocs);
    linkBlock(spare);
    
    next = GET_NEXT(mp);
    prev = mp->prior;
    mprAssert(mp->last || next->free);
    mprAssert(prev == 0);
    
    unlockHeap();
    return mp;
}


/*
    Add a block to a free q. Must be called locked.
 */
static void linkBlock(MprMem *mp) 
{
    MprFreeMem  *freeq, *fp;
//  MOB
    MprMem *next, *prev;    
    int         index, group, bucket;

    CHECK(mp);

    mp->dynamic = 0;
    mp->visited = 0;
    mp->builtin = 0;
    mp->isRoot = 0;
    mp->hasManager = 0;

    /* 
        Mark block as free and eternal so sweeper will skip 
     */
    mp->gen = heap->eternal;
    mp->mark = heap->eternal;
    mp->free = 1;
    
    /*
        Set free space bitmap
     */
    index = getQueueIndex(mp->size, 0);
    group = index / MPR_ALLOC_NUM_BUCKETS;
    bucket = index % MPR_ALLOC_NUM_BUCKETS;
    heap->groupMap |= (((ssize) 1) << group);
    heap->bucketMap[group] |= (((ssize) 1) << bucket);

    /*
        Link onto free queue
     */
    fp = (MprFreeMem*) mp;
    freeq = &heap->freeq[index];
    mprAssert(fp != freeq);
    fp->next = freeq->next;
    fp->prev = freeq;
    freeq->next->prev = fp;
    freeq->next = fp;
    mprAssert(fp != fp->next);
    mprAssert(fp != fp->prev);

    heap->stats.bytesFree += mp->size;
#if BLD_MEMORY_STATS
    freeq->info.stats.count++;
#endif

    next = GET_NEXT(mp);
    prev = mp->prior;
    mprAssert(next == 0 || !next->free);
    mprAssert(prev == 0 || !prev->free);
    mprAssert(mp->free);
    mprAssert(mp->gen == heap->eternal);
}


/*
    Remove a block from a free q. Must be called locked.
 */
static void unlinkBlock(MprFreeMem *fp) 
{
    MprMem  *mp;

    CHECK(fp);
    fp->prev->next = fp->next;
    fp->next->prev = fp->prev;
#if BLD_MEMORY_DEBUG
    fp->next = fp->prev = NULL;
#endif

    mp = (MprMem*) fp;
    heap->stats.bytesFree -= mp->size;
    mprAssert(mp->free);
    mp->free = 0;
#if BLD_MEMORY_STATS
{
    MprFreeMem *freeq = getQueue(mp->size);
    freeq->info.stats.count--;
    mprAssert(freeq->info.stats.count >= 0);
}
#endif
}


static void allocException(ssize size, bool granted)
{
    heap->hasError = 1;

    lockHeap();
    INC(errors);
    if (heap->stats.inMemException) {
        unlockHeap();
        return;
    }
    heap->stats.inMemException = 1;
    unlockHeap();

    if (heap->notifier) {
        (heap->notifier)(granted ? MPR_MEM_LOW : MPR_MEM_DEPLETED, size);
    }
    heap->stats.inMemException = 0;

    if (!granted) {
        switch (heap->allocPolicy) {
        case MPR_ALLOC_POLICY_EXIT:
            mprError("Application exiting due to memory allocation failure.");
            mprTerminate(0);
            break;
        case MPR_ALLOC_POLICY_RESTART:
            mprError("Application restarting due to memory allocation failure.");
            //  TODO - Other systems
#if BLD_UNIX_LIKE
            execv(MPR->argv[0], MPR->argv);
#endif
            break;
        }
    }
}


/*
    Allocate virtual memory and check a memory allocation request against configured maximums and redlines. 
    Do this so that the application does not need to check the result of every little memory allocation. Rather, 
    an application-wide memory allocation failure can be invoked proactively when a memory redline is exceeded. 
    It is the application's responsibility to set the red-line value suitable for the system.
 */
void *mprVirtAlloc(ssize size, int mode)
{
    ssize       used;
    void        *ptr;

    used = mprGetMem();
    if (heap->pageSize) {
        size = MPR_PAGE_ALIGN(size, heap->pageSize);
    }
    if ((size + used) > heap->stats.maxMemory) {
        allocException(size, 0);
        /* Prevent allocation as over the maximum memory limit.  */
        return NULL;

    } else if ((size + used) > heap->stats.redLine) {
        /* Warn if allocation puts us over the red line. Then continue to grant the request.  */
        allocException(size, 1);
    }
#if BLD_CC_MMU
    #if BLD_UNIX_LIKE
        ptr = mmap(0, size, mode, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (ptr == (void*) -1) {
            ptr = 0;
        }
    #elif BLD_WIN_LIKE
        ptr = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, winPageModes(mode));
    #else
        ptr = malloc(size);
    #endif
#else
    ptr = malloc(size);
#endif
    if (ptr == NULL) {
        allocException(size, 0);
        return 0;
    }
    return ptr;
}


void mprVirtFree(void *ptr, ssize size)
{
#if BLD_CC_MMU
    #if BLD_UNIX_LIKE
        if (munmap(ptr, size) != 0) {
            mprAssert(0);
        }
    #elif BLD_WIN_LIKE
        VirtualFree(ptr, 0, MEM_RELEASE);
    #else
        free(ptr);
    #endif
#else
    free(ptr);
#endif
    heap->stats.bytesAllocated -= size;
    mprAssert(heap->stats.bytesAllocated >= 0);
}


static void getSystemInfo()
{
    MprMemStats   *ap;

    ap = &heap->stats;
    ap->numCpu = 1;

#if MACOSX
    #ifdef _SC_NPROCESSORS_ONLN
        ap->numCpu = sysconf(_SC_NPROCESSORS_ONLN);
    #else
        ap->numCpu = 1;
    #endif
    ap->pageSize = sysconf(_SC_PAGESIZE);
#elif SOLARIS
{
    FILE *ptr;
    if  ((ptr = popen("psrinfo -p", "r")) != NULL) {
        fscanf(ptr, "%d", &alloc.numCpu);
        (void) pclose(ptr);
    }
    alloc.pageSize = sysconf(_SC_PAGESIZE);
}
#elif BLD_WIN_LIKE
{
    SYSTEM_INFO     info;

    GetSystemInfo(&info);
    ap->numCpu = info.dwNumberOfProcessors;
    ap->pageSize = info.dwPageSize;

}
#elif FREEBSD
    {
        int     cmd[2];
        ssize   len;

        /*
            Get number of CPUs
         */
        cmd[0] = CTL_HW;
        cmd[1] = HW_NCPU;
        len = sizeof(ap->numCpu);
        if (sysctl(cmd, 2, &ap->numCpu, &len, 0, 0) < 0) {
            ap->numCpu = 1;
        }

        /*
            Get page size
         */
        ap->pageSize = sysconf(_SC_PAGESIZE);
    }
#elif LINUX
    {
        static const char processor[] = "processor\t:";
        char    c;
        int     fd, col, match;

        fd = open("/proc/cpuinfo", O_RDONLY);
        if (fd < 0) {
            return;
        }
        match = 1;
        for (col = 0; read(fd, &c, 1) == 1; ) {
            if (c == '\n') {
                col = 0;
                match = 1;
            } else {
                if (match && col < (sizeof(processor) - 1)) {
                    if (c != processor[col]) {
                        match = 0;
                    }
                    col++;

                } else if (match) {
                    ap->numCpu++;
                    match = 0;
                }
            }
        }
        --ap->numCpu;
        close(fd);
        ap->pageSize = sysconf(_SC_PAGESIZE);
    }
#else
        ap->pageSize = 4096;
#endif
    if (ap->pageSize <= 0 || ap->pageSize >= (16 * 1024)) {
        ap->pageSize = 4096;
    }
    heap->pageSize = ap->pageSize;
}


#if BLD_WIN_LIKE
static int winPageModes(int flags)
{
    if (flags & MPR_MAP_EXECUTE) {
        return PAGE_EXECUTE_READWRITE;
    } else if (flags & MPR_MAP_WRITE) {
        return PAGE_READWRITE;
    }
    return PAGE_READONLY;
}
#endif


MprMemStats *mprGetMemStats()
{
#if LINUX
    char            buf[1024], *cp;
    size_t          len;
    int             fd;

    heap->stats.ram = MAXSSIZE;
    if ((fd = open("/proc/meminfo", O_RDONLY)) >= 0) {
        if ((len = read(fd, buf, sizeof(buf) - 1)) > 0) {
            buf[len] = '\0';
            if ((cp = strstr(buf, "MemTotal:")) != 0) {
                for (; *cp && !isdigit((int) *cp); cp++) {}
                heap->stats.ram = ((ssize) atoi(cp) * 1024);
            }
        }
        close(fd);
    }
#endif
#if MACOSX || FREEBSD
    ssize       ram, usermem;
    size_t      len;
    int         mib[2];

    mib[0] = CTL_HW;
#if FREEBSD
    mib[1] = HW_MEMSIZE;
#else
    mib[1] = HW_PHYSMEM;
#endif
    len = sizeof(ram);
    sysctl(mib, 2, &ram, &len, NULL, 0);
    heap->stats.ram = ram;

    mib[0] = CTL_HW;
    mib[1] = HW_USERMEM;
    len = sizeof(usermem);
    sysctl(mib, 2, &usermem, &len, NULL, 0);
    heap->stats.user = usermem;
#endif
    heap->stats.rss = mprGetMem();
    return &heap->stats;
}


ssize mprGetMem()
{
#if LINUX || MACOSX || FREEBSD
    struct rusage   rusage;
    getrusage(RUSAGE_SELF, &rusage);
    return rusage.ru_maxrss;
#else
    return heap->stats.bytesAllocated;
#endif
}


#if NEED_FFSL
#if USE_FFSL_ASM_X86

static MPR_INLINE int ffsl(ulong x)
{
    long    r;

    asm("bsf %1,%0\n\t"
        "jnz 1f\n\t"
        "mov $-1,%0\n"
        "1:" : "=r" (r) : "rm" (x));
    return (int) r + 1;
}


static MPR_INLINE int flsl(ulong x)
{
    long r;

    asm("bsr %1,%0\n\t"
        "jnz 1f\n\t"
        "mov $-1,%0\n"
        "1:" : "=r" (r) : "rm" (x));
    return (int) r + 1;
}
#else /* USE_FFSL_ASM_X86 */ 


/* 
    Find first bit set in word 
 */
static MPR_INLINE int ffsl(ulong word)
{
    int     b;

    for (b = 0; word; word >>= 1, b++) {
        if (word & 0x1) {
            b++;
            break;
        }
    }
    return b;
}


/* 
    Find last bit set in word 
 */
static MPR_INLINE int flsl(ulong word)
{
    int     b;

    for (b = 0; word; word >>= 1, b++) ;
    return b;
}
#endif /* !USE_FFSL_ASM_X86 */
#endif /* NEED_FFSL */


#if BLD_MEMORY_STATS
static void printQueueStats() 
{
    MprFreeMem  *freeq;
    int         i, index;

    printf("\nFree Queue Stats\n Bucket                     Size   Count\n");
    for (i = 0, freeq = heap->freeq; freeq != heap->freeEnd; freeq++, i++) {
        index = (int) (freeq - heap->freeq);
        printf("%7d %24d %7d\n", i, freeq->info.stats.minSize, freeq->info.stats.count);
    }
}


static void printGCStats()
{
    MprRegion   *region;
    MprMem      *mp;
    ssize       bytes[MPR_MAX_GEN + 2];
    int         regionCount, i, freeCount, allocatedCount, counts[MPR_MAX_GEN + 2], free;

    for (i = 0; i < (MPR_MAX_GEN + 2); i++) {
        counts[i] = 0;
        bytes[i] = 0;
    }
    printf("\nRegion Stats\n");
    regionCount = 0;
    free = heap->eternal + 1;
    for (region = heap->regions; region; region = region->next) {
        freeCount = allocatedCount = 0;
        for (mp = region->start; mp; mp = GET_NEXT(mp)) {
            if (mp->free) {
                freeCount++;
                counts[free]++;
                bytes[free] += mp->size;
            } else {
                counts[mp->gen]++;
                bytes[mp->gen] += mp->size;
                allocatedCount++;
            }
        }
        regionCount++;
        printf("  Region %d is %d bytes, has %d allocated %d free\n", i, (int) region->size, allocatedCount, freeCount);
    }
    printf("Regions: %d\n", regionCount);

    printf("\nGC Stats\n");
    printf("  Eternal generation has %9d blocks, %12d bytes\n", counts[heap->eternal], (int) bytes[heap->eternal]);
    printf("  Stale generation has   %9d blocks, %12d bytes\n", counts[heap->stale], (int) bytes[heap->stale]);
    printf("  Active generation has  %9d blocks, %12d bytes\n", counts[heap->active], (int) bytes[heap->active]);
    printf("  Dead generation has    %9d blocks, %12d bytes\n", counts[heap->dead], (int) bytes[heap->dead]);
    printf("  Free generation has    %9d blocks, %12d bytes\n", counts[free], (int) bytes[free]);
}
#endif /* BLD_MEMORY_STATS */


void mprPrintMem(cchar *msg, int detail)
{
#if BLD_MEMORY_STATS
    MprMemStats   *ap;

    ap = mprGetMemStats();

    printf("\n\nMPR Memory Report %s\n", msg);
    printf("------------------------------------------------------------------------------------------\n");
    printf("  Total memory        %14d K\n",             (int) mprGetMem());
    printf("  Current heap memory %14d K\n",             (int) (ap->bytesAllocated / 1024));
    printf("  Free heap memory    %14d K\n",             (int) (ap->bytesFree / 1024));
    printf("  Allocation errors   %14d\n",               ap->errors);
    printf("  Memory limit        %14d MB (%d %%)\n",    (int) (ap->maxMemory / (1024 * 1024)),
       percent(ap->bytesAllocated / 1024, ap->maxMemory / 1024));
    printf("  Memory redline      %14d MB (%d %%)\n",    (int) (ap->redLine / (1024 * 1024)),
       percent(ap->bytesAllocated / 1024, ap->redLine / 1024));

    printf("  Memory requests     %14Ld\n",              ap->requests);
    printf("  O/S allocations     %14d %%\n",            percent(ap->allocs, ap->requests));
    printf("  Block unpinns       %14d %%\n",            percent(ap->unpins, ap->requests));
    printf("  Block reuse         %14d %%\n",            percent(ap->reuse, ap->requests));
    printf("  Joins               %14d %%\n",            percent(ap->joins, ap->requests));
    printf("  Splits              %14d %%\n",            percent(ap->splits, ap->requests));

    printGCStats();
    if (detail) {
        printQueueStats();
    }
#endif /* BLD_MEMORY_STATS */
}


#if BLD_MEMORY_DEBUG
static int validBlk(MprMem *mp)
{
    mprAssert(mp->magic == MPR_ALLOC_MAGIC);
    mprAssert(mp->size > 0);
    return (mp->magic == MPR_ALLOC_MAGIC) && (mp->size > 0);
}


void mprCheckBlock(MprMem *mp)
{
    char    msg[80];

    if (mp->magic != MPR_ALLOC_MAGIC || mp->size <= 0) {
        mprSprintf(msg, sizeof(msg), 
            "Memory corruption in memory at %x (MprBlk %x, seqno %d)\n"
            "This most likely happend earlier in the program execution", GET_PTR(mp), mp, mp->seqno);
        mprAssertError(NULL, msg);
    }
}


static void breakpoint(MprMem *mp) 
{
    if (mp == stopAlloc || mp->seqno == stopSeqno) {
        mprBreakpoint();
    }
}
#endif

/***************************************************** Garbage Colllector *************************************************/

void mprStartGCService()
{
    if (heap->flags & MPR_MARK_THREAD) {
        mprLog(7, "DEBUG: startMemWorkers: start marker");
        if ((heap->marker = mprCreateThread("marker", marker, NULL, 0)) == 0) {
            mprError("Can't create marker thread");
            MPR->hasError = 1;
        } else {
            mprStartThread(heap->marker);
        }
    }
    if (heap->flags & MPR_SWEEP_THREAD) {
        mprLog(7, "DEBUG: startMemWorkers: start sweeper");
        heap->hasSweeper = 1;
        if ((heap->sweeper = mprCreateThread("sweeper", sweeper, NULL, 0)) == 0) {
            mprError("Can't create sweeper thread");
            MPR->hasError = 1;
        } else {
            mprStartThread(heap->sweeper);
        }
    }
}


void mprStopGCService()
{
    mprSignalCond(heap->markerCond);
}


void mprRequestGC(int complete)
{
    mprLog(7, "DEBUG: mprRequestGC");
    if (complete) {
        heap->sweeps = 3;
    }
    mprSignalCond(heap->markerCond);
    mprYield(NULL, 0);
}


/*
    Marker synchronization point. At the end of each GC mark/sweep, all threads must rendezvous at the 
    synchronization point.  This happens infrequently and is essential to safely move to a new generation.
    All threads must yield to the marker (including sweeper)
 */
static void synchronize()
{
    mprLog(4, "DEBUG: synchronize GC");

    heap->mustYield = 1;
    if (heap->notifier) {
        mprLog(4, "DEBUG: Call notifier");
        (heap->notifier)(MPR_MEM_YIELD, 0);
    }
    if (!mprIsExiting()) {
        if (mprSyncThreads(MPR_TIMEOUT_GC_SYNC)) {
            mprLog(7, "DEBUG: GC Advance generation");
            nextGen();
            heap->mustYield = 0;
            mprResumeThreads();
        } else {
            mprLog(7, "DEBUG: Pause for GC sync timed out");
            heap->mustYield = 0;
        }
    }
}


static void mark()
{
    ssize     priorFree;

    heap->requested = 0;
    heap->newCount = 0;
    priorFree = heap->stats.bytesFree;

    mprLog(1, "DEBUG: mark started");
    markRoots();
    if (!heap->hasSweeper) {
        sweep();
    }
#if BLD_MEMORY_STATS
    mprLog(5, "GC Complete: MARKED %d/%d, SWEPT %d/%d, bytesFree %d (before %d)\n", 
        heap->stats.marked, heap->stats.markVisited, 
        heap->stats.swept, heap->stats.sweepVisited, (int) heap->stats.bytesFree, priorFree);
#endif
    synchronize();
}


static void sweep()
{
    MprRegion   *region, *nextRegion, *prior;
    MprMem      *mp, *next;
//  MOB
        MprMem *prev = NULL;
    ssize       total;
    
    if (!heap->enabled) {
        mprLog(7, "DEBUG: sweep: Abort sweep - GC disabled");
        return;
    }
    mprLog(1, "DEBUG: sweep started");

    /*
        Run all destructors so all can guarantee dependant memory blocks will still exist
     */
    for (region = heap->regions; region; region = region->next) {
        for (mp = region->start; mp; mp = GET_NEXT(mp)) {
            if (unlikely(mp->gen == heap->dead && mp->hasManager)) {
                mprAssert(!mp->free);
                CHECK(mp);
                BREAKPOINT(mp);
                (GET_MANAGER(mp))(GET_PTR(mp), MPR_MANAGE_FREE);
            }
        }
    }
    heap->stats.sweepVisited = 0;
    heap->stats.swept = 0;
    total = 0;

    /*
        growHeap() will append new regions to the front of heap->regions and so will not race with this code. This code
        is the only code that frees regions.
        RACE: Take from the front. Racing with growHeap.
     */
    prior = NULL;
    for (region = heap->regions; region; region = nextRegion) {
        nextRegion = region->next;
        prev = NULL;
        for (mp = region->start; mp; mp = next) {
            CHECK(mp);
            INC(sweepVisited);
            if (unlikely(mp->gen == heap->dead)) {
                int f = mp->free;
                mprAssert(!mp->free);
                CHECK(mp);
                BREAKPOINT(mp);
                INC(swept);
                total += mp->size;
                next = freeBlock(mp);
            } else {
                /*
                    RACE: Block could be allocated here, but will never be coalesced (sweeper is the only one to do that).
                    So mp->size may be reduced so we may skip a newly created block -- no problem. Get it next scan.
                 */
                next = GET_NEXT(mp);
            }
            prev = mp;
        }
        if (region->freeable) {
            if (prior) {
                prior->next = nextRegion;
            } else {
                heap->regions = nextRegion;
            }
            mprVirtFree(region, region->size);
        } else {
            prior = region;
        }
    }
    mprLog(7, "DEBUG: sweep swept %,d", total);
}


static void markRoots()
{
    void    *root;
    int     index, scans;

    heap->stats.markVisited = 0;
    heap->stats.marked = 0;
    heap->rescanRoots = 0;

    scans = 0;
    do {
        for (index = 0; (root = getNextRoot(&index)) != 0; ) {
            mprMark(root);
        }
    } while (heap->rescanRoots && ++scans < 2);
    mprMark(heap->roots);
}


void mprMarkBlock(cvoid *ptr)
{
    MprMem      *mp;

    mprAssert(ptr);
    mp = MPR_GET_MEM(ptr);
    CHECK(mp);
    INC(markVisited);

    if (mp->mark != heap->active) {
        BREAKPOINT(mp);
        INC(marked);
        mp->mark = heap->active;
        if (mp->gen != heap->eternal) {
            mp->gen = heap->active;
        }
        if (mp->hasManager) {
            (GET_MANAGER(mp))((void*) ptr, MPR_MANAGE_MARK);
        }
    }
}


void mprHold(void *ptr)
{
    if (ptr) {
        GET_MEM(ptr)->gen = heap->eternal;
    }
}


void mprRelease(void *ptr)
{
    MprMem  *mp;

    if (ptr) {
        mp = GET_MEM(ptr);
        if (!mp->free) {
            mp->gen = heap->active;
        }
    }
}


/*
    Marker thread main program
 */
static void marker(void *unused, MprThread *tp)
{
    mprLog(2, "DEBUG: marker thread started");
    mprStickyYield(NULL, 1);

    while (!mprIsExiting()) {
        if (heap->sweeps <= 0) {
            mprWaitForCond(heap->markerCond, -1);
        } else {
            --heap->sweeps;
        }
        mark();
    }
}


/*
    Sweeper thread main program. May be called from the marker thread.
 */
static void sweeper(void *unused, MprThread *tp) 
{
    mprLog(2, "DEBUG: sweeper thread started");

    while (!mprIsExiting()) {
        sweep();
        mprYield(NULL, 1);
    }
}


bool mprEnableGC(bool on)
{
    bool    old;

    old = heap->enabled;
    heap->enabled = on;
    return old;
}


int mprWaitForSync()
{
    return heap->mustYield;
}


void mprAddRoot(void *root)
{
    mprSpinLock(&heap->rootLock);
    mprAddItem(heap->roots, root);
    GET_MEM(root)->isRoot = 1;
    mprSpinUnlock(&heap->rootLock);
}


void mprRemoveRoot(void *root)
{
    mprSpinLock(&heap->rootLock);
    mprRemoveItem(heap->roots, root);
    heap->rescanRoots = 1;
    GET_MEM(root)->isRoot = 0;
    mprSpinUnlock(&heap->rootLock);
}


static void *getNextRoot(int *indexp)
{
    void    *root;

    mprSpinLock(&heap->rootLock);
    root = mprGetNextItem(heap->roots, indexp);
    mprSpinUnlock(&heap->rootLock);
    return root;
}


#ifndef mprSetName
void *mprSetName(void *ptr, cchar *name) 
{
    MPR_GET_MEM(ptr)->name = name;
    return ptr;
}
#endif


/*
    Default memory handler
 */
static int memoryNotifier(int flags, ssize size)
{
    if (flags & MPR_MEM_DEPLETED) {
        mprPrintfError("Can't allocate memory block of size %d\n", size);
        mprPrintfError("Total memory used %d\n", mprGetMem());
        exit(255);

    } else if (flags & MPR_MEM_LOW) {
        mprPrintfError("Memory request for %d bytes exceeds memory red-line\n", size);
        mprPrintfError("Total memory used %d\n", mprGetMem());
    }
    return 0;
}


#if BLD_WIN_LIKE
Mpr *mprGetMpr()
{
    return MPR;
}
#endif


int mprGetPageSize()
{
    return heap->pageSize;
}


ssize mprGetBlockSize(cvoid *ptr)
{
    MprMem      *mp;

    mp = GET_MEM(ptr);
    if (ptr == 0 || !VALID_BLK(mp)) {
        return 0;
    }
    return GET_USIZE(mp);
}


void mprSetMemNotifier(MprMemNotifier cback)
{
    heap->notifier = cback;
}


void mprSetMemLimits(ssize redLine, ssize maxMemory)
{
    if (redLine > 0) {
        heap->stats.redLine = redLine;
    }
    if (maxMemory > 0) {
        heap->stats.maxMemory = maxMemory;
    }
}


void mprSetMemPolicy(int policy)
{
    heap->allocPolicy = policy;
}


void mprSetMemError()
{
    heap->hasError = 1;
}


bool mprHasMemError()
{
    return heap->hasError;
}


void mprResetMemError()
{
    heap->hasError = 0;
}


int mprIsValid(cvoid *ptr)
{
    return ptr && VALID_BLK(GET_MEM(ptr));
}


static int dummyManager(void *ptr, int flags) { 
    return 0; 
}


void *mprSetManager(void *ptr, void *manager)
{
    MprMem      *mp;

    mp = GET_MEM(ptr);
    mprAssert(mp->hasManager);
    if (mp->hasManager) {
        if (!manager) {
            manager = dummyManager;
        }
        SET_MANAGER(mp, manager);
    }
    return ptr;
}


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

