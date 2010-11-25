/** 
    benchMpr.c - Benchmark various MPR facilities
  
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/********************************** Locals ************************************/

static int      testAllocOnly = 0;      /* Test alloc only  */
static int      iterations = 1;         /* Benchmark iterations */
static int      workers = 0;            /* Number of worker threads */

static Mpr      *mpr;
static MprCond  *complete;              /* Condition set when benchmark complete */
static int      markCount;              /* Flag set when benchmark complete */
static MprMutex *mutex;                 /* Test synchronization */

/***************************** Forward Declarations ***************************/

static void     doBenchmark(void *thread);
static void     endMark(MprTime start, int count, char *msg);
static void     eventCallback(void *data, MprEvent *ep);
static size_t   memsize();
static MprTime  startMark();
static void     testMalloc();
static void     timerCallback(void *data, MprEvent *ep);
volatile int    testComplete;

/*********************************** Code *************************************/

int benchMain(int argc, char *argv[])
{
    MprThread       *thread;
    char            *argp;
    int             err, i, nextArg;

    mpr = mprCreate(argc, argv, 0);

#if VXWORKS || WINCE
    /*
        These platforms pass an arg string in via the argc value. Assumes 32-bit.
     */
    mprMakeArgv("http", (char*) argc, &argc, &argv);
#endif

    iterations = 5;
    err = 0;

    for (nextArg = 1; nextArg < argc; nextArg++) {
        argp = argv[nextArg];
        if (*argp != '-') {
            break;
        }
        if (strcmp(argp, "--iterations") == 0 || strcmp(argp, "-i") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                iterations = atoi(argv[++nextArg]);
            }

        } else if (strcmp(argp, "--alloc") == 0 || strcmp(argp, "-a") == 0) {
            testAllocOnly++;
        } else if (strcmp(argp, "--workers") == 0 || strcmp(argp, "-w") == 0) {
            if (nextArg >= argc) {
                err++;
            } else {
                i = atoi(argv[++nextArg]);
                if (i <= 0 || i > 100) {
                    mprError("%s: Bad number of worker threads (0-100)", mprGetAppName(mpr));
                    exit(2);
                
                }
                workers = i;
            }
        } else {
            err++;
        }
    }
    if (err) {
        mprPrintf("usage: bench [-em] [-i iterations] [-t workers]\n");
        mprRawLog(0, "usage: %s [options]\n"
            "    --iterations count  # Number of iterations to run the test\n"
            "    --workers count     # Set maximum worker threads\n",
            mprGetAppName(mpr));
        exit(2);
    }

    mutex = mprCreateLock(mpr);
    mprSetMaxWorkers(workers);
    mprStart(mpr);

    thread = mprCreateThread("bench", (MprThreadProc) doBenchmark, (void*) MPR, 0);
    mprStartThread(thread);
    
    mprSleep(999999);
    
    while (!testComplete) {
        mprServiceEvents(mprGetDispatcher(mpr), 250, 0);
    }
    mprPrintMemReport("Memory Report", 0);
    mprFree(mpr);
    return 0;
}


#if WINCE
int APIENTRY WinMain(HINSTANCE inst, HINSTANCE junk, LPWSTR args, int junk2)
{
    return benchMain((int) args, NULL);
}

#elif !VXWORKS
int main(int argc, char **argv)
{
    return benchMain(argc, argv);
}
#endif


/*
    Do a performance benchmark
 */ 
static void doBenchmark(void *thread)
{
    MprTime         start;
    MprList         *list;
    MprDispatcher   *dispatcher;
    void            *mp;
    int             count, i;
    MprMutex        *lock;

    mprCollectGarbage();

    mprPrintf("Group\t%-30s\t%13s\t%12s\n", "Benchmark", "Microsec", "Elapsed-sec");
    complete = mprCreateCond();

    testMalloc();
    mprCollectGarbage();

    if (!testAllocOnly) {
        /*
            Locking primitives
         */
        mprPrintf("Lock Benchmarks\n");
        lock = mprCreateLock();
        count = 5000000 * iterations;
        start = startMark();
        for (i = 0; i < count; i++) {
            mprLock(lock);
            mprUnlock(lock);
        }
        endMark(start, count, "Mutex lock|unlock");
        mprFree(lock);

        /*
            Condition signal / wait
         */
        mprPrintf("Cond Benchmarks\n");
        count = 1000000 * iterations;
        start = startMark();
        mprResetCond(complete);
        for (i = 0; i < count; i++) {
            mprSignalCond(complete);
            mprWaitForCond(complete, -1);
        }
        endMark(start, count, "Cond signal|wait");

        /*
            List
         */
        mprPrintf("List Benchmarks\n");
        count = 20000000 * iterations;
        list = mprCreateList();
        start = startMark();
        for (i = 0; i < count; i++) {
            mprAddItem(list, (void*) (long) i);
            mprRemoveItem(list, (void*) (long) i);
        }
        endMark(start, count, "Link insert|remove");
        mprFree(list);

        /*
            Events
         */
        mprPrintf("Event Benchmarks\n");
        mprResetCond(complete);
        count = 100000 * iterations;
        markCount = count;
        start = startMark();
        dispatcher = mprGetDispatcher();
        for (i = 0; i < count; i++) {
            mprCreateEvent(dispatcher, "eventBenchmark", 0, eventCallback, (void*) (long) i, 0);
        }
        mprWaitForCond(complete, -1);
        endMark(start, count, "Event (create|run|delete)");


        /*
            Test timer creation, run and delete (make a million timers!)
         */
        mprPrintf("Timer\n");
        mprResetCond(complete);
        count = 50000 * iterations;
        markCount = count;
        start = startMark();
        for (i = 0; i < count; i++) {
            mprCreateTimerEvent(mprGetDispatcher(), "timerBenchmark", 0, timerCallback, (void*) (long) i, 0);
        }
        mprWaitForCond(complete, -1);
        endMark(start, count, "Timer (create|delete)");

        /*
            Malloc (1K)
         */
        mprPrintf("Malloc 1K Benchmarks\n");
        count = 2000000 * iterations;
        start = startMark();
        for (i = 0; i < count; i++) {
            mp = mprAlloc(1024);
            mprFree(mp);
        }
        endMark(start, count, "Alloc mprAlloc(1K)|mprFree");
    }
    testComplete = 1;
}


