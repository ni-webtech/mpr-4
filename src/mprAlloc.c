/**
    mprAlloc.c - Memory Allocator. 

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes ***********************************/

#define MPR_IN_ALLOC 1

#include    "mpr.h"

/******************************* Local Defines ********************************/

#define GET_BLK(ptr)            ((MprBlk*) (((char*) (ptr)) - MPR_ALLOC_HDR_SIZE))
#define GET_PTR(bp)             ((char*) (((char*) (bp)) + MPR_ALLOC_HDR_SIZE))
#define GET_NEXT(bp)            ((bp)->last) ? NULL : ((MprBlk*) ((char*) bp + bp->size))
#define GET_USIZE(bp)           (bp->size - MPR_ALLOC_HDR_SIZE - (bp->pad * sizeof(void*)))
#define INIT_LIST(bp)           if (1) { bp->next = bp->prev = bp; } else

/*
    Trailing block. This is optional and will look like:
        Destructor
        Children
        Trailer
 */
#define PAD_PTR(bp, offset)     (((char*) bp) + bp->size - ((offset) * sizeof(void*)))

#if BLD_MEMORY_VERIFY
#define TRAILER_SIZE            1
#define TRAILER_OFFSET          1
#define HAS_TRAILER(bp)         (bp->pad >= TRAILER_OFFSET)
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
#define HAS_CHILDREN(bp)        (bp->pad >= CHILDREN_OFFSET)
#define GET_CHILDREN(bp)        (HAS_CHILDREN(bp) ? ((MprBlk*) PAD_PTR(bp, CHILDREN_OFFSET)) : NULL)

#define DESTRUCTOR_SIZE         (CHILDREN_SIZE + 1)
#define DESTRUCTOR_OFFSET       (TRAILER_OFFSET + 3)
#define HAS_DESTRUCTOR(bp)      (bp->pad >= DESTRUCTOR_OFFSET)
#define GET_DESTRUCTOR(bp)      (HAS_DESTRUCTOR(bp) ? (*(MprDestructor*) (PAD_PTR(bp, DESTRUCTOR_OFFSET))) : NULL)
#define SET_DESTRUCTOR(bp, fn)  if (1) { *((MprDestructor*) PAD_PTR(bp, DESTRUCTOR_OFFSET)) = fn; } else

#if BLD_MEMORY_DEBUG
#define BREAKPOINT(bp)          breakpoint(bp);
#define CHECK(bp)               check(bp)
#define CHECK_PTR(ptr)          CHECK(GET_BLK(ptr))

/*
    WARN: this will reset pad words too.
 */
#define RESET_MEM(bp)           if (bp != GET_BLK(MPR)) { \
                                    memset(GET_PTR(bp), 0xFE, bp->size - MPR_ALLOC_HDR_SIZE); } else
#define SET_MAGIC(bp)           if (1) { (bp)->magic = MPR_ALLOC_MAGIC; } else
#define SET_SEQ(bp)             if (1) { (bp)->seqno = heap->nextSeqno++; } else
#define INIT_BLK(bp, len)       if (1) { SET_MAGIC(bp); SET_SEQ(bp); bp->size = len; bp->pad = 0; } else
#define VALID_BLK(bp)           validBlk(bp)

/*
    Set this address to break when this address is allocated or freed
 */
static MprBlk *stopAlloc;
static int stopSeqno = -1;

#else
#define BREAKPOINT(bp)
#define CHECK(bp)           
#define CHECK_PTR(bp)           
#define RESET_MEM(bp)           
#define SET_MAGIC(bp)
#define SET_SEQ(bp)           
#define INIT_BLK(bp, len)       if (1) { bp->size = len; bp->pad = 0; } else
#define VALID_BLK(bp)           1
#endif

#if BLD_MEMORY_STATS
    #define INC(field)          if (1) { heap->stats.field++; } else 
    #define LOCKED_INC(field)   if (1) { lockHeap(heap); heap->stats.field++; unlockHeap(heap);} else 
#else
    #define INC(field)              
    #define LOCKED_INC(field)
#endif

