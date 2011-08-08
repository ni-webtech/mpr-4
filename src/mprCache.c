/**
    mprCache.c - In-process caching

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/************************************ Locals **********************************/

static MprCache *shared;                /* Singleton shared cache */

typedef struct CacheItem
{
    char        *key;                   /* Original key */
    char        *data;                  /* Cache data */
    MprTime     expires;                /* Fixed expiry date. If zero, key is imortal. */
    MprTime     lifespan;               /* Lifespan after each access to key (msec) */
    int64       version;
} CacheItem;

#define CACHE_TIMER_PERIOD      (60 * MPR_TICKS_PER_SEC)
#define CACHE_HASH_SIZE         257
#define CACHE_LIFESPAN          (86400 * MPR_TICKS_PER_SEC)

/*********************************** Forwards *********************************/

static void localPruner(MprCache *cache, MprEvent *event);
static void manageCache(MprCache *cache, int flags);
static void manageCacheItem(CacheItem *item, int flags);
static void removeItem(MprCache *cache, CacheItem *item);

/************************************* Code ***********************************/

MprCache *mprCreateCache(int options)
{
    MprCache    *cache;
    int         wantShared;

    if ((cache = mprAllocObj(MprCache, manageCache)) == 0) {
        return 0;
    }
    wantShared = (options & MPR_CACHE_SHARED);
    if (wantShared && shared) {
        cache->shared = shared;
    } else {
        cache->mutex = mprCreateLock();
        cache->store = mprCreateHash(CACHE_HASH_SIZE, 0);
        cache->maxMem = MAXSSIZE;
        cache->maxKeys = MAXSSIZE;
        cache->resolution = CACHE_TIMER_PERIOD;
        cache->lifespan = CACHE_LIFESPAN;
        if (wantShared) {
            shared = cache;
        }
    }
    return cache;
}


void *mprDestroyCache(MprCache *cache)
{
    mprAssert(cache);

    if (cache->timer && cache != shared) {
        mprRemoveEvent(cache->timer);
        cache->timer = 0;
    }
    //  MOB - race here
    if (cache == shared) {
        shared = 0;
    }
    return 0;
}


int mprExpireCache(MprCache *cache, cchar *key, MprTime expires)
{
    CacheItem   *item;

    mprAssert(cache);
    mprAssert(key && *key);

    if (cache->shared) {
        cache = cache->shared;
        mprAssert(cache == shared);
    }
    lock(cache);
    if ((item = mprLookupKey(cache->store, key)) == 0) {
        unlock(cache);
        return MPR_ERR_CANT_FIND;
    }
    if (expires == 0) {
        removeItem(cache, item);
    } else {
        item->expires = expires;
    }
    unlock(cache);
    return 0;
}


int64 mprIncCache(MprCache *cache, cchar *key, int64 amount)
{
    CacheItem   *item;
    char        nbuf[32];

    mprAssert(cache);
    mprAssert(key && *key);

    if (cache->shared) {
        cache = cache->shared;
        mprAssert(cache == shared);
    }

    lock(cache);
    if ((item = mprLookupKey(cache->store, key)) == 0) {
        if ((item = mprAllocObj(CacheItem, manageCacheItem)) == 0) {
            return 0;
        }
    } else {
        amount += stoi(item->data, 10, 0);
    }
    if (item->data) {
        cache->usedMem -= slen(item->data);
    }
    item->data = itos(nbuf, sizeof(nbuf), amount, 10);
    cache->usedMem += slen(item->data);
    item->version++;
    unlock(cache);
    return stoi(item->data, 10, 0);
}


char *mprReadCache(MprCache *cache, cchar *key, int64 *version)
{
    CacheItem   *item;
    char        *result;

    mprAssert(cache);
    mprAssert(key && *key);

    if (cache->shared) {
        cache = cache->shared;
        mprAssert(cache == shared);
    }
    lock(cache);
    if ((item = mprLookupKey(cache->store, key)) == 0) {
        unlock(cache);
        return 0;
    }
    if (item->expires && item->expires <= mprGetTime()) {
        unlock(cache);
        return 0;
    }
    if (version) {
        *version = item->version;
    } else {
        result = item->data;
    }
    unlock(cache);
    return result;
}


bool mprRemoveCache(MprCache *cache, cchar *key)
{
    CacheItem   *item;
    bool        result;

    mprAssert(cache);
    mprAssert(key && *key);

    if (cache->shared) {
        cache = cache->shared;
        mprAssert(cache == shared);
    }
    lock(cache);
    if (key) {
        if ((item = mprLookupKey(cache->store, key)) != 0) {
            cache->usedMem -= (slen(key) + slen(item->data));
            mprRemoveKey(cache->store, key);
            result = 1;
        } else {
            result = 0;
        }

    } else {
        /* Remove all keys */
        result = mprGetHashLength(cache->store) ? 1 : 0;
        cache->store = mprCreateHash(CACHE_HASH_SIZE, 0);
        cache->usedMem = 0;
    }
    unlock(cache);
    return result;
}


