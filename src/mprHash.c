/*
    mprHash.c - Fast hashing table lookup module

    This hash table uses a fast key lookup mechanism. Keys may be C strings or unicode strings. The hash value entries 
    are arbitrary pointers. The keys are hashed into a series of buckets which then have a chain of hash entries.
    The chain in in collating sequence so search time through the chain is on average (N/hashSize)/2.

    This module is not thread-safe. It is the callers responsibility to perform all thread synchronization.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/**************************** Forward Declarations ****************************/

static inline void *dupKey(MprHashTable *table, MprHash *sp, cvoid *key);
static int hashIndex(MprHashTable *table, cvoid *key, int size);
static MprHash  *lookupHash(int *bucketIndex, MprHash **prevSp, MprHashTable *table, cvoid *key);

/*********************************** Code *************************************/
/*
    Create a new hash table of a given size. Caller should provide a size that is a prime number for the greatest
    efficiency. Caller should use mprFree to free the hash table.
 */

MprHashTable *mprCreateHash(MprCtx ctx, int hashSize, int flags)
{
    MprHashTable    *table;

    if ((table = mprAllocObj(ctx, MprHashTable, NULL)) == 0) {
        return 0;
    }
    /*  TODO -- should support rehashing */
    if (hashSize < MPR_DEFAULT_HASH_SIZE) {
        hashSize = MPR_DEFAULT_HASH_SIZE;
    }
    table->hashSize = hashSize;
    table->flags = flags;
    table->count = 0;
    table->hashSize = hashSize;
    table->buckets = (MprHash**) mprAllocZeroed(table, sizeof(MprHash*) * hashSize);
    if (table->buckets == 0) {
        mprFree(table);
        return 0;
    }
    return table;
}


MprHashTable *mprCopyHash(MprCtx ctx, MprHashTable *master)
{
    MprHash         *hp;
    MprHashTable    *table;

    table = mprCreateHash(ctx, master->hashSize, master->flags);
    if (table == 0) {
        return 0;
    }
    hp = mprGetFirstHash(master);
    while (hp) {
        mprAddHash(table, hp->key, hp->data);
        hp = mprGetNextHash(master, hp);
    }
    return table;
}


/*
    Insert an entry into the hash table. If the entry already exists, update its value. 
    Order of insertion is not preserved.
 */
MprHash *mprAddHash(MprHashTable *table, cvoid *key, cvoid *ptr)
{
    MprHash     *sp, *prevSp;
    int         index;

    sp = lookupHash(&index, &prevSp, table, key);
    if (sp != 0) {
        /*
            Already exists. Just update the data.
         */
        sp->data = ptr;
        return sp;
    }
    /*
        New entry
     */
    sp = mprAllocObj(table, MprHash, NULL);
    if (sp == 0) {
        return 0;
    }
    sp->data = ptr;
    if (!(table->flags & MPR_HASH_PERM_KEYS)) {
        sp->key = dupKey(table, sp, key);
    } else {
        sp->key = (void*) key;
    }
    sp->bucket = index;
    sp->next = table->buckets[index];
    table->buckets[index] = sp;
    table->count++;
    return sp;
}


/*
    Multiple insertion. Insert an entry into the hash table allowing for multiple entries with the same key.
    Order of insertion is not preserved. Lookup cannot be used to retrieve all duplicate keys, some will be shadowed. 
    Use enumeration to retrieve the keys.
 */
MprHash *mprAddDuplicateHash(MprHashTable *table, cvoid *key, cvoid *ptr)
{
    MprHash     *sp;
    int         index;

    sp = mprAllocObj(table, MprHash, NULL);
    if (sp == 0) {
        return 0;
    }
    index = hashIndex(table, key, table->hashSize);

    sp->data = ptr;
    if (!(table->flags & MPR_HASH_PERM_KEYS)) {
        sp->key = dupKey(table, sp, key);
    } else {
        sp->key = (void*) key;
    }
    sp->bucket = index;
    sp->next = table->buckets[index];
    table->buckets[index] = sp;
    table->count++;
    return sp;
}


