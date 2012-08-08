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
    lp->flags = flags | MPR_OBJ_LIST;
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
    ssize   size;

    if (initialSize <= 0) {
        initialSize = MPR_LIST_INCR;
    }
    if (maxSize <= 0) {
        maxSize = MAXINT;
    }
    size = initialSize * sizeof(void*);

    lock(lp);
    if (lp->items == 0) {
        if ((lp->items = mprAlloc(size)) == 0) {
            mprAssert(!MPR_ERR_MEMORY);
            unlock(lp);
            return MPR_ERR_MEMORY;
        }
        memset(lp->items, 0, size);
        lp->capacity = initialSize;
    }
    lp->maxSize = maxSize;
    unlock(lp);
    return 0;
}


int mprCopyListContents(MprList *dest, MprList *src)
{
    void        *item;
    int         next;

    mprClearList(dest);

    lock(src);
    if (mprSetListLimits(dest, src->capacity, src->maxSize) < 0) {
        mprAssert(!MPR_ERR_MEMORY);
        unlock(src);
        return MPR_ERR_MEMORY;
    }
    for (next = 0; (item = mprGetNextItem(src, &next)) != 0; ) {
        if (mprAddItem(dest, item) < 0) {
            mprAssert(!MPR_ERR_MEMORY);
            unlock(src);
            return MPR_ERR_MEMORY;
        }
    }
    unlock(src);
    return 0;
}


MprList *mprCloneList(MprList *src)
{
    MprList     *lp;

    if ((lp = mprCreateList(src->capacity, src->flags)) == 0) {
        return 0;
    }
    if (mprCopyListContents(lp, src) < 0) {
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
    int     index;

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
    index = lp->length++;
    lp->items[index] = (void*) item;
    unlock(lp);
    return index;
}


int mprAddNullItem(MprList *lp)
{
    int     index;

    mprAssert(lp);
    mprAssert(lp->capacity >= 0);
    mprAssert(lp->length >= 0);

    lock(lp);
    if (lp->length != 0 && lp->items[lp->length - 1] == 0) {
        index = lp->length - 1;
    } else {
        if (lp->length >= lp->capacity) {
            if (growList(lp, 1) < 0) {
                unlock(lp);
                return MPR_ERR_TOO_MANY;
            }
        }
        index = lp->length;
        lp->items[index] = 0;
    }
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
int mprRemoveItem(MprList *lp, cvoid *item)
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
    memmove(&items[index], &items[index + 1], (lp->length - index - 1) * sizeof(void*));
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


/*
    Remove a string item from the list. Return the index where the item resided.
 */
int mprRemoveStringItem(MprList *lp, cchar *str)
{
    int     index;

    mprAssert(lp);

    lock(lp);
    index = mprLookupStringItem(lp, str);
    if (index < 0) {
        unlock(lp);
        return index;
    }
    index = mprRemoveItemAtPos(lp, index);
    mprAssert(index >= 0);
    unlock(lp);
    return index;
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
    lock(lp);
    index = *next;
    if (index < lp->length) {
        item = lp->items[index];
        *next = ++index;
        unlock(lp);
        return item;
    }
    unlock(lp);
    return 0;
}


void *mprGetPrevItem(MprList *lp, int *next)
{
    void    *item;
    int     index;

    mprAssert(next);

    if (lp == 0) {
        return 0;
    }
    lock(lp);
    if (*next < 0) {
        *next = lp->length;
    }
    index = *next;
    if (--index < lp->length && index >= 0) {
        *next = index;
        item = lp->items[index];
        unlock(lp);
        return item;
    }
    unlock(lp);
    return 0;
}


int mprPushItem(MprList *lp, cvoid *item)
{
    return mprAddItem(lp, item);
}


void *mprPopItem(MprList *lp)
{
    void    *item;
    int     index;

    item = NULL;
    if (lp->length > 0) {
        lock(lp);
        index = lp->length - 1;
        item = mprGetItem(lp, index);
        mprRemoveItemAtPos(lp, index);
        unlock(lp);
    }
    return item;
}


#ifndef mprGetListLength
int mprGetListLength(MprList *lp)
{
    if (lp == 0) {
        return 0;
    }
    return lp->length;
}
#endif


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

    lock(lp);
    for (i = 0; i < lp->length; i++) {
        lp->items[i] = 0;
    }
    lp->length = 0;
    unlock(lp);
}


int mprLookupItem(MprList *lp, cvoid *item)
{
    int     i;

    mprAssert(lp);
    
    lock(lp);
    for (i = 0; i < lp->length; i++) {
        if (lp->items[i] == item) {
            unlock(lp);
            return i;
        }
    }
    unlock(lp);
    return MPR_ERR_CANT_FIND;
}


int mprLookupStringItem(MprList *lp, cchar *str)
{
    int     i;

    mprAssert(lp);
    
    lock(lp);
    for (i = 0; i < lp->length; i++) {
        if (smatch(lp->items[i], str)) {
            unlock(lp);
            return i;
        }
    }
    unlock(lp);
    return MPR_ERR_CANT_FIND;
}


/*
    Grow the list by the requried increment
 */
static int growList(MprList *lp, int incr)
{
    ssize       memsize;
    int         len;

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
        mprAssert(!MPR_ERR_MEMORY);
        return MPR_ERR_MEMORY;
    }
    lp->capacity = len;
    return 0;
}


