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
#define GET_REGION(mp)          ((MprRegion*) (((char*) mp) - sizeof(MprRegion)))
#define GET_USIZE(mp)           (mp->size - sizeof(MprMem) - (mp->hasManager * sizeof(void*)))

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

/*
    WARN: this will reset pad words too.
 */
#define RESET_MEM(mp)           if (mp != GET_MEM(MPR)) { memset(GET_PTR(mp), 0xFE, mp->size - sizeof(MprMem)); } else
#define SET_MAGIC(mp)           if (1) { (mp)->magic = MPR_ALLOC_MAGIC; } else
#define SET_SEQ(mp)             if (1) { (mp)->seqno = heap->nextSeqno++; } else
#define INIT_BLK(mp, len)       if (1) { SET_MAGIC(mp); SET_SEQ(mp); mp->size = len; mp->name = NULL; } else
#define VALID_BLK(mp)           validBlk(mp)

#else
#define BREAKPOINT(mp)
#define CHECK(mp)           
#define CHECK_PTR(mp)           
#define RESET_MEM(mp)           
#define SET_MAGIC(mp)
#define SET_SEQ(mp)           
#define INIT_BLK(mp, len)       if (1) { mp->size = len; } else
#define VALID_BLK(mp)           1
#endif

#if BLD_MEMORY_STATS
    #define INC(field)          if (1) { heap->stats.field++; } else 
#else
    #define INC(field)
#endif

#define lockHeap()              mprSpinLock(&heap->spin);
#define unlockHeap()            mprSpinUnlock(&heap->spin);

#define percent(a,b) ((int) ((a) * 100 / (b)))

#if !MACOSX && !FREEBSD
    #define NEED_FFSL 1
    #if WIN
    #elif BLD_HOST_CPU_ARCH == MPR_CPU_IX86 || BLD_HOST_CPU_ARCH == MPR_CPU_IX64
        #define USE_FFSL_ASM_X86 1
    #endif
    static inline int ffsl(ulong word);
    static inline int flsl(ulong word);
#elif BSD_EMULATION
    #define ffsl FFSL
    #define flsl FLSL
    #define NEED_FFSL 1
    #define USE_FFSL_ASM_X86 1
    static inline int ffsl(ulong word);
    static inline int flsl(ulong word);
#endif

/********************************** Data **************************************/

Mpr             *MPR;
static MprHeap  *heap;
static int      padding[] = { 0, MANAGER_SIZE };

/***************************** Forward Declarations ***************************/

static void allocException(size_t size, bool granted);
static void checkGarbage();
static void deq(MprFreeMem *fp);
static void enq(MprFreeMem *head, MprFreeMem *fp);
static void freeBlock(MprMem *mp);
static MprMem *growHeap(size_t size);
static int getQueueIndex(size_t size, int roundup);
static void getSystemInfo();
static int initFree();
static void initGen();
static void linkFreeBlock(MprMem *mp); 
static void mark();
static void markRoots();
static void nextGen();
static MprMem *searchFree(size_t size, int *indexp);
static MprMem *splitBlock(MprMem *mp, size_t required, int qspare);
static void sweep();
static void synchronize();
static void unlinkFreeBlock(MprFreeMem *fp);

#if MPR_GC_WORKERS >= 1
static void marker(void *unused, MprWorker *worker);
#endif
#if MPR_GC_WORKERS >= 2
static void sweeper(void *unused, MprWorker *worker);
#endif

#if BLD_WIN_LIKE
    static int winPageModes(int flags);
#endif
#if BLD_MEMORY_DEBUG
    static void breakpoint(MprMem *mp);
    static int validBlk(MprMem *mp);
#endif
#if BLD_MEMORY_STATS
    static MprFreeMem *getQueue(size_t size);
    static void printQueueStats();
    static void printGCStats();
#endif

void checkPrior(MprMem *mp)
{
#if CHECK_PRIOR
    MprMem  *p;

    for (p = mp; p; p = p->prior) {
        CHECK(p);
    }
#endif
}


/************************************* Code ***********************************/
/*
    Initialize the memory subsystem
 */
