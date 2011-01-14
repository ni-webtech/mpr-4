/**
    mprList.c - Simple list type.

    The list supports two modes of operation. Compact mode where the list is compacted after removing list items, 
    and no-compact mode where removed items are zeroed. No-compact mode implies that all valid list entries must 
    be non-zero.

    This module is not thread-safe. It is the callers responsibility to perform all thread synchronization.

    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

/********************************** Includes **********************************/

#include    "mpr.h"

/****************************** Forward Declarations **************************/

static int growList(MprList *lp, int incr);
static void manageList(MprList *lp, int flags);

/************************************ Code ************************************/
/*
    Create a general growable list structure
 */
MprList *mprCreateList(int size, int flags)
{
    MprList     *lp;

    if ((lp = mprAllocObj(MprList, manageList)) == 0) {
        return 0;
    }
    lp->maxSize = MAXINT;
    lp->flags = flags;
    lp->mutex = mprCreateLock();
    if (size != 0) {
        mprSetListLimits(lp, size, -1);
    }
    return lp;
}


static void manageList(MprList *lp, int flags)
{
    int     i;

    if (flags & MPR_MANAGE_MARK) {
        mprMark(lp->mutex);
        lock(lp);
        mprMark(lp->items);
        if (!(lp->flags & MPR_LIST_STATIC_VALUES)) {
            for (i = 0; i < lp->length; i++) {
                mprAssert(lp->items[i] == 0 || mprIsValid(lp->items[i]));
                mprMark(lp->items[i]);
            }
        }
        unlock(lp);
    }
}

/*
    Initialize a list which may not be a memory context.
 */
void mprInitList(MprList *lp)
{
    lp->capacity = 0;
    lp->length = 0;
    lp->maxSize = MAXINT;
    lp->items = 0;
}


/*
    Define the list maximum size. If the list has not yet been written to, the initialSize will be observed.
 */
int mprSetListLimits(MprList *lp, int initialSize, int maxSize)
{
    int       size;

    if (initialSize <= 0) {
        initialSize = MPR_LIST_INCR;
    }
    if (maxSize <= 0) {
        maxSize = MAXINT;
    }
    size = initialSize * sizeof(void*);

    if (lp->items == 0) {
        lp->items = mprAlloc(size);
        if (lp->items == 0) {
            return MPR_ERR_MEMORY;
        }
        memset(lp->items, 0, size);
        lp->capacity = initialSize;
    }
    lp->maxSize = maxSize;
    return 0;
}


int mprCopyList(MprList *dest, MprList *src)
{
    void        *item;
    int         next;

    mprClearList(dest);

    if (mprSetListLimits(dest, src->capacity, src->maxSize) < 0) {
        return MPR_ERR_MEMORY;
    }
    for (next = 0; (item = mprGetNextItem(src, &next)) != 0; ) {
        if (mprAddItem(dest, item) < 0) {
            return MPR_ERR_MEMORY;
        }
    }
    return 0;
}


MprList *mprCloneList(MprList *src)
{
    MprList     *lp;

    lp = mprCreateList(src->capacity, src->flags);
    if (lp == 0) {
        return 0;
    }
    if (mprCopyList(lp, src) < 0) {
        return 0;
    }
    return lp;
}


MprList *mprAppendList(MprList *lp, MprList *add)
{
    void        *item;
    int         next;

    mprAssert(lp);

    for (next = 0; ((item = mprGetNextItem(add, &next)) != 0); ) {
        if (mprAddItem(lp, item) < 0) {
            return 0;
        }
    }
    return lp;
}


/*
    Change the item in the list at index. Return the old item.
 */
void *mprSetItem(MprList *lp, int index, cvoid *item)
{
    void    *old;
    int     length;

    mprAssert(lp);
    mprAssert(lp->capacity >= 0);
    mprAssert(lp->length >= 0);
    mprAssert(index >= 0);

    length = lp->length;

    if (index >= length) {
        length = index + 1;
    }
    lock(lp);
    if (length > lp->capacity) {
        if (growList(lp, length - lp->capacity) < 0) {
            unlock(lp);
            return 0;
        }
    }
    old = lp->items[index];
    lp->items[index] = (void*) item;
    lp->length = length;
    unlock(lp);
    return old;
}



/*
    Add an item to the list and return the item index.
 */
