/**
    mprAlloc.c - Memory Allocator. 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#include    "mpr.h"

/******************************* Local Defines ********************************/

#define GET_BLK(ptr)            ((MprBlk*) (((char*) (ptr)) - MPR_ALLOC_HDR_SIZE))
#define GET_PTR(bp)             ((char*) (((char*) (bp)) + MPR_ALLOC_HDR_SIZE))

#define GET_SIZE(bp)            ((bp)->size)
#define GET_USIZE(bp)           (GET_SIZE(bp) - MPR_ALLOC_HDR_SIZE - (GET_PAD(bp) * sizeof(void*)))
#define SET_SIZE(bp, len)       if (1) { (bp)->size = len; } else

//  MOB - loose the size_t
#define GET_PAD(bp)             (((size_t) ((bp)->pad)))
#define SET_PAD(bp, padWords)   if (1) { (bp)->pad = padWords; } else
#define GET_PRIOR(bp)           ((bp)->prior)
#define SET_PRIOR_REF(bp, ref)  if (1) { (bp)->prior = (MprBlk*) (ref); } else

/*
    Trailing block. This is optional and will look like:
        Destructor
        Children
        Trailer
 */
#define PAD_PTR(bp, offset)     (((char*) bp) + GET_SIZE(bp) - ((offset) * sizeof(void*)))
#define INIT_LIST(bp)           if (1) { bp->next = bp->prev = bp; } else

#if BLD_MEMORY_DEBUG
#define TRAILER_SIZE            1
#define TRAILER_OFFSET          1
#define HAS_TRAILER(bp)         (GET_PAD(bp) >= TRAILER_OFFSET)
#define GET_TRAILER(bp)         (HAS_TRAILER(bp) ? (*((int*) PAD_PTR(bp, TRAILER_OFFSET))) : MPR_ALLOC_MAGIC)
#define SET_TRAILER(bp, v)      if (1) { *((int*) PAD_PTR(bp, TRAILER_OFFSET)) = v; } else
#else
#define TRAILER_SIZE            0
#define TRAILER_OFFSET          0
#define GET_TRAILER(bp)         MPR_ALLOC_MAGIC
#define SET_TRAILER(bp, v)         
#endif

/*
    Children take 2 words
 */
#define CHILDREN_SIZE           (TRAILER_SIZE + 2)
#define CHILDREN_OFFSET         (TRAILER_OFFSET + 2)
#define HAS_CHILDREN(bp)        (GET_PAD(bp) >= CHILDREN_OFFSET)
#define GET_CHILDREN(bp)        (HAS_CHILDREN(bp) ? ((MprBlk*) PAD_PTR(bp, CHILDREN_OFFSET)) : NULL)

#define DESTRUCTOR_SIZE         (CHILDREN_SIZE + 1)
#define DESTRUCTOR_OFFSET       (TRAILER_OFFSET + 3)
#define HAS_DESTRUCTOR(bp)      (GET_PAD(bp) >= DESTRUCTOR_OFFSET)
#define GET_DESTRUCTOR(bp)      (HAS_DESTRUCTOR(bp) ? (*(MprDestructor*) (PAD_PTR(bp, DESTRUCTOR_OFFSET))) : NULL)
#define SET_DESTRUCTOR(bp, fn)  if (1) { *((MprDestructor*) PAD_PTR(bp, DESTRUCTOR_OFFSET)) = fn; } else

#if BLD_MEMORY_DEBUG
#define BREAKPOINT(bp)          breakpoint(bp);
#define CHECK_BLK(bp)           check(bp)
#define CHECK_PTR(ptr)          CHECK_BLK(GET_BLK(ptr))

/*
    WARN: this will reset pad words too.
 */
#define RESET_MEM(bp)           if (bp != GET_BLK(MPR)) { \
                                    memset(GET_PTR(bp), 0xFE, GET_SIZE(bp) - MPR_ALLOC_HDR_SIZE); } else
#define SET_MAGIC(bp)           if (1) { (bp)->magic = MPR_ALLOC_MAGIC; } else
#define VALID_BLK(bp)           validBlk(bp)

/*
    Set this address to break when this address is allocated or freed
 */
static MprBlk *stopAlloc;
static int stopSeqno = -1;

#else
#define BREAKPOINT(bp)
#define CHECK_BLK(bp)           
#define CHECK_PTR(bp)           
#define RESET_MEM(bp)           
#define SET_MAGIC(bp)
#define VALID_BLK(bp)           1
#endif

#define lockHeap(heap)          mprSpinLock(&heap->spin);
#define unlockHeap(heap)        mprSpinUnlock(&heap->spin);

Mpr                 *MPR;
static MprHeap      *heap;

#define percent(a,b) ((int) ((a) * 100 / (b)))

/***************************** Forward Declarations ***************************/