Mpr *mprCreateMemService(MprMemNotifier cback, MprManager manager)
{
    MprRegion   *region;
    MprHeap     initHeap;
    MprMem      *mp, *spare;
    size_t      regionSize, size, mprSize;

    heap = &initHeap;
    memset(heap, 0, sizeof(MprHeap));
    heap->stats.maxMemory = MAXINT;
    heap->stats.redLine = MAXINT / 100 * 99;
    mprInitSpinLock(heap, &heap->spin);

    /*
        Hand-craft the Mpr structure this include the MprRegion and MprMem headers
     */
    mprSize = MPR_ALLOC_ALIGN(sizeof(MprMem) + sizeof(Mpr) + (MANAGER_SIZE * sizeof(void*)));
    regionSize = MPR_ALLOC_ALIGN(sizeof(MprRegion));
    size = max(mprSize + regionSize, MPR_REGION_MIN_SIZE);

    if ((region = (MprRegion*) mprVirtAlloc(size, MPR_MAP_READ | MPR_MAP_WRITE)) == NULL) {
        return NULL;
    }
    mp = region->start = (MprMem*) (((char*) region) + regionSize);
    region->size = size;

    MPR = (Mpr*) GET_PTR(mp);
    INIT_BLK(mp, mprSize);
    mp->gen = MPR_GEN_ETERNAL;
    mp->hasManager = 1;
    SET_MANAGER(mp, manager);
    mprSetName(MPR, "Mpr");

    spare = (MprMem*) (((char*) mp) + mprSize);
    INIT_BLK(spare, size - regionSize - mprSize);
    spare->last = 1;
    spare->prior = mp;

    heap = &MPR->heap;
    heap->notifier = cback;
    heap->nextSeqno = 1;
    heap->chunkSize = MPR_REGION_MIN_SIZE;
    heap->stats.maxMemory = MAXINT;
    heap->stats.redLine = MAXINT / 100 * 99;
    heap->newQuota = MPR_NEW_QUOTA;
    heap->regions = region;

    //  MOB - re-enable
    heap->enabled = 0;

    heap->stats.bytesAllocated += size;
    mprInitSpinLock(heap, &heap->spin);
    getSystemInfo();

    initFree();
    initGen();
    linkFreeBlock(spare);

#if UNUSED
    heap->cond = mprCreateCond(heap);
#endif
    heap->roots = mprCreateList(MPR);
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
    if (heap->enabled) {
        printGCStats();
    }
}


void *mprAllocBlock(MprCtx IGNORED, size_t usize, int flags)
{
    MprMem      *mp;
    void        *ptr;
    size_t      maxBlock, size;
    int         bucket, group, index, padWords;

    mprAssert(usize > 0);

    if (usize == 0) {
        usize = 1;
    }
    padWords = padding[flags & MPR_ALLOC_PAD_MASK];
    size = MPR_ALLOC_ALIGN(usize + sizeof(MprMem) + (padWords * sizeof(void*)));
    
    if ((mp = searchFree(size, &index)) == NULL) {
        if ((mp = growHeap(size)) == NULL) {
            return NULL;
        }
    }
    BREAKPOINT(mp);
    mprAssert(mp->size >= size);

    if (mp->size >= (size + MPR_ALLOC_MIN_SPLIT)) {
        index = getQueueIndex(size, 1);
        group = index / MPR_ALLOC_NUM_BUCKETS;
        bucket = index % MPR_ALLOC_NUM_BUCKETS;
        maxBlock = (((size_t) 1 ) << group | (((size_t) bucket) << (max(0, group - 1)))) << MPR_ALIGN_SHIFT;
        maxBlock += sizeof(MprMem);
        if (mp->size > maxBlock) {
            splitBlock(mp, size, 1);
        }
    }
    ptr = GET_PTR(mp);
    if (flags & MPR_ALLOC_ZERO) {
        memset(ptr, 0, usize);
    }
    if (flags & MPR_ALLOC_MANAGER) {
        mp->hasManager = 1;
        *((MprManager*) PAD_PTR(mp, padWords)) = NULL;
    }
    CHECK(mp);
    mp->gen = heap->active;
    heap->newCount++;
checkPrior(mp);
    return ptr;
}


/*
    Free a block of memory
 */
