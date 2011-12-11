/**
    mprThread.c - Primitive multi-threading support for Windows

    This module provides threading, mutex and condition variable APIs.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************* Includes **********************************/

#include    "mpr.h"

/*************************** Forward Declarations ****************************/

static int changeState(MprWorker *worker, int state);
static MprWorker *createWorker(MprWorkerService *ws, ssize stackSize);
static int getNextThreadNum(MprWorkerService *ws);
static void manageThreadService(MprThreadService *ts, int flags);
static void manageThread(MprThread *tp, int flags);
static void manageWorker(MprWorker *worker, int flags);
static void manageWorkerService(MprWorkerService *ws, int flags);
static void pruneWorkers(MprWorkerService *ws, MprEvent *timer);
static void threadProc(MprThread *tp);
static void workerMain(MprWorker *worker, MprThread *tp);

/************************************ Code ***********************************/

MprThreadService *mprCreateThreadService()
{
    MprThreadService    *ts;

    ts = mprAllocObj(MprThreadService, manageThreadService);
    if (ts == 0) {
        return 0;
    }
    //  TODO - not used
    if ((ts->mutex = mprCreateLock()) == 0) {
        return 0;
    }
    if ((ts->cond = mprCreateCond()) == 0) {
        return 0;
    }
    if ((ts->threads = mprCreateList(-1, 0)) == 0) {
        return 0;
    }
    MPR->mainOsThread = mprGetCurrentOsThread();
    MPR->threadService = ts;
    ts->stackSize = MPR_DEFAULT_STACK;
    /*
        Don't actually create the thread. Just create a thread object for this main thread.
     */
    if ((ts->mainThread = mprCreateThread("main", NULL, NULL, 0)) == 0) {
        return 0;
    }
    ts->mainThread->isMain = 1;
    ts->mainThread->osThread = mprGetCurrentOsThread();
    return ts;
}


void mprStopThreadService()
{
}


static void manageThreadService(MprThreadService *ts, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ts->threads);
        mprMark(ts->mainThread);
        mprMark(ts->mutex);
        mprMark(ts->cond);

    } else if (flags & MPR_MANAGE_FREE) {
        mprStopThreadService();
    }
}


void mprSetThreadStackSize(ssize size)
{
    MPR->threadService->stackSize = size;
}


MprThread *mprGetCurrentThread()
{
    MprThreadService    *ts;
    MprThread           *tp;
    MprOsThread         id;
    int                 i;

    ts = MPR->threadService;
    id = mprGetCurrentOsThread();
    if (ts->threads->mutex) {
        lock(ts->threads);
    }
    for (i = 0; i < ts->threads->length; i++) {
        tp = mprGetItem(ts->threads, i);
        if (tp->osThread == id) {
            unlock(ts->threads);
            return tp;
        }
    }
    if (ts->threads->mutex) {
        unlock(ts->threads);
    }
    return 0;
}


cchar *mprGetCurrentThreadName()
{
    MprThread       *tp;

    if ((tp = mprGetCurrentThread()) == 0) {
        return 0;
    }
    return tp->name;
}


/*
    Return the current thread object
 */
void mprSetCurrentThreadPriority(int pri)
{
    MprThread       *tp;

    if ((tp = mprGetCurrentThread()) == 0) {
        return;
    }
    mprSetThreadPriority(tp, pri);
}


/*
    Create a main thread
 */
MprThread *mprCreateThread(cchar *name, void *entry, void *data, ssize stackSize)
{
    MprThreadService    *ts;
    MprThread           *tp;

    ts = MPR->threadService;
    tp = mprAllocObj(MprThread, manageThread);
    if (tp == 0) {
        return 0;
    }
    tp->data = data;
    tp->entry = entry;
    tp->name = sclone(name);
    tp->mutex = mprCreateLock();
    tp->cond = mprCreateCond();
    tp->pid = getpid();
    tp->priority = MPR_NORMAL_PRIORITY;

    if (stackSize == 0) {
        tp->stackSize = ts->stackSize;
    } else {
        tp->stackSize = stackSize;
    }
#if BLD_WIN_LIKE
    tp->threadHandle = 0;
#endif
    mprAssert(ts);
    mprAssert(ts->threads);
    if (mprAddItem(ts->threads, tp) < 0) {
        return 0;
    }
    return tp;
}