static MprBlk *allocBlockFromHeap(size_t size);
static void allocException(size_t size, bool granted);
static void deq(MprBlk *bp);
static void enq(MprBlk *bp); 
static void freeBlock(MprBlk *bp);
static void freeChildren(MprBlk *bp);
static MprBlk *getBlock(size_t usize, int padWords, int flags);
static int getQueueIndex(size_t size, int roundup);
static int growHeap(size_t size);
static int initQueues();
static void linkChild(MprBlk *parent, MprBlk *bp);
static MprBlk *splitBlock(MprBlk *bp, size_t required, int swap);
static void getSystemInfo();
static void unlinkChild(MprBlk *bp);
static void *virtAlloc(size_t size);
static void virtFree(MprBlk *bp);

#if BLD_WIN_LIKE
static int winPageModes(int flags);
#endif

#if BLD_MEMORY_DEBUG
static void breakpoint(MprBlk *bp);
static void check(MprBlk *bp);
static int validBlk(MprBlk *bp);
#endif
#if BLD_MEMORY_STATS
static MprFreeBlk *getQueue(size_t size);
static void printQueueStats();
#endif

/************************************* Code ***********************************/
/*
    Initialize the memory subsystem
 */
Mpr *mprCreateAllocService(MprAllocFailure cback, MprDestructor destructor)
{
    MprHeap     initHeap;
    MprBlk      *bp, *children;
    size_t      usize, size;
    int         padWords;

    heap = &initHeap;
    memset(heap, 0, sizeof(MprHeap));
    mprInitSpinLock(heap, &heap->spin);

    heap->stats.maxMemory = INT_MAX;
    heap->stats.redLine = INT_MAX / 100 * 99;
    heap->notifier = cback;
    heap->notifierCtx = MPR;
#if BLD_MEMORY_DEBUG
    heap->nextSeqno = 1;
#endif
    getSystemInfo();

    padWords = DESTRUCTOR_SIZE;
    usize = sizeof(Mpr) + (padWords * sizeof(void*));
    size = MPR_ALLOC_ALIGN(MPR_ALLOC_HDR_SIZE + usize);

    if (growHeap(size) < 0) {
        return 0;
    }
    bp = (MprBlk*) heap->nextMem;
    heap->nextMem += size;

    SET_SIZE(bp, size);
    SET_PAD(bp, padWords);

    INIT_LIST(bp);
    children = GET_CHILDREN(bp);
    INIT_LIST(children);

    SET_MAGIC(bp);
    SET_DESTRUCTOR(bp, destructor);
    SET_TRAILER(bp, MPR_ALLOC_MAGIC);

    MPR = (Mpr*) GET_PTR(bp);
    memcpy(&MPR->heap, heap, sizeof(MprHeap));
    heap = &MPR->heap;

    if (initQueues() < 0 || (MPR->ctx = mprAllocCtx(MPR, 0)) == 0) {
        return 0;
    }
    return MPR;
}


void *mprAllocBlock(MprCtx ctx, size_t usize, int flags)
{
    MprBlk      *bp, *parent, *children;
    int         padWords;

    mprAssert(usize >= 0);

    if (ctx == NULL) {
        ctx = MPR->ctx;
    }
    parent = GET_BLK(ctx);
    CHECK_BLK(parent);
    
    padWords = TRAILER_SIZE;
    if (flags & MPR_ALLOC_DESTRUCTOR) {
        padWords = DESTRUCTOR_SIZE;
    } else if (flags & MPR_ALLOC_CHILDREN) {
        padWords = CHILDREN_SIZE;
    }
    if ((bp = getBlock(usize, padWords, flags)) != 0) {
        if (flags & MPR_ALLOC_CHILDREN) {
            children = GET_CHILDREN(bp);
            mprAssert(children);
            INIT_LIST(children);
        }
        linkChild(parent, bp);
        return GET_PTR(bp);
    }
    return 0;
}


/*
    Free a block of memory. Free all children recursively. Return 0 if the memory was freed. A destructor may prevent
    memory being deleted by returning non-zero.
 */
int mprFree(void *ptr)
{
    MprBlk  *bp;

    if (likely(ptr)) {
        bp = GET_BLK(ptr);
        CHECK_BLK(bp);
        mprAssert(!bp->free);

        if (unlikely(HAS_DESTRUCTOR(bp)) && (GET_DESTRUCTOR(bp))(ptr) != 0) {
            /* Destructor aborted the free */
            return 1;
        }
        if (HAS_CHILDREN(bp)) {
            freeChildren(bp);
        }
        unlinkChild(bp);
        freeBlock(bp);
    }
    return 0;
}


static void freeChildren(MprBlk *bp)
{
    MprBlk      *children, *child, *next;
    int         count;

    CHECK_BLK(bp);
    if ((children = GET_CHILDREN(bp)) != NULL) {
        count = 0;
        for (child = children->next; child != children; child = next) {
            next = child->next;
            if (!HAS_DESTRUCTOR(child) || (GET_DESTRUCTOR(child))(GET_PTR(child)) == 0) {
                if (HAS_CHILDREN(child)) {
                    freeChildren(child);
                }
                freeBlock(child);
            }
            count++;
        }
        INIT_LIST(children);
    }
}


void mprFreeChildren(MprCtx ptr)
{
    if (likely(ptr)) {
        freeChildren(GET_BLK(ptr));
    }
}