void mprSetCacheLimits(MprCache *cache, int64 keys, MprTime lifespan, int64 memory, int resolution)
{
    mprAssert(cache);

    if (cache->shared) {
        cache = cache->shared;
        mprAssert(cache == shared);
    }
    if (keys) {
        cache->maxKeys = (ssize) keys;
        if (cache->maxKeys <= 0) {
            cache->maxKeys = MAXSSIZE;
        }
    }
    if (lifespan) {
        cache->lifespan = lifespan;
    }
    if (memory) {
        cache->maxMem = (ssize) memory;
        if (cache->maxMem <= 0) {
            cache->maxMem = MAXSSIZE;
        }
    }
    if (resolution) {
        cache->resolution = resolution;
        if (cache->resolution <= 0) {
            cache->resolution = CACHE_TIMER_PERIOD;
        }
    }
}


ssize mprWriteCache(MprCache *cache, cchar *key, cchar *value, MprTime lifespan, int64 version, int options)
{
    CacheItem   *item;
    MprHash     *hp;
    ssize       len, oldLen;
    int         exists, add, set, prepend, append, throw;

    mprAssert(cache);
    mprAssert(key && *key);
    mprAssert(value && *value);

    if (cache->shared) {
        cache = cache->shared;
        mprAssert(cache == shared);
    }
    exists = add = prepend = append = throw = 0;
    add = options & MPR_CACHE_ADD;
    append = options & MPR_CACHE_APPEND;
    prepend = options & MPR_CACHE_PREPEND;
    set = options & MPR_CACHE_SET;
    if ((add + append + prepend) == 0) {
        set = 1;
    }
    lock(cache);
    if ((hp = mprLookupKeyEntry(cache->store, key)) != 0) {
        exists++;
        item = (CacheItem*) hp->data;
        if (version) {
            if (item->version != version) {
                unlock(cache);
                return MPR_ERR_BAD_STATE;
            }
        }
    } else {
        if ((item = mprAllocObj(CacheItem, manageCacheItem)) == 0) {
            unlock(cache);
            return 0;
        }
        mprAddKey(cache->store, key, item);
        item->key = sclone(key);
        set = 1;
    }
    oldLen = (item->data) ? (slen(item->key) + slen(item->data)) : 0;
    if (set) {
        item->data = sclone(value);
    } else if (add) {
        if (exists) {
            return 0;
        }
        item->data = sclone(value);
    } else if (append) {
        item->data = sjoin(item->data, value, 0);
    } else if (prepend) {
        item->data = sjoin(value, item->data, 0);
    }
    item->lifespan = lifespan;
    item->expires = mprGetTime() + lifespan;
    item->version++;
    len = slen(item->key) + slen(item->data);
    cache->usedMem += (len - oldLen);

    if (cache->timer == 0) {
        mprLog(5, "Start Cache pruner with resolution %d", cache->resolution);
        /* 
            Use the MPR dispatcher incase this VM is destroyed 
         */
        cache->timer = mprCreateTimerEvent(MPR->dispatcher, "localCacheTimer", cache->resolution, localPruner, cache, 
            MPR_EVENT_STATIC_DATA); 
    }
    unlock(cache);
    return len;
}


static void removeItem(MprCache *cache, CacheItem *item)
{
    mprAssert(cache);
    mprAssert(item);

    lock(cache);
    mprRemoveKey(cache->store, item->key);
    cache->usedMem -= (slen(item->key) + slen(item->data));
    unlock(cache);
}


/*
    Check for expired keys
 */
static void localPruner(MprCache *cache, MprEvent *event)
{
    MprTime         when, factor;
    MprHash         *hp;
    CacheItem       *item;
    ssize           excessKeys;

    mprAssert(cache);
    mprAssert(event);

    if (mprTryLock(cache->mutex)) {
        when = mprGetTime();
        for (hp = 0; (hp = mprGetNextKey(cache->store, hp)) != 0; ) {
            item = (CacheItem*) hp->data;
            mprLog(6, "Cache: \"%@\" lifespan %d, expires in %d secs", item->key, 
                    item->lifespan / 1000, (item->expires - when) / 1000);
            if (item->expires && item->expires <= when) {
                mprLog(5, "Cache prune expired key %s", hp->key);
                removeItem(cache, item);
            }
        }
        mprAssert(cache->usedMem >= 0);

        /*
            If too many keys or too much memory used, prune keys expiring first.
         */
        if (cache->maxKeys < MAXSSIZE || cache->maxMem < MAXSSIZE) {
            excessKeys = mprGetHashLength(cache->store) - cache->maxKeys;
            while (excessKeys > 0 || cache->usedMem > cache->maxMem) {
                for (factor = 3600; excessKeys > 0 && factor < (86400 * 1000); factor *= 4) {
                    for (hp = 0; (hp = mprGetNextKey(cache->store, hp)) != 0; ) {
                        item = (CacheItem*) hp->data;
                        if (item->expires && item->expires <= when) {
                            mprLog(5, "Cache too big execess keys %Ld, mem %Ld, prune key %s", 
                                    excessKeys, (cache->maxMem - cache->usedMem), hp->key);
                            removeItem(cache, item);
                        }
                    }
                    when += factor;
                }
            }
        }
        mprAssert(cache->usedMem >= 0);

        if (mprGetHashLength(cache->store) == 0) {
            mprRemoveEvent(event);
            cache->timer = 0;
        }
        unlock(cache);
    }
}


static void manageCache(MprCache *cache, int flags) 
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(cache->store);
        mprMark(cache->mutex);
        mprMark(cache->timer);
        mprMark(cache->shared);

    } else if (flags & MPR_MANAGE_FREE) {
        if (cache == shared) {
            shared = 0;
        }
    }
}


static void manageCacheItem(CacheItem *item, int flags) 
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(item->key);
        mprMark(item->data);
    }
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