static void manageThread(MprThread *tp, int flags)
{
    MprThreadService    *ts;

    ts = MPR->threadService;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(tp->name);
        mprMark(tp->data);
        mprMark(tp->cond);
        mprMark(tp->mutex);

    } else if (flags & MPR_MANAGE_FREE) {
        if (ts->threads) {
            mprRemoveItem(ts->threads, tp);
        }
#if BLD_WIN_LIKE
        if (tp->threadHandle) {
            CloseHandle(tp->threadHandle);
        }
#endif
    }
}


/*
    Entry thread function
 */ 
#if BLD_WIN_LIKE
static uint __stdcall threadProcWrapper(void *data) 
{
    threadProc((MprThread*) data);
    return 0;
}
#elif VXWORKS

static int threadProcWrapper(void *data) 
{
    threadProc((MprThread*) data);
    return 0;
}

#else
void *threadProcWrapper(void *data) 
{
    threadProc((MprThread*) data);
    return 0;
}

#endif


/*
    Thread entry
 */
static void threadProc(MprThread *tp)
{
    mprAssert(tp);

    tp->osThread = mprGetCurrentOsThread();

#if VXWORKS
    tp->pid = tp->osThread;
#else
    tp->pid = getpid();
#endif
    (tp->entry)(tp->data, tp);
    mprRemoveItem(MPR->threadService->threads, tp);
}


/*
    Start a thread
 */
int mprStartThread(MprThread *tp)
{
    //  TODO - lock not needed
    lock(tp);

#if BLD_WIN_LIKE
{
    HANDLE          h;
    uint            threadId;

#if WINCE
    h = (HANDLE) CreateThread(NULL, 0, threadProcWrapper, (void*) tp, 0, &threadId);
#else
    h = (HANDLE) _beginthreadex(NULL, 0, threadProcWrapper, (void*) tp, 0, &threadId);
#endif
    if (h == NULL) {
        return MPR_ERR_CANT_INITIALIZE;
    }
    tp->osThread = (int) threadId;
    tp->threadHandle = (HANDLE) h;
}
#elif VXWORKS
{
    int     taskHandle, pri;

    taskPriorityGet(taskIdSelf(), &pri);
    taskHandle = taskSpawn(tp->name, pri, VX_FP_TASK, tp->stackSize, (FUNCPTR) threadProcWrapper, (int) tp, 
        0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (taskHandle < 0) {
        mprError("Can't create thread %s\n", tp->name);
        return MPR_ERR_CANT_INITIALIZE;
    }
}
#else /* UNIX */
{
    pthread_attr_t  attr;
    pthread_t       h;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, tp->stackSize);

    if (pthread_create(&h, &attr, threadProcWrapper, (void*) tp) != 0) { 
        mprAssert(0);
        pthread_attr_destroy(&attr);
        return MPR_ERR_CANT_CREATE;
    }
    pthread_attr_destroy(&attr);
}
#endif
    unlock(tp);
    return 0;
}


MprOsThread mprGetCurrentOsThread()
{
#if BLD_UNIX_LIKE
    return (MprOsThread) pthread_self();
#elif BLD_WIN_LIKE
    return (MprOsThread) GetCurrentThreadId();
#elif VXWORKS
    return (MprOsThread) taskIdSelf();
#endif
}


void mprSetThreadPriority(MprThread *tp, int newPriority)
{
    int     osPri;

    lock(tp);
    osPri = mprMapMprPriorityToOs(newPriority);

#if BLD_WIN_LIKE
    SetThreadPriority(tp->threadHandle, osPri);
#elif VXWORKS
    taskPrioritySet(tp->osThread, osPri);
#else
    setpriority(PRIO_PROCESS, (int) tp->pid, osPri);
#endif
    tp->priority = newPriority;
    unlock(tp);
}