void *mprRealloc(MprCtx ctx, void *ptr, size_t usize)
{
    MprBlk      *bp, *newb;
    void        *newptr;
    int         padWords;

    CHECK_PTR(ctx);
    mprAssert(usize > 0);

    if (ptr == 0) {
        return mprAllocBlock(ctx, usize, 0);
    }
    bp = GET_BLK(ptr);
    mprAssert(bp);

    if (usize <= GET_USIZE(bp)) {
        return ptr;
    }
    padWords = GET_PAD(bp);
    if ((newb = getBlock(usize, padWords, 0)) == 0) {
        return 0;
    }
    newptr = GET_PTR(newb);
    memcpy(newptr, ptr, GET_SIZE(bp) - MPR_ALLOC_HDR_SIZE);
    unlinkChild(bp);
    linkChild(GET_BLK(ctx), newb);
    freeBlock(bp);
    return newptr;
}


char *mprStrdup(MprCtx ctx, cchar *str)
{
    char    *ptr;
    int     len;

    CHECK_PTR(ctx);

    if (str == NULL) {
        str = "";
    }
    len = (int) strlen(str) + 1;
    if ((ptr = (char*) mprAllocBlock(ctx, len, 0)) != NULL) {
        memcpy(ptr, str, len);
    }
    return ptr;
}


char *mprStrndup(MprCtx ctx, cchar *str, size_t usize)
{
    char    *ptr;
    size_t  len;

    CHECK_PTR(ctx);

    if (str == NULL) {
        str = "";
    }
    len = (int) strlen(str) + 1;
    len = min(len, usize);
    if ((ptr = (char*) mprAllocBlock(ctx, len, 0)) != 0) {
        memcpy(ptr, str, len);
    }
    return ptr;
}


void *mprMemdup(MprCtx ctx, cvoid *ptr, size_t usize)
{
    char    *newp;

    CHECK_PTR(ctx);

    if ((newp = (char*) mprAllocBlock(ctx, usize, 0)) != 0) {
        memcpy(newp, ptr, usize);
    }
    return newp;
}


bool mprIsParent(MprCtx ctx, cvoid *ptr)
{
    MprBlk  *bp, *child, *children;

    if (ptr == 0 || !VALID_BLK(GET_BLK(ptr))) {
        return 0;
    }
    bp = GET_BLK(ptr);
    if ((children = GET_CHILDREN(bp)) != NULL) {
        for (child = children->next; child != children; child = child->next) {
            if (child == bp) {
                return 1;
            }
        }
    }
    return 0;
}


/*
    Steal a block from one context and insert in a new context.
 */
void mprStealBlock(MprCtx ctx, cvoid *ptr)
{
    MprBlk      *bp;

    if (ptr) {
        CHECK_PTR(ctx);
        CHECK_PTR(ptr);
        bp = GET_BLK(ptr);
        unlinkChild(bp);
        linkChild(GET_BLK(ctx), bp);
    }
}


//  MOB - temporary until new Ejs GC.

MprBlk *mprGetFirstChild(cvoid *ptr)
{
    MprBlk  *bp, *children;

    bp = GET_BLK(ptr);
    if ((children = GET_CHILDREN(bp)) != NULL && children->next != children) {
        return children->next;
    }
    return NULL;
}


MprBlk *mprGetNextChild(cvoid *ptr)
{
    MprBlk  *bp;

    bp = GET_BLK(ptr);
    return bp->next;
}


MprBlk *mprGetEndChildren(cvoid *ptr)
{
    MprBlk  *bp, *children;

    bp = GET_BLK(ptr);
    if ((children = GET_CHILDREN(bp)) != NULL && children->prev != children) {
        return children->prev;
    }
    return NULL;
}


int mprGetPageSize()
{
    return heap->stats.pageSize;
}


int mprGetBlockSize(cvoid *ptr)
{
    MprBlk      *bp;

    bp = GET_BLK(ptr);
    if (ptr == 0 || !VALID_BLK(bp)) {
        return 0;
    }
    int usize = GET_USIZE(bp);
    int size = GET_SIZE(bp);
    size -= MPR_ALLOC_HDR_SIZE;
    size -= GET_PAD(bp) * sizeof(void*);
    mprAssert(size == usize);
    mprAssert(GET_USIZE(bp) == (GET_SIZE(bp) - MPR_ALLOC_HDR_SIZE - (GET_PAD(bp) * sizeof(void*))));
    return GET_USIZE(bp);
}


void mprSetAllocCallback(MprCtx ctx, MprAllocFailure cback)
{
    heap->notifier = cback;
    heap->notifierCtx = ctx;
}


void mprSetAllocLimits(MprCtx ctx, int redLine, int maxMemory)
{
    if (redLine > 0) {
        heap->stats.redLine = redLine;
    }
    if (maxMemory > 0) {
        heap->stats.maxMemory = maxMemory;
    }
}


void mprSetAllocPolicy(MprCtx ctx, int policy)
{
    heap->allocPolicy = policy;
}


void mprSetAllocError()
{
    heap->hasError = 1;
}


bool mprHasAllocError()
{
    return heap->hasError;
}