#define lockHeap(heap)          mprSpinLock(&heap->spin);
#define unlockHeap(heap)        mprSpinUnlock(&heap->spin);

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
static int      padding[] = { TRAILER_SIZE, CHILDREN_SIZE, DESTRUCTOR_SIZE, DESTRUCTOR_SIZE };

/***************************** Forward Declarations ***************************/

static void allocException(size_t size, bool granted);
static void deq(MprBlk *bp);
static void enq(MprBlk *bp); 
static void freeBlock(MprBlk *bp);
static void freeChildren(MprBlk *bp);
static MprBlk *getBlock(size_t usize, int padWords, int flags);
static int getQueueIndex(size_t size, int roundup);
static MprBlk *growHeap(size_t size);
static int initFree();
static void linkChild(MprBlk *parent, MprBlk *bp);
static MprBlk *splitBlock(MprBlk *bp, size_t required, int qspare);
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
    MprBlk      *bp, *children, *spare;
    size_t      usize, size, required, extra;
    int         padWords;

    heap = &initHeap;
    memset(heap, 0, sizeof(MprHeap));

    padWords = DESTRUCTOR_SIZE;
    usize = sizeof(Mpr) + (padWords * sizeof(void*));
    required = MPR_ALLOC_ALIGN(MPR_ALLOC_HDR_SIZE + usize);

    size = max(required, MPR_REGION_MIN_SIZE);
    extra = size - required;
    if ((bp = (MprBlk*) mprVirtAlloc(size, MPR_MAP_READ | MPR_MAP_WRITE)) == NULL) {
        return 0;
    }
    INIT_BLK(bp, required);
    bp->pad = padWords;
    INIT_LIST(bp);
    SET_DESTRUCTOR(bp, destructor);
    SET_TRAILER(bp, MPR_ALLOC_MAGIC);
    children = GET_CHILDREN(bp);
    INIT_LIST(children);

    spare = (MprBlk*) (((char*) bp) + required);
    INIT_BLK(spare, extra);
    spare->last = 1;
    spare->prior = bp;

    MPR = (Mpr*) GET_PTR(bp);
    heap = &MPR->heap;
    heap->notifier = cback;
    heap->notifierCtx = MPR;
    heap->nextSeqno = 1;
    heap->chunkSize = MPR_REGION_MIN_SIZE;
    heap->stats.maxMemory = INT_MAX;
    heap->stats.redLine = INT_MAX / 100 * 99;
    heap->stats.bytesAllocated += size;
    mprInitSpinLock(heap, &heap->spin);
    getSystemInfo();

    if (initFree() < 0) {
        return 0;
    }
    enq(spare);

    if ((MPR->ctx = mprAllocCtx(MPR, 0)) == NULL) {
        return 0;
    }
    return MPR;
}


void mprInitBlock(MprCtx ctx, void *ptr, size_t size, int flags)
{
    MprBlk      *bp, *parent;

    parent = GET_BLK(ctx);
    CHECK(parent);

    bp = GET_BLK(ptr);
    INIT_BLK(bp, size);

    memset(ptr, 0, size);
    bp->last = 1;
    bp->prior = NULL;
    linkChild(parent, bp);
}


