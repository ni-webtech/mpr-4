/*
    mprHash.c - Fast hashing hash lookup module

    This hash hash uses a fast key lookup mechanism. Keys may be C strings or unicode strings. The hash value entries 
    are arbitrary pointers. The keys are hashed into a series of buckets which then have a chain of hash entries.
    The chain in in collating sequence so search time through the chain is on average (N/hashSize)/2.

    This module is not thread-safe. It is the callers responsibility to perform all thread synchronization.
    There is locking solely for the purpose of synchronization with the GC marker()

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/**************************** Forward Declarations ****************************/

static void *dupKey(MprHash *hash, MprKey *sp, cvoid *key);
static MprKey *lookupHash(int *index, MprKey **prevSp, MprHash *hash, cvoid *key);
static void manageHashTable(MprHash *hash, int flags);

/*********************************** Code *************************************/
/*
    Create a new hash hash of a given size. Caller should provide a size that is a prime number for the greatest efficiency.
 */
MprHash *mprCreateHash(int hashSize, int flags)
{
    MprHash     *hash;

    if ((hash = mprAllocObj(MprHash, manageHashTable)) == 0) {
        return 0;
    }
    if (hashSize < MPR_DEFAULT_HASH_SIZE) {
        hashSize = MPR_DEFAULT_HASH_SIZE;
    }
    if ((hash->buckets = mprAllocZeroed(sizeof(MprKey*) * hashSize)) == 0) {
        return NULL;
    }
    hash->size = hashSize;
    hash->flags = flags;
    hash->length = 0;
    hash->mutex = mprCreateLock();
#if BLD_CHAR_LEN > 1
    if (hash->flags & MPR_HASH_UNICODE) {
        if (hash->flags & MPR_HASH_CASELESS) {
            hash->fn = (MprHashProc) whashlower;
        } else {
            hash->fn = (MprHashProc) whash;
        }
    } else 
#endif
    {
        if (hash->flags & MPR_HASH_CASELESS) {
            hash->fn = (MprHashProc) shashlower;
        } else {
            hash->fn = (MprHashProc) shash;
        }
    }
    return hash;
}


static void manageHashTable(MprHash *hash, int flags)
{
    MprKey      *sp;
    int         i;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(hash->mutex);
        mprMark(hash->buckets);
        lock(hash);
        for (i = 0; i < hash->size; i++) {
            for (sp = (MprKey*) hash->buckets[i]; sp; sp = sp->next) {
                mprAssert(mprIsValid(sp));
                mprMark(sp);
                if (!(hash->flags & MPR_HASH_STATIC_VALUES)) {
                    mprAssert(sp->data == 0 || mprIsValid(sp->data));
                    mprMark(sp->data);
                }
                if (!(hash->flags & MPR_HASH_STATIC_KEYS)) {
                    mprAssert(mprIsValid(sp->key));
                    mprMark(sp->key);
                }
            }
        }
        unlock(hash);
    }
}


/*
    Insert an entry into the hash hash. If the entry already exists, update its value. 
    Order of insertion is not preserved.
 */
MprKey *mprAddKey(MprHash *hash, cvoid *key, cvoid *ptr)
{
    MprKey      *sp, *prevSp;
    int         index;

    if (hash == 0) {
        return 0;
    }
    lock(hash);
    sp = lookupHash(&index, &prevSp, hash, key);
    if (sp != 0) {
        /*
            Already exists. Just update the data.
         */
        sp->data = ptr;
        unlock(hash);
        return sp;
    }
    /*
        Hash entries are managed by manageHashTable
     */
    if ((sp = mprAllocStruct(MprKey)) == 0) {
        unlock(hash);
        return 0;
    }
    sp->data = ptr;
    if (!(hash->flags & MPR_HASH_STATIC_KEYS)) {
        sp->key = dupKey(hash, sp, key);
    } else {
        sp->key = (void*) key;
    }
    sp->bucket = index;
    sp->next = hash->buckets[index];
    hash->buckets[index] = sp;
    hash->length++;
    unlock(hash);
    return sp;
}


MprKey *mprAddKeyFmt(MprHash *hash, cvoid *key, cchar *fmt, ...)
{
    va_list     ap;
    char        *value;

    va_start(ap, fmt);
    value = sfmtv(fmt, ap);
    va_end(ap);
    return mprAddKey(hash, key, value);
}


/*
    Multiple insertion. Insert an entry into the hash hash allowing for multiple entries with the same key.
    Order of insertion is not preserved. Lookup cannot be used to retrieve all duplicate keys, some will be shadowed. 
    Use enumeration to retrieve the keys.
 */