void mprResetAllocError()
{
    heap->hasError = 0;
}


int mprIsValid(cvoid *ptr)
{
    return ptr && VALID_BLK(GET_BLK(ptr));
}


static int dummyAllocDestructor() { return 0; }


void *mprUpdateDestructor(void *ptr, MprDestructor destructor)
{
    MprBlk      *bp;

    bp = GET_BLK(ptr);
    mprAssert(HAS_DESTRUCTOR(bp));
    if (!destructor) {
        destructor = dummyAllocDestructor;
    }
    SET_DESTRUCTOR(bp, destructor);
    return ptr;
}


static int initQueues() 
{
    MprFreeBlk  *freeq;
    
    /*
        The free map is a two dimensional array of free queues. The first dimension is indexed by
        the most significant bit (MSB) set in the requested block size. The second dimension is the next 
        MPR_ALLOC_BUCKET_SHIFT (4) bits below the MSB.

        +-------------------------------+
        |       |MSB|  Bucket   | rest  |
        +-------------------------------+
        | 0 | 0 | 1 | 1 | 1 | 1 | X | X |
        +-------------------------------+
     */
    heap->freeEnd = &heap->free[MPR_ALLOC_NUM_GROUPS * MPR_ALLOC_NUM_BUCKETS];
    for (freeq = heap->free; freeq != heap->freeEnd; freeq++) {
#if BLD_MEMORY_STATS
        size_t      bit, size, groupBits, bucketBits;
        int         index, group, bucket;
        /*
            NOTE: skip the buckets with MSB == 0 (round up)
         */
        index = (freeq - heap->free);
        group = index / MPR_ALLOC_NUM_BUCKETS;
        bucket = index % MPR_ALLOC_NUM_BUCKETS;

        bit = (group != 0);
        groupBits = bit << (group + MPR_ALLOC_BUCKET_SHIFT - 1);
        bucketBits = ((int64) bucket) << (max(0, group - 1));

        size = groupBits | bucketBits;
        freeq->size = size << MPR_ALIGN_SHIFT;
#endif
        freeq->forw = freeq->back = freeq;
    }
#if BLD_MEMORY_STATS && UNUSED && KEEP
    printQueueStats();
#endif
    return 0;
}


static int getQueueIndex(size_t size, int roundup)
{   
    size_t      usize, asize;
    int         aligned;
    
    mprAssert(MPR_ALLOC_ALIGN(size) == size);

    /*
        Allocate based on user sizes (sans header). This permits block searches to avoid scanning the next 
        highest queue for common block sizes: eg. 1K.
     */
    usize = (size - MPR_ALLOC_HDR_SIZE);
    asize = usize >> MPR_ALIGN_SHIFT;

    //  Zero based most significant bit
    int msb = flsl(asize) - 1;

    int group = max(0, msb - MPR_ALLOC_BUCKET_SHIFT + 1);
    mprAssert(group < MPR_ALLOC_NUM_GROUPS);

    int bucket = (asize >> max(0, group - 1)) & (MPR_ALLOC_NUM_BUCKETS - 1);
    mprAssert(bucket < MPR_ALLOC_NUM_BUCKETS);

    int index = (group * MPR_ALLOC_NUM_BUCKETS) + bucket;
    mprAssert(index < (heap->freeEnd - heap->free));
    
#if BLD_MEMORY_STATS
    mprAssert(heap->free[index].size <= usize && usize < heap->free[index + 1].size);
#endif
    
    if (roundup) {
        /*
            Check if the requested size is the smallest possible size in a queue. If not the smallest,
            must look at the next queue higher up to guarantee a block of sufficient size.
            This is part of the "good-fit" strategy.
         */
        aligned = (asize & ((((size_t) 1) << (group + MPR_ALLOC_BUCKET_SHIFT - 1)) - 1)) == 0;
        if (!aligned) {
            index++;
        }
    }
    return index;
}


#if BLD_MEMORY_STATS
static MprFreeBlk *getQueue(size_t size)
{   
    MprFreeBlk  *freeq;
    int         index;
    
    index = getQueueIndex(size, 0);
    freeq = &heap->free[index];
    return freeq;
}
#endif


static MprBlk *searchQueues(size_t size)
{
    MprFreeBlk  *freeq;
    MprBlk      *bp;
    size_t      freeMap;
    int         bucket, group, index, last;
    
    lockHeap(heap);

    index = getQueueIndex(size, 1);
    group = index / MPR_ALLOC_NUM_BUCKETS;
    bucket = index % MPR_ALLOC_NUM_BUCKETS;

    freeMap = heap->freeMap;
    if (group) {
        //  Mask out freeMap entries smaller than the current index.
        freeMap &= ~((((size_t) 1) << group) - 1);
    }
    for (group = ffsl(freeMap) - 1; freeMap; group++) {
        if (freeMap & ((((size_t) 1) << group))) {
            index = (group * MPR_ALLOC_NUM_BUCKETS) + bucket;
            mprAssert(index < (heap->freeEnd - heap->free));
            last = (index | (MPR_ALLOC_NUM_BUCKETS - 1)) + 1;
#if BLD_MEMORY_STATS
            heap->stats.groupsScanned++;
#endif
            for (freeq = &heap->free[index]; freeq < &heap->free[last]; freeq++) {
                if (freeq->forw != freeq) {
#if BLD_MEMORY_STATS
                    mprAssert((size - MPR_ALLOC_HDR_SIZE) <= freeq->size);
                    heap->stats.queuesScanned++;
#endif
                    bp = (MprBlk*) freeq->forw;
                    mprAssert(bp->size >= size);
                    deq(bp);
                    unlockHeap(heap);
                    return bp;
                }
            }
            /* All buckets in this group are empty */
            heap->freeMap &= ~(((size_t) 1) << group);
            freeMap &= ~(((size_t) 1) << group);
        }
        /* Examine all buckets in subsequent groups */
        bucket = 0;
    }
    unlockHeap(heap);
    return NULL;
}


