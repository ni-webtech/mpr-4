/*
    mprHash.c - Fast hashing table lookup module

    This hash table uses a fast key lookup mechanism. Keys may be C strings or unicode strings. The hash value entries 
    are arbitrary pointers. The keys are hashed into a series of buckets which then have a chain of hash entries.
    The chain in in collating sequence so search time through the chain is on average (N/hashSize)/2.

    This module is not thread-safe. It is the callers responsibility to perform all thread synchronization.
    There is locking solely for the purpose of synchronization with the GC marker()

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/**************************** Forward Declarations ****************************/

static void *dupKey(MprHashTable *table, MprHash *sp, cvoid *key);
static MprHash  *lookupHash(int *index, MprHash **prevSp, MprHashTable *table, cvoid *key);
static void manageHashTable(MprHashTable *table, int flags);

/*********************************** Code *************************************/
/*
    Create a new hash table of a given size. Caller should provide a size that is a prime number for the greatest efficiency.
 */
MprHashTable *mprCreateHash(int hashSize, int flags)
{
    MprHashTable    *table;

    if ((table = mprAllocObj(MprHashTable, manageHashTable)) == 0) {
        return 0;
    }
    if (hashSize < MPR_DEFAULT_HASH_SIZE) {
        hashSize = MPR_DEFAULT_HASH_SIZE;
    }
    if ((table->buckets = mprAllocZeroed(sizeof(MprHash*) * hashSize)) == 0) {
        return NULL;
    }
    table->hashSize = hashSize;
    table->flags = flags;
    table->length = 0;
    table->mutex = mprCreateLock();
#if BLD_CHAR_LEN > 1
    if (table->flags & MPR_HASH_UNICODE) {
        if (table->flags & MPR_HASH_CASELESS) {
            table->hash = (MprHashProc) whashlower;
        } else {
            table->hash = (MprHashProc) whash;
        }
    } else 
#endif
    {
        if (table->flags & MPR_HASH_CASELESS) {
            table->hash = (MprHashProc) shashlower;
        } else {
            table->hash = (MprHashProc) shash;
        }
    }
    return table;
}


static void manageHashTable(MprHashTable *table, int flags)
{
    MprHash     *sp;
    int         i;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(table->mutex);
        mprMark(table->buckets);
        lock(table);
        for (i = 0; i < table->hashSize; i++) {
            for (sp = (MprHash*) table->buckets[i]; sp; sp = sp->next) {
                mprAssert(mprIsValid(sp));
                mprMark(sp);
                if (!(table->flags & MPR_HASH_STATIC_VALUES)) {
                    mprAssert(sp->data == 0 || mprIsValid(sp->data));
                    mprMark(sp->data);
                }
                if (!(table->flags & MPR_HASH_STATIC_KEYS)) {
                    mprAssert(mprIsValid(sp->key));
                    mprMark(sp->key);
                }
            }
        }
        unlock(table);
    }
}


MprHashTable *mprCloneHash(MprHashTable *master)
{
    MprHash         *hp;
    MprHashTable    *table;

    table = mprCreateHash(master->hashSize, master->flags);
    if (table == 0) {
        return 0;
    }
    hp = mprGetFirstKey(master);
    while (hp) {
        mprAddKey(table, hp->key, hp->data);
        hp = mprGetNextKey(master, hp);
    }
    return table;
}


/*
    Insert an entry into the hash table. If the entry already exists, update its value. 
    Order of insertion is not preserved.
 */
MprHash *mprAddKey(MprHashTable *table, cvoid *key, cvoid *ptr)
{
    MprHash     *sp, *prevSp;
    int         index;

    lock(table);
    sp = lookupHash(&index, &prevSp, table, key);
    if (sp != 0) {
        /*
            Already exists. Just update the data.
         */
        sp->data = ptr;
        unlock(table);
        return sp;
    }
    /*
        Hash entries are managed by manageHashTable
     */
    if ((sp = mprAllocStruct(MprHash)) == 0) {
        unlock(table);
        return 0;
    }
    sp->data = ptr;
    if (!(table->flags & MPR_HASH_STATIC_KEYS)) {
        sp->key = dupKey(table, sp, key);
    } else {
        sp->key = (void*) key;
    }
    sp->bucket = index;
    sp->next = table->buckets[index];
    table->buckets[index] = sp;
    table->length++;
    unlock(table);
    return sp;
}


MprHash *mprAddKeyFmt(MprHashTable *table, cvoid *key, cchar *fmt, ...)
{
    va_list     ap;
    char        *value;

    va_start(ap, fmt);
    value = sfmtv(fmt, ap);
    va_end(ap);
    return mprAddKey(table, key, value);
}


/*
    Multiple insertion. Insert an entry into the hash table allowing for multiple entries with the same key.
    Order of insertion is not preserved. Lookup cannot be used to retrieve all duplicate keys, some will be shadowed. 
    Use enumeration to retrieve the keys.
 */