void mprFree(void *ptr)
{
    MprMem  *mp;

    if (likely(ptr)) {
        mp = GET_MEM(ptr);
checkPrior(mp);
        CHECK(mp);
        mprAssert(!mp->free);
        if (unlikely(mp->hasManager)) {
// printf("FREE CALL MANAGER %d %s\n", mp->seqno, basename((char*) mp->name));
            GET_MANAGER(mp)(GET_PTR(mp), MPR_MANAGE_FREE);
            mp->hasManager = 0;
        } else {
// printf("FREE %d %s\n", mp->seqno, basename((char*) mp->name));
        }
        mp->gen = heap->stale;
    }
}


#if BLD_DEBUG
void mprFreeBlock(void *ptr, cchar *loc)
{
    size_t      size;

    size = GET_USIZE(GET_MEM(ptr));
    mprFree(ptr);
    strncpy(ptr, loc, size);
}
#endif


void *mprRealloc(MprCtx IGNORED, void *ptr, size_t usize)
{
    MprMem      *mp, *newb;
    void        *newptr;
    int         flags;

    mprAssert(usize > 0);

    if (ptr == 0) {
        return mprAllocBlock(IGNORED, usize, 0);
    }
    mp = GET_MEM(ptr);
    CHECK(mp);
    if (usize <= GET_USIZE(mp)) {
        return ptr;
    }
    flags = mp->hasManager ? MPR_ALLOC_MANAGER : 0;
    if ((newptr = mprAllocBlock(IGNORED, usize, flags)) == NULL) {
        return 0;
    }
    newb = GET_MEM(newptr);
    memcpy(newptr, ptr, mp->size - sizeof(MprMem));
checkPrior(mp);
    freeBlock(mp);
checkPrior(mp);
checkPrior(newb);
    return newptr;
}


void *mprMemdup(MprCtx IGNORED, cvoid *ptr, size_t usize)
{
    char    *newp;

    if ((newp = mprAllocBlock(IGNORED, usize, 0)) != 0) {
        memcpy(newp, ptr, usize);
    }
    return newp;
}


int mprMemcmp(cvoid *s1, int s1Len, cvoid *s2, int s2Len)
{
    int     len, rc;

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
int mprMemcpy(void *dest, int destMax, cvoid *src, int nbytes)
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


#if BLD_WIN_LIKE
//  MOB - remove arg
Mpr *mprGetMpr(MprCtx IGNORED)
{
    return MPR;
}
#endif


int mprGetPageSize()
{
    return heap->pageSize;
}


size_t mprGetBlockSize(cvoid *ptr)
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


void mprSetMemCollect(MprMemCollect collect)
{
    heap->collect = collect;
}


void mprSetMemLimits(MprCtx IGNORED, int redLine, int maxMemory)
{
    if (redLine > 0) {
        heap->stats.redLine = redLine;
    }
    if (maxMemory > 0) {
        heap->stats.maxMemory = maxMemory;
    }
}


void mprSetMemPolicy(MprCtx IGNORED, int policy)
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
    
    heap->freeEnd = &heap->freeq[MPR_ALLOC_NUM_GROUPS * MPR_ALLOC_NUM_BUCKETS];
    for (freeq = heap->freeq; freeq != heap->freeEnd; freeq++) {
#if BLD_MEMORY_STATS
        size_t      bit, size, groupBits, bucketBits;
        int         index, group, bucket;
        /*
            NOTE: skip the buckets with MSB == 0 (round up)
         */
        index = (freeq - heap->freeq);
        group = index / MPR_ALLOC_NUM_BUCKETS;
        bucket = index % MPR_ALLOC_NUM_BUCKETS;

        bit = (group != 0);
        groupBits = bit << (group + MPR_ALLOC_BUCKET_SHIFT - 1);
        bucketBits = ((size_t) bucket) << (max(0, group - 1));

        size = groupBits | bucketBits;
        freeq->stats.minSize = size << MPR_ALIGN_SHIFT;
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
    heap->mark = heap->active + 1;
}


static void nextGen() 
{
    int     active;

    active = (heap->active + 1) % MPR_MAX_GEN;
    heap->active = active;
    heap->stale = (active - 1 + MPR_MAX_GEN) % MPR_MAX_GEN;
    heap->dead = (active - 2 + MPR_MAX_GEN) % MPR_MAX_GEN;
    heap->mark = active + 1;
}


static int getQueueIndex(size_t size, int roundup)
{   
    size_t      usize, asize;
    int         aligned, bucket, group, index, msb;
    
    mprAssert(MPR_ALLOC_ALIGN(size) == size);

    /*
        Allocate based on user sizes (sans header). This permits block searches to avoid scanning the next 
        highest queue for common block sizes: eg. 1K.
     */
    usize = (size - sizeof(MprMem));
    asize = usize >> MPR_ALIGN_SHIFT;

    //  Zero based most significant bit
    msb = flsl(asize) - 1;

    group = max(0, msb - MPR_ALLOC_BUCKET_SHIFT + 1);
    mprAssert(group < MPR_ALLOC_NUM_GROUPS);

    bucket = (asize >> max(0, group - 1)) & (MPR_ALLOC_NUM_BUCKETS - 1);
    mprAssert(bucket < MPR_ALLOC_NUM_BUCKETS);

    index = (group * MPR_ALLOC_NUM_BUCKETS) + bucket;
    mprAssert(index < (heap->freeEnd - heap->freeq));
    
#if BLD_MEMORY_STATS
    mprAssert(heap->freeq[index].stats.minSize <= usize && usize < heap->freeq[index + 1].stats.minSize);
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
            size_t mask = (((size_t) 1) << (msb - MPR_ALLOC_BUCKET_SHIFT)) - 1;
            aligned = (asize & mask) == 0;
            if (!aligned) {
                index++;
            }
        }
    }
    return index;
}