/*
    Add a block to a free q. Must be called locked.
 */
static void enq(MprBlk *bp) 
{
    MprFreeBlk  *freeq, *fb;
    size_t      size;
    int         index;

    bp->pad = 0;
    bp->free = 1;
    
    size = GET_SIZE(bp);
    index = getQueueIndex(size, 0);
    heap->freeMap |= (((size_t) 1) << (index / MPR_ALLOC_NUM_BUCKETS));
    freeq = &heap->free[index];
    fb = (MprFreeBlk*) bp;
    fb->forw = freeq->forw;
    fb->back = freeq;
    freeq->forw->back = fb;
    freeq->forw = fb;
#if BLD_MEMORY_STATS
    freeq->count++;
#endif
    heap->stats.bytesFree += size;
}


/*
    Remove a block from a free q. Must be called locked.
 */
static void deq(MprBlk *bp) 
{
    MprFreeBlk  *fb;
    size_t      size;

    fb = (MprFreeBlk*) bp;
    size = GET_SIZE(bp);
#if BLD_MEMORY_STATS
    MprFreeBlk *freeq = getQueue(size);
    freeq->reuse++;
    freeq->count--;
    mprAssert(freeq->count >= 0);
#endif
    fb->back->forw = fb->forw;
    fb->forw->back = fb->back;
#if BLD_MEMORY_DEBUG
    fb->forw = fb->back = NULL;
#endif
    bp->free = 0;
    heap->stats.bytesFree -= size;
    mprAssert(heap->stats.bytesFree >= 0);
}


static void linkChild(MprBlk *parent, MprBlk *bp)
{
    MprBlk  *children;

    mprAssert(bp != parent);
    if (!HAS_CHILDREN(parent)) {
        mprError(MPR, "Parent is not a context object, use mprAllocObj or mprAllocCtx on parent");
    }
    mprAssert(HAS_CHILDREN(parent));
    children = GET_CHILDREN(parent);
    mprAssert(children);
    mprAssert(children->next && children->prev);

    lockHeap(heap);
    //  MOB -- CLEANUP remove
    mprAssert(bp != children);
    mprAssert(bp != children->next);
    mprAssert(bp != children->prev);
    CHECK_BLK(bp);
    if (children != children->prev) {
        CHECK_BLK(children->prev);
        CHECK_BLK(children->next);
    }
    //  MOB -- CLEANUP remove

    bp->next = children->next;
    bp->prev = children;
    children->next->prev = bp;
    children->next = bp;
    unlockHeap(heap);
}


static void unlinkChild(MprBlk *bp)
{
    //  MOB -- CLEANUP remove
    mprAssert(bp->prev != bp || bp == GET_BLK(MPR));
    mprAssert(bp->next != bp || bp == GET_BLK(MPR));
    //  MOB -- CLEANUP remove

    CHECK_BLK(bp);
    lockHeap(heap);
    bp->prev->next = bp->next;
    bp->next->prev = bp->prev;
    unlockHeap(heap);
}


static MprBlk *allocBlockFromHeap(size_t size)
{
    MprBlk  *bp;
    size_t  gap;

    mprAssert(size > 0);

    if (size >= MPR_ALLOC_BIG_BLOCK) {
        size = MPR_PAGE_ALIGN(size, heap->stats.pageSize);
        if ((bp = (MprBlk*) virtAlloc(size)) == 0) {
            return 0;
        }
        bp->last = 1;
    } else {
        lockHeap(heap);
        if ((heap->nextMem + size) >= heap->end) {
            gap = heap->end - heap->nextMem;
            if (gap >= (MPR_ALLOC_HDR_SIZE + MPR_ALIGN)) {
                bp = (MprBlk*) heap->nextMem;
                bp->size = gap;
                SET_MAGIC(bp);
                bp->last = 1;
                enq(bp);
            }
            if (growHeap(size) < 0) {
                unlockHeap(heap);
                return 0;
            }
        }
        bp = (MprBlk*) heap->nextMem;
        heap->nextMem += size;
        /*
            The last block in a region is marked as LAST to prevent coalescing with unallocated memory
            The second and subsequent blocks point back to their prior allocation.
         */
        if ((heap->end - heap->nextMem) < sizeof(MprBlk)) {
            bp->last = 1;
        } else {
            ((MprBlk*) heap->nextMem)->prior = bp;
        }
#if BLD_MEMORY_DEBUG
        bp->seqno = heap->nextSeqno++;
#endif
        unlockHeap(heap);
    }
    bp->size = size;
    SET_MAGIC(bp);
    return bp;
}