static void manageThreadLocal(MprThreadLocal *tls, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
#if !BLD_UNIX_LIKE && !BLD_WIN_LIKE
        mprMark(tls->store);
#endif
    } else if (flags & MPR_MANAGE_FREE) {
#if BLD_UNIX_LIKE
        if (tls->key) {
            pthread_key_delete(tls->key);
        }
#elif BLD_WIN_LIKE
        if (tls->key >= 0) {
            TlsFree(tls->key);
        }
#endif
    }
}


MprThreadLocal *mprCreateThreadLocal()
{
    MprThreadLocal      *tls;

    if ((tls = mprAllocObj(MprThreadLocal, manageThreadLocal)) == 0) {
        return 0;
    }
#if BLD_UNIX_LIKE
    if (pthread_key_create(&tls->key, NULL) != 0) {
        tls->key = 0;
        return 0;
    }
#elif BLD_WIN_LIKE
    if ((tls->key = TlsAlloc()) < 0) {
        return 0;
    }
#else
    if ((tls->store = mprCreateHash(0, MPR_HASH_STATIC_VALUES)) == 0) {
        return 0;
    }
#endif
    return tls;
}


int mprSetThreadData(MprThreadLocal *tls, void *value)
{
    bool    err;

#if BLD_UNIX_LIKE
    err = pthread_setspecific(tls->key, value) != 0;
#elif BLD_WIN_LIKE
    err = TlsSetValue(tls->key, value) != 0;
#else
    {
        char    key[32];
        itosbuf(key, sizeof(key), (int64) mprGetCurrentOsThread(), 10);
        err = mprAddKey(tls->store, key, value) == 0;
    }
#endif
    return (err) ? MPR_ERR_CANT_WRITE: 0;
}


void *mprGetThreadData(MprThreadLocal *tls)
{
#if BLD_UNIX_LIKE
    return pthread_getspecific(tls->key);
#elif BLD_WIN_LIKE
    return TlsGetValue(tls->key);
#else
    {
        char    key[32];
        itosbuf(key, sizeof(key), (int64) mprGetCurrentOsThread(), 10);
        return mprLookupKey(tls->store, key);
    }
#endif
}


#if BLD_WIN_LIKE
/*
    Map Mpr priority to Windows native priority. Windows priorities range from -15 to +15 (zero is normal). 
    Warning: +15 will not yield the CPU, -15 may get starved. We should be very wary going above +11.
 */

int mprMapMprPriorityToOs(int mprPriority)
{
    mprAssert(mprPriority >= 0 && mprPriority <= 100);
 
    if (mprPriority <= MPR_BACKGROUND_PRIORITY) {
        return THREAD_PRIORITY_LOWEST;
    } else if (mprPriority <= MPR_LOW_PRIORITY) {
        return THREAD_PRIORITY_BELOW_NORMAL;
    } else if (mprPriority <= MPR_NORMAL_PRIORITY) {
        return THREAD_PRIORITY_NORMAL;
    } else if (mprPriority <= MPR_HIGH_PRIORITY) {
        return THREAD_PRIORITY_ABOVE_NORMAL;
    } else {
        return THREAD_PRIORITY_HIGHEST;
    }
}


/*
    Map Windows priority to Mpr priority
 */ 
int mprMapOsPriorityToMpr(int nativePriority)
{
    int     priority;

    priority = (45 * nativePriority) + 50;
    if (priority < 0) {
        priority = 0;
    }
    if (priority >= 100) {
        priority = 99;
    }
    return priority;
}


#elif VXWORKS
/*
    Map MPR priority to VxWorks native priority.
 */

int mprMapMprPriorityToOs(int mprPriority)
{
    int     nativePriority;

    mprAssert(mprPriority >= 0 && mprPriority < 100);

    nativePriority = (100 - mprPriority) * 5 / 2;

    if (nativePriority < 10) {
        nativePriority = 10;
    } else if (nativePriority > 255) {
        nativePriority = 255;
    }
    return nativePriority;
}


