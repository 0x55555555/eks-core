// modified version of qhash.cpp

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "XUnorderedMap"

#ifdef truncate
#undef truncate
#endif

#include <qbitarray.h>
#include <qstring.h>
#include <stdlib.h>
#ifdef QT_QHASH_DEBUG
#include <qstring.h>
#endif

/*
    The prime_deltas array is a table of selected prime values, even
    though it doesn't look like one. The primes we are using are 1,
    2, 5, 11, 17, 37, 67, 131, 257, ..., i.e. primes in the immediate
    surrounding of a power of two.

    The primeForNumBits() function returns the prime associated to a
    power of two. For example, primeForNumBits(8) returns 257.
*/

static const uchar prime_deltas[] = {
    0,  0,  1,  3,  1,  5,  3,  3,  1,  9,  7,  5,  3,  9, 25,  3,
    1, 21,  3, 21,  7, 15,  9,  5,  3, 29, 15,  0,  0,  0,  0,  0
};

static inline int primeForNumBits(int numBits)
{
    return (1 << numBits) + prime_deltas[numBits];
}

/*
    Returns the smallest integer n such that
    primeForNumBits(n) >= hint.
*/
static int countBits(int hint)
{
    int numBits = 0;
    int bits = hint;

    while (bits > 1) {
        bits >>= 1;
        numBits++;
    }

    if (numBits >= (int)sizeof(prime_deltas)) {
        numBits = sizeof(prime_deltas) - 1;
    } else if (primeForNumBits(numBits) < hint) {
        ++numBits;
    }
    return numBits;
}

/*
    A XUnorderedMap has initially around pow(2, MinNumBits) buckets. For
    example, if MinNumBits is 4, it has 17 buckets.
*/
const int MinNumBits = 4;

XUnorderedMapData XUnorderedMapData::shared_null = {
    0, 0, Q_BASIC_ATOMIC_INITIALIZER(1), 0, 0, MinNumBits, 0, 0, true, false, 0, 0
};

void *XUnorderedMapData::allocateNode()
{
    return allocateNode(0);
}

void *XUnorderedMapData::allocateNode(int nodeAlign)
{
    void *ptr = strictAlignment ? allocator->alloc(nodeSize, nodeAlign) : allocator->alloc(nodeSize);
    Q_CHECK_PTR(ptr);
    return ptr;
}

void XUnorderedMapData::freeNode(void *node)
{
    allocator->free(node);
}

XUnorderedMapData *XUnorderedMapData::detach_helper(void (*node_duplicate)(Node *, void *), int nodeSize, XAllocatorBase *inpAlloc)
{
    return detach_helper2( node_duplicate, 0, nodeSize, 0, inpAlloc );
}

XUnorderedMapData *XUnorderedMapData::detach_helper2(void (*node_duplicate)(Node *, void *),
                                     void (*node_delete)(Node *),
                                     int nodeSize,
                                     int nodeAlign,
                                     XAllocatorBase *inpAlloc)
{
    union {
        XUnorderedMapData *d;
        Node *e;
    };
    d = xAllocateAndConstruct(inpAlloc, XUnorderedMapData);
    d->fakeNext = 0;
    d->buckets = 0;
    d->ref = 1;
    d->size = size;
    d->nodeSize = nodeSize;
    d->userNumBits = userNumBits;
    d->numBits = numBits;
    d->numBuckets = numBuckets;
    d->sharable = true;
    d->strictAlignment = nodeAlign > 8;
    d->reserved = 0;
    d->allocator = inpAlloc;

    if (numBuckets) {
        QT_TRY {
            d->buckets = (Node **)(d->allocator->alloc(sizeof(Node *) * numBuckets));
        } QT_CATCH(...) {
            // restore a consistent state for d
            d->numBuckets = 0;
            // roll back
            d->free_helper(node_delete);
            QT_RETHROW;
        }

        Node *this_e = reinterpret_cast<Node *>(this);
        for (int i = 0; i < numBuckets; ++i) {
            Node **nextNode = &d->buckets[i];
            Node *oldNode = buckets[i];
            while (oldNode != this_e) {
                QT_TRY {
                    Node *dup = static_cast<Node *>(allocateNode(nodeAlign));

                    QT_TRY {
                        node_duplicate(oldNode, dup);
                    } QT_CATCH(...) {
                        freeNode( dup );
                        QT_RETHROW;
                    }

                    dup->h = oldNode->h;
                    *nextNode = dup;
                    nextNode = &dup->next;
                    oldNode = oldNode->next;
                } QT_CATCH(...) {
                    // restore a consistent state for d
                    *nextNode = e;
                    d->numBuckets = i+1;
                    // roll back
                    d->free_helper(node_delete);
                    QT_RETHROW;
                }
            }
            *nextNode = e;
        }
    }
    return d;
}