#if BLD_MEMORY_STATS
static MprFreeMem *getQueue(size_t size)
{   
    MprFreeMem  *freeq;
    int         index;
    
    index = getQueueIndex(size, 0);
    freeq = &heap->freeq[index];
    return freeq;
}
#endif


static MprMem *searchFree(size_t size, int *indexp)
{
    MprFreeMem  *freeq, *fp;
    size_t      groupMap, bucketMap;
    int         bucket, baseGroup, group, index;
    
    *indexp = index = getQueueIndex(size, 1);
    baseGroup = index / MPR_ALLOC_NUM_BUCKETS;
    bucket = index % MPR_ALLOC_NUM_BUCKETS;

    lockHeap();
    INC(requests);
    
    /* Mask groups lower than the base group */
    groupMap = heap->groupMap & ~((((size_t) 1) << baseGroup) - 1);
    while (groupMap) {
        group = ffsl(groupMap) - 1;
        if (groupMap & ((((size_t) 1) << group))) {
            bucketMap = heap->bucketMap[group];
            if (baseGroup == group) {
                bucketMap &= ~((((size_t) 1) << bucket) - 1);
            }
            while (bucketMap) {
                bucket = ffsl(bucketMap) - 1;
                index = (group * MPR_ALLOC_NUM_BUCKETS) + bucket;
                freeq = &heap->freeq[index];
                if (freeq->next != freeq) {
                    fp = freeq->next;
                    unlinkFreeBlock(fp);
                    INC(reuse);
                    unlockHeap();
                    CHECK(fp);
                    return (MprMem*) fp;
                }
                bucketMap &= ~(((size_t) 1) << bucket);
                heap->bucketMap[group] &= ~(((size_t) 1) << bucket);
            }
            groupMap &= ~(((size_t) 1) << group);
            heap->groupMap &= ~(((size_t) 1) << group);

            /*
                Put test here so we don't do this in the common path where the block can be satisfied from the free list
             */
            unlockHeap();
            checkGarbage();
            lockHeap();
        }
    }
    unlockHeap();
    checkGarbage();
    return NULL;
}


static void enq(MprFreeMem *head, MprFreeMem *fp)
{
    CHECK(fp);
    mprAssert(fp != head);
    mprAssert(fp->next == NULL);
    mprAssert(fp->prev == NULL);

    fp->next = head->next;
    fp->prev = head;
    head->next->prev = fp;
    head->next = fp;

    mprAssert(fp != fp->next);
    mprAssert(fp != fp->prev);
}