/*
    Map O/S priority to Mpr priority.
 */ 
int mprMapOsPriorityToMpr(int nativePriority)
{
    int     priority;

    priority = (255 - nativePriority) * 2 / 5;
    if (priority < 0) {
        priority = 0;
    }
    if (priority >= 100) {
        priority = 99;
    }
    return priority;
}


#else /* UNIX */
/*
    Map MR priority to linux native priority. Unix priorities range from -19 to +19. Linux does -20 to +19. 
 */
int mprMapMprPriorityToOs(int mprPriority)
{
    mprAssert(mprPriority >= 0 && mprPriority < 100);

    if (mprPriority <= MPR_BACKGROUND_PRIORITY) {
        return 19;
    } else if (mprPriority <= MPR_LOW_PRIORITY) {
        return 10;
    } else if (mprPriority <= MPR_NORMAL_PRIORITY) {
        return 0;
    } else if (mprPriority <= MPR_HIGH_PRIORITY) {
        return -8;
    } else {
        return -19;
    }
    mprAssert(0);
    return 0;
}


/*
    Map O/S priority to Mpr priority.
 */ 
int mprMapOsPriorityToMpr(int nativePriority)
{
    int     priority;

    priority = (nativePriority + 19) * (100 / 40); 
    if (priority < 0) {
        priority = 0;
    }
    if (priority >= 100) {
        priority = 99;
    }
    return priority;
}

#endif /* UNIX */


MprWorkerService *mprCreateWorkerService()
{
    MprWorkerService      *ws;

    ws = mprAllocObj(MprWorkerService, manageWorkerService);
    if (ws == 0) {
        return 0;
    }
    ws->mutex = mprCreateLock();
    ws->minThreads = MPR_DEFAULT_MIN_THREADS;
    ws->maxThreads = MPR_DEFAULT_MAX_THREADS;

    /*
        Presize the lists so they cannot get memory allocation failures later on.
     */
    ws->idleThreads = mprCreateList(0, 0);
    mprSetListLimits(ws->idleThreads, ws->maxThreads, -1);
    ws->busyThreads = mprCreateList(0, 0);
    mprSetListLimits(ws->busyThreads, ws->maxThreads, -1);
    return ws;
}


static void manageWorkerService(MprWorkerService *ws, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(ws->busyThreads);
        mprMark(ws->idleThreads);
        mprMark(ws->mutex);
        mprMark(ws->pruneTimer);
    }
}


int mprStartWorkerService()
{
    MprWorkerService    *ws;

    /*
        Create a timer to trim excess workers
     */
    ws = MPR->workerService;
    mprSetMinWorkers(ws->minThreads);
    ws->pruneTimer = mprCreateTimerEvent(NULL, "pruneWorkers", MPR_TIMEOUT_PRUNER, pruneWorkers, ws, MPR_EVENT_QUICK);
    return 0;
}


void mprWakeWorkers()
{
    MprWorkerService    *ws;
    MprWorker           *worker;
    int                 next;

    ws = MPR->workerService;
    mprLock(ws->mutex);
    if (ws->pruneTimer) {
        mprRemoveEvent(ws->pruneTimer);
    }
    /*
        Wake up all idle workers. Busy workers take care of themselves. An idle thread will wakeup, exit and be 
        removed from the busy list and then delete the thread. We progressively remove the last thread in the idle
        list. ChangeState will move the workers to the busy queue.
     */
    for (next = -1; (worker = (MprWorker*) mprGetPrevItem(ws->idleThreads, &next)) != 0; ) {
        changeState(worker, MPR_WORKER_BUSY);
    }
    mprUnlock(ws->mutex);
}


/*
    Define the new minimum number of workers. Pre-allocate the minimum.
 */
