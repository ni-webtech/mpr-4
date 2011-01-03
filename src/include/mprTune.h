/*
    mprTune.h - Header for the Multithreaded Portable Runtime (MPR) Base.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/******************************* Documentation ********************************/
/*
    See mpr.dox for additional documentation.
 */

/******************************************************************************/

#ifndef _h_MPR_TUNE
#define _h_MPR_TUNE 1

/********************************** Includes **********************************/

#ifdef __cplusplus
extern "C" {
#endif

/******************************* Tunable Constants ****************************/
/*
    Build tuning
 */
#define MPR_TUNE_SIZE       1       /* Tune for size */
#define MPR_TUNE_BALANCED   2       /* Tune balancing speed and size */
#define MPR_TUNE_SPEED      3       /* Tune for speed */

#ifndef BLD_TUNE
#define BLD_TUNE MPR_TUNE_BALANCED
#endif

#if BLD_TUNE == MPR_TUNE_SIZE || DOXYGEN
    /*
        Squeeze mode optimizes to reduce memory usage
     */
    #define MPR_MAX_FNAME           256           /**< Reasonable filename size */
    #define MPR_MAX_PATH            512           /**< Reasonable path name size */
    #define MPR_MAX_URL             512           /**< Max URL size. Also request URL size. */
    #define MPR_DEFAULT_STACK       (64 * 1024)   /**< Default thread stack size (64K) */
    #define MPR_MAX_STRING          1024          /**< Maximum (stack) string size */
    #define MPR_DEFAULT_ALLOC       64            /**< Default small alloc size */
    #define MPR_DEFAULT_HASH_SIZE   23            /**< Default size of hash table */ 
    #define MPR_MAX_ARGC            128           /**< Reasonable max of args */
    #define MPR_MAX_LOG_STRING      512           /**< Maximum log message */
    #define MPR_BUFSIZE             4096          /**< Reasonable size for buffers */
    #define MPR_BUF_INCR            4096          /**< Default buffer growth inc */
    #define MPR_EPOLL_SIZE          32            /**< Epoll backlog */
    #define MPR_MAX_BUF             4194304       /**< Max buffer size */
    #define MPR_XML_BUFSIZE         4096          /**< XML read buffer size */
    #define MPR_SSL_BUFSIZE         4096          /**< SSL has 16K max*/
    #define MPR_LIST_INCR           8             /**< Default list growth inc */
    #define MPR_FILES_HASH_SIZE     29            /**< Hash size for rom file system */
    #define MPR_TIME_HASH_SIZE      67            /**< Hash size for time token lookup */
    #define MPR_MEM_REGION_SIZE     (128 * 1024)  /**< Memory allocation chunk size */
    #define MPR_GC_LOW_MEM          (32 * 1024)   /**< Free memory low water mark before invoking GC */
    #define MPR_NEW_QUOTA           (4 * 1024)    /**< new allocations before a GC is worthwhile */
    #define MPR_GC_WORKERS          0             /**< Run garbage collection non-concurrently */
    
#elif BLD_TUNE == MPR_TUNE_BALANCED
    
    /*
        Tune balancing speed and size
     */
    #define MPR_MAX_FNAME           256
    #define MPR_MAX_PATH            1024
    #define MPR_MAX_URL             2048
    #define MPR_DEFAULT_STACK       (128 * 1024)
    #define MPR_MAX_STRING          2048
    #define MPR_DEFAULT_ALLOC       256
    #define MPR_DEFAULT_HASH_SIZE   43
    #define MPR_MAX_ARGC            256
    #define MPR_MAX_LOG_STRING      8192
    #define MPR_BUFSIZE             4096
    #define MPR_BUF_INCR            4096
    #define MPR_MAX_BUF             -1
    #define MPR_EPOLL_SIZE          64
    #define MPR_XML_BUFSIZE         4096
    #define MPR_SSL_BUFSIZE         4096
    #define MPR_LIST_INCR           16
    #define MPR_FILES_HASH_SIZE     61
    #define MPR_TIME_HASH_SIZE      89
    #define MPR_MEM_REGION_SIZE     (256 * 1024)
    #define MPR_GC_LOW_MEM          (64 * 1024)
    #define MPR_NEW_QUOTA           (16 * 1024) 
    #define MPR_GC_WORKERS          1
    
#else
    /*
        Tune for speed
     */
    #define MPR_MAX_FNAME           1024
    #define MPR_MAX_PATH            2048
    #define MPR_MAX_URL             4096
    #define MPR_DEFAULT_STACK       (256 * 1024)
    #define MPR_MAX_STRING          4096
    #define MPR_DEFAULT_ALLOC       512
    #define MPR_DEFAULT_HASH_SIZE   97
    #define MPR_MAX_ARGC            512
    #define MPR_MAX_LOG_STRING      8192
    #define MPR_BUFSIZE             8192
    #define MPR_MAX_BUF             -1
    #define MPR_EPOLL_SIZE          128
    #define MPR_XML_BUFSIZE         4096
    #define MPR_SSL_BUFSIZE         8192
    #define MPR_LIST_INCR           16
    #define MPR_BUF_INCR            1024
    #define MPR_FILES_HASH_SIZE     61
    #define MPR_TIME_HASH_SIZE      97
    #define MPR_MEM_REGION_SIZE     (1024 * 1024)
    #define MPR_GC_LOW_MEM          (128 * 1024)
    #define MPR_NEW_QUOTA           (1024 * 32) 
    #define MPR_GC_WORKERS          2
#endif

/*
    Select wakeup port. Port can be any free port number. If this is not free, the MPR will use the next free port.
 */
#define MPR_DEFAULT_BREAK_PORT  9473
#define MPR_FD_MIN              32

#define MPR_MAX_IP_NAME         128
#define MPR_MAX_IP_ADDR         16
#define MPR_MAX_IP_PORT         6
#define MPR_MAX_IP_ADDR_PORT    32

/*
    Signal sent on Unix to break out of a select call.
 */
#define MPR_WAIT_SIGNAL         (SIGUSR2)

/*
    Socket event message
 */
#define MPR_SOCKET_MESSAGE      (WM_USER + 32)

/*
    Priorities
 */
#define MPR_BACKGROUND_PRIORITY 15          /**< May only get CPU if idle */
#define MPR_LOW_PRIORITY        25
#define MPR_NORMAL_PRIORITY     50          /**< Normal (default) priority */
#define MPR_HIGH_PRIORITY       75
#define MPR_CRITICAL_PRIORITY   99          /**< May not yield */

#define MPR_EVENT_PRIORITY      50          /**< Normal priority */ 
#define MPR_WORKER_PRIORITY     50          /**< Normal priority */
#define MPR_REQUEST_PRIORITY    50          /**< Normal priority */

/* 
    Timeouts
 */
#define MPR_TIMEOUT_CMD         60000       /**< Command Request timeout (60 sec) */
#define MPR_TIMEOUT_SOCKETS     10000       /**< General sockets timeout */
#define MPR_TIMEOUT_LOG_STAMP   3600000     /**< Time between log time stamps (1 hr) */
#define MPR_TIMEOUT_PRUNER      600000      /**< Time between pruner runs (10 min) */
#define MPR_TIMEOUT_START_TASK  2000        /**< Time to start tasks running */
#define MPR_TIMEOUT_STOP_TASK   10000       /**< Time to stop or reap tasks */
#define MPR_TIMEOUT_STOP_THREAD 10000       /**< Time to stop running threads */
#define MPR_TIMEOUT_STOP        5000        /**< Wait when stopping resources */
#define MPR_TIMEOUT_LINGER      2000        /**< Close socket linger timeout */
#define MPR_TIMEOUT_HANDLER     10000       /**< Wait period when removing a wait handler */
#define MPR_TIMEOUT_GC_SYNC     5000        /**< Wait period for threads to synchronize */

#define MPR_TICKS_PER_SEC       1000        /**< Time ticks per second */
#define MPR_MAX_TIMEOUT         MAXINT


/*
    Default thread counts
 */
#define MPR_DEFAULT_MIN_THREADS 0           /**< Default min threads */
#define MPR_DEFAULT_MAX_THREADS 20          /**< Default max threads */

/*
    Debug control
 */
#define MPR_MAX_BLOCKED_LOCKS   100         /* Max threads blocked on lock */
#define MPR_MAX_RECURSION       15          /* Max recursion with one thread */
#define MPR_MAX_LOCKS           512         /* Total lock count max */
#define MPR_MAX_LOCK_TIME       (60 * 1000) /* Time in msec to hold a lock */

#define MPR_TIMER_TOLERANCE     2           /* Used in timer calculations */
#define MPR_CMD_TIMER_PERIOD    5000        /* Check for expired commands */

/*
    Events
 */
#define MPR_EVENT_TIME_SLICE    20          /* 20 msec */

/*
    Maximum number of files
 */
#define MPR_MAX_FILE            256

/*
    Event notification mechanism
 */
#if LINUX || FREEBSD
    #define MPR_EVENT_EPOLL     1
#elif MACOSX || SOLARIS
    #define MPR_EVENT_KQUEUE    1
#elif VXWORKS || WINCE || CYGWIN
    #define MPR_EVENT_SELECT    1
#elif WIN
    #define MPR_EVENT_ASYNC     1
#else
    #define MPR_EVENT_POLL      1
#endif

#if UNUSED
#undef MPR_EVENT_EPOLL
#undef MPR_EVENT_POLL
#undef MPR_EVENT_SELECT
#undef MPR_EVENT_ASYNC
#undef MPR_EVENT_KQUEUE
#define MPR_EVENT_SELECT 1
#endif

/*
    Garbage collector tuning
 */
#define MPR_MIN_TIME_FOR_GC     2                               /**< Wait till 2 milliseconds of idle time possible */
    
#ifdef __cplusplus
}
#endif

#endif /* _h_MPR_TUNE */


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
