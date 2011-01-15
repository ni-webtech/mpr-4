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

#define GET_MEM(ptr)                ((MprMem*) (((char*) (ptr)) - sizeof(MprMem)))
#define GET_PTR(mp)                 ((char*) (((char*) mp) + sizeof(MprMem)))
#define GET_USIZE(mp)               ((ssize) (GET_SIZE(mp) - sizeof(MprMem) - (HAS_MANAGER(mp) * sizeof(void*))))
#define UNMARKED                    MPR_GEN_ETERNAL

#if BLD_FEATURE_VALLOC
    #define GET_NEXT(mp)            (IS_LAST(mp)) ? NULL : ((MprMem*) ((char*) mp + GET_SIZE(mp)))
    #define GET_REGION(mp)          ((MprRegion*) (((char*) mp) - MPR_ALLOC_ALIGN(sizeof(MprRegion))))
#else
    #define GET_NEXT(mp)            ((((MprMem*) mp)->next == head) ? NULL : ((MprMem*) mp)->next)
#endif

/*
    Macros to set and extract "prior" fields. All accesses (read and write) must be done locked.
        prior | last << 1 | hasManager
 */
#if BLD_FEATURE_VALLOC
    #define GET_PRIOR(mp)           ((MprMem*) ((mp->field1 & MPR_MASK_PRIOR) >> MPR_SHIFT_PRIOR))
    #define SET_PRIOR(mp, value)    mp->field1 = ((((size_t) value) << MPR_SHIFT_PRIOR) | (mp->field1 & ~MPR_MASK_PRIOR))
    #define IS_LAST(mp)             ((mp->field1 & MPR_MASK_LAST) >> MPR_SHIFT_LAST)
    #define SET_LAST(mp, value)     mp->field1 = ((value << MPR_SHIFT_LAST) | (mp->field1 & ~MPR_MASK_LAST))
    #define SET_FIELD1(mp, prior, last, hasManager) mp->field1 = (((size_t) prior) << MPR_SHIFT_PRIOR) | \
                                    ((last) << MPR_SHIFT_LAST) | ((hasManager) << MPR_SHIFT_HAS_MANAGER)
#else
    #define SET_FIELD1(mp, prior, last, hasManager) \
                                    mp->field1 = ((hasManager) << MPR_SHIFT_HAS_MANAGER)
#endif

#define HAS_MANAGER(mp)             ((mp->field1 & MPR_MASK_HAS_MANAGER) >> MPR_SHIFT_HAS_MANAGER)
#define SET_HAS_MANAGER(mp, value)  mp->field1 = ((mp->field1 & ~MPR_MASK_HAS_MANAGER) | (value << MPR_SHIFT_HAS_MANAGER))

/*
    Macros to set and extract "size" fields. Accesses can be done unlocked. Updates must be done lock-free.
        gen/2 << 30 | free/1 << 29 | size/29 | mark/2
 */
#define GET_SIZE(mp)                ((ssize) ((mp->field2 & MPR_MASK_SIZE) >> MPR_SHIFT_SIZE))
#define SET_SIZE(mp, value)         mp->field2 = ((value) << MPR_SHIFT_SIZE) | (mp->field2 & ~MPR_MASK_SIZE)
#define IS_FREE(mp)                 ((mp->field2 & MPR_MASK_FREE) >> MPR_SHIFT_FREE)
#define SET_FREE(mp, value)         mp->field2 = (((size_t) (value)) << MPR_SHIFT_FREE) | (mp->field2 & ~MPR_MASK_FREE)
#define GET_GEN(mp)                 ((mp->field2 & MPR_MASK_GEN) >> MPR_SHIFT_GEN)
#define SET_GEN(mp, value)          mp->field2 = (((size_t) value) << MPR_SHIFT_GEN) | (mp->field2 & ~MPR_MASK_GEN)
#define GET_MARK(mp)                (mp->field2 & MPR_MASK_MARK)
#define SET_MARK(mp, value)         mp->field2 = (value) | (mp->field2 & ~MPR_MASK_MARK)
#define SET_FIELD2(mp, size, gen, mark, free) mp->field2 = \
                                        (((size_t) (gen)) << MPR_SHIFT_GEN) | \
                                        (((size_t) (free)) << MPR_SHIFT_FREE) | \
                                        ((size) << MPR_SHIFT_SIZE) | \
                                        ((mark) << MPR_SHIFT_MARK)

/*
    Padding fields (only manager stored in padding region)
 */
#define PAD_PTR(mp, offset)     ((void*) (((char*) mp) + GET_SIZE(mp) - ((offset) * sizeof(void*))))
#define MANAGER_SIZE            1
#define MANAGER_OFFSET          1
#define GET_MANAGER(mp)         ((MprManager) (*(void**) ((PAD_PTR(mp, MANAGER_OFFSET)))))
#define SET_MANAGER(mp, fn)     *((MprManager*) PAD_PTR(mp, MANAGER_OFFSET)) = fn

/*
    Memory checking and breakpoints
 */
#if BLD_MEMORY_DEBUG
#define BREAKPOINT(mp)          breakpoint(mp)
#define CHECK(mp)               mprCheckBlock((MprMem*) mp)
#define CHECK_FREE_MEMORY(mp)   checkFreeMem(mp)
#define CHECK_PTR(ptr)          CHECK(GET_MEM(ptr))
#define RESET_MEMORY(mp)        if (heap->verify && mp != GET_MEM(MPR)) { \
                                    memset((char*) mp + sizeof(MprFreeMem), 0xFE, GET_SIZE(mp) - sizeof(MprFreeMem)); \
                                } else
#define SET_MAGIC(mp)           mp->magic = MPR_ALLOC_MAGIC
#define SET_SEQ(mp)             mp->seqno = heap->nextSeqno++
#define VALID_BLK(mp)           validBlk(mp)
#define SET_NAME(mp, value)     mp->name = value

#else /* Release mode */
#define BREAKPOINT(mp)
#define CHECK(mp)           
#define CHECK_PTR(mp)           
#define RESET_MEMORY(mp)           
#define CHECK_FREE_MEMORY(mp)           
#define SET_NAME(mp, value)
#define SET_MAGIC(mp)
#define SET_SEQ(mp)           
#define VALID_BLK(mp)           1
#endif

#if BLD_MEMORY_STATS
    #define INC(field)          if (1) { heap->stats.field++; } else 
#else
    #define INC(field)
#endif

#define INIT_BLK(mp, size, hasManager, last, prior) if (1) { \
    SET_FIELD1(mp, prior, last, hasManager); \
    SET_FIELD2(mp, size, heap->active, heap->eternal, 0); \
    SET_MAGIC(mp); \
    SET_SEQ(mp); \
    SET_NAME(mp, NULL); \
    } else