void mprSetMinWorkers(int n)
{ 
    MprWorker           *worker;
    MprWorkerService    *ws;

    ws = MPR->workerService;
    mprLock(ws->mutex);
    ws->minThreads = n; 
    mprLog(4, "Pre-start %d workers", ws->minThreads);
    
    while (ws->numThreads < ws->minThreads) {
        worker = createWorker(ws, ws->stackSize);
        ws->numThreads++;
        ws->maxUseThreads = max(ws->numThreads, ws->maxUseThreads);
        changeState(worker, MPR_WORKER_BUSY);
        mprStartThread(worker->thread);
    }
    mprUnlock(ws->mutex);
}


/*
    Define a new maximum number of theads. Prune if currently over the max.
 */
void mprSetMaxWorkers(int n)
{
    MprWorkerService  *ws;

    ws = MPR->workerService;

    mprLock(ws->mutex);
    ws->maxThreads = n; 
    if (ws->numThreads > ws->maxThreads) {
        pruneWorkers(ws, 0);
    }
    if (ws->minThreads > ws->maxThreads) {
        ws->minThreads = ws->maxThreads;
    }
    mprUnlock(ws->mutex);
}


int mprGetMaxWorkers()
{
    return MPR->workerService->maxThreads;
}


/*
    Return the current worker thread object
 */
MprWorker *mprGetCurrentWorker()
{
    MprWorkerService    *ws;
    MprWorker           *worker;
    MprThread           *thread;
    int                 next;

    ws = MPR->workerService;

    mprLock(ws->mutex);
    thread = mprGetCurrentThread();
    for (next = -1; (worker = (MprWorker*) mprGetPrevItem(ws->busyThreads, &next)) != 0; ) {
        if (worker->thread == thread) {
            mprUnlock(ws->mutex);
            return worker;
        }
    }
    mprUnlock(ws->mutex);
    return 0;
}


#if UNUSED && FUTURE && KEEP
/*
    Set the worker as dedicated to the current task
 */
void mprDedicateWorker(MprWorker *worker)
{
    mprLock(worker->workerService->mutex);
    worker->flags |= MPR_WORKER_DEDICATED;
    mprUnlock(worker->workerService->mutex);
}


void mprReleaseWorker(MprWorker *worker)
{
    mprLock(worker->workerService->mutex);
    worker->flags &= ~MPR_WORKER_DEDICATED;
    mprUnlock(worker->workerService->mutex);
}
#endif


void mprActivateWorker(MprWorker *worker, MprWorkerProc proc, void *data)
{
    MprWorkerService    *ws;

    ws = worker->workerService;

    mprLock(ws->mutex);
    worker->proc = proc;
    worker->data = data;
#if UNUSED && FUTURE && KEEP
    mprAssert(worker->flags & MPR_WORKER_DEDICATED);
#endif
    changeState(worker, MPR_WORKER_BUSY);
    mprUnlock(ws->mutex);
}


void mprSetWorkerStartCallback(MprWorkerProc start)
{
    MPR->workerService->startWorker = start;
}


int mprAvailableWorkers()
{
    MprWorkerService    *ws;
    int                 count;

    ws = MPR->workerService;
    mprLock(ws->mutex);
    count = mprGetListLength(ws->idleThreads) + (ws->maxThreads - ws->numThreads);
    mprUnlock(ws->mutex);
    return count;

#if FUTURE && UNUSED && KEEP
    for (next = 0; (worker = (MprWorker*) mprGetNextItem(ws->idleThreads, &next)) != 0; ) {
        if (!(worker->flags & MPR_WORKER_DEDICATED)) {
            count++;
        }
    }
#endif
}