static void deq(MprFreeMem *fp)
{
    CHECK(fp);

    fp->prev->next = fp->next;
    fp->next->prev = fp->prev;
#if BLD_MEMORY_DEBUG
    fp->next = fp->prev = NULL;
#endif
}


/*
    Add a block to a free q. Must be called locked.
 */
static void linkFreeBlock(MprMem *mp) 
{
    MprFreeMem  *freeq, *fp;
    int         index, group, bucket;

    //  MOB - just for free list
    mp->dynamic = 0;
    mp->visited = 0;
    mp->builtin = 0;

    mp->free = 1;
    mp->gen = heap->eternal;
    mp->hasManager = 0;
    fp = (MprFreeMem*) mp;
#if BLD_DEBUG
    fp->next = fp->prev = NULL;
#endif
    index = getQueueIndex(mp->size, 0);
    group = index / MPR_ALLOC_NUM_BUCKETS;
    bucket = index % MPR_ALLOC_NUM_BUCKETS;
    heap->groupMap |= (((size_t) 1) << group);
    heap->bucketMap[group] |= (((size_t) 1) << bucket);

    freeq = &heap->freeq[index];
    enq(freeq, fp);

    heap->stats.bytesFree += mp->size;
#if BLD_MEMORY_STATS
    freeq->stats.count++;
#endif
}


/*
    Remove a block from a free q. Must be called locked.
 */
static void unlinkFreeBlock(MprFreeMem *fp) 
{
    MprMem  *mp;
    deq(fp);
    mp = (MprMem*) fp;
    heap->stats.bytesFree -= mp->size;
    mp->free = 0;
#if BLD_MEMORY_STATS
{
    MprFreeMem *freeq = getQueue(mp->size);
    freeq->stats.count--;
    mprAssert(freeq->stats.count >= 0);
}
#endif
}


#if BLD_MEMORY_DEBUG
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


static void freeBlock(MprMem *mp)
{
    MprRegion   *region;
    MprMem      *prev, *next, *after;
    size_t      size;

    BREAKPOINT(mp);

    size = mp->size;
    after = next = prev = NULL;
    RESET_MEM(mp);
    
    /*
        Coalesce with next if it is also free.
     */
    lockHeap();
    next = GET_NEXT(mp);
    if (next && next->free) {
        BREAKPOINT(next);
        unlinkFreeBlock((MprFreeMem*) next);
        if ((after = GET_NEXT(next)) != NULL) {
            mprAssert(after->prior == next);
            after->prior = mp;
        } else {
            mp->last = 1;
        }
        size += next->size;
        mp->size = size;
        INC(joins);
    }
    /*
        Coalesce with previous if it is also free.
     */
    prev = mp->prior;
    if (prev && prev->free) {
        BREAKPOINT(prev);
        unlinkFreeBlock((MprFreeMem*) prev);
        if ((after = GET_NEXT(mp)) != NULL) {
            mprAssert(after->prior == mp);
            after->prior = prev;
        } else {
            prev->last = 1;
        }
        size += prev->size;
        prev->size = size;
        mp = prev;
        INC(joins);
    }
#if BLD_CC_MMU
    /*
        Release entire regions back to the O/S. (Region has no prior and is also last when complete)
     */
#if BLD_MEMORY_DEBUG
    if (mp->prior == NULL) {
        mprAssert(isFirst(mp) || mp->seqno == 0);
    }
#endif
    if (mp->prior == NULL && mp->last && heap->stats.bytesFree > (MPR_REGION_MIN_SIZE * 4)) {
        INC(unpins);
        unlockHeap();
        region = GET_REGION(mp);
        mprVirtFree(region, region->size);
    } else
#endif
    {
        linkFreeBlock(mp);
        unlockHeap();
    }
}


/*
    Split a block. Required specifies the number of bytes needed in the block. If swap, then put mp back on the free
    queue instead of the second half.
 */
