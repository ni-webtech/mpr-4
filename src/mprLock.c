/**
    mprLock.c - Thread Locking Support

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/*********************************** Includes *********************************/

#include    "mpr.h"

/***************************** Forward Declarations ***************************/

static void manageLock(MprMutex *lock, int flags);

/************************************ Code ************************************/

MprMutex *mprCreateLock()
{
    MprMutex    *lock;
#if BIT_UNIX_LIKE
    pthread_mutexattr_t attr;
#endif
    if ((lock = mprAllocObj(MprMutex, manageLock)) == 0) {
        return 0;
    }
#if BIT_UNIX_LIKE
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&lock->cs, &attr);
    pthread_mutexattr_destroy(&attr);
#elif WINCE
    InitializeCriticalSection(&lock->cs);
#elif BIT_WIN_LIKE
    InitializeCriticalSectionAndSpinCount(&lock->cs, 5000);
#elif VXWORKS
    /* Removed SEM_INVERSION_SAFE */
    lock->cs = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE);
    if (lock->cs == 0) {
        mprAssert(0);
        return 0;
    }
#endif
    return lock;
}


static void manageLock(MprMutex *lock, int flags)
{
    if (flags & MPR_MANAGE_FREE) {
        mprAssert(lock);
#if BIT_UNIX_LIKE
        pthread_mutex_destroy(&lock->cs);
#elif BIT_WIN_LIKE
        DeleteCriticalSection(&lock->cs);
        lock->cs.SpinCount = 0;
#elif VXWORKS
        semDelete(lock->cs);
#endif
    }
}


MprMutex *mprInitLock(MprMutex *lock)
{
#if BIT_UNIX_LIKE
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&lock->cs, &attr);
    pthread_mutexattr_destroy(&attr);

#elif WINCE
    InitializeCriticalSection(&lock->cs);

#elif BIT_WIN_LIKE
    InitializeCriticalSectionAndSpinCount(&lock->cs, 5000);

#elif VXWORKS
    /* Removed SEM_INVERSION_SAFE */
    lock->cs = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE);
    if (lock->cs == 0) {
        mprAssert(0);
        return 0;
    }
#endif
    return lock;
}


/*
    Try to attain a lock. Do not block! Returns true if the lock was attained.
 */
bool mprTryLock(MprMutex *lock)
{
    int     rc;

    if (lock == 0) return 0;

#if BIT_UNIX_LIKE
    rc = pthread_mutex_trylock(&lock->cs) != 0;
#elif BIT_WIN_LIKE
    /* Rely on SpinCount being non-zero */
    if (lock->cs.SpinCount) {
        rc = TryEnterCriticalSection(&lock->cs) == 0;
    } else {
        rc = 0;
    }
#elif VXWORKS
    rc = semTake(lock->cs, NO_WAIT) != OK;
#endif
#if BIT_DEBUG
    lock->owner = mprGetCurrentOsThread();
#endif
    return (rc) ? 0 : 1;
}


MprSpin *mprCreateSpinLock()
{
    MprSpin    *lock;

    if ((lock = mprAllocObj(MprSpin, mprManageSpinLock)) == 0) {
        return 0;
    }
    return mprInitSpinLock(lock);
}


void mprManageSpinLock(MprSpin *lock, int flags)
{
    if (flags & MPR_MANAGE_FREE) {
        mprAssert(lock);
#if USE_MPR_LOCK || MACOSX
        ;
#elif BIT_UNIX_LIKE && BIT_HAS_SPINLOCK
        pthread_spin_destroy(&lock->cs);
#elif BIT_UNIX_LIKE
        pthread_mutex_destroy(&lock->cs);
#elif BIT_WIN_LIKE
        DeleteCriticalSection(&lock->cs);
        lock->cs.SpinCount = 0;
#elif VXWORKS
        semDelete(lock->cs);
#endif
    }
}


/*
    Static version just for mprAlloc which needs locks that don't allocate memory.
 */
MprSpin *mprInitSpinLock(MprSpin *lock)
{
#if BIT_UNIX_LIKE && !BIT_HAS_SPINLOCK && !MACOSX
    pthread_mutexattr_t attr;
#endif

#if USE_MPR_LOCK
    mprInitLock(&lock->cs);
#elif MACOSX
    lock->cs = OS_SPINLOCK_INIT;
#elif BIT_UNIX_LIKE && BIT_HAS_SPINLOCK
    pthread_spin_init(&lock->cs, 0);
#elif BIT_UNIX_LIKE
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&lock->cs, &attr);
    pthread_mutexattr_destroy(&attr);
#elif WINCE
    InitializeCriticalSection(&lock->cs);
#elif BIT_WIN_LIKE
    InitializeCriticalSectionAndSpinCount(&lock->cs, 5000);