int mprStartWorker(MprWorkerProc proc, void *data)
{
    MprWorkerService    *ws;
    MprWorker           *worker;

    ws = MPR->workerService;
    mprLock(ws->mutex);

    /*
        Try to find an idle thread and wake it up. It will wakeup in workerMain(). If not any available, then add 
        another thread to the worker. Must account for workers we've already created but have not yet gone to work 
        and inserted themselves in the idle/busy queues.
     */
#if UNUSED
    for (next = 0; (worker = (MprWorker*) mprGetNextItem(ws->idleThreads, &next)) != 0; ) {
        if (!(worker->flags & MPR_WORKER_DEDICATED)) {
            break;
        }
    }
#else
    worker = mprGetFirstItem(ws->idleThreads);
#endif
    if (worker) {
        worker->proc = proc;
        worker->data = data;
        changeState(worker, MPR_WORKER_BUSY);

    } else if (ws->numThreads < ws->maxThreads) {

        /*
            Can't find an idle thread. Try to create more workers in the pool. Otherwise, we will have to wait. 
            No need to wakeup the thread -- it will immediately go to work.
         */
        worker = createWorker(ws, ws->stackSize);

        ws->numThreads++;
        ws->maxUseThreads = max(ws->numThreads, ws->maxUseThreads);
        worker->proc = proc;
        worker->data = data;

        changeState(worker, MPR_WORKER_BUSY);
        mprStartThread(worker->thread);

    } else {
        static int warned = 0;
        /*
            No free workers and can't create anymore
         */
        if (warned++ == 0) {
            mprError("No free workers. (Count %d of %d)", ws->numThreads, ws->maxThreads);
        }
        mprUnlock(ws->mutex);
        return MPR_ERR_BUSY;
    }
    mprUnlock(ws->mutex);
    return 0;
}


/*
    Trim idle workers
 */
static void pruneWorkers(MprWorkerService *ws, MprEvent *timer)
{
    MprWorker     *worker;
    int           index;

    if (mprGetDebugMode()) {
        return;
    }
    mprLog(4, "Check to prune idle workers. Pool has %d workers. Limits %d-%d", 
        ws->numThreads, ws->minThreads, ws->maxThreads);
    mprLock(ws->mutex);
    for (index = 0; index < ws->idleThreads->length; index++) {
        if (ws->numThreads <= ws->minThreads) {
            break;
        }
        worker = mprGetItem(ws->idleThreads, index);
        if ((worker->lastActivity + MPR_TIMEOUT_WORKER) < MPR->eventService->now) {
            changeState(worker, MPR_WORKER_PRUNED);
        }
    }
    mprUnlock(ws->mutex);
}


int mprGetAvailableWorkers()
{
    MprWorkerService  *ws;

    ws = MPR->workerService;
    return (int) ws->idleThreads->length + (ws->maxThreads - ws->numThreads); 
}


static int getNextThreadNum(MprWorkerService *ws)
{
    int     rc;

    mprLock(ws->mutex);
    rc = ws->nextThreadNum++;
    mprUnlock(ws->mutex);
    return rc;
}


/*
    Define a new stack size for new workers. Existing workers unaffected.
 */
void mprSetWorkerStackSize(int n)
{
    MPR->workerService->stackSize = n; 
}


void mprGetWorkerServiceStats(MprWorkerService *ws, MprWorkerStats *stats)
{
    mprAssert(ws);

    stats->maxThreads = ws->maxThreads;
    stats->minThreads = ws->minThreads;
    stats->numThreads = ws->numThreads;
    stats->maxUse = ws->maxUseThreads;
    stats->idleThreads = (int) ws->idleThreads->length;
    stats->busyThreads = (int) ws->busyThreads->length;
}


/*
    Create a new thread for the task
 */
static MprWorker *createWorker(MprWorkerService *ws, ssize stackSize)
{
    MprWorker   *worker;

    char    name[16];

    if ((worker = mprAllocObj(MprWorker, manageWorker)) == 0) {
        return 0;
    }
    worker->flags = 0;
    worker->proc = 0;
    worker->cleanup = 0;
    worker->data = 0;
    worker->state = 0;
    worker->workerService = ws;
    worker->idleCond = mprCreateCond();

    mprSprintf(name, sizeof(name), "worker.%u", getNextThreadNum(ws));
    worker->thread = mprCreateThread(name, (MprThreadProc) workerMain, worker, 0);
    return worker;
}