static MprMem *splitBlock(MprMem *mp, size_t required, int qspare)
{
    MprMem      *spare, *after;
    size_t      size, extra;

    mprAssert(mp);
    mprAssert(required > 0);

    CHECK(mp);
    size = mp->size;
    extra = size - required;
    mprAssert(extra >= MPR_ALLOC_MIN_SPLIT);

    spare = (MprMem*) ((char*) mp + required);
    INIT_BLK(spare, extra);
    spare->last = mp->last;
    spare->prior = mp;

    lockHeap();
    mp->size = required;
    mp->last = 0;
    if ((after = GET_NEXT(spare)) != NULL) {
        after->prior = spare;
    }
    INC(splits);
    if (qspare) {
        linkFreeBlock(spare);
    }
    CHECK(spare);
    CHECK(mp);
checkPrior(mp);
checkPrior(spare);
    unlockHeap();
    return (qspare) ? NULL : spare;
}


/*
    Grow the heap and return a block of the required size (unqueued)
 */
static MprMem *growHeap(size_t required)
{
    MprRegion   *region;
    MprMem      *mp;
    size_t      size, rsize;

    mprAssert(required > 0);

    rsize = MPR_ALLOC_ALIGN(sizeof(MprRegion));
    size = max(required + rsize, (size_t) heap->chunkSize);
    size = MPR_PAGE_ALIGN(size, heap->pageSize);

    if ((region = mprVirtAlloc(size, MPR_MAP_READ | MPR_MAP_WRITE)) == NULL) {
        return 0;
    }
    lockHeap();
    region->next = heap->regions;
    region->size = size;
    region->start = (MprMem*) (((char*) region) + rsize);
    heap->regions = region;

    mp = (MprMem*) region->start;
    INIT_BLK(mp, size - rsize);
    mp->last = 1;
    unlockHeap();
    CHECK(mp);
    return mp;
}


static void allocException(size_t size, bool granted)
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
        (heap->notifier)(granted ? MPR_ALLOC_LOW : MPR_ALLOC_DEPLETED, size);
    }
    heap->stats.inMemException = 0;

    if (!granted) {
        switch (heap->allocPolicy) {
        case MPR_ALLOC_POLICY_EXIT:
            mprError(MPR, "Application exiting due to memory allocation failure.");
            mprTerminate(MPR, 0);
            break;
        case MPR_ALLOC_POLICY_RESTART:
            mprError(MPR, "Application restarting due to memory allocation failure.");
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
void *mprVirtAlloc(size_t size, int mode)
{
    size_t      used;
    void        *ptr;

    used = mprGetUsedMemory();
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
    lockHeap();
    INC(allocs);
    heap->stats.bytesAllocated += size;
    unlockHeap();
    return ptr;
}


void mprVirtFree(void *ptr, size_t size)
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
        size_t  len;

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
    int             fd, len;

    heap->stats.ram = MAXINT64;
    if ((fd = open("/proc/meminfo", O_RDONLY)) >= 0) {
        if ((len = read(fd, buf, sizeof(buf) - 1)) > 0) {
            buf[len] = '\0';
            if ((cp = strstr(buf, "MemTotal:")) != 0) {
                for (; *cp && !isdigit((int) *cp); cp++) {}
                heap->stats.ram = ((size_t) atoi(cp) * 1024);
            }
        }
        close(fd);
    }
#endif
#if MACOSX || FREEBSD
    size_t      ram, usermem, len;
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
    heap->stats.rss = mprGetUsedMemory();
    return &heap->stats;
}


size_t mprGetUsedMemory()
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

static inline int ffsl(ulong x)
{
    long    r;

    asm("bsf %1,%0\n\t"
        "jnz 1f\n\t"
        "mov $-1,%0\n"
        "1:" : "=r" (r) : "rm" (x));
    return (int) r + 1;
}


static inline int flsl(ulong x)
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
static inline int ffsl(ulong word)
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
static inline int flsl(ulong word)
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
        index = (freeq - heap->freeq);
        printf("%7d %24d %7d\n", i, freeq->stats.minSize, freeq->stats.count);
    }
}