#elif VXWORKS
    #if FUTURE
        spinLockTaskInit(&lock->cs, 0);
    #else
        /* Removed SEM_INVERSION_SAFE */
        lock->cs = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE);
        if (lock->cs == 0) {
            mprAssert(0);
            return 0;
        }
    #endif
#endif /* VXWORKS */
#if BIT_DEBUG
    lock->owner = 0;
#endif
    return lock;
}


/*
    Try to attain a lock. Do not block! Returns true if the lock was attained.
 */
bool mprTrySpinLock(MprSpin *lock)
{
    int     rc;

    if (lock == 0) return 0;

#if USE_MPR_LOCK
    mprTryLock(&lock->cs);
#elif MACOSX
    rc = !OSSpinLockTry(&lock->cs);
#elif BIT_UNIX_LIKE && BIT_HAS_SPINLOCK
    rc = pthread_spin_trylock(&lock->cs) != 0;
#elif BIT_UNIX_LIKE
    rc = pthread_mutex_trylock(&lock->cs) != 0;
#elif BIT_WIN_LIKE
    /* Rely on SpinCount being non-zero */
    if (lock->cs.SpinCount) {
        rc = TryEnterCriticalSection(&lock->cs) == 0;
    } else {
        rc = 0;
    }
#elif VXWORKS
    rc = semTake(lock->cs, NO_WAIT) != OK;
#endif
#if BIT_DEBUG
    if (rc == 0) {
        mprAssert(lock->owner != mprGetCurrentOsThread());
        lock->owner = mprGetCurrentOsThread();
    }
#endif
    return (rc) ? 0 : 1;
}


/*
    Big global lock. Avoid using this.
 */
void mprGlobalLock()
{
    if (MPR && MPR->mutex) {
        mprLock(MPR->mutex);
    }
}


void mprGlobalUnlock()
{
    if (MPR && MPR->mutex) {
        mprUnlock(MPR->mutex);
    }
}


#if BIT_USE_LOCK_MACROS
/*
    Still define these even if using macros to make linking with *.def export files easier
 */
#undef mprLock
#undef mprUnlock
#undef mprSpinLock
#undef mprSpinUnlock
#endif

/*
    Lock a mutex
 */
void mprLock(MprMutex *lock)
{
    if (lock == 0) return;
#if BIT_UNIX_LIKE
    pthread_mutex_lock(&lock->cs);
#elif BIT_WIN_LIKE
    /* Rely on SpinCount being non-zero */
    if (lock->cs.SpinCount) {
        EnterCriticalSection(&lock->cs);
    }
#elif VXWORKS
    semTake(lock->cs, WAIT_FOREVER);
#endif
#if BIT_DEBUG
    /* Store last locker only */ 
    lock->owner = mprGetCurrentOsThread();
#endif
}


void mprUnlock(MprMutex *lock)
{
    if (lock == 0) return;
#if BIT_UNIX_LIKE
    pthread_mutex_unlock(&lock->cs);
#elif BIT_WIN_LIKE
    LeaveCriticalSection(&lock->cs);
#elif VXWORKS
    semGive(lock->cs);
#endif
}


/*
    Use functions for debug mode. Production release uses macros
 */
/*
    Lock a mutex
 */
void mprSpinLock(MprSpin *lock)
{
    if (lock == 0) return;

#if BIT_DEBUG
    /*
        Spin locks don't support recursive locking on all operating systems.
     */
    mprAssert(lock->owner != mprGetCurrentOsThread());
#endif

#if USE_MPR_LOCK
    mprLock(&lock->cs);
#elif MACOSX
    OSSpinLockLock(&lock->cs);
#elif BIT_UNIX_LIKE && BIT_HAS_SPINLOCK
    pthread_spin_lock(&lock->cs);
#elif BIT_UNIX_LIKE
    pthread_mutex_lock(&lock->cs);
#elif BIT_WIN_LIKE
    if (lock->cs.SpinCount) {
        EnterCriticalSection(&lock->cs);
    }
#elif VXWORKS
    semTake(lock->cs, WAIT_FOREVER);
#endif
#if BIT_DEBUG
    mprAssert(lock->owner != mprGetCurrentOsThread());
    lock->owner = mprGetCurrentOsThread();
#endif
}


void mprSpinUnlock(MprSpin *lock)
{
    if (lock == 0) return;

#if BIT_DEBUG
    lock->owner = 0;
#endif

#if USE_MPR_LOCK
    mprUnlock(&lock->cs);
#elif MACOSX
    OSSpinLockUnlock(&lock->cs);
#elif BIT_UNIX_LIKE && BIT_HAS_SPINLOCK
    pthread_spin_unlock(&lock->cs);
#elif BIT_UNIX_LIKE
    pthread_mutex_unlock(&lock->cs);
#elif BIT_WIN_LIKE
    LeaveCriticalSection(&lock->cs);
#elif VXWORKS
    semGive(lock->cs);
#endif
}


/*
    @copy   default

    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.

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