static void manageWorker(MprWorker *worker, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(worker->data);
        mprMark(worker->thread);
        mprMark(worker->workerService);
        mprMark(worker->idleCond);
    }
}


static void workerMain(MprWorker *worker, MprThread *tp)
{
    MprWorkerService    *ws;

    ws = MPR->workerService;
    mprAssert(worker->state == MPR_WORKER_BUSY);
    mprAssert(!worker->idleCond->triggered);

    if (ws->startWorker) {
        (*ws->startWorker)(worker->data, worker);
    }
    mprLock(ws->mutex);

    while (!(worker->state & MPR_WORKER_PRUNED) && !mprIsStopping()) {
        if (worker->proc) {
            mprUnlock(ws->mutex);
            (*worker->proc)(worker->data, worker);
            mprLock(ws->mutex);
            worker->proc = 0;
        }
        worker->lastActivity = MPR->eventService->now;
        changeState(worker, MPR_WORKER_IDLE);

        mprAssert(worker->cleanup == 0);
        if (worker->cleanup) {
            (*worker->cleanup)(worker->data, worker);
            worker->cleanup = NULL;
        }
        worker->data = 0;
        mprUnlock(ws->mutex);

        /*
            Sleep till there is more work to do. Yield for GC first.
         */
        mprYield(MPR_YIELD_STICKY);
        mprWaitForCond(worker->idleCond, -1);
        mprResetYield();
        mprLock(ws->mutex);
    }
    changeState(worker, 0);
    worker->thread = 0;
    ws->numThreads--;
    mprUnlock(ws->mutex);
    mprLog(4, "Worker exiting. There are %d workers remaining in the pool.", ws->numThreads);
}


static int changeState(MprWorker *worker, int state)
{
    MprWorkerService    *ws;
    MprList             *lp;
    int                 wake;

    mprAssert(worker->state != state);

    wake = 0;
    lp = 0;
    ws = worker->workerService;
    mprLock(ws->mutex);

    switch (worker->state) {
    case MPR_WORKER_BUSY:
        lp = ws->busyThreads;
        break;

    case MPR_WORKER_IDLE:
#if UNUSED && FUTURE && KEEP
        if (!(worker->flags & MPR_WORKER_DEDICATED)) {
#endif
            lp = ws->idleThreads;
#if UNUSED && FUTURE && KEEP
        }
#endif
        wake = 1;
        break;
        
    case MPR_WORKER_PRUNED:
        break;
    }

    /*
        Reassign the worker to the appropriate queue
     */
    if (lp) {
        mprRemoveItem(lp, worker);
    }
    lp = 0;
    switch (state) {
    case MPR_WORKER_BUSY:
        lp = ws->busyThreads;
        break;

    case MPR_WORKER_IDLE:
#if UNUSED && FUTURE && KEEP
        if (!(worker->flags & MPR_WORKER_DEDICATED)) {
#endif
            lp = ws->idleThreads;
#if UNUSED && FUTURE && KEEP
        }
#endif
        mprWakePendingDispatchers();
        break;

    case MPR_WORKER_PRUNED:
        /* Don't put on a queue and the thread will exit */
        mprWakePendingDispatchers();
        break;
    }
    worker->state = state;

    if (lp) {
        if (mprAddItem(lp, worker) < 0) {
            mprUnlock(ws->mutex);
            mprAssert(!MPR_ERR_MEMORY);
            return MPR_ERR_MEMORY;
        }
    }
    mprUnlock(ws->mutex);
    if (wake) {
        mprSignalCond(worker->idleCond); 
    }
    return 0;
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
    details at: http://embedthis.com/downloads/gplLicense.html
    
    This program is distributed WITHOUT ANY WARRANTY; without even the 
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
    
    This GPL license does NOT permit incorporating this software into 
    proprietary programs. If you are unable to comply with the GPL, you must
    acquire a commercial license to use this software. Commercial licenses 
    for this software and support services are available from Embedthis 
    Software at http://embedthis.com 
    
    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