/*
    Get a block off a free queue or allocate if required
 */
static MprBlk *getBlock(size_t usize, int padWords, int flags)
{
    MprBlk      *bp;
    int         size;

    mprAssert(usize >= 0);
    size = MPR_ALLOC_ALIGN(usize + MPR_ALLOC_HDR_SIZE + (padWords * sizeof(void*)));
    
    bp = (heap->freeMap) ? searchQueues(size) : NULL;
    if (bp) {
        if (GET_SIZE(bp) >= (size + MPR_ALLOC_MIN_SPLIT)) {
            splitBlock(bp, size, 0);
        }
        if (flags & MPR_ALLOC_ZERO) {
            memset(GET_PTR(bp), 0, usize);
        }
#if BLD_MEMORY_STATS
        heap->stats.reuse++;
#endif
    } else {
        bp = allocBlockFromHeap(size);
    }
    if (bp) {
        BREAKPOINT(bp);
        if (padWords) {
            lockHeap(heap);
            SET_PAD(bp, padWords);
            unlockHeap(heap);
            memset(PAD_PTR(bp, padWords), 0, padWords * sizeof(void*));
            SET_TRAILER(bp, MPR_ALLOC_MAGIC);
        }
        //MOB  SET_MAGIC(bp);
        mprAssert(GET_PAD(bp) == padWords);
    }
#if BLD_MEMORY_STATS
    heap->stats.requests++;
#endif
    return bp;
}


static MprBlk *getNextBlockInMemory(MprBlk *bp) 
{
    if (!bp->last) {
        return (MprBlk*) ((char*) bp + GET_SIZE(bp));
    }
    return 0;
}


static MprBlk *getPrevBlockInMemory(MprBlk *bp) 
{
    MprBlk  *prior;

    if ((prior = GET_PRIOR(bp)) != 0) {
        return prior;
    }
    return 0;
}


static void freeBlock(MprBlk *bp)
{
    MprBlk  *prev, *next, *after;
    size_t  size;

    after = next = prev = NULL;
    
    BREAKPOINT(bp);
    RESET_MEM(bp);
    
    size = GET_SIZE(bp);
    if (size >= MPR_ALLOC_BIG_BLOCK) {
        mprVirtFree(bp, size);
        heap->stats.bytesAllocated -= size;
        mprAssert(heap->stats.bytesAllocated >= 0);

    } else {
        /*
            Coalesce with next if it is also free.
         */
        lockHeap(heap);
        next = getNextBlockInMemory(bp);
        if (next && next->free) {
            BREAKPOINT(next);
            deq(next);
            if ((after = getNextBlockInMemory(next)) != 0) {
                mprAssert(GET_PRIOR(after) == next);
                SET_PRIOR_REF(after, bp);
            } else {
                bp->last = 1;
            }
            size += GET_SIZE(next);
            SET_SIZE(bp, size);
#if BLD_MEMORY_STATS
            heap->stats.joins++;
#endif
        }
        /*
            Coalesce with previous if it is also free.
         */
        prev = getPrevBlockInMemory(bp);
        if (prev && prev->free) {
            BREAKPOINT(prev);
            deq(prev);
            if ((after = getNextBlockInMemory(bp)) != 0) {
                mprAssert(GET_PRIOR(after) == bp);
                SET_PRIOR_REF(after, prev);
            } else {
                prev->last = 1;
            }
            size += GET_SIZE(prev);
            SET_SIZE(prev, size);
            bp = prev;
#if BLD_MEMORY_STATS
            heap->stats.joins++;
#endif
        }
#if BLD_CC_MMU && 0
        if (size > 25000)
        printf("Size %d / %d, free %d\n", (int) GET_SIZE(bp), MPR_ALLOC_RETURN, (int) heap->stats.bytesFree);
        if (GET_SIZE(bp) >= MPR_ALLOC_RETURN && heap->stats.bytesFree > (MPR_REGION_MIN_SIZE * 4)) {
            virtFree(bp);
        }
#endif
        /*
            Return block to the appropriate free queue
         */
        enq(bp);

#if BLD_MEMORY_DEBUG
        if ((after = getNextBlockInMemory(bp)) != 0) {
            mprAssert(GET_PRIOR(after) == bp);
        }
#endif
        unlockHeap(heap);
    }
}


/*
    Split a block. Required specifies the number of bytes needed in the block. If swap, then put bp back on the free
    queue instead of the second half.
 */