static int defaultSort(char **q1, char **q2, void *ctx)
{
    return scmp(*q1, *q2);
}


void mprSortList(MprList *lp, MprSortProc compare, void *ctx)
{
    lock(lp);
    if (!compare) {
        compare = (MprSortProc) defaultSort;
    }
    mprSort(lp->items, lp->length, sizeof(void*), compare, ctx);
    unlock(lp);
}


static void manageKeyValue(MprKeyValue *pair, int flags)
{
    if (flags & MPR_MANAGE_MARK) {
        mprMark(pair->key);
        mprMark(pair->value);
    }
}


MprKeyValue *mprCreateKeyPair(cchar *key, cchar *value)
{
    MprKeyValue     *pair;
    
    if ((pair = mprAllocObj(MprKeyValue, manageKeyValue)) == 0) {
        return 0;
    }
    pair->key = sclone(key);
    pair->value = sclone(value);
    return pair;
}


static void swapElt(char *a, char *b, ssize width)
{
    char tmp;

    if (a != b) {
        while (width--) {
            tmp = *a;
            *a++ = *b;
            *b++ = tmp;
        }
    }
}


static void shortsort(char *lo, char *hi, ssize width, MprSortProc comp, void *ctx)
{
    char    *p, *max;

    while (hi > lo) {
        max = lo;
        for (p = lo + width; p <= hi; p += width) {
            if (comp(p, max, ctx) > 0) {
                max = p;
            }
        }
        swapElt(max, hi, width);
        hi -= width;
    }
}

void mprSort(void *base, ssize num, ssize width, MprSortProc comp, void *ctx) 
{
    char    *lo, *hi, *mid, *l, *h, *lostk[30], *histk[30];
    ssize   size;
    int     stkptr;

    if (num < 2 || width == 0) {
        return;
    }
    stkptr = 0;
    lo = base;
    hi = (char *) base + width * (num - 1);

recurse:
    size = (int) (hi - lo) / width + 1;
    if (size <= 8) {
        shortsort(lo, hi, width, comp, ctx);
    } else {
        mid = lo + (size / 2) * width;
        swapElt(mid, lo, width);
        l = lo;
        h = hi + width;

        for (;;) {
            do { l += width; } while (l <= hi && comp(l, lo, ctx) <= 0);
            do { h -= width; } while (h > lo && comp(h, lo, ctx) >= 0);
            if (h < l) break;
            swapElt(l, h, width);
        }
        swapElt(lo, h, width);

        if (h - 1 - lo >= hi - l) {
            if (lo + width < h) {
                lostk[stkptr] = lo;
                histk[stkptr] = h - width;
                ++stkptr;
            }
            if (l < hi) {
                lo = l;
                goto recurse;
            }
        } else {
            if (l < hi) {
                lostk[stkptr] = l;
                histk[stkptr] = hi;
                ++stkptr;
            }
            if (lo + width < h) {
                hi = h - width;
                goto recurse;
            }
        }
    }
    --stkptr;
    if (stkptr >= 0) {
        lo = lostk[stkptr];
        hi = histk[stkptr];
        goto recurse;
    } else {
        return;
    }
}


/*
    @copy   default
    
    Copyright (c) Embedthis Software LLC, 2003-2012. All Rights Reserved.
    Copyright (c) Michael O'Brien, 1993-2012. All Rights Reserved.
    
    This software is distributed under commercial and open source licenses.
    You may use the GPL open source license described below or you may acquire 
    a commercial license from Embedthis Software. You agree to be fully bound 
    by the terms of either license. Consult the LICENSE.md distributed with 
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