static void testMalloc()
{
    MprTime     start;
    size_t      base;
    char        *ptr;
    int         count, i;

#if UNUSED && KEEP
    /*
        Demonstrate validation checking if end of block is overwritten. 
        Must build with debug on and MPR_ALLOC_VERIFY enabled in mpr.h
     */
    ptr = mprAlloc(1024);
    memset(ptr, 0, 1024 + (2 * sizeof(void*)));
    mprFree(ptr);
#endif

    mprPrintf("Alloc/Malloc overhead\n");
    count = 200000 * iterations;

#if MALLOC
    /*
        malloc(1)
     */
    base = memsize();
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = malloc(1);
        memset(ptr, 0, 1);
    }
    endMark(start, count, "Alloc malloc(1)");
    mprPrintf("\tMalloc overhead per block %d\n\n", ((memsize() - base) / count) - 1);

    /*
        malloc(8)
     */
    base = memsize();
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = malloc(8);
        memset(ptr, 0, 8);
    }
    endMark(start, count, "Alloc malloc(8)");
    mprPrintf("\tMalloc overhead per block %d (approx)\n\n", ((memsize() - base) / count) - 8);

    /*
        malloc(16)
     */
    base = memsize();
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = malloc(16);
        memset(ptr, 0, 16);
    }
    endMark(start, count, "Alloc malloc(16)");
    mprPrintf("\tMalloc overhead per block %d (approx)\n\n", ((memsize() - base) / count) - 16);

    /*
        malloc(32)
     */
    base = memsize();
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = malloc(32);
        memset(ptr, 0, 32);
    }
    endMark(start, count, "Alloc malloc(32)");
    mprPrintf("\tMalloc overhead per block %d (approx)\n\n", ((memsize() - base) / count) - 32);

    /*
        malloc+free(8)
     */
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = malloc(8);
        memset(ptr, 0, 8);
        mprNop(ptr);
        free(ptr);
    }
    endMark(start, count, "Alloc malloc+free(8)");
    mprPrintf("\n");
#endif

    /*
        mprAlloc(1)
     */
    base = memsize();
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = mprAlloc(1);
        memset(ptr, 0, 1);
    }
    endMark(start, count, "Alloc mprAlloc(1)");
    // mprPrintf("\tMpr overhead per block %d (approx)\n\n", ((memsize() - base) / count) - 1);

    /*
        mprAlloc(8)
     */
    base = memsize();
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = mprAlloc(8);
        memset(ptr, 0, 8);
    }
    endMark(start, count, "Alloc mprAlloc(8)");
    // mprPrintf("\tMpr overhead per block %d (approx)\n\n", ((memsize() - base) / count) - 8);

    /*
        mprAlloc(16)
     */
    base = memsize();
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = mprAlloc(16);
        memset(ptr, 0, 16);
    }
    endMark(start, count, "Alloc mprAlloc(16)");
    // mprPrintf("\tMpr overhead per block %d (approx)\n\n", ((memsize() - base) / count) - 16);

    /*
        mprAlloc(32)
     */
    base = memsize();
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = mprAlloc(32);
        memset(ptr, 0, 32);
    }
    endMark(start, count, "Alloc mprAlloc(32)");
    // mprPrintf("\tMpr overhead per block %d (approx)\n\n", ((memsize() - base) / count) - 32);

    /*
        mprAlloc + mprFree(8)
     */
    start = startMark();
    for (i = 0; i < count; i++) {
        ptr = mprAlloc(8);
        memset(ptr, 0, 8);
        mprFree(ptr);
    }
    endMark(start, count, "Alloc mprAlloc + mprFree(8)");
    mprPrintf("\n");
}


/*
    Event callback 
 */
static void eventCallback(void *data, MprEvent *event)
{
    mprLock(mutex);
    if (--markCount == 0) {
        mprSignalCond(complete);
    }
    mprStopContinuousEvent(event);
    mprUnlock(mutex);
    mprFree(event);
}


/*
    Timer callback 
 */
static void timerCallback(void *data, MprEvent *event)
{
    mprLock(mutex);
    if (--markCount == 0) {
        mprSignalCond(complete);
    }
    mprStopContinuousEvent(event);
    mprUnlock(mutex);
    mprFree(event);
}


static MprTime startMark()
{
    MprTime     now;

    now = mprGetTime();
    return now;
}


static void endMark(MprTime start, int count, char *msg)
{
    MprTime     elapsed;

    elapsed = mprGetElapsedTime(start);
    mprPrintf("\t%-30s\t%13.2f\t%12.2f\n", msg, elapsed * 1000.0 / count, elapsed / 1000.0);
}


static size_t memsize() 
{
#if MACOSX || FREEBSD
    struct rusage   rusage;

    getrusage(RUSAGE_SELF, &rusage);
    return rusage.ru_maxrss;
#else
    return 1;
#endif
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