static MprBlk *splitBlock(MprBlk *bp, size_t required, int swap)
{
    MprBlk      *secondHalf, *after;
    size_t      size, spare;

    mprAssert(bp);
    CHECK_BLK(bp);
    mprAssert(required > 0);
    BREAKPOINT(bp);

    size = GET_SIZE(bp);
    spare = size - required;
    mprAssert(spare >= MPR_ALLOC_MIN_SPLIT);

    /*
        Save pointer to block after the split-block in memory so prior can be updated to maintain the chain
     */
    secondHalf = (MprBlk*) ((char*) bp + required);
    secondHalf->last = bp->last;
    secondHalf->size = spare;
    secondHalf->next = NULL;
    secondHalf->prev = NULL;
    secondHalf->prior = bp;
    SET_MAGIC(secondHalf);    
#if BLD_MEMORY_DEBUG
    secondHalf->seqno = heap->nextSeqno++;
#endif
    BREAKPOINT(secondHalf);

    lockHeap(heap);
    after = getNextBlockInMemory(secondHalf);
    if (after) {
        SET_PRIOR_REF(after, secondHalf);
    }
    bp->size = required;
    bp->last = 0;

#if BLD_MEMORY_STATS
    heap->stats.splits++;
#endif
    if (swap) {
        RESET_MEM(bp);
        enq(bp);
        unlockHeap(heap);
        return secondHalf;
    } else {
        RESET_MEM(secondHalf);
        enq(secondHalf);
        unlockHeap(heap);
        return bp;
    }
}


static void virtFree(MprBlk *bp)
{
    MprBlk      *after, *tail;
    size_t      size, ptr, aligned;
    int         gap, pageSize;

    size = GET_SIZE(bp);
    ptr = (size_t) bp;
    pageSize = heap->stats.pageSize;

    lockHeap(heap);
    after = getNextBlockInMemory(bp);

    aligned = MPR_PAGE_ALIGN(ptr, pageSize);
    gap = aligned - ptr;
    if (gap) {
        if (gap < (MPR_ALLOC_HDR_SIZE + MPR_ALIGN)) {
            /* Gap must be useful. If too small, preserve one page with it */
            aligned += pageSize;
            gap += pageSize;
        }
        ptr = aligned;
        size -= gap;
        bp->size = gap;
        enq(bp);
    }
    gap = size % pageSize;
    if (gap) {
        if (gap < (MPR_ALLOC_HDR_SIZE + MPR_ALIGN)) {
            gap += pageSize;
        }
        size -= gap;
        tail = (MprBlk*) (ptr + size);
        tail->size = gap;
        enq(tail);
    }

    mprVirtFree((void*) ptr, size);

   //  MOB -- problem after unpinning, getNextBlockInMemory will segfault.

    if (after) {
        SET_PRIOR_REF(after, NULL);
    }
    unlockHeap(heap);
#if BLD_MEMORY_STATS
    heap->stats.unpins++;
    heap->stats.bytesAllocated -= size;
#endif
}


/*
    Allocate virtual memory and check a memory allocation request against configured maximums and redlines. 
    Do this so that the application does not need to check the result of every little memory allocation. Rather, an 
    application-wide memory allocation failure can be invoked proactively when a memory redline is exceeded. 
    It is the application's responsibility to set the red-line value suitable for the system.
 */
static void *virtAlloc(size_t size)
{
    void        *mem;
    size_t      used;

    used = mprGetUsedMemory();
    if ((size + used) > heap->stats.maxMemory) {
        allocException(size, 0);
        /* Prevent allocation as over the maximum memory limit.  */
        return 0;

    } else if ((size + used) > heap->stats.redLine) {
        /* Warn if allocation puts us over the red line. Then continue to grant the request.  */
        allocException(size, 1);
    }
    if ((mem = mprVirtAlloc(size, MPR_MAP_READ | MPR_MAP_WRITE)) == 0) {
        allocException(size, 0);
        return 0;
    }
#if BLD_MEMORY_STATS
    heap->stats.allocs++;
#endif
    heap->stats.bytesAllocated += size;
    return mem;
}


/*
    Grow the heap to satify the request
 */
static int growHeap(size_t size)
{
    size_t      chunkSize;

    mprAssert(size > 0);

    /*
        Each time we grow the heap, double the size of the next region of memory. Use 30MB so we don't double regions
        that are just under 32MB.
     */
    if (heap->memory) {
        chunkSize = (heap->end - heap->memory) * 2;
        if (chunkSize > MPR_REGION_MAX_SIZE) {
            chunkSize = MPR_REGION_MAX_SIZE;
        }
    } else {
        chunkSize = MPR_REGION_MIN_SIZE;
    }
    size = max(size, chunkSize);
    size = MPR_PAGE_ALIGN(size, heap->stats.pageSize);

    if ((heap->memory = virtAlloc(size)) == 0) {
        return MPR_ERR_NO_MEMORY;
    }
#if UNUSED
    /*
        Ensure there is always an empty block at the start and end of the region. This enables getNextBlockInMemory 
        and coalescing to not worry about segfaults when probing the next block.
     */
    heap->nextMem = &heap->memory[sizeof(MprBlk)];;
    heap->end = &heap->memory[size - sizeof(MprBlk)];
#else
    heap->nextMem = heap->memory;
    heap->end = &heap->memory[size];
#endif
    return 0;
}