MprHash *mprAddDuplicateKey(MprHashTable *table, cvoid *key, cvoid *ptr)
{
    MprHash     *sp;
    int         index;

    if ((sp = mprAllocStruct(MprHash)) == 0) {
        return 0;
    }
    sp->data = ptr;
    if (!(table->flags & MPR_HASH_STATIC_KEYS)) {
        sp->key = dupKey(table, sp, key);
    } else {
        sp->key = (void*) key;
    }
    lock(table);
    index = table->hash(key, -1) % table->hashSize;
    sp->bucket = index;
    sp->next = table->buckets[index];
    table->buckets[index] = sp;
    table->length++;
    unlock(table);
    return sp;
}


int mprRemoveKey(MprHashTable *table, cvoid *key)
{
    MprHash     *sp, *prevSp;
    int         index;

    lock(table);
    if ((sp = lookupHash(&index, &prevSp, table, key)) == 0) {
        unlock(table);
        return MPR_ERR_CANT_FIND;
    }
    if (prevSp) {
        prevSp->next = sp->next;
    } else {
        table->buckets[index] = sp->next;
    }
    table->length--;
    unlock(table);
    return 0;
}


/*
    Lookup a key and return the hash entry
 */
MprHash *mprLookupKeyEntry(MprHashTable *table, cvoid *key)
{
    mprAssert(key);

    return lookupHash(0, 0, table, key);
}


/*
    Lookup a key and return the hash entry data
 */
void *mprLookupKey(MprHashTable *table, cvoid *key)
{
    MprHash     *sp;

    mprAssert(key);

    if ((sp = lookupHash(0, 0, table, key)) == 0) {
        return 0;
    }
    return (void*) sp->data;
}


/*
    Exponential primes
 */
static int hashSizes[] = {
     19, 29, 59, 79, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 0
};


static int getHashSize(int numKeys)
{
    int     i;

    for (i = 0; hashSizes[i]; i++) {
        if (numKeys < hashSizes[i]) {
            return hashSizes[i];
        }
    }
    return hashSizes[i - 1];
}


/*
    This is unlocked because it is read-only
 */
static MprHash *lookupHash(int *bucketIndex, MprHash **prevSp, MprHashTable *table, cvoid *key)
{
    MprHash     *sp, *prev, *next;
    MprHash     **buckets;
    int         hashSize, i, index, rc;

    mprAssert(key);

    if (key == 0 || table == 0) {
        return 0;
    }
    if (table->length > table->hashSize) {
        hashSize = getHashSize(table->length * 4 / 3);
        if (table->hashSize < hashSize) {
            if ((buckets = mprAllocZeroed(sizeof(MprHash*) * hashSize)) != 0) {
                table->length = 0;
                for (i = 0; i < table->hashSize; i++) {
                    for (sp = table->buckets[i]; sp; sp = next) {
                        next = sp->next;
                        mprAssert(next != sp);
                        index = table->hash(sp->key, slen(sp->key)) % hashSize;
                        if (buckets[index]) {
                            sp->next = buckets[index];
                        } else {
                            sp->next = 0;
                        }
                        buckets[index] = sp;
                        sp->bucket = index;
                        table->length++;
                    }
                }
                table->hashSize = hashSize;
                table->buckets = buckets;
            }
        }
    }
    index = table->hash(key, slen(key)) % table->hashSize;
    if (bucketIndex) {
        *bucketIndex = index;
    }
    sp = table->buckets[index];
    prev = 0;

    while (sp) {
#if BLD_CHAR_LEN > 1
        if (table->flags & MPR_HASH_UNICODE) {
            MprChar *u1, *u2;
            u1 = (MprChar*) sp->key;
            u2 = (MprChar*) key;
            rc = -1;
            if (table->flags & MPR_HASH_CASELESS) {
                rc = wcasecmp(u1, u2);
            } else {
                rc = wcmp(u1, u2);
            }
        } else 
#endif
        if (table->flags & MPR_HASH_CASELESS) {
            rc = scasecmp(sp->key, key);
        } else {
            rc = strcmp(sp->key, key);
        }
        if (rc == 0) {
            if (prevSp) {
                *prevSp = prev;
            }
            return sp;
        }
        prev = sp;
        mprAssert(sp != sp->next);
        sp = sp->next;
    }
    return 0;
}


int mprGetHashLength(MprHashTable *table)
{
    return table->length;
}


/*
    Return the first entry in the table.
 */
MprHash *mprGetFirstKey(MprHashTable *table)
{
    MprHash     *sp;
    int         i;

    mprAssert(table);

    for (i = 0; i < table->hashSize; i++) {
        if ((sp = (MprHash*) table->buckets[i]) != 0) {
            return sp;
        }
    }
    return 0;
}


/*
    Return the next entry in the table
 */
MprHash *mprGetNextKey(MprHashTable *table, MprHash *last)
{
    MprHash     *sp;
    int         i;

    mprAssert(table);

    if (last == 0) {
        return mprGetFirstKey(table);
    }
    if (last->next) {
        return last->next;
    }
    for (i = last->bucket + 1; i < table->hashSize; i++) {
        if ((sp = (MprHash*) table->buckets[i]) != 0) {
            return sp;
        }
    }
    return 0;
}


static void *dupKey(MprHashTable *table, MprHash *sp, cvoid *key)
{
#if BLD_CHAR_LEN > 1
    if (table->flags & MPR_HASH_UNICODE) {
        return wclone(sp, (MprChar*) key, -1);
    } else
#endif
        return sclone(key);
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