int mprAddItem(MprList *lp, cvoid *item)
{
    int   index;

    mprAssert(lp);
    mprAssert(lp->capacity >= 0);
    mprAssert(lp->length >= 0);

    lock(lp);
    if (lp->length >= lp->capacity) {
        if (growList(lp, 1) < 0) {
            unlock(lp);
            return MPR_ERR_TOO_MANY;
        }
    }
    index = (int) lp->length++;
    lp->items[index] = (void*) item;
    unlock(lp);
    return index;
}


/*
    Insert an item to the list at a specified position. We insert before the item at "index".
    ie. The inserted item will go into the "index" location and the other elements will be moved up.
 */
int mprInsertItemAtPos(MprList *lp, int index, cvoid *item)
{
    void    **items;
    int     i;

    mprAssert(lp);
    mprAssert(lp->capacity >= 0);
    mprAssert(lp->length >= 0);
    mprAssert(index >= 0);

    if (index < 0) {
        index = 0;
    }
    lock(lp);
    if (index >= lp->capacity) {
        if (growList(lp, index - lp->capacity + 1) < 0) {
            unlock(lp);
            return MPR_ERR_TOO_MANY;
        }

    } else if (lp->length >= lp->capacity) {
        if (growList(lp, 1) < 0) {
            unlock(lp);
            return MPR_ERR_TOO_MANY;
        }
    }
    if (index >= lp->length) {
        lp->length = index + 1;
    } else {
        /*
            Copy up items to make room to insert
         */
        items = lp->items;
        for (i = lp->length; i > index; i--) {
            items[i] = items[i - 1];
        }
        lp->length++;
    }
    lp->items[index] = (void*) item;
    unlock(lp);
    return index;
}


/*
    Remove an item from the list. Return the index where the item resided.
 */
int mprRemoveItem(MprList *lp, void *item)
{
    int     index;

    mprAssert(lp);

    lock(lp);
    index = mprLookupItem(lp, item);
    if (index < 0) {
        unlock(lp);
        return index;
    }
    index = mprRemoveItemAtPos(lp, index);
    mprAssert(index >= 0);
    unlock(lp);
    return index;
}


int mprRemoveLastItem(MprList *lp)
{
    mprAssert(lp);
    mprAssert(lp->capacity > 0);
    mprAssert(lp->length > 0);

    if (lp->length <= 0) {
        return MPR_ERR_CANT_FIND;
    }
    return mprRemoveItemAtPos(lp, lp->length - 1);
}


/*
    Remove an index from the list. Return the index where the item resided.
    The list is compacted.
 */
int mprRemoveItemAtPos(MprList *lp, int index)
{
    void    **items;
    int     i;

    mprAssert(lp);
    mprAssert(lp->capacity > 0);
    mprAssert(index >= 0 && index < lp->capacity);
    mprAssert(lp->length > 0);

    if (index < 0 || index >= lp->length) {
        return MPR_ERR_CANT_FIND;
    }
    lock(lp);
    items = lp->items;
#if FUTURE
    void    **ip;
    if (index == (lp->length - 1)) {
        /* Scan backwards to find last non-null item */
        for (ip = &items[index - 1]; ip >= items && *ip == 0; ip--) ;
        lp->length = ++ip - items;
        mprAssert(lp->length >= 0);
    } else {
        /* Copy down following items */
        for (ip = &items[index]; ip < &items[lp->length]; ip++) {
            *ip = ip[1];
        }
        lp->length--;
    }
#else
    for (i = index; i < (lp->length - 1); i++) {
        items[i] = items[i + 1];
    }
    lp->length--;
#endif
    lp->items[lp->length] = 0;
    mprAssert(lp->length >= 0);
    unlock(lp);
    return index;
}


/*
    Remove a set of items. Return 0 if successful.
 */
int mprRemoveRangeOfItems(MprList *lp, int start, int end)
{
    void    **items;
    int     i, count;

    mprAssert(lp);
    mprAssert(lp->capacity > 0);
    mprAssert(lp->length > 0);
    mprAssert(start > end);

    if (start < 0 || start >= lp->length) {
        return MPR_ERR_CANT_FIND;
    }
    if (end < 0 || end >= lp->length) {
        return MPR_ERR_CANT_FIND;
    }
    if (start > end) {
        return MPR_ERR_BAD_ARGS;
    }

    /*
        Copy down to compress
     */
    items = lp->items;
    count = end - start;
    lock(lp);
    for (i = start; i < (lp->length - count); i++) {
        items[i] = items[i + count];
    }
    lp->length -= count;
    for (i = lp->length; i < lp->capacity; i++) {
        items[i] = 0;
    }
    unlock(lp);
    return 0;
}