static void printGCStats()
{
    MprRegion   *region;
    MprMem      *mp;
    int         regionCount, i, freeCount, allocatedCount, counts[MPR_MAX_GEN + 2], bytes[MPR_MAX_GEN + 2], free;

    for (i = 0; i < (MPR_MAX_GEN + 2); i++) {
        counts[i] = bytes[i] = 0;
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
    printf("  Eternal generation has %9d blocks, %12d bytes\n", counts[heap->eternal], bytes[heap->eternal]);
    printf("  Stale generation has   %9d blocks, %12d bytes\n", counts[heap->stale], bytes[heap->stale]);
    printf("  Active generation has  %9d blocks, %12d bytes\n", counts[heap->active], bytes[heap->active]);
    printf("  Dead generation has    %9d blocks, %12d bytes\n", counts[heap->dead], bytes[heap->dead]);
    printf("  Free generation has    %9d blocks, %12d bytes\n", counts[free], bytes[free]);
}
#endif /* BLD_MEMORY_STATS */


void mprPrintMemReport(cchar *msg, int detail)
{
#if BLD_MEMORY_STATS
    MprMemStats   *ap;

    ap = mprGetMemStats();

    printf("\n\nMPR Memory Report %s\n", msg);
    printf("------------------------------------------------------------------------------------------\n");
    printf("  Total memory        %14ld K\n",            mprGetUsedMemory());
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
    mprAssert(VALID_BLK(mp));
}


static void breakpoint(MprMem *mp) 
{
    if (mp == stopAlloc || mp->seqno == stopSeqno) {
        mprBreakpoint();
    }
}
#endif

/***************************************************** Garbage Colllector *************************************************/
/*
    Collect all garbage. This conducts three sweeps of garbage.
 */
void mprCollectAllGarbage()
{
    if (heap->enabled) {
        mprPrintMemReport("GC", 0);
    }
    mprCollectGarbage();
    mprCollectGarbage();
    mprCollectGarbage();
    if (heap->enabled) {
        mprPrintMemReport("GC", 0);
    }
}


#if MPR_GC_WORKERS > 0
static void startMemWorkers()
{
#if MPR_GC_WORKERS == 2
    heap->hasSweeper = 1;
    mprStartWorker(NULL, marker, NULL);
    mprStartWorker(NULL, sweeper, NULL);
#elif MPR_GC_WORKERS == 1
    mprStartWorker(NULL, marker, NULL);
#endif
}
#endif


/*
    Initiate a garbage collection. This can be called by any thread. If GC workers are configured, the call may return
    before GC has completed.
 */
void mprCollectGarbage()
{
    lockHeap();
    heap->newCount = 0;
    if (heap->collecting || !heap->enabled) {
        unlockHeap();
        return;
    }
    heap->collecting = 1;
    unlockHeap();

#if MPR_GC_WORKERS == 0
    mark();
#else
    startMemWorkers();
#if UNUSED
    if (heap->cond) {
        mprSignalCond(heap->cond);
    }
#endif
    mprYieldThread(NULL);
#endif
}


static void checkGarbage()
{
    if (heap->newCount >= heap->newQuota && !heap->collecting && heap->stats.bytesFree < MPR_GC_LOW_MEM) {
        mprCollectGarbage();
    }
}


/*
    Return true if there is time to do a garbage collection and if we will benefit from it.
 */
int mprIsTimeForGC(int timeTillNextEvent)
{
    //  MOB -- but what if GC already running
    if (timeTillNextEvent < MPR_MIN_TIME_FOR_GC) {
        return 0;
    }
    //  MOB - newCount is never incremented
    if (!heap->enabled || heap->newCount < heap->newQuota || heap->stats.bytesFree < MPR_GC_LOW_MEM) {
        return 0;
    }
    mprLog(NULL, 7, "Time for GC. Work done %d, time till next event %d", heap->newCount, timeTillNextEvent);
    return 1;
}


/*
    Marker synchronization point. At the end of each GC mark/sweep, all threads must rendezvous at the 
    synchronization point.  This happens infrequently and is essential to safely move to a new generation.
    All threads must yield to the marker (including sweeper)
 */
static void synchronize()
{
    heap->mustYield = 1;
    //  MOB - Need proper timeout here - should be short
    if (mprPauseForGCSync(120 * 1000)) {
        nextGen();
        heap->mustYield = 0;
        mprResumeThreadsAfterGC();
    } else {
        heap->mustYield = 0;
    }
}


static void mark()
{
    /*
        Yield this thread first so mprPauseForGCSync will not wait for self in synchronize()
     */
    mprYieldThread(NULL);
    markRoots();
    if (!heap->hasSweeper) {
        sweep();
    }
    synchronize();
    mprResumeThread(NULL);
    heap->collecting = 0;
}


static void sweep()
{
    MprRegion   *region;
    MprMem      *mp;
    
    if (!heap->enabled) {
        return;
    }
    heap->stats.sweepVisited = 0;
    heap->stats.swept = 0;

    for (region = heap->regions; region; region = region->next) {
        for (mp = region->start; mp; mp = GET_NEXT(mp)) {
            CHECK(mp);
            BREAKPOINT(mp);
            INC(sweepVisited);
            if (mp->gen == heap->dead) {
                INC(swept);
                if (unlikely(mp->hasManager)) {
                    //  TODO OPT - would be good to have a separate bit for free required
printf("SWEEP seqno %d, %s CALL MANAGER()\n", mp->seqno, basename((char*) mp->name));
                    GET_MANAGER(mp)(GET_PTR(mp), MPR_MANAGE_FREE);
                } else {
printf("SWEEP seqno %d, %s\n", mp->seqno, basename((char*) mp->name));
                }
#if FUTURE
                //  MOB - if required, could allow managers to re-activate blocks by calling mprKeep(ptr)
                //  This could set mp->gen and then test here if gen == dead and only free if so.
                freeBlock(mp);
#else
                RESET_MEM(mp);
#endif
            }
        }
    }
    printf("MARKED %d/%d, SWEPT %d/%d\n", heap->stats.marked, heap->stats.markVisited, 
        heap->stats.swept, heap->stats.sweepVisited);
}


static void markRoots()
{
    void    *root;
    int     index;

    heap->stats.markVisited = 0;
    heap->stats.marked = 0;

    mprMark(heap->roots);
    for (index = 0; (root = mprGetNextItem(heap->roots, &index)) != 0; ) {
        mprMark(root);
    }
}


void mprMarkBlock(cvoid *ptr)
{
    MprMem      *mp;

    mprAssert(ptr);
    mp = MPR_GET_MEM(ptr);
    CHECK(mp);
    INC(markVisited);

    if (mp->mark != heap->mark) {
        BREAKPOINT(mp);
        INC(marked);
        mp->mark = heap->mark;
        mprAssert(mp->gen != heap->dead);
        mp->gen = heap->active;
        if (mp->hasManager) {
            mprAssert(mp->seqno != -1);
            (GET_MANAGER(mp))((void*) ptr, MPR_MANAGE_MARK);
        }
    }
}


void mprEternalize(void *ptr)
{
    MprMem  *mp;
    int     save;

    if (ptr) {
        mp = GET_MEM(ptr);
        checkPrior(mp);
        CHECK(mp);

        if (mp->hasManager) {
            lockHeap();
            while (heap->collecting) {
                unlockHeap();
                mprYieldThread(NULL);
                lockHeap();
            }
            save = heap->mark;
            heap->mark = heap->eternal;
            mprMarkBlock(ptr);
            heap->mark = save;
            unlockHeap();
        } else {
            mp->gen = heap->eternal;
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
    if (ptr) {
        GET_MEM(ptr)->gen = heap->active;
    }
}


#if MPR_GC_WORKERS >= 1
static void marker(void *unused, MprWorker *worker)
{
    mark();
}
#endif


#if MPR_GC_WORKERS >= 2
static void sweeper(void *unused, MprWorker *worker) 
{
    sweep();
    mprYieldThread(NULL);
    mprResumeThread(NULL);
}
#endif


bool mprEnableGC(bool on)
{
    bool    old;

    old = heap->enabled;
    heap->enabled = on;
    return old;
}


int mprGCPending()
{
    return heap->mustYield;
}


//  MOB - can't use lockHeap, but this causes races with the marker
void mprAddRoot(void *ptr)
{
    mprAddItem(heap->roots, ptr);
}


//  MOB - can't use lockHeap, but this causes races with the marker
void mprRemoveRoot(void *ptr)
{
    mprRemoveItem(heap->roots, ptr);
}


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