void *mprAllocBlock(MprCtx ctx, size_t usize, int flags)
{
    MprBlk      *bp, *parent, *children;

    mprAssert(usize >= 0);

    if (ctx == NULL) {
        ctx = MPR->ctx;
    }
    parent = GET_BLK(ctx);
    CHECK(parent);
    
    if ((bp = getBlock(usize, padding[flags & MPR_ALLOC_PAD_MASK], flags)) != 0) {
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
        CHECK(bp);
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

    CHECK(bp);
    if ((children = GET_CHILDREN(bp)) != NULL) {
        count = 0;
        for (child = children->next; child != children; child = next) {
            next = child->next;
            if (!HAS_DESTRUCTOR(child) || (GET_DESTRUCTOR(child))(GET_PTR(child)) == 0) {
                if (HAS_CHILDREN(child)) {
                    freeChildren(child);
                }
                unlinkChild(child);
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

    CHECK_PTR(ctx);
    mprAssert(usize > 0);

    if (ptr == 0) {
        return mprAllocBlock(ctx, usize, 0);
    }
    bp = GET_BLK(ptr);
    if (usize <= GET_USIZE(bp)) {
        return ptr;
    }
    if ((newb = getBlock(usize, bp->pad, 0)) == 0) {
        return 0;
    }
    newptr = GET_PTR(newb);
    memcpy(newptr, ptr, bp->size - MPR_ALLOC_HDR_SIZE);
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


Mpr *mprGetMpr(MprCtx ctx)
{
    return MPR;
}


int mprGetPageSize()
{
    return heap->pageSize;
}


int mprGetBlockSize(cvoid *ptr)
{
    MprBlk      *bp;

    bp = GET_BLK(ptr);
    if (ptr == 0 || !VALID_BLK(bp)) {
        return 0;
    }
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


static int dummyAllocDestructor(MprCtx ctx) { return 0; }


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
    MprFreeBlk  *freeq;
    
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
        bucketBits = ((size_t) bucket) << (max(0, group - 1));

        size = groupBits | bucketBits;
        freeq->size = size << MPR_ALIGN_SHIFT;
#endif
        freeq->forw = freeq->back = freeq;
    }
    return 0;
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
    usize = (size - MPR_ALLOC_HDR_SIZE);
    asize = usize >> MPR_ALIGN_SHIFT;

    //  Zero based most significant bit
    msb = flsl(asize) - 1;

    group = max(0, msb - MPR_ALLOC_BUCKET_SHIFT + 1);
    mprAssert(group < MPR_ALLOC_NUM_GROUPS);

    bucket = (asize >> max(0, group - 1)) & (MPR_ALLOC_NUM_BUCKETS - 1);
    mprAssert(bucket < MPR_ALLOC_NUM_BUCKETS);

    index = (group * MPR_ALLOC_NUM_BUCKETS) + bucket;
    mprAssert(index < (heap->freeEnd - heap->free));
    
#if BLD_MEMORY_STATS
    mprAssert(heap->free[index].size <= usize && usize < heap->free[index + 1].size);
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
static MprFreeBlk *getQueue(size_t size)
{   
    MprFreeBlk  *freeq;
    int         index;
    
    index = getQueueIndex(size, 0);
    freeq = &heap->free[index];
    return freeq;
}
#endif


static MprBlk *searchFree(size_t size, int *indexp)
{
    MprFreeBlk  *freeq;
    MprBlk      *bp;
    size_t      groupMap, bucketMap;
    int         bucket, baseGroup, group, index;
    
    *indexp = index = getQueueIndex(size, 1);
    baseGroup = index / MPR_ALLOC_NUM_BUCKETS;
    bucket = index % MPR_ALLOC_NUM_BUCKETS;

    lockHeap(heap);
    
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
                freeq = &heap->free[index];
                if (freeq->forw != freeq) {
                    bp = (MprBlk*) freeq->forw;
                    deq(bp);
                    INC(reuse);
                    unlockHeap(heap);
                    CHECK(bp);
                    return bp;
                }
                bucketMap &= ~(((size_t) 1) << bucket);
                heap->bucketMap[group] &= ~(((size_t) 1) << bucket);
            }
            groupMap &= ~(((size_t) 1) << group);
            heap->groupMap &= ~(((size_t) 1) << group);
        }
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
    int         index, group, bucket;

    bp->free = 1;
    bp->pad = 0;
    
    index = getQueueIndex(bp->size, 0);
    group = index / MPR_ALLOC_NUM_BUCKETS;
    bucket = index % MPR_ALLOC_NUM_BUCKETS;
    heap->groupMap |= (((size_t) 1) << group);
    heap->bucketMap[group] |= (((size_t) 1) << bucket);

    freeq = &heap->free[index];
    fb = (MprFreeBlk*) bp;
    fb->forw = freeq->forw;
    fb->back = freeq;
    freeq->forw->back = fb;
    freeq->forw = fb;
#if BLD_MEMORY_STATS
    freeq->count++;
#endif
    heap->stats.bytesFree += bp->size;
}


/*
    Remove a block from a free q. Must be called locked.
 */
static void deq(MprBlk *bp) 
{
    MprFreeBlk  *fb;

    fb = (MprFreeBlk*) bp;
    fb->back->forw = fb->forw;
    fb->forw->back = fb->back;
#if BLD_MEMORY_DEBUG
    fb->forw = fb->back = NULL;
#endif
    bp->free = 0;
    heap->stats.bytesFree -= bp->size;
    mprAssert(heap->stats.bytesFree >= 0);
#if BLD_MEMORY_STATS
{
    MprFreeBlk *freeq = getQueue(bp->size);
    freeq->count--;
    mprAssert(freeq->count >= 0);
}
#endif
}


static void linkChild(MprBlk *parent, MprBlk *bp)
{
    MprBlk  *children;

    CHECK(bp);
    mprAssert(bp != parent);

    if (!HAS_CHILDREN(parent)) {
        mprError(MPR, "Parent is not a context object, use mprAllocObj or mprAllocCtx on parent");
        return;
    }
    children = GET_CHILDREN(parent);

    lockHeap(heap);
    bp->next = children->next;
    bp->prev = children;
    children->next->prev = bp;
    children->next = bp;
    unlockHeap(heap);
}


static void unlinkChild(MprBlk *bp)
{
    CHECK(bp);

    lockHeap(heap);
    bp->prev->next = bp->next;
    bp->next->prev = bp->prev;
    unlockHeap(heap);
}


/*
    Get a block off a free queue or allocate if required
 */
static MprBlk *getBlock(size_t usize, int padWords, int flags)
{
    MprBlk      *bp;
    size_t      maxBlock, size;
    int         bucket, group, index;

	int mob = MPR_ALLOC_HDR_SIZE;
    mprAssert(usize >= 0);
	mob = usize + MPR_ALLOC_HDR_SIZE + (padWords * sizeof(void*));
    size = MPR_ALLOC_ALIGN(usize + MPR_ALLOC_HDR_SIZE + (padWords * sizeof(void*)));
    
    if ((bp = searchFree(size, &index)) == NULL) {
        if ((bp = growHeap(size)) == NULL) {
            return NULL;
        }
        index = getQueueIndex(size, 1);
    }
    BREAKPOINT(bp);
    mprAssert(bp->size >= size);

    if (bp->size >= (size + MPR_ALLOC_MIN_SPLIT)) {
        group = index / MPR_ALLOC_NUM_BUCKETS;
        bucket = index % MPR_ALLOC_NUM_BUCKETS;
        maxBlock = (((size_t) 1 ) << group | (((size_t) bucket) << (max(0, group - 1)))) << MPR_ALIGN_SHIFT;
        maxBlock += MPR_ALLOC_HDR_SIZE;
        if (bp->size > maxBlock) {
            // int gap = bp->size - maxBlock;
            splitBlock(bp, size, 1);
        }
    }

    if (flags & MPR_ALLOC_ZERO) {
        memset(GET_PTR(bp), 0, usize);
    }
    if (padWords) {
        bp->pad = padWords;
        memset(PAD_PTR(bp, padWords), 0, padWords * sizeof(void*));
        SET_TRAILER(bp, MPR_ALLOC_MAGIC);
    }
    CHECK(bp);
#if BLD_MEMORY_STATS
{
    MprFreeBlk *freeq = getQueue(bp->size);
    lockHeap(heap)
    INC(requests);
    freeq->reuse++;
    unlockHeap(heap);
}
#endif
    return bp;
}


static void freeBlock(MprBlk *bp)
{
    MprBlk  *prev, *next, *after;
    size_t  size;

    after = next = prev = NULL;
    
    BREAKPOINT(bp);
    RESET_MEM(bp);
    
    size = bp->size;
    bp->pad = 0;

    /*
        Coalesce with next if it is also free.
     */
    lockHeap(heap);
    next = GET_NEXT(bp);
    if (next && next->free) {
        BREAKPOINT(next);
        deq(next);
        if ((after = GET_NEXT(next)) != NULL) {
            mprAssert(after->prior == next);
            after->prior = bp;
        } else {
            bp->last = 1;
        }
        size += next->size;
        bp->size = size;
        INC(joins);
    }
    /*
        Coalesce with previous if it is also free.
     */
    prev = bp->prior;
    if (prev && prev->free) {
        BREAKPOINT(prev);
        deq(prev);
        if ((after = GET_NEXT(bp)) != NULL) {
            mprAssert(after->prior == bp);
            after->prior = prev;
        } else {
            prev->last = 1;
        }
        size += prev->size;
        prev->size = size;
        bp = prev;
        INC(joins);
    }
#if BLD_MEMORY_DEBUG
    if ((after = GET_NEXT(bp)) != 0) {
        mprAssert(after->prior == bp);
    }
#endif
#if BLD_CC_MMU
    if (bp->size >= MPR_ALLOC_RETURN && heap->stats.bytesFree > (MPR_REGION_MIN_SIZE * 4)) {
        INC(unpins);
        unlockHeap(heap);
        virtFree(bp);
    } else
#endif
    {
        enq(bp);
        unlockHeap(heap);
    }
}


/*
    Split a block. Required specifies the number of bytes needed in the block. If swap, then put bp back on the free
    queue instead of the second half.
 */
static MprBlk *splitBlock(MprBlk *bp, size_t required, int qspare)
{
    MprBlk      *spare, *after;
    size_t      size, extra;

    mprAssert(bp);
    mprAssert(required > 0);

    CHECK(bp);
    BREAKPOINT(bp);

    size = bp->size;
    extra = size - required;
    mprAssert(extra >= MPR_ALLOC_MIN_SPLIT);

    spare = (MprBlk*) ((char*) bp + required);
    INIT_BLK(spare, extra);
    spare->last = bp->last;
    spare->prior = bp;
    BREAKPOINT(spare);

    lockHeap(heap);
    bp->size = required;
    bp->last = 0;
    if ((after = GET_NEXT(spare)) != NULL) {
        after->prior = spare;
    }
    INC(splits);
    if (qspare) {
        enq(spare);
    }
    CHECK(spare);
    CHECK(bp);
    unlockHeap(heap);
    return (qspare) ? NULL : spare;
}


static void virtFree(MprBlk *bp)
{
    MprBlk      *spare, *after;
    size_t      gap;

    /*
        If block is non-aligned, split the portion off the front sand save
     */
    gap = MPR_PAGE_ALIGN(bp, heap->pageSize) - (size_t) bp;
    if (gap) {
        if (gap < MPR_ALLOC_MIN_SPLIT) {
            /* Gap must be useful -- If too small, preserve one extra page with it */
            gap += heap->pageSize;
        }
        spare = splitBlock(bp, gap, 0);
        bp->last = 1;
        lockHeap(heap);
        enq(bp);
        unlockHeap(heap);
        bp = spare;
    }

    /*
        If non-aligned tail, then split the tail and save
     */
    gap = bp->size % heap->pageSize;
    if (gap) {
        if (gap < MPR_ALLOC_MIN_SPLIT) {
            gap += heap->pageSize;
        }
        splitBlock(bp, bp->size - gap, 1);
    }

    lockHeap(heap);
    if (bp->prior) {
        bp->prior->last = 1;
    }
    if ((after = GET_NEXT(bp)) != NULL) {
        after->prior = NULL;
    }
    heap->stats.bytesAllocated -= bp->size;
    mprAssert(heap->stats.bytesAllocated >= 0);
    unlockHeap(heap);

    mprVirtFree((void*) bp, bp->size);
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
    lockHeap(heap);
    INC(allocs);
    heap->stats.bytesAllocated += size;
    unlockHeap(heap);
    return mem;
}


/*
    Grow the heap and return a block of the required size (unqueued)
 */
static MprBlk *growHeap(size_t required)
{
    MprBlk      *bp;
    size_t      size;

    mprAssert(required > 0);

    size = max(required, (size_t) heap->chunkSize);
    size = MPR_PAGE_ALIGN(size, heap->pageSize);

    if ((bp = (MprBlk*) virtAlloc(size)) == NULL) {
        return 0;
    }
    INIT_BLK(bp, size);
    bp->last = 1;
    CHECK(bp);
    return bp;
}


static void allocException(size_t size, bool granted)
{
    heap->hasError = 1;

    lockHeap(heap);
    INC(errors);
    if (heap->stats.inAllocException) {
        unlockHeap(heap);
        return;
    }
    heap->stats.inAllocException = 1;

    if (heap->notifier) {
        (heap->notifier)(heap->notifierCtx, size, heap->stats.bytesAllocated, granted);
    }
    heap->stats.inAllocException = 0;
    unlockHeap(heap);

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


void *mprVirtAlloc(size_t size, int mode)
{
    void        *ptr;

    if (heap->pageSize) {
        size = MPR_PAGE_ALIGN(size, heap->pageSize);
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
    if (ptr == 0) {
        return 0;
    }
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
    MprAllocStats   *ap;

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


MprAllocStats *mprGetAllocStats()
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
    MprFreeBlk  *freeq;
    int         i, index;

    mprRawLog(MPR, 0, "\nFree Queue Stats\n Bucket                     Size   Count        Reuse\n");
    for (i = 0, freeq = heap->free; freeq != heap->freeEnd; freeq++, i++) {
        index = (freeq - heap->free);
        mprRawLog(MPR, 0, "%7d %24lu %7d %12d\n", i, freeq->size, freeq->count, freeq->reuse);
    }
}
#endif /* BLD_MEMORY_STATS */


void mprPrintAllocReport(cchar *msg, int detail)
{
#if BLD_MEMORY_STATS
    MprAllocStats   *ap;

    ap = mprGetAllocStats();

    mprRawLog(MPR, 0, "\n\nMPR Memory Report %s\n", msg);
    mprRawLog(MPR, 0, "------------------------------------------------------------------------------------------\n");
    mprRawLog(MPR, 0, "  Total memory           %,14d K\n",              mprGetUsedMemory());
    mprRawLog(MPR, 0, "  Current heap memory    %,14d K\n",              ap->bytesAllocated / 1024);
    mprRawLog(MPR, 0, "  Free heap memory       %,14d K\n",              ap->bytesFree / 1024);
    mprRawLog(MPR, 0, "  Allocation errors      %,14d\n",                ap->errors);
    mprRawLog(MPR, 0, "  Memory limit           %,14d MB (%d %%)\n",     ap->maxMemory / (1024 * 1024), 
       percent(ap->bytesAllocated / 1024, ap->maxMemory / 1024));
    mprRawLog(MPR, 0, "  Memory redline         %,14d MB (%d %%)\n",     ap->redLine / (1024 * 1024), 
       percent(ap->bytesAllocated / 1024, ap->redLine / 1024));

    mprRawLog(MPR, 0, "  Memory requests        %,14Ld\n",                ap->requests);
    mprRawLog(MPR, 0, "  O/S allocations        %d %%\n",                 percent(ap->allocs, ap->requests));
    mprRawLog(MPR, 0, "  Block unpinns          %d %%\n",                 percent(ap->unpins, ap->requests));
    mprRawLog(MPR, 0, "  Block reuse            %d %%\n",                 percent(ap->reuse, ap->requests));
    mprRawLog(MPR, 0, "  Joins                  %d %%\n",                 percent(ap->joins, ap->requests));
    mprRawLog(MPR, 0, "  Splits                 %d %%\n",                 percent(ap->splits, ap->requests));

    if (detail) {
        printQueueStats();
    }
#endif /* BLD_MEMORY_STATS */
}


#if BLD_MEMORY_DEBUG
static int validBlk(MprBlk *bp)
{
    mprAssert(bp->magic == MPR_ALLOC_MAGIC);
    mprAssert(bp->size > 0);
    mprAssert(GET_TRAILER(bp) == MPR_ALLOC_MAGIC);
    return (bp->magic == MPR_ALLOC_MAGIC) && (bp->size > 0) && (GET_TRAILER(bp) == MPR_ALLOC_MAGIC);
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