MprKey *mprAddDuplicateKey(MprHash *hash, cvoid *key, cvoid *ptr)
{
    MprKey      *sp;
    int         index;

    if ((sp = mprAllocStruct(MprKey)) == 0) {
        return 0;
    }
    sp->data = ptr;
    if (!(hash->flags & MPR_HASH_STATIC_KEYS)) {
        sp->key = dupKey(hash, sp, key);
    } else {
        sp->key = (void*) key;
    }
    lock(hash);
    index = hash->fn(key, slen(key)) % hash->size;
    sp->bucket = index;
    sp->next = hash->buckets[index];
    hash->buckets[index] = sp;
    hash->length++;
    unlock(hash);
    return sp;
}


int mprRemoveKey(MprHash *hash, cvoid *key)
{
    MprKey      *sp, *prevSp;
    int         index;

    lock(hash);
    if ((sp = lookupHash(&index, &prevSp, hash, key)) == 0) {
        unlock(hash);
        return MPR_ERR_CANT_FIND;
    }
    if (prevSp) {
        prevSp->next = sp->next;
    } else {
        hash->buckets[index] = sp->next;
    }
    hash->length--;
    unlock(hash);
    return 0;
}


MprHash *mprBlendHash(MprHash *hash, MprHash *extra)
{
    MprKey      *kp;

    if (hash == 0 || extra == 0) {
        return hash;
    }
    for (ITERATE_KEYS(extra, kp)) {
        mprAddKey(hash, kp->key, kp->data);
    }
    return hash;
}


MprHash *mprCloneHash(MprHash *master)
{
    MprKey      *kp;
    MprHash     *hash;

    hash = mprCreateHash(master->size, master->flags);
    if (hash == 0) {
        return 0;
    }
    kp = mprGetFirstKey(master);
    while (kp) {
        mprAddKey(hash, kp->key, kp->data);
        kp = mprGetNextKey(master, kp);
    }
    return hash;
}


/*
    Lookup a key and return the hash entry
 */
MprKey *mprLookupKeyEntry(MprHash *hash, cvoid *key)
{
    mprAssert(key);

    return lookupHash(0, 0, hash, key);
}


/*
    Lookup a key and return the hash entry data
 */
void *mprLookupKey(MprHash *hash, cvoid *key)
{
    MprKey      *sp;

    mprAssert(key);

    if ((sp = lookupHash(0, 0, hash, key)) == 0) {
        return 0;
    }
    return (void*) sp->data;
}


#if UNUSED
void mprSetKeyBits(MprKey *kp, int bits)
{
    kp->bits = bits;
}