void XUnorderedMapData::free_helper(void (*node_delete)(Node *))
{
    if (node_delete) {
        Node *this_e = reinterpret_cast<Node *>(this);
        Node **bucket = reinterpret_cast<Node **>(this->buckets);

        int n = numBuckets;
        while (n--) {
            Node *cur = *bucket++;
            while (cur != this_e) {
                Node *next = cur->next;
                node_delete(cur);
                freeNode(cur);
                cur = next;
            }
        }
    }
    allocator->free(buckets);
    allocator->free(this);
}

XUnorderedMapData::Node *XUnorderedMapData::nextNode(Node *node)
{
    union {
        Node *next;
        Node *e;
        XUnorderedMapData *d;
    };
    next = node->next;
    Q_ASSERT_X(next, "XUnorderedMap", "Iterating beyond end()");
    if (next->next)
        return next;

    int start = (node->h % d->numBuckets) + 1;
    Node **bucket = d->buckets + start;
    int n = d->numBuckets - start;
    while (n--) {
        if (*bucket != e)
            return *bucket;
        ++bucket;
    }
    return e;
}

XUnorderedMapData::Node *XUnorderedMapData::previousNode(Node *node)
{
    union {
        Node *e;
        XUnorderedMapData *d;
    };

    e = node;
    while (e->next)
        e = e->next;

    int start;
    if (node == e)
        start = d->numBuckets - 1;
    else
        start = node->h % d->numBuckets;

    Node *sentinel = node;
    Node **bucket = d->buckets + start;
    while (start >= 0) {
        if (*bucket != sentinel) {
            Node *prev = *bucket;
            while (prev->next != sentinel)
                prev = prev->next;
            return prev;
        }

        sentinel = e;
        --bucket;
        --start;
    }
    Q_ASSERT_X(start >= 0, "XUnorderedMap", "Iterating backward beyond begin()");
    return e;
}

/*
    If hint is negative, -hint gives the approximate number of
    buckets that should be used for the hash table. If hint is
    nonnegative, (1 << hint) gives the approximate number
    of buckets that should be used.
*/
void XUnorderedMapData::rehash(int hint)
{
    if (hint < 0) {
        hint = countBits(-hint);
        if (hint < MinNumBits)
            hint = MinNumBits;
        userNumBits = hint;
        while (primeForNumBits(hint) < (size >> 1))
            ++hint;
    } else if (hint < MinNumBits) {
        hint = MinNumBits;
    }

    if (numBits != hint) {
        Node *e = reinterpret_cast<Node *>(this);
        Node **oldBuckets = buckets;
        int oldNumBuckets = numBuckets;

        int nb = primeForNumBits(hint);
        buckets = (Node**)allocator->alloc(sizeof(Node *) * nb);
        numBits = hint;
        numBuckets = nb;
        for (int i = 0; i < numBuckets; ++i)
            buckets[i] = e;

        for (int i = 0; i < oldNumBuckets; ++i) {
            Node *firstNode = oldBuckets[i];
            while (firstNode != e) {
                uint h = firstNode->h;
                Node *lastNode = firstNode;
                while (lastNode->next != e && lastNode->next->h == h)
                    lastNode = lastNode->next;

                Node *afterLastNode = lastNode->next;
                Node **beforeFirstNode = &buckets[h % numBuckets];
                while (*beforeFirstNode != e)
                    beforeFirstNode = &(*beforeFirstNode)->next;
                lastNode->next = *beforeFirstNode;
                *beforeFirstNode = firstNode;
                firstNode = afterLastNode;
            }
        }
        allocator->free(oldBuckets);
    }
}

void XUnorderedMapData::destroyAndFree()
{
    free_helper(0);
}

#ifdef QT_QHASH_DEBUG

void XUnorderedMapData::dump()
{
    qDebug("Hash data (ref = %d, size = %d, nodeSize = %d, userNumBits = %d, numBits = %d, numBuckets = %d)",
            int(ref), size, nodeSize, userNumBits, numBits,
            numBuckets);
    qDebug("    %p (fakeNode = %p)", this, fakeNext);
    for (int i = 0; i < numBuckets; ++i) {
        QString line;
        Node *n = buckets[i];
        if (n != reinterpret_cast<Node *>(this)) {
            line.sprintf("%d:", i);
            while (n != reinterpret_cast<Node *>(this)) {
                line += QString().sprintf(" -> [%p]", n);
                if (!n) {
                    line += " (CORRUPT)";
                    break;
                }
                n = n->next;
            }
            qDebug(qPrintable(line));
        }
    }
}

void XUnorderedMapData::checkSanity()
{
    if (fakeNext)
        qFatal("Fake next isn't 0");

    for (int i = 0; i < numBuckets; ++i) {
        Node *n = buckets[i];
        Node *p = n;
        if (!n)
            qFatal("%d: Bucket entry is 0", i);
        if (n != reinterpret_cast<Node *>(this)) {
            while (n != reinterpret_cast<Node *>(this)) {
                if (!n->next)
                    qFatal("%d: Next of %p is 0, should be %p", i, n, this);
                n = n->next;
            }
        }
    }
}
#endif