/*
    Remove an entry from the table
 */
int mprRemoveHash(MprHashTable *table, cvoid *key)
{
    MprHash     *sp, *prevSp;
    int         index;

    if ((sp = lookupHash(&index, &prevSp, table, key)) == 0) {
        return MPR_ERR_NOT_FOUND;
    }
    if (prevSp) {
        prevSp->next = sp->next;
    } else {
        table->buckets[index] = sp->next;
    }
    table->count--;
    mprFree(sp);
    return 0;
}


/*
    Lookup a key and return the hash entry
 */
MprHash *mprLookupHashEntry(MprHashTable *table, cvoid *key)
{
    mprAssert(key);

    return lookupHash(0, 0, table, key);
}


/*
    Lookup a key and return the hash entry data
 */
cvoid *mprLookupHash(MprHashTable *table, cvoid *key)
{
    MprHash     *sp;

    mprAssert(key);

    sp = lookupHash(0, 0, table, key);
    if (sp == 0) {
        return 0;
    }
    return sp->data;
}


static MprHash *lookupHash(int *bucketIndex, MprHash **prevSp, MprHashTable *table, cvoid *key)
{
    MprHash     *sp, *prev;
    MprUni      *u1, *u2;
    int         index, rc, i;

    mprAssert(key);

    index = hashIndex(table, key, table->hashSize);
    if (bucketIndex) {
        *bucketIndex = index;
    }
    sp = table->buckets[index];
    prev = 0;

    while (sp) {
        if (table->flags & MPR_HASH_UNICODE) {
            u1 = (MprUni*) sp->key;
            u2 = (MprUni*) key;
            rc = -1;
            if (u1->length == u2->length) {
                for (i = 0; i < u1->length; i++) {
                    rc = u1->value[i] == u2->value[i];
                }
            }
        } else if (table->flags & MPR_HASH_CASELESS) {
            rc = mprStrcmpAnyCase(sp->key, key);
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


int mprGetHashCount(MprHashTable *table)
{
    return table->count;
}


/*
    Return the first entry in the table.
 */
MprHash *mprGetFirstHash(MprHashTable *table)
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
MprHash *mprGetNextHash(MprHashTable *table, MprHash *last)
{
    MprHash     *sp;
    int         i;

    mprAssert(table);

    if (last == 0) {
        return mprGetFirstHash(table);
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


/*
    Hash the key to produce a hash index.
 */
static int hashIndex(MprHashTable *table, cvoid *vkey, int size)
{
    MprUni  *ukey;
    cchar   *key;
    uint    sum;
    int     c, i;

    //  MOB TODO OPT - Get a better hash. See Ejscript

    if (table->flags & MPR_HASH_UNICODE) {
        ukey = (MprUni*) vkey;
        sum = 0;
        if (table->flags & MPR_HASH_CASELESS) {
            for (i = 0; i < ukey->length; i++) {
                c = ukey->value[i];
                sum += (sum * 33) + tolower(c);
            }
        } else {
            for (i = 0; i < ukey->length; i++) {
                sum += (sum * 33) + ukey->value[i];
            }
        }
    } else {
        if (table->flags & MPR_HASH_CASELESS) {
            key = (char*) vkey;
            sum = 0;
            while (*key) {
                c = *key++;
                sum += (sum * 33) + tolower(c);
            }
        } else {
            key = (char*) vkey;
            sum = 0;
            while (*key) {
                sum += (sum * 33) + *key++;
            }
        }
    }
    return sum % size;
}


static inline void *dupKey(MprHashTable *table, MprHash *sp, cvoid *key)
{
    char    *ptr;
    int     len;

    if (table->flags & MPR_HASH_UNICODE) {
        MprUni  *ukey;
        len = ((ukey->length + 1) * sizeof(MprChar));
        ptr = mprAlloc(sp, len);
        memcpy(ptr, ukey->value, len);
        return ptr;
    } else {
        return mprStrdup(sp, key);
    }
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