int mprGetKeyBits(MprKey *kp)
{
    return kp->bits;
}
#endif


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
static MprKey *lookupHash(int *bucketIndex, MprKey **prevSp, MprHash *hash, cvoid *key)
{
    MprKey      *sp, *prev, *next;
    MprKey      **buckets;
    int         hashSize, i, index, rc;

    mprAssert(key);

    if (key == 0 || hash == 0) {
        return 0;
    }
    if (hash->length > hash->size) {
        hashSize = getHashSize(hash->length * 4 / 3);
        if (hash->size < hashSize) {
            if ((buckets = mprAllocZeroed(sizeof(MprKey*) * hashSize)) != 0) {
                hash->length = 0;
                for (i = 0; i < hash->size; i++) {
                    for (sp = hash->buckets[i]; sp; sp = next) {
                        next = sp->next;
                        mprAssert(next != sp);
                        index = hash->fn(sp->key, slen(sp->key)) % hashSize;
                        if (buckets[index]) {
                            sp->next = buckets[index];
                        } else {
                            sp->next = 0;
                        }
                        buckets[index] = sp;
                        sp->bucket = index;
                        hash->length++;
                    }
                }
                hash->size = hashSize;
                hash->buckets = buckets;
            }
        }
    }
    index = hash->fn(key, slen(key)) % hash->size;
    if (bucketIndex) {
        *bucketIndex = index;
    }
    sp = hash->buckets[index];
    prev = 0;

    while (sp) {
#if BLD_CHAR_LEN > 1
        if (hash->flags & MPR_HASH_UNICODE) {
            MprChar *u1, *u2;
            u1 = (MprChar*) sp->key;
            u2 = (MprChar*) key;
            rc = -1;
            if (hash->flags & MPR_HASH_CASELESS) {
                rc = wcasecmp(u1, u2);
            } else {
                rc = wcmp(u1, u2);
            }
        } else 
#endif
        if (hash->flags & MPR_HASH_CASELESS) {
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


int mprGetHashLength(MprHash *hash)
{
    return hash->length;
}


/*
    Return the first entry in the hash.
 */
MprKey *mprGetFirstKey(MprHash *hash)
{
    MprKey      *sp;
    int         i;

    mprAssert(hash);

    for (i = 0; i < hash->size; i++) {
        if ((sp = (MprKey*) hash->buckets[i]) != 0) {
            return sp;
        }
    }
    return 0;
}


/*
    Return the next entry in the hash
 */
MprKey *mprGetNextKey(MprHash *hash, MprKey *last)
{
    MprKey      *sp;
    int         i;

    if (hash == 0) {
        return 0;
    }
    if (last == 0) {
        return mprGetFirstKey(hash);
    }
    if (last->next) {
        return last->next;
    }
    for (i = last->bucket + 1; i < hash->size; i++) {
        if ((sp = (MprKey*) hash->buckets[i]) != 0) {
            return sp;
        }
    }
    return 0;
}


static void *dupKey(MprHash *hash, MprKey *sp, cvoid *key)
{
#if BLD_CHAR_LEN > 1
    if (hash->flags & MPR_HASH_UNICODE) {
        return wclone(sp, (MprChar*) key, -1);
    } else
#endif
        return sclone(key);
}


/*
    The serial format is a subset of JSON without array support.
    This is designed to be as fast as possible for encoding one level of properties.
 */
static MprHash *parseHash(MprHash *hash, cchar **token)
{
    cchar   *cp, *ep;
    char    key[MPR_MAX_STRING];
    int     quote;

    for (cp = *token; *cp; cp++) {
        while (isspace((int) *cp)) cp++;
        if (*cp == '{') {
            ++cp;
            hash = parseHash(mprCreateHash(0, 0), &cp);

        } else if ((ep = strchr(cp, ':')) != 0 && (ep == *token || ep[-1] != '\\')) {
            if (*cp == '}') {
                /* By continuing, we permit:  {options}{more options} */
                continue;
            } else if (*cp == ',') {
                continue;
            }
            if (hash == 0) {
                /* Missing opening "{" */
                break;
            }
            if (*cp == '\'') {
                sncopy(key, sizeof(key), &cp[1], ep - cp - 2);
            } else {
                sncopy(key, sizeof(key), cp, ep - cp);
            }
            for (cp = ep + 1; isspace((int) *cp); cp++) ;
            if (*cp == '{') {
                ++cp;
                mprAddKey(hash, key, parseHash(mprCreateHash(0, 0), &cp));

            } else if (*cp == '"' || *cp == '\'') {
                quote = *cp;
                if ((ep = strchr(++cp, quote)) != 0 && ep[-1] != '\\') {
                    mprAddKey(hash, key, snclone(cp, ep - cp));
                    cp = ep;
                } else {
                    /* missing closing quote */
                    break;
                }

            } else if ((ep = strchr(cp, ',')) != 0 && ep[-1] != '\\') {
                mprAddKey(hash, key, snclone(cp, ep - cp));
                cp = ep - 1;

            } else if ((ep = strchr(cp, '}')) != 0 && ep[-1] != '\\') {
                /* Close of object "}" */
                break;

            } else if (ep == 0) {
                mprAddKey(hash, key, sclone(cp));
                break;
            }
        }
    }
    *token = cp;
    return hash;
}


MprHash *mprParseHash(cchar *str)
{
    if (str == 0 || *str == '\0') {
        return mprCreateHash(-1, 0);
    }
    return parseHash(NULL, &str);
}


/*
    Serialize into JSON format. Assumes that all key data is simple strings.
 */
cchar *mprHashToString(MprHash *hash, int flags)
{
    MprKey  *kp;
    MprBuf  *buf;
    int     pretty;

    pretty = (flags & MPR_HASH_PRETTY);
    if ((buf = mprCreateBuf(0, 0)) == 0) {
        return 0;
    }
    mprPutCharToBuf(buf, '{');
    if (pretty) mprPutCharToBuf(buf, '\n');
    for (ITERATE_KEYS(hash, kp)) {
        if (pretty) mprPutStringToBuf(buf, "    ");
        //  MOB - printable?
        mprPutFmtToBuf(buf, "'%s': '%s',", kp->key, kp->data);
        if (pretty) mprPutCharToBuf(buf, '\n');
    }
    mprPutCharToBuf(buf, '}');
    if (pretty) mprPutCharToBuf(buf, '\n');
    return sclone(mprGetBufStart(buf));
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