#define lockHeap()              mprSpinLock(&heap->heapLock);
#define unlockHeap()            mprSpinUnlock(&heap->heapLock);

#define percent(a,b) ((int) ((a) * 100 / (b)))

/*
    Fast find first/last bit set
 */
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

#if !BLD_FEATURE_VALLOC
static MprMem   headBlock, *head;
#endif

/***************************** Forward Declarations ***************************/

static void allocException(ssize size, bool granted);
static void dummyManager(void *ptr, int flags);
static void *getNextRoot();
static void getSystemInfo();
static void initGen();
static void mark();
static void marker(void *unused, MprThread *tp);
static void markRoots();
static int memoryNotifier(int flags, ssize size);
static void nextGen();
static void sweep();
static void sweeper(void *unused, MprThread *tp);
static void synchronize();
static int syncThreads(int timeout);
static void triggerGC(int force);

#if BLD_WIN_LIKE
    static int winPageModes(int flags);
#endif
#if BLD_MEMORY_DEBUG
    static void breakpoint(MprMem *mp);
    static int validBlk(MprMem *mp);
    static void checkFreeMem(MprMem *mp);
#endif
#if BLD_MEMORY_STATS
#if FUTURE
    static void showMem(MprMem *mp);
#endif
    static void printQueueStats();
    static void printGCStats();
#endif

#if BLD_FEATURE_VALLOC
    #define allocBlock(required, flags) allocFromHeap(required, flags)
    #define freeBlock(mp) freeToHeap(mp)
    static int initFree();
    static MprMem *allocFromHeap(ssize size, int flags);
    static MprMem *freeToHeap(MprMem *mp);
    static int getQueueIndex(ssize size, int roundup);
    static MprMem *growHeap(ssize size, int flags);
    static void linkBlock(MprMem *mp); 
    static void unlinkBlock(MprFreeMem *fp);
    #define valloc(size, flags) mprVirtAlloc(size, flags)
    #define vfree(ptr, size) mprVirtFree(ptr, size)
    #if BLD_MEMORY_STATS
        static MprFreeMem *getQueue(ssize size);
    #endif
#else
    #define allocBlock(required, flags) allocFromMalloc(required, flags)
    #define freeBlock(mp) freeToMalloc(mp)
    static MprMem *allocFromMalloc(ssize required, int flags);
    static MprMem *freeToMalloc(MprMem *mp);
    #define valloc(size, flags) malloc(size)
    #define vfree(ptr, size) free(ptr)
#endif

/************************************* Code ***********************************/
/*
    Initialize the memory subsystem
 */
Mpr *mprCreateMemService(MprManager manager, int flags)
{
    MprHeap     initHeap;
    MprMem      *mp;
    ssize       size, mprSize;
#if BLD_FEATURE_VALLOC
    MprMem      *spare;
    MprRegion   *region;
    ssize       regionSize;
#endif

    if (!(flags & MPR_OWN_GC)) {
        flags |= MPR_THREAD_PATTERN;
    }
    heap = &initHeap;
    memset(heap, 0, sizeof(MprHeap));
    heap->stats.maxMemory = MAXINT;
    heap->stats.redLine = MAXINT / 100 * 99;
    mprInitSpinLock(&heap->heapLock);
    initGen();

    /*
        Hand-craft the Mpr structure
     */
    mprSize = MPR_ALLOC_ALIGN(sizeof(MprMem) + sizeof(Mpr) + (MANAGER_SIZE * sizeof(void*)));

#if BLD_FEATURE_VALLOC
    regionSize = MPR_ALLOC_ALIGN(sizeof(MprRegion));
    size = max(mprSize + regionSize, MPR_MEM_REGION_SIZE);
    if ((region = valloc(size, MPR_MAP_READ | MPR_MAP_WRITE)) == NULL) {
        return NULL;
    }
    mp = region->start = (MprMem*) (((char*) region) + regionSize);
    region->size = size;
#else
    head = &headBlock;
    head->next = head->prev = head;
    mp = malloc(mprSize);
    size = mprSize;
#endif

    MPR = (Mpr*) GET_PTR(mp);
    INIT_BLK(mp, mprSize, 1, 0, NULL);
    SET_MANAGER(mp, manager);
    mprSetName(MPR, "Mpr");

    heap = &MPR->heap;
    heap->flags = flags;
    heap->notifier = (MprMemNotifier) memoryNotifier;
    heap->nextSeqno = 1;
    heap->chunkSize = MPR_MEM_REGION_SIZE;
    heap->stats.maxMemory = MAXINT;
    heap->stats.redLine = MAXINT / 100 * 99;
    heap->newQuota = MPR_NEW_QUOTA;
    heap->earlyYieldQuota = MPR_NEW_QUOTA * 5;
    heap->enabled = !(flags & MPR_DISABLE_GC);
    if (scmp(getenv("MPR_DISABLE_GC"), "1") == 0) {
        heap->enabled = 0;
    }
    if (scmp(getenv("MPR_VERIFY_MEM"), "1") == 0) {
        heap->verify = 1;
    }
    heap->stats.bytesAllocated += size;
    INC(allocs);

    mprInitSpinLock(&heap->heapLock);
    mprInitSpinLock(&heap->heapLock2);
    mprInitSpinLock(&heap->rootLock);
    getSystemInfo();
    initGen();

#if BLD_FEATURE_VALLOC
    spare = (MprMem*) (((char*) mp) + mprSize);
    INIT_BLK(spare, size - regionSize - mprSize, 0, 1, mp);
    SET_GEN(spare, heap->eternal);
    SET_FREE(spare, 1);
    heap->regions = region;
    initFree();
    linkBlock(spare);
#endif

    heap->markerCond = mprCreateCond();
    heap->roots = mprCreateList(-1, MPR_LIST_STATIC_VALUES);
    mprAddRoot(MPR);
    return MPR;
}


/*
    Shutdown memory service. Run managers on all allocated blocks
 */
void mprDestroyMemService()
{
    volatile MprRegion  *region;
    MprMem              *mp, *next;

    if (heap->destroying) {
        return;
    }
    heap->destroying = 1;
    for (region = heap->regions; region; region = region->next) {
        for (mp = region->start; mp; mp = next) {
            next = GET_NEXT(mp);
            if (unlikely(HAS_MANAGER(mp))) {
                (GET_MANAGER(mp))(GET_PTR(mp), MPR_MANAGE_FREE);
                SET_HAS_MANAGER(mp, 0);
            }
        }
    }
}