void *mprGetItem(MprList *lp, int index)
{
    mprAssert(lp);

    if (index < 0 || index >= lp->length) {
        return 0;
    }
    return lp->items[index];
}


void *mprGetFirstItem(MprList *lp)
{
    mprAssert(lp);

    if (lp == 0) {
        return 0;
    }
    if (lp->length == 0) {
        return 0;
    }
    return lp->items[0];
}


void *mprGetLastItem(MprList *lp)
{
    mprAssert(lp);

    if (lp == 0) {
        return 0;
    }
    if (lp->length == 0) {
        return 0;
    }
    return lp->items[lp->length - 1];
}


void *mprGetNextItem(MprList *lp, int *next)
{
    void    *item;
    int     index;

    mprAssert(next);
    mprAssert(*next >= 0);

    if (lp == 0) {
        return 0;
    }
    index = *next;

    if (index < lp->length) {
        item = lp->items[index];
        *next = ++index;
        return item;
    }
    return 0;
}


void *mprGetPrevItem(MprList *lp, int *next)
{
    int     index;

    mprAssert(next);

    if (lp == 0) {
        return 0;
    }
    if (*next < 0) {
        *next = lp->length;
    }
    index = *next;

    if (--index < lp->length && index >= 0) {
        *next = index;
        return lp->items[index];
    }
    return 0;
}


int mprPushItem(MprList *lp, cvoid *item)
{
    return mprAddItem(lp, item);
}


cvoid *mprPopItem(MprList *lp)
{
    cvoid   *item;
    int     index;

    item = NULL;
    if (lp->length > 0) {
        index = lp->length - 1;
        item = mprGetItem(lp, index);
        mprRemoveItemAtPos(lp, index);
    }
    return item;
}


int mprGetListLength(MprList *lp)
{
    if (lp == 0) {
        return 0;
    }
    return lp->length;
}


int mprGetListCapacity(MprList *lp)
{
    mprAssert(lp);

    if (lp == 0) {
        return 0;
    }
    return lp->capacity;
}


void mprClearList(MprList *lp)
{
    int     i;

    mprAssert(lp);

    for (i = 0; i < lp->length; i++) {
        lp->items[i] = 0;
    }
    lp->length = 0;
}


int mprLookupItem(MprList *lp, cvoid *item)
{
    int     i;

    mprAssert(lp);
    
    for (i = 0; i < lp->length; i++) {
        if (lp->items[i] == item) {
            return i;
        }
    }
    return MPR_ERR_CANT_FIND;
}


/*
    Grow the list by the requried increment
 */
static int growList(MprList *lp, int incr)
{
    int     len, memsize;

    if (lp->maxSize <= 0) {
        lp->maxSize = MAXINT;
    }

    /*
        Need to grow the list
     */
    if (lp->capacity >= lp->maxSize) {
        mprAssert(lp->capacity < lp->maxSize);
        return MPR_ERR_TOO_MANY;
    }

    /*
        If growing by 1, then use the default increment which exponentially grows. Otherwise, assume the caller knows exactly
        how much the list needs to grow.
     */
    if (incr <= 1) {
        len = MPR_LIST_INCR + (lp->capacity * 2);
    } else {
        len = lp->capacity + incr;
    }
    memsize = len * sizeof(void*);

    /*
        Lock free realloc. Old list will be intact via lp->items until mprRealloc returns.
     */
    if ((lp->items = mprRealloc(lp->items, memsize)) == NULL) {
        return MPR_ERR_MEMORY;
    }
    lp->capacity = len;
    return 0;
}


void mprSortList(MprList *lp, MprListCompareProc compare)
{
    qsort(lp->items, lp->length, sizeof(void*), compare);
}


static void manageKeyValue(MprKeyValue *pair, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(pair->key);
        mprMark(pair->value);
    } else if (flags & MPR_MANAGE_FREE) {
    }
}


MprKeyValue *mprCreateKeyPair(cchar *key, cchar *value)
{
    MprKeyValue     *pair;
    
    pair = mprAllocObj(MprKeyValue, manageKeyValue);
    if (pair == 0) {
        return 0;
    }
    pair->key = sclone(key);
    pair->value = sclone(value);
    return pair;
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