static void allocException(size_t size, bool granted)
{
    MprHeap     *hp;

    heap->hasError = 1;

    lockHeap(heap);
    heap->stats.errors++;
    if (heap->stats.inAllocException == 0) {
        heap->stats.inAllocException = 1;
        unlockHeap(heap);

        if (hp->notifier) {
            (hp->notifier)(hp->notifierCtx, size, heap->stats.bytesAllocated, granted);
        }
        heap->stats.inAllocException = 0;
    } else {
        unlockHeap(heap);
    }
    if (!granted) {
        switch (heap->allocPolicy) {
        case MPR_ALLOC_POLICY_EXIT:
            mprError(MPR, "Application exiting due to memory allocation failure.");
            mprTerminate(MPR, 0);
            break;
        case MPR_ALLOC_POLICY_RESTART:
            mprError(MPR, "Application restarting due to memory allocation failure.");
            //  MOB - Other systems
#if BLD_UNIX_LIKE
            execv(MPR->argv[0], MPR->argv);
#endif
            break;
        }
    }
}


void *mprVirtAlloc(size_t size, int mode)
{
    void        *ptr;

    size = MPR_PAGE_ALIGN(size, heap->stats.pageSize);

#if BLD_CC_MMU
    /*
        Has virtual memory
     */
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
    if (ptr == 0) {
        return 0;
    }
    return ptr;
}


void mprVirtFree(void *ptr, size_t size)
{
#if BLD_CC_MMU
    /*
        Has virtual memory
     */
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
    MprAlloc    *ap;

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


MprAlloc *mprGetAllocStats()
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


#if BLD_MEMORY_STATS
static void printQueueStats() 
{
    MprFreeBlk  *freeq;
    int         i, index, total;

    mprLog(MPR, 0, "\nFree Queue Stats\n Bucket                     Size   Count        Reuse");
    for (i = 0, freeq = heap->free; freeq != heap->freeEnd; freeq++, i++) {
        total += freeq->size * freeq->count;
        index = (freeq - heap->free);
        mprLog(MPR, 0, "%7d %24lu %7d %12d", i, freeq->size, freeq->count, freeq->reuse);
    }
}
#endif /* BLD_MEMORY_STATS */


void mprPrintAllocReport(cchar *msg, int detail)
{
#if BLD_MEMORY_STATS
    MprAlloc    *ap;

    ap = mprGetAllocStats();

    mprLog(MPR, 0, "\n\n\nMPR Memory Report %s", msg);
    mprLog(MPR, 0, "------------------------------------------------------------------------------------------\n");
    mprLog(MPR, 0, "  Total memory           %,14d K",              mprGetUsedMemory());
    mprLog(MPR, 0, "  Current heap memory    %,14d K",              ap->bytesAllocated / 1024);
    mprLog(MPR, 0, "  Free heap memory       %,14d K",              ap->bytesFree / 1024);
    mprLog(MPR, 0, "  Allocation errors      %,14d",                ap->errors);
    mprLog(MPR, 0, "  Memory limit           %,14d MB (%d %%)",     ap->maxMemory / (1024 * 1024), 
       percent(ap->bytesAllocated / 1024, ap->maxMemory / 1024));
    mprLog(MPR, 0, "  Memory redline         %,14d MB (%d %%)",     ap->redLine / (1024 * 1024), 
       percent(ap->bytesAllocated / 1024, ap->redLine / 1024));

    mprLog(MPR, 0, "  Memory requests        %,14Ld",                ap->requests);
    mprLog(MPR, 0, "  O/S allocations        %d %%",                 percent(ap->allocs, ap->requests));
    mprLog(MPR, 0, "  Block unpinns          %d %%",                 percent(ap->unpins, ap->requests));
    mprLog(MPR, 0, "  Block reuse            %d %%",                 percent(ap->reuse, ap->requests));
    mprLog(MPR, 0, "  Joins                  %d %%",                 percent(ap->joins, ap->requests));
    mprLog(MPR, 0, "  Splits                 %d %%",                 percent(ap->splits, ap->requests));
    mprLog(MPR, 0, "  Queues scanned         %d %%",                 percent(ap->queuesScanned, ap->requests));
    mprLog(MPR, 0, "  Groups scanned         %d %%",                 percent(ap->groupsScanned, ap->requests));

    if (detail) {
        printQueueStats();
    }
#endif /* BLD_MEMORY_STATS */
}


#if BLD_MEMORY_DEBUG
static int validBlk(MprBlk *bp)
{
    mprAssert(bp->magic == MPR_ALLOC_MAGIC);
    mprAssert(GET_SIZE(bp) > 0);
    mprAssert(GET_TRAILER(bp) == MPR_ALLOC_MAGIC);
    return (bp->magic == MPR_ALLOC_MAGIC) && (GET_SIZE(bp) > 0) && (GET_TRAILER(bp) == MPR_ALLOC_MAGIC);
}


static void check(MprBlk *bp)
{
    mprAssert(VALID_BLK(bp));
}


static void breakpoint(MprBlk *bp) 
{
    if (bp == stopAlloc || bp->seqno == stopSeqno) {
        mprBreakpoint();
    }
}
#endif


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