void *mprAllocBlock(ssize usize, int flags)
{
    MprMem      *mp;
    void        *ptr;
    ssize       size;
    int         padWords;

    padWords = padding[flags & MPR_ALLOC_PAD_MASK];
    size = usize + sizeof(MprMem) + (padWords * sizeof(void*));
    size = max(size, usize + (ssize) sizeof(MprFreeMem));
    size = MPR_ALLOC_ALIGN(size);
    
    if ((mp = allocBlock(size, flags)) == NULL) {
        return NULL;
    }
    ptr = GET_PTR(mp);
    if (flags & MPR_ALLOC_ZERO) {
        /* Note: real usize may be bigger than requested */
        memset(ptr, 0, GET_USIZE(mp));
    }
    BREAKPOINT(mp);
    CHECK(mp);
    mprAssert(GET_GEN(mp) != heap->eternal);
    return ptr;
}


/*
    Realloc will always zero new memory
 */
void *mprRealloc(void *ptr, ssize usize)
{
    MprMem      *mp, *newb;
    void        *newptr;
    ssize       oldSize, oldUsize;
    int         flags, hasManager;

    mprAssert(usize > 0);

    if (ptr == 0) {
        return mprAllocBlock(usize, 0);
    }
    mp = GET_MEM(ptr);
    CHECK(mp);
    mprAssert(!IS_FREE(mp));
    mprAssert(GET_GEN(mp) != heap->dead);
    oldUsize = GET_USIZE(mp);

    if (usize <= oldUsize) {
        return ptr;
    }
    hasManager = HAS_MANAGER(mp);
    flags = hasManager ? MPR_ALLOC_MANAGER : 0;
    if ((newptr = mprAllocBlock(usize, flags)) == NULL) {
        return 0;
    }
    newb = GET_MEM(newptr);
    if (hasManager) {
        SET_MANAGER(newb, GET_MANAGER(mp));
    }
    if (GET_GEN(mp) == heap->eternal) {
        /* Lock-free update */
        SET_FIELD2(newb, GET_SIZE(newb), heap->eternal, UNMARKED, 0);
    }
    oldSize = GET_SIZE(mp);
    memcpy(newptr, ptr, oldSize - sizeof(MprMem));
    /* Note: real usize may be bigger than requested */
    memset(&((char*) newptr)[oldUsize], 0, GET_USIZE(newb) - oldUsize);
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
    int         rc;

    mprAssert(s1);
    mprAssert(s2);
    mprAssert(s1Len >= 0);
    mprAssert(s2Len >= 0);

    rc = memcmp(s1, s2, min(s1Len, s2Len));
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
    mprMemcpy will support insitu copy where src and destination overlap
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

#if BLD_FEATURE_VALLOC
/*************************** Virtual Heap Allocations *************************/
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


static MprMem *allocFromHeap(ssize required, int flags)
{
    MprFreeMem  *freeq, *fp;
    MprMem      *mp, *after, *spare;
    ssize       size, maxBlock;
    ulong       groupMap, bucketMap;
    int         bucket, baseGroup, group, index;
    
    index = getQueueIndex(required, 1);
    baseGroup = index / MPR_ALLOC_NUM_BUCKETS;
    bucket = index % MPR_ALLOC_NUM_BUCKETS;
    heap->newCount += index;
    INC(requests);

    /*
        MOB OPT - could break this locked section up.
        - Can update bit maps conservatively and lockfree
        - Put locks around freeq unqueue
        - use unlinkBlock or linkBlock only. Do locks internally in these routines
        - Probably need unlinkFirst
        - Long term use lockfree
     */
    lockHeap();
    
    /* Mask groups lower than the base group */
    groupMap = heap->groupMap & ~((((ssize) 1) << baseGroup) - 1);
    while (groupMap) {
        group = (int) (ffsl(groupMap) - 1);
        if (groupMap & ((((ssize) 1) << group))) {
            bucketMap = heap->bucketMap[group];
            if (baseGroup == group) {
                /* Mask buckets lower than the base bucket */
                bucketMap &= ~((((ssize) 1) << bucket) - 1);
            }
            while (bucketMap) {
                bucket = (int) (ffsl(bucketMap) - 1);
                index = (group * MPR_ALLOC_NUM_BUCKETS) + bucket;
                freeq = &heap->freeq[index];

                if (freeq->next != freeq) {
                    fp = freeq->next;
                    mp = (MprMem*) fp;
                    mprAssert(IS_FREE(mp));
                    unlinkBlock(fp);

                    mprAssert(GET_GEN(mp) == heap->eternal);
                    SET_GEN(mp, heap->active);

                    //  MOB -- cleanup
                    mprAtomicBarrier();
                    if (flags & MPR_ALLOC_MANAGER) {
                        SET_MANAGER(mp, dummyManager);
                        SET_HAS_MANAGER(mp, 1);
                    }
                    INC(reuse);
                    CHECK(mp);
                    CHECK_FREE_MEMORY(mp);
                    if (GET_SIZE(mp) >= (ssize) (required + MPR_ALLOC_MIN_SPLIT)) {
                        //  MOB -- what is this trying to do?
                        maxBlock = (((ssize) 1 ) << group | (((ssize) bucket) << (max(0, group - 1)))) << MPR_ALIGN_SHIFT;
                        maxBlock += sizeof(MprMem);

                        size = GET_SIZE(mp);
                        if (size > maxBlock) {
                            spare = (MprMem*) ((char*) mp + required);
                            INIT_BLK(spare, size - required, 0, IS_LAST(mp), mp);
                            if ((after = GET_NEXT(spare)) != NULL) {
                                SET_PRIOR(after, spare);
                            }
                            SET_SIZE(mp, required);
                            //  MOB -- cleanup
                            mprAtomicBarrier();
                            SET_LAST(mp, 0);
                            mprAtomicBarrier();
                            INC(splits);
                            linkBlock(spare);
                        }
                    }
                    unlockHeap();
                    return mp;
                }
                bucketMap &= ~(((ssize) 1) << bucket);
                heap->bucketMap[group] &= ~(((ssize) 1) << bucket);
            }
            groupMap &= ~(((ssize) 1) << group);
            heap->groupMap &= ~(((ssize) 1) << group);
            triggerGC(0);
        }
    }
    unlockHeap();
    triggerGC(0);
    return growHeap(required, flags);
}


/*
    Grow the heap and return a block of the required size (unqueued)
 */
static MprMem *growHeap(ssize required, int flags)
{
    MprRegion           *region;
    MprMem              *mp, *spare;
    ssize               size, rsize;
    int                 hasManager;

    mprAssert(required > 0);

    rsize = MPR_ALLOC_ALIGN(sizeof(MprRegion));
    size = max(required + rsize, (ssize) heap->chunkSize);
    size = MPR_PAGE_ALIGN(size, heap->pageSize);

    if ((region = valloc(size, MPR_MAP_READ | MPR_MAP_WRITE)) == NULL) {
        return 0;
    }
    mprInitSpinLock(&((MprRegion*) region)->lock);
    region->size = size;
    region->start = (MprMem*) (((char*) region) + rsize);
    mp = (MprMem*) region->start;
    hasManager = (flags & MPR_ALLOC_MANAGER) ? 1 : 0;
    INIT_BLK(mp, required, hasManager, 0, NULL);
    if (hasManager) {
        SET_MANAGER(mp, dummyManager);
    }
    CHECK(mp);

    spare = (MprMem*) ((char*) mp + required);
    INIT_BLK(spare, size - required - rsize, 0, 1, mp);
    CHECK(spare);

#if MOB
    mprAtomicListInsert(&heap->regions, &region->next, region);
#endif
    do {
        region->next = heap->regions;
    } while (!mprAtomicCas((void* volatile*) &heap->regions, region->next, region));

    lockHeap();
    INC(allocs);
    linkBlock(spare);
    unlockHeap();
    return mp;
}


/*
    Free a block. MUST only ever be called by the sweeper. The sweeper takes advantage of the fact that only it 
    coalesces blocks.
 */
static MprMem *freeToHeap(MprMem *mp)
{
    MprMem      *prev, *next, *after;
    ssize       size;
#if BLD_CC_MMU
    MprRegion   *region;
#endif
    BREAKPOINT(mp);

    size = GET_SIZE(mp);
    prev = NULL;
    
    /*
        Coalesce with next if it is free
     */
    lockHeap();

    //  MOB - GET_NEXT should be safe lockfree in the sweeper.
    next = GET_NEXT(mp);
    if (next && IS_FREE(next)) {
        BREAKPOINT(next);
//  MOB - lockfree race here as someone else may claim this block
        unlinkBlock((MprFreeMem*) next);
        if ((after = GET_NEXT(next)) != NULL) {
            mprAssert(GET_PRIOR(after) == next);
            SET_PRIOR(after, mp);
        } else {
            SET_LAST(mp, 1);
        }
        size += GET_SIZE(next);
        SET_SIZE(mp, size);
        INC(joins);
    }
    /*
        Coalesce with previous if it is free
     */
    prev = GET_PRIOR(mp);
    if (prev && IS_FREE(prev)) {
        BREAKPOINT(prev);
        //  MOB - lockfree race here as someone else may claim this block
        unlinkBlock((MprFreeMem*) prev);
        if ((after = GET_NEXT(mp)) != NULL) {
            mprAssert(GET_PRIOR(after) == mp);
            SET_PRIOR(after, prev);
        } else {
            SET_LAST(prev, 1);
        }
        size += GET_SIZE(prev);
        SET_SIZE(prev, size);
        // printf("  JOIN PREV %x size %d\n", prev, size);
        mp = prev;
        INC(joins);
        prev = GET_PRIOR(mp);
        if (prev) CHECK(prev);
        mprAssert(prev == 0 || !IS_FREE(prev));
    }
    next = GET_NEXT(mp);

    /*
        Release entire regions back to the O/S. (Blocks equal to Empty regions have no prior and are last)
     */
#if BLD_CC_MMU
    if (GET_PRIOR(mp) == NULL && IS_LAST(mp) && heap->stats.bytesFree > (MPR_MEM_REGION_SIZE * 4)) {
        INC(unpins);
        unlockHeap();
        region = GET_REGION(mp);
        region->freeable = 1;
        mprAssert(next == NULL);
    } else
#endif
    {
        linkBlock(mp);
        unlockHeap();
    }
    /*
        WARN: there is a race here. Another thread may allocate and split the block just freed. So next will be
        pessimistic and there may be newly created intervening blocks.
     */
    return next;
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


/*
    Add a block to a free q. Must be called locked.
    Called by user threads from allocFromHeap and by sweeper from freeToHeap.
 */
static void linkBlock(MprMem *mp) 
{
    MprFreeMem  *freeq, *fp;
    ssize       size;
    int         index, group, bucket;

    CHECK(mp);
    RESET_MEMORY(mp);

    /* 
        Mark block as free and eternal so sweeper will skip 
     */
    size = GET_SIZE(mp);
    SET_FIELD2(mp, size, heap->eternal, UNMARKED, 1);
    SET_HAS_MANAGER(mp, 0);
    
    /*
        Set free space bitmap
     */
    index = getQueueIndex(size, 0);
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

    heap->stats.bytesFree += size;
#if BLD_MEMORY_STATS
    freeq->info.stats.count++;
#endif
}


/*
    Remove a block from a free q. Must be called locked.
 */
static void unlinkBlock(MprFreeMem *fp) 
{
    MprMem  *mp;
    ssize   size;

    CHECK(fp);
    fp->prev->next = fp->next;
    fp->next->prev = fp->prev;
#if BLD_MEMORY_DEBUG
    fp->next = fp->prev = NULL;
#endif

    mp = (MprMem*) fp;
    size = GET_SIZE(mp);
    heap->stats.bytesFree -= size;
    mprAssert(IS_FREE(mp));
    SET_FREE(mp, 0);
    mprAtomicBarrier();
#if BLD_MEMORY_STATS
{
    MprFreeMem *freeq = getQueue(size);
    freeq->info.stats.count--;
    mprAssert(freeq->info.stats.count >= 0);
}
#endif
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

/****************************** Malloc Allocations ****************************/
#else /* !BLD_FEATURE_VALLOC */

static MprMem *allocFromMalloc(ssize required, int flags)
{
    MprMem      *mp;
    int         hasManager;
    
    lockHeap();
    if ((mp = malloc(required)) == 0) {
        unlockHeap();
        return NULL;
    }
    //  MOB - should not do this - alloc block will zero if requried
    memset(mp, 0, required);
    hasManager = (flags & MPR_ALLOC_MANAGER) ? 1 : 0;
    INIT_BLK(mp, required, hasManager, 0, NULL);
    SET_GEN(mp, heap->active);
    SET_SIZE(mp, required);
    if (hasManager) {
        SET_MANAGER(mp, dummyManager);
    }
    mp->next = head->next;
    mp->prev = head;
    head->next->prev = mp;
    head->next = mp;
    CHECK(mp);
    CHECK_FREE_MEMORY(mp);
    heap->newCount += 5;
    triggerGC(0);
    mprAssert(mp->next);
    heap->stats.bytesAllocated += required;
    unlockHeap();
    return mp;
}


static MprMem *freeToMalloc(MprMem *mp)
{
    MprMem  *next;

    lockHeap();
    next = GET_NEXT(mp);
    mp->prev->next = mp->next;
    mp->next->prev = mp->prev;
    mp->next = mp->prev = mp;
    heap->stats.bytesAllocated -= GET_SIZE(mp);
    memset((char*) mp, 0xFE, GET_SIZE(mp));
    free(mp);
    unlockHeap();
    return next;
}
#endif /* !BLD_FEATURE_VALLOC */


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
    //  MOB locking
    heap->stats.bytesAllocated += size;
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
    //  MOB locking
    heap->stats.bytesAllocated -= size;
    mprAssert(heap->stats.bytesAllocated >= 0);
}

/***************************************************** Garbage Colllector *************************************************/

void mprStartGCService()
{
    if (heap->enabled) {
        if (heap->flags & MPR_MARK_THREAD) {
            LOG(7, "DEBUG: startMemWorkers: start marker");
            if ((heap->marker = mprCreateThread("marker", marker, NULL, 0)) == 0) {
                mprError("Can't create marker thread");
                MPR->hasError = 1;
            } else {
                mprStartThread(heap->marker);
            }
        }
        if (heap->flags & MPR_SWEEP_THREAD) {
            LOG(7, "DEBUG: startMemWorkers: start sweeper");
            heap->hasSweeper = 1;
            if ((heap->sweeper = mprCreateThread("sweeper", sweeper, NULL, 0)) == 0) {
                mprError("Can't create sweeper thread");
                MPR->hasError = 1;
            } else {
                mprStartThread(heap->sweeper);
            }
        }
    }
}


void mprWakeGCService()
{
    mprSignalCond(heap->markerCond);
    mprResumeThreads();
}


static void triggerGC(int force)
{
    if (!heap->gc && (force || (heap->newCount > heap->newQuota))) {
        heap->gc = 1;
#if !PARALLEL_GC
        heap->mustYield = 1;
#endif
        if (heap->flags & MPR_MARK_THREAD) {
            mprSignalCond(heap->markerCond);
        }
    }
}


void mprRequestGC(int flags)
{
    int     i, count;

    LOG(7, "DEBUG: mprRequestGC");

    count = (flags & MPR_COMPLETE_GC) ? 3 : 1;
    for (i = 0; i < count; i++) {
        if ((flags & MPR_FORCE_GC) || (heap->newCount > heap->newQuota)) {
            heap->mustYield = 1;
            triggerGC(1);
        }
        mprYield((flags & MPR_WAIT_GC) ? MPR_YIELD_BLOCK: 0);
    }
#if OLD
    MprThread   *tp;
    int         wasSticky;

    tp = mprGetCurrentThread();
    wasSticky = tp->stickyYield;

    if (flags & MPR_COMPLETE_GC) {
        mprYield(MPR_YIELD_STICKY);
    }
    if ((flags & MPR_FORCE_GC) || (heap->newCount > heap->newQuota)) {
        if (heap->flags & MPR_OWN_GC) {
            ownGC(flags);
        } else {
            mprSignalCond(heap->markerCond);
            if (flags & MPR_WAIT_GC) {
                mprYield(MPR_YIELD_BLOCK);
            } else {
                mprYield(0);
            }
        }
    }
    tp->stickyYield = wasSticky;
#endif
}


/*
    Marker synchronization point. At the end of each GC mark/sweep, all threads must rendezvous at the 
    synchronization point.  This happens infrequently and is essential to safely move to a new generation.
    All threads must yield to the marker (including sweeper)
 */
static void synchronize()
{
#if BLD_MEMORY_STATS
    //7
    LOG(2, "GC: MARKED %,d/%,d, SWEPT %,d/%,d, freed %,d, bytesFree %,d (prior %,d), newCount %,d/%,d, " 
            "blocks %,d bytes %,d",
            heap->stats.marked, heap->stats.markVisited, heap->stats.swept, heap->stats.sweepVisited, 
            (int) heap->stats.freed, (int) heap->stats.bytesFree, (int) heap->priorFree, heap->priorNewCount, heap->newQuota,
            heap->stats.sweepVisited - heap->stats.swept, (int) heap->stats.bytesAllocated);
#endif
#if PARALLEL_GC
    heap->mustYield = 1;
    if (heap->notifier) {
        (heap->notifier)(MPR_MEM_ATTENTION, 0);
    }
    if (syncThreads(MPR_TIMEOUT_GC_SYNC)) {
        nextGen();
    } else {
        LOG(7, "DEBUG: Pause for GC sync timed out");
    }
#else
    nextGen();
#endif
    heap->mustYield = 0;
    mprResumeThreads();
}


static void mark()
{
    //7
    LOG(2, "GC: mark started");

#if !PARALLEL_GC
    heap->mustYield = 1;
    if (!syncThreads(MPR_TIMEOUT_GC_SYNC)) {
        LOG(0, "DEBUG: Pause for GC sync timed out");
        return;
    }
#else
    if (heap->newCount > heap->earlyYieldQuota) {
        heap->mustYield = 1;
    }
#endif
    heap->priorNewCount = heap->newCount;
    heap->priorFree = heap->stats.bytesFree;
    heap->newCount = 0;
    heap->gc = 0;
    markRoots();
    if (!heap->hasSweeper) {
        sweep();
    }
    synchronize();
}


/*
    Sweep up the garbage.
    WARNING: This code uses lock-free algorithms. The sweeper traverses the region list and block list without locking. 
    Other code must similarly use lock-free code -- only add regions to the start of the regions list and never 
    otherwise modify the region list. Other code may modify blocks on the list, but must atomically update MprMem.field1.
    The sweeper is the only routine to do coalesing, other code may split blocks, but this can be done in a lock-free 
    manner by creating the spare 2nd half block first and then updating mp->field2 with the size and last bit.
*/
static void sweep()
{
#if BLD_FEATURE_VALLOC
    MprRegion   *rp, *region, *nextRegion, *prior;
#endif
    MprMem      *mp, *next;
    MprManager  mgr;
    
    if (!heap->enabled) {
        LOG(7, "DEBUG: sweep: Abort sweep - GC disabled");
        return;
    }
    //7
    LOG(2, "GC: sweep started");
    heap->stats.freed = 0;

    if (heap->newCount > heap->earlyYieldQuota) {
        heap->mustYield = 1;
    }

    /*
        Run all destructors first so all destructors can guarantee dependant memory blocks will still exist.
        Actually free the memory in a 2nd pass below.
     */
#if BLD_FEATURE_VALLOC
    for (region = heap->regions; region; region = region->next) {
        /*
            This code assumes that no other code coalesces blocks and that splitting blocks will be done lock-free
         */
        for (mp = region->start; mp; mp = GET_NEXT(mp)) {
#else
        for (mp = head->next; mp; mp = GET_NEXT(mp)) {
#endif
            if (unlikely(GET_GEN(mp) == heap->dead && HAS_MANAGER(mp))) {
                mgr = GET_MANAGER(mp);
                mprAssert(!IS_FREE(mp));
                CHECK(mp);
                BREAKPOINT(mp);
                (mgr)(GET_PTR(mp), MPR_MANAGE_FREE);
            }
        }
#if BLD_FEATURE_VALLOC
    }
#endif
    heap->stats.sweepVisited = 0;
    heap->stats.swept = 0;

    /*
        growHeap() will append new regions to the front of heap->regions and so will not race with this code. This code
        is the only code that frees regions.
        RACE: Take from the front. Racing with growHeap.
     */
#if BLD_FEATURE_VALLOC
    prior = NULL;
    for (region = heap->regions; region; region = nextRegion) {
        nextRegion = region->next;
        /*
            This code assumes that no other code coalesces blocks and that splitting blocks will be done lock-free
         */
        for (mp = region->start; mp; mp = next) {
#else
        for (mp = head->next; mp; mp = next) {
#endif
            CHECK(mp);
            INC(sweepVisited);
            if (unlikely(GET_GEN(mp) == heap->dead)) {
                mprAssert(!IS_FREE(mp));
                CHECK(mp);
                BREAKPOINT(mp);
                INC(swept);
                heap->stats.freed += GET_SIZE(mp);
                next = freeBlock(mp);
            } else {
                /*
                    RACE: Block could be allocated here, but will never be coalesced (sweeper is the only one to do that).
                    So mp->field2 may be reduced so we may skip a newly created block -- no problem. Get it next scan.
                 */
                next = GET_NEXT(mp);
            }
#if BLD_FEATURE_VALLOC
        }
#endif

#if BLD_FEATURE_VALLOC
        /*
            The sweeper is the only one who removes regions. Do this lock-free because user code traverses the region list.
         */ 
        if (region->freeable) {
            if (prior) {
                prior->next = nextRegion;
            } else {
                /*
                    The region was the first in the list. If the update to the list head fails, some other thread beta us. 
                    Thereafter, we can guarantee there is a prior entry, so rescan for the region and then update the prior.
                 */
                if (!mprAtomicCas((void*volatile*) &heap->regions, region, nextRegion)) {
                    for (rp = heap->regions; rp; rp = rp->next) {
                        if (rp->next == region) {
                            rp->next = nextRegion;
                            break;
                        }
                    }
                    mprAssert(rp != NULL);
                }
            }
            LOG(5, "DEBUG: Unpin %p to %p size %d", region, ((char*) region) + region->size, region->size);
            mprVirtFree(region, region->size);
        } else {
            prior = region;
        }
#endif
    }
}


static void markRoots()
{
    void    *root;

    heap->stats.markVisited = 0;
    heap->stats.marked = 0;
    mprMark(heap->roots);

    heap->rootIndex = 0;
    while ((root = getNextRoot()) != 0) {
        mprMark(root);
    }
    heap->rootIndex = -1;
}


void mprMarkBlock(cvoid *ptr)
{
    MprMem      *mp;
    int         gen;

    if (ptr == 0) {
        return;
    }
    mp = MPR_GET_MEM(ptr);
#if BLD_DEBUG
    if (!mprIsValid(ptr)) {
        mprStaticError("Memory block is either not dynamically allocated, or is corrupted");
        return;
    }
    mprAssert(!IS_FREE(mp));
    mprAssert(GET_MARK(mp) != heap->dead);
    mprAssert(GET_GEN(mp) != heap->dead);
    if (GET_MARK(mp) == heap->dead || IS_FREE(mp)) {
        mprAssert(0);
        return;
    }
#endif
    CHECK(mp);
    INC(markVisited);
    mprAssert((GET_MARK(mp) != heap->active) || GET_GEN(mp) == heap->active);

    if (GET_MARK(mp) != heap->active) {
        BREAKPOINT(mp);
        INC(marked);
        gen = GET_GEN(mp);
        if (gen != heap->eternal) {
            gen = heap->active;
        }
        /* Lock-free update */
        SET_FIELD2(mp, GET_SIZE(mp), gen, heap->active, 0);
        if (HAS_MANAGER(mp)) {
            (GET_MANAGER(mp))((void*) ptr, MPR_MANAGE_MARK);
        }
    }
}


//  MOB - these are dangerous as they don't hold component allocations
void mprHold(void *ptr)
{
    MprMem  *mp;

    if (ptr) {
        mp = GET_MEM(ptr);
        /* Lock-free update of mp->gen */
        SET_FIELD2(mp, GET_SIZE(mp), heap->eternal, UNMARKED, 0);
    }
}


void mprRelease(void *ptr)
{
    MprMem  *mp;

    if (ptr) {
        mp = GET_MEM(ptr);
        mprAssert(!IS_FREE(mp));
        /* Lock-free update of mp->gen */
        SET_FIELD2(mp, GET_SIZE(mp), heap->active, UNMARKED, 0);
    }
}


/*
    Marker thread main program
 */
static void marker(void *unused, MprThread *tp)
{
    LOG(5, "DEBUG: marker thread started");
    MPR->marking = 1;
    tp->stickyYield = 1;
    tp->yielded = 1;

    while (!mprIsStoppingCore()) {
        if (!heap->mustYield) {
            mprWaitForCond(heap->markerCond, -1);
        }
        mark();
    }
    //  MOB - is this ever used?
    MPR->marking = 0;
    heap->mustYield = 0;
    mprResumeThreads();
}


/*
    Sweeper thread main program. May be called from the marker thread.
 */
static void sweeper(void *unused, MprThread *tp) 
{
    LOG(5, "DEBUG: sweeper thread started");

    MPR->sweeping = 1;
    while (!mprIsStoppingCore()) {
        sweep();
        mprYield(MPR_YIELD_BLOCK);
    }
    MPR->sweeping = 0;
}


#if UNUSED
/*
    Called by user code to signify the thread is ready for GC and all object references are saved. 
    If the GC marker is synchronizing, this call will block at the GC sync point (should be brief).
 */
//  MOB - do we need this?
static void ownGC(int flags)
{
    MprThread   *tp;
    int         i;

    if (!heap->enabled || (!heap->gc && !(flags & MPR_FORCE_GC))) {
        return;
    }
    if (heap->flags & (MPR_MARK_THREAD | MPR_SWEEP_THREAD)) {
        mprYield(0);
        return;
    }
    tp = mprGetCurrentThread();
    lockHeap();
    if (heap->collecting) {
        unlockHeap();
        while (tp->yielded && heap->mustYield) {
            LOG(7, "mprYieldThread %s must wait", tp->name);
            mprWaitForCond(tp->cond, -1);
        }
    } else {
        heap->collecting = 1;
        unlockHeap();
        tp->yielded = 1;
        mark();
        tp->yielded = 0;
        heap->collecting = 0;
    }
}
#endif


/*
    Called by user code to signify the thread is ready for GC and all object references are saved. 
    If the GC marker is synchronizing, this call will block at the GC sync point (should be brief).
 */
void mprYield(int flags)
{
    MprThreadService    *ts;
    MprThread           *tp;

    ts = MPR->threadService;
    tp = mprGetCurrentThread();

    LOG(7, "mprYield thread \"%s\" yielded was %d, block %d", tp->name, tp->yielded, flags & MPR_YIELD_BLOCK);
    tp->yielded = 1;
    if (flags & MPR_YIELD_STICKY) {
        tp->stickyYield = 1;
    }
    while (tp->yielded && (heap->mustYield || (flags & MPR_YIELD_BLOCK))) {
        if (heap->flags & MPR_MARK_THREAD) {
            mprSignalCond(ts->cond);
        }
        LOG(7, "mprYieldThread thread \"%s\" must wait", tp->name);
        mprWaitForCond(tp->cond, -1);
        flags &= ~MPR_YIELD_BLOCK;
    }
    if (!tp->stickyYield) {
        tp->yielded = 0;
    }
}


void mprResetYield()
{
    MprThread   *tp;

    tp = mprGetCurrentThread();
    tp->stickyYield = 0;
    tp->yielded = 0;
}


/*
    Pause until all threads have yielded. Called by the GC marker only.
    MOB - this functions differently if parallel. If so, then it will abort waiting. If !parallel, it waits for all
    threads to yield.
 */
static int syncThreads(int timeout)
{
    MprThreadService    *ts;
    MprThread           *tp;
    MprTime             mark;
    int                 i, allYielded;

    ts = MPR->threadService;
    LOG(7, "syncThreads: wait for threads to yield, timeout %d", timeout);
    mark = mprGetTime();
    if (mprGetDebugMode()) {
        timeout = timeout * 500;
    }
    do {
        allYielded = 1;
        mprLock(ts->mutex);
        for (i = 0; i < ts->threads->length; i++) {
            tp = (MprThread*) mprGetItem(ts->threads, i);
            if (!tp->yielded) {
                allYielded = 0;
                break;
            }
        }
        mprUnlock(ts->mutex);
        if (allYielded) {
            break;
        }
        LOG(7, "syncThreads: waiting for threads to yield");
        //  MOB -- should have a longer nap here. Should not matter if this is big
        mprWaitForCond(ts->cond, 20);

    } while (!allYielded && mprGetElapsedTime(mark) < timeout);

    mprAssert(allYielded);
    LOG(7, "syncThreads: all yielded %d", allYielded);
    return (allYielded) ? 1 : 0;
}


/*
    Resume all yielded threads. Called by the GC marker only.
 */
void mprResumeThreads()
{
    MprThreadService    *ts;
    MprThread           *tp;
    int                 i;

    ts = MPR->threadService;
    LOG(7, "mprResumeThreadsAfterGC sync");

    mprLock(ts->mutex);
    for (i = 0; i < ts->threads->length; i++) {
        tp = (MprThread*) mprGetItem(ts->threads, i);
        if (tp->yielded) {
            if (!tp->stickyYield) {
                tp->yielded = 0;
            }
            mprSignalCond(tp->cond);
        }
    }
    mprUnlock(ts->mutex);
}


void mprVerifyMem()
{
#if BLD_MEMORY_DEBUG
    MprRegion   *region;
    MprMem      *mp;
    MprFreeMem  *freeq, *fp;
    uchar       *ptr;
    int         i, usize;
    
    lockHeap();
    for (region = heap->regions; region; region = region->next) {
        for (mp = region->start; mp; mp = GET_NEXT(mp)) {
            CHECK(mp);
        }
    }
    for (i = 0, freeq = heap->freeq; freeq != heap->freeEnd; freeq++, i++) {
        for (fp = freeq->next; fp != freeq; fp = fp->next) {
            mp = (MprMem*) fp;
            CHECK(mp);
            mprAssert(GET_GEN(mp) == heap->eternal);
            mprAssert(IS_FREE(mp));
            if (heap->verify) {
                ptr = (uchar*) ((char*) mp + sizeof(MprFreeMem));
                usize = GET_SIZE(mp) - sizeof(MprFreeMem);
                if (HAS_MANAGER(mp)) {
                    usize -= sizeof(MprManager);
                }
                for (i = 0; i < usize; i++) {
                    if (ptr[i] != 0xFE) {
                        mprStaticError("Free memory block %x has been modified at offset %d (MprBlk %x, seqno %d)\n"
                                       "Memory was last allocated by %s", GET_PTR(mp), i, mp, mp->seqno, mp->name);
                    }
                }
            }
        }
    }
    unlockHeap();
#endif
}


/*
    WARNING: Caller must be locked so that the sweeper will not free this block. 
 */
int mprIsDead(cvoid *ptr)
{
    MprMem      *mp;

    mp = GET_MEM(ptr);
    if (VALID_BLK(mp)) {
        return GET_GEN(mp) == heap->dead;
    }
    return 0;
}


/*
    WARNING: Caller must be locked so that the sweeper will not free this block. 
 */
void mprRevive(cvoid *ptr)
{
    MprMem      *mp;

    mp = GET_MEM(ptr);
    SET_GEN(mp, heap->active);
    SET_MARK(mp, heap->eternal);
}


bool mprEnableGC(bool on)
{
    bool    old;

    old = heap->enabled;
    heap->enabled = on;
    return old;
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
    heap->iteration++;
    LOG(2, "GC: Iteration %d, active %d, stale %d, dead %d, eternal %d",
        heap->iteration, heap->active, heap->stale, heap->dead, heap->eternal);
}


void mprAddRoot(void *root)
{
    /*
        Need to use root lock because mprAddItem may allocate
     */
    mprSpinLock(&heap->rootLock);
    mprAddItem(heap->roots, root);
    mprSpinUnlock(&heap->rootLock);
}


void mprRemoveRoot(void *root)
{
    int     index;

    mprSpinLock(&heap->rootLock);
    index = mprRemoveItem(heap->roots, root);
    /*
        RemoveItem copies down. If the item was equal or before the current marker root, must adjust the marker rootIndex
        so we don't skip a root.
     */
    if (index <= heap->rootIndex && heap->rootIndex > 0) {
        heap->rootIndex--;
    }
    mprSpinUnlock(&heap->rootLock);
}


static void *getNextRoot()
{
    void    *root;

    mprSpinLock(&heap->rootLock);
    root = mprGetNextItem(heap->roots, &heap->rootIndex);
    mprSpinUnlock(&heap->rootLock);
    return root;
}


/****************************************************** Debug *************************************************************/

#if BLD_MEMORY_STATS
static void printQueueStats() 
{
    MprFreeMem  *freeq;
    int         i;

    printf("\nFree Queue Stats\n Bucket                     Size   Count\n");
    for (i = 0, freeq = heap->freeq; freeq != heap->freeEnd; freeq++, i++) {
        if (freeq->info.stats.count) {
            printf("%7d %24d %7d\n", i, freeq->info.stats.minSize, freeq->info.stats.count);
        }
    }
}


static void printGCStats()
{
    MprRegion   *region;
    MprMem      *mp;
    ssize       size, bytes[MPR_MAX_GEN + 2];
    int         regionCount, i, freeCount, allocatedCount, counts[MPR_MAX_GEN + 2], free, gen;

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
            size = GET_SIZE(mp);
            gen = GET_GEN(mp);
            if (IS_FREE(mp)) {
                freeCount++;
                counts[free]++;
                bytes[free] += size;
            } else {
                counts[gen]++;
                bytes[gen] += size;
                allocatedCount++;
            }
        }
        regionCount++;
        printf("  Region %d is %d bytes, has %d allocated %d free\n", regionCount, (int) region->size, 
            allocatedCount, freeCount);
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
    ssize   size;

    size = GET_SIZE(mp);
    mprAssert(mp->magic == MPR_ALLOC_MAGIC);
    mprAssert(size > 0);
    return (mp->magic == MPR_ALLOC_MAGIC) && (size > 0);
}


void mprCheckBlock(MprMem *mp)
{
    if (mp->magic != MPR_ALLOC_MAGIC || GET_SIZE(mp) <= 0) {
        mprStaticError("Memory corruption in memory block %x (MprBlk %x, seqno %d)\n"
            "This most likely happend earlier in the program execution", GET_PTR(mp), mp, mp->seqno);
    }
}


static void checkFreeMem(MprMem *mp)
{
    uchar   *ptr;
    int     usize, i;

    if (heap->verify) {
        ptr = (uchar*) ((char*) mp + sizeof(MprFreeMem));
        usize = GET_SIZE(mp) - sizeof(MprFreeMem);
        if (HAS_MANAGER(mp)) {
            usize -= sizeof(MprManager);
        }
        for (i = 0; i < usize; i++) {
            if (ptr[i] != 0xFE) {
                mprStaticError("Free memory block %x has been modified at offset %d (MprBlk %x, seqno %d)\n"
                    "Memory was last allocated by %s", GET_PTR(mp), i, mp, mp->seqno, mp->name);
                break;
            }
        }
    }
}


static void breakpoint(MprMem *mp) 
{
    if (mp == stopAlloc || mp->seqno == stopSeqno) {
        mprBreakpoint();
    }
}

void *mprSetName(void *ptr, cchar *name) 
{
    MPR_GET_MEM(ptr)->name = name;
    return ptr;
}


#else
void mprCheckBlock(MprMem *mp) {}
#undef mprSetName
void *mprSetName(void *ptr, cchar *name) { return 0;}
#endif

/******************************************************* Misc *************************************************************/

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

        cmd[0] = CTL_HW;
        cmd[1] = HW_NCPU;
        len = sizeof(ap->numCpu);
        if (sysctl(cmd, 2, &ap->numCpu, &len, 0, 0) < 0) {
            ap->numCpu = 1;
        }
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
    CHECK(mp);
    return GET_USIZE(mp);
}


int mprGetHeapFlags()
{
    return heap->flags;
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
    MprMem      *mp;

    mp = GET_MEM(ptr);
#if BLD_WIN
    if (isBadWritePtr(mp, sizeof(MprMem))) {
        return 0;
    }
    if (!VALID_BLK(GET_MEM(ptr)) {
        return 0;
    }
    if (isBadWritePtr(ptr, GET_SIZE(mp))) {
        return 0;
    }
    return 0;
#else
#if BLD_DEBUG
    return ptr && mp->magic == MPR_ALLOC_MAGIC && GET_SIZE(mp) > 0;
#else
    return ptr && GET_SIZE(mp) > 0;
#endif
#endif
}


static void dummyManager(void *ptr, int flags) 
{
}


void *mprSetManager(void *ptr, void *manager)
{
    MprMem      *mp;

    mp = GET_MEM(ptr);
    mprAssert(HAS_MANAGER(mp));
    if (HAS_MANAGER(mp)) {
        if (!manager) {
            manager = dummyManager;
        }
        SET_MANAGER(mp, manager);
    }
    return ptr;
}


#if BLD_MEMORY_STATS && FUTURE
static void showMem(MprMem *mp)
{
    char    *gen, *mark, buf[MPR_MAX_STRING];
    int     g, m;

    g = GET_GEN(mp);
    m = GET_MARK(mp);
    if (g == heap->eternal) {
        gen = "eternal";
    } else if (g == heap->active) {
        gen = "active";
    } else if (g == heap->stale) {
        gen = "stale";
    } else if (g == heap->dead) {
        gen = "dead";
    } else {
        gen = "INVALID";
    }
    if (m == heap->eternal) {
        mark = "eternal";
    } else if (m == heap->active) {
        mark = "active";
    } else if (m == heap->stale) {
        mark = "stale";
    } else if (m == heap->dead) {
        mark = "dead";
    } else {
        mark = "INVALID";
    }
    sprintf(buf, "Mem 0x%p, size %d, free %d, mgr %d, last %d, prior 0x%p, gen \"%s\", mark \"%s\"\n",
        mp, (int) GET_SIZE(mp), (int) IS_FREE(mp), (int) HAS_MANAGER(mp), (int) IS_LAST(mp), GET_PRIOR(mp), gen, mark);
#if BLD_WIN
    OutputDebugString(buf);
#else
    print(buf);
#endif
}
#endif


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
