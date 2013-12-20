// Modified version of QHash from Qt 4

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

#ifndef XUNORDERERMAP_H
#define XUNORDERERMAP_H

#include "XGlobal.h"
#include "Utilities/XAssert.h"
#include "XCore.h"
#include "Containers/XVector.h"
#include <atomic>


class QBitArray;
class QByteArray;
class QString;
class QStringRef;

namespace Eks
{

struct EKSCORE_EXPORT UnorderedMapData
{
    struct Node {
        Node *next;
        uint h;
    };

    Node *fakeNext;
    Node **buckets;
    std::atomic<int> ref;
    int size;
    int nodeSize;
    short userNumBits;
    short numBits;
    int numBuckets;
    uint sharable : 1;
    uint strictAlignment : 1;
    uint reserved : 30;
    AllocatorBase *allocator;

    UnorderedMapData() { }
    enum DefaultCtor { Dummy };
    UnorderedMapData(DefaultCtor dummy);

    void *allocateNode(); // ### Qt5 remove me
    void *allocateNode(int nodeAlign);
    void freeNode(void *node);
    UnorderedMapData *detach_helper(void (*node_duplicate)(Node *, void *), int nodeSize, AllocatorBase *inpAlloc); // ### Qt5 remove me
    UnorderedMapData *detach_helper2(void (*node_duplicate)(Node *, void *), void (*node_delete)(Node *),
                              int nodeSize, int nodeAlign, AllocatorBase *inpAlloc);
    void mightGrow();
    bool willGrow();
    void hasShrunk();
    void rehash(int hint);
    void free_helper(void (*node_delete)(Node *));
    void destroyAndFree(); // ### Qt5 remove me
    Node *firstNode();
#ifdef QT_QHASH_DEBUG
    void dump();
    void checkSanity();
#endif
    static Node *nextNode(Node *node);
    static Node *previousNode(Node *node);

    static UnorderedMapData shared_null;
};

inline void UnorderedMapData::mightGrow() // ### Qt 5: eliminate
{
    if (size >= numBuckets)
        rehash(numBits + 1);
}

inline bool UnorderedMapData::willGrow()
{
    if (size >= numBuckets) {
        rehash(numBits + 1);
        return true;
    } else {
        return false;
    }
}

inline void UnorderedMapData::hasShrunk()
{
    if (size <= (numBuckets >> 3) && numBits > userNumBits) {
        QT_TRY {
            rehash(qMax(int(numBits) - 2, int(userNumBits)));
        } QT_CATCH(const std::bad_alloc &) {
            // ignore bad allocs - shrinking shouldn't throw. rehash is exception safe.
        }
    }
}

inline UnorderedMapData::Node *UnorderedMapData::firstNode()
{
    Node *e = reinterpret_cast<Node *>(this);
    Node **bucket = buckets;
    int n = numBuckets;
    while (n--) {
        if (*bucket != e)
            return *bucket;
        ++bucket;
    }
    return e;
}

struct UnorderedMapDummyValue
{
};

inline bool operator==(const UnorderedMapDummyValue & /* v1 */, const UnorderedMapDummyValue & /* v2 */)
{
    return true;
}

}

Q_DECLARE_TYPEINFO(Eks::UnorderedMapDummyValue, Q_MOVABLE_TYPE | Q_DUMMY_TYPE);

namespace Eks
{


template <class Key, class T>
struct UnorderedMapDummyNode
{
    UnorderedMapDummyNode *next;
    uint h;
    Key key;

    inline UnorderedMapDummyNode(const Key &key0) : key(key0) {}
};

template <class Key, class T>
struct UnorderedMapNode
{
    UnorderedMapNode *next;
    uint h;
    Key key;
    T value;

    inline UnorderedMapNode(const Key &key0) : key(key0) {} // ### remove in 5.0
    inline UnorderedMapNode(const Key &key0, const T &value0) : key(key0), value(value0) {}
    inline bool same_key(uint h0, const Key &key0) { return h0 == h && key0 == key; }
};

#ifndef QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION
#define Q_HASH_DECLARE_INT_NODES(key_type) \
    template <class T> \
    struct UnorderedMapDummyNode<key_type, T> { \
        UnorderedMapDummyNode *next; \
        union { uint h; key_type key; }; \
\
        inline UnorderedMapDummyNode(key_type /* key0 */) {} \
    }; \
\
    template <class T> \
    struct UnorderedMapNode<key_type, T> { \
        UnorderedMapNode *next; \
        union { uint h; key_type key; }; \
        T value; \
\
        inline UnorderedMapNode(key_type /* key0 */) {} \
        inline UnorderedMapNode(key_type /* key0 */, const T &value0) : value(value0) {} \
        inline bool same_key(uint h0, key_type) { return h0 == h; } \
    }

#if defined(Q_BYTE_ORDER) && Q_BYTE_ORDER == Q_LITTLE_ENDIAN
Q_HASH_DECLARE_INT_NODES(short);
Q_HASH_DECLARE_INT_NODES(ushort);
#endif
Q_HASH_DECLARE_INT_NODES(int);
Q_HASH_DECLARE_INT_NODES(uint);
#undef Q_HASH_DECLARE_INT_NODES
#endif // QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION

template <class Key, class T>
class UnorderedMap
{
    typedef UnorderedMapDummyNode<Key, T> DummyNode;
    typedef UnorderedMapNode<Key, T> Node;

    union {
        UnorderedMapData *d;
        UnorderedMapNode<Key, T> *e;
    };
    AllocatorBase *allocator;

    static inline Node *concrete(UnorderedMapData::Node *node) {
        return reinterpret_cast<Node *>(node);
    }

#ifdef Q_ALIGNOF
    static inline int alignOfNode() { return qMax<int>(sizeof(void*), Q_ALIGNOF(Node)); }
    static inline int alignOfDummyNode() { return qMax<int>(sizeof(void*), Q_ALIGNOF(DummyNode)); }
#else
    static inline int alignOfNode() { return 0; }
    static inline int alignOfDummyNode() { return 0; }
#endif

public:
    inline UnorderedMap(AllocatorBase *a=Core::defaultAllocator()) : d(&UnorderedMapData::shared_null), allocator(a)
      {
#if X_UNORDERED_MAP_THREAD_SAFE
      d->ref.ref();
#else
      ++d->ref;
#endif
      }
    inline UnorderedMap(const UnorderedMap<Key, T> &other) : d(other.d), allocator(other.allocator) { d->ref++; if (!d->sharable) detach(); }
    inline ~UnorderedMap()
      {
#if X_UNORDERED_MAP_THREAD_SAFE
      if (!d->ref.deref())
#else
      if (!(--d->ref))
#endif
        {
        freeData(d);
        }
      }

    UnorderedMap<Key, T> &operator=(const UnorderedMap<Key, T> &other);

    bool operator==(const UnorderedMap<Key, T> &other) const;
    inline bool operator!=(const UnorderedMap<Key, T> &other) const { return !(*this == other); }

    inline int size() const { return d->size; }

    inline bool isEmpty() const { return d->size == 0; }

    inline int capacity() const { return d->numBuckets; }
    void reserve(int size);
    inline void squeeze() { reserve(1); }

    inline void detach() { if (d->ref != 1) detach_helper(); }
    inline bool isDetached() const { return d->ref == 1; }
    inline void setSharable(bool sharable) { if (!sharable) detach(); d->sharable = sharable; }
    inline bool isSharedWith(const UnorderedMap<Key, T> &other) const { return d == other.d; }

    void clear();

    int remove(const Key &key);
    T take(const Key &key);

    bool contains(const Key &key) const;
    const Key key(const T &value) const;
    const Key key(const T &value, const Key &defaultKey) const;
    const T value(const Key &key) const;
    const T value(const Key &key, const T &defaultValue) const;
    T &operator[](const Key &key);
    const T operator[](const Key &key) const;

    Vector<Key> uniqueKeys() const;
    Vector<Key> keys() const;
    Vector<Key> keys(const T &value) const;
    Vector<T> values() const;
    Vector<T> values(const Key &key) const;
    int count(const Key &key) const;

    class const_iterator;

    class iterator
    {
        friend class const_iterator;
        UnorderedMapData::Node *i;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef qptrdiff difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;

        // ### Qt 5: get rid of 'operator Node *'
        inline operator Node *() const { return concrete(i); }
        inline iterator() : i(0) { }
        explicit inline iterator(void *node) : i(reinterpret_cast<UnorderedMapData::Node *>(node)) { }

        inline const Key &key() const { return concrete(i)->key; }
        inline T &value() const { return concrete(i)->value; }
        inline T &operator*() const { return concrete(i)->value; }
        inline T *operator->() const { return &concrete(i)->value; }
        inline bool operator==(const iterator &o) const { return i == o.i; }
        inline bool operator!=(const iterator &o) const { return i != o.i; }

        inline iterator &operator++() {
            i = UnorderedMapData::nextNode(i);
            return *this;
        }
        inline iterator operator++(int) {
            iterator r = *this;
            i = UnorderedMapData::nextNode(i);
            return r;
        }
        inline iterator &operator--() {
            i = UnorderedMapData::previousNode(i);
            return *this;
        }
        inline iterator operator--(int) {
            iterator r = *this;
            i = UnorderedMapData::previousNode(i);
            return r;
        }
        inline iterator operator+(int j) const
        { iterator r = *this; if (j > 0) while (j--) ++r; else while (j++) --r; return r; }
        inline iterator operator-(int j) const { return operator+(-j); }
        inline iterator &operator+=(int j) { return *this = *this + j; }
        inline iterator &operator-=(int j) { return *this = *this - j; }

        // ### Qt 5: not sure this is necessary anymore
#ifdef QT_STRICT_ITERATORS
    private:
#else
    public:
#endif
        inline bool operator==(const const_iterator &o) const
            { return i == o.i; }
        inline bool operator!=(const const_iterator &o) const
            { return i != o.i; }

    private:
        // ### Qt 5: remove
        inline operator bool() const { return false; }
    };
    friend class iterator;

    class const_iterator
    {
        friend class iterator;
        UnorderedMapData::Node *i;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef qptrdiff difference_type;
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        // ### Qt 5: get rid of 'operator Node *'
        inline operator Node *() const { return concrete(i); }
        inline const_iterator() : i(0) { }
        explicit inline const_iterator(void *node)
            : i(reinterpret_cast<UnorderedMapData::Node *>(node)) { }
#ifdef QT_STRICT_ITERATORS
        explicit inline const_iterator(const iterator &o)
#else
        inline const_iterator(const iterator &o)
#endif
        { i = o.i; }

        inline const Key &key() const { return concrete(i)->key; }
        inline const T &value() const { return concrete(i)->value; }
        inline const T &operator*() const { return concrete(i)->value; }
        inline const T *operator->() const { return &concrete(i)->value; }
        inline bool operator==(const const_iterator &o) const { return i == o.i; }
        inline bool operator!=(const const_iterator &o) const { return i != o.i; }

        inline const_iterator &operator++() {
            i = UnorderedMapData::nextNode(i);
            return *this;
        }
        inline const_iterator operator++(int) {
            const_iterator r = *this;
            i = UnorderedMapData::nextNode(i);
            return r;
        }
        inline const_iterator &operator--() {
            i = UnorderedMapData::previousNode(i);
            return *this;
        }
        inline const_iterator operator--(int) {
            const_iterator r = *this;
            i = UnorderedMapData::previousNode(i);
            return r;
        }
        inline const_iterator operator+(int j) const
        { const_iterator r = *this; if (j > 0) while (j--) ++r; else while (j++) --r; return r; }
        inline const_iterator operator-(int j) const { return operator+(-j); }
        inline const_iterator &operator+=(int j) { return *this = *this + j; }
        inline const_iterator &operator-=(int j) { return *this = *this - j; }

        // ### Qt 5: not sure this is necessary anymore
#ifdef QT_STRICT_ITERATORS
    private:
        inline bool operator==(const iterator &o) const { return operator==(const_iterator(o)); }
        inline bool operator!=(const iterator &o) const { return operator!=(const_iterator(o)); }
#endif

    private:
        // ### Qt 5: remove
        inline operator bool() const { return false; }
    };
    friend class const_iterator;

    // STL style
    inline iterator begin() { detach(); return iterator(d->firstNode()); }
    inline const_iterator begin() const { return const_iterator(d->firstNode()); }
    inline const_iterator constBegin() const { return const_iterator(d->firstNode()); }
    inline iterator end() { detach(); return iterator(e); }
    inline const_iterator end() const { return const_iterator(e); }
    inline const_iterator constEnd() const { return const_iterator(e); }
    iterator erase(iterator it);

    // more Qt
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    inline int count() const { return d->size; }
    iterator find(const Key &key);
    const_iterator find(const Key &key) const;
    const_iterator constFind(const Key &key) const;
    iterator insert(const Key &key, const T &value);
    iterator insertMulti(const Key &key, const T &value);
    UnorderedMap<Key, T> &unite(const UnorderedMap<Key, T> &other);

    // STL compatibility
    typedef T mapped_type;
    typedef Key key_type;
    typedef qptrdiff difference_type;
    typedef int size_type;

    inline bool empty() const { return isEmpty(); }

#ifdef QT_QHASH_DEBUG
    inline void dump() const { d->dump(); }
    inline void checkSanity() const { d->checkSanity(); }
#endif

private:
    void detach_helper();
    void freeData(UnorderedMapData *d);
    Node **findNode(const Key &key, uint *hp = 0) const;
    Node *createNode(uint h, const Key &key, const T &value, Node **nextNode);
    void deleteNode(Node *node);
    static void deleteNode2(UnorderedMapData::Node *node);

    static void duplicateNode(UnorderedMapData::Node *originalNode, void *newNode);
};


template <class Key, class T>
Q_INLINE_TEMPLATE void UnorderedMap<Key, T>::deleteNode(Node *node)
{
    deleteNode2(reinterpret_cast<UnorderedMapData::Node*>(node));
    d->freeNode(node);
}

template <class Key, class T>
Q_INLINE_TEMPLATE void UnorderedMap<Key, T>::deleteNode2(UnorderedMapData::Node *node)
{
#ifdef Q_CC_BOR
    concrete(node)->~UnorderedMapNode<Key, T>();
#elif defined(QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION)
    concrete(node)->~UnorderedMapNode();
#else
    concrete(node)->~Node();
#endif
}

template <class Key, class T>
Q_INLINE_TEMPLATE void UnorderedMap<Key, T>::duplicateNode(UnorderedMapData::Node *node, void *newNode)
{
    Node *concreteNode = concrete(node);
    if (QTypeInfo<T>::isDummy) {
        (void) new (newNode) DummyNode(concreteNode->key);
    } else {
        (void) new (newNode) Node(concreteNode->key, concreteNode->value);
    }
}

template <class Key, class T>
Q_INLINE_TEMPLATE typename UnorderedMap<Key, T>::Node *
UnorderedMap<Key, T>::createNode(uint ah, const Key &akey, const T &avalue, Node **anextNode)
{
    Node *node;

    if (QTypeInfo<T>::isDummy) {
        node = reinterpret_cast<Node *>(new (d->allocateNode(alignOfDummyNode())) DummyNode(akey));
    } else {
        node = new (d->allocateNode(alignOfNode())) Node(akey, avalue);
    }

    node->h = ah;
    node->next = *anextNode;
    *anextNode = node;
    ++d->size;
    return node;
}

template <class Key, class T>
Q_INLINE_TEMPLATE UnorderedMap<Key, T> &UnorderedMap<Key, T>::unite(const UnorderedMap<Key, T> &other)
{
    UnorderedMap<Key, T> copy(other);
    const_iterator it = copy.constEnd();
    while (it != copy.constBegin()) {
        --it;
        insertMulti(it.key(), it.value());
    }
    return *this;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE void UnorderedMap<Key, T>::freeData(UnorderedMapData *x)
{
    x->free_helper(deleteNode2);
}

template <class Key, class T>
Q_INLINE_TEMPLATE void UnorderedMap<Key, T>::clear()
{
    *this = UnorderedMap<Key,T>();
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE void UnorderedMap<Key, T>::detach_helper()
{
    UnorderedMapData *x = d->detach_helper2(duplicateNode, deleteNode2,
        QTypeInfo<T>::isDummy ? sizeof(DummyNode) : sizeof(Node),
        QTypeInfo<T>::isDummy ? alignOfDummyNode() : alignOfNode(), allocator);
#if X_UNORDERED_MAP_THREAD_SAFE
    if (!d->ref.deref())
#else
    if (!(--d->ref))
#endif
      {
      freeData(d);
      }
    d = x;
}

template <class Key, class T>
Q_INLINE_TEMPLATE UnorderedMap<Key, T> &UnorderedMap<Key, T>::operator=(const UnorderedMap<Key, T> &other)
{
    if (d != other.d) {
        UnorderedMapData *o = other.d;
        ++o->ref;
        if (!--d->ref)
            freeData(d);
        d = o;
        if (!d->sharable)
            detach_helper();
    }
    return *this;
}

template <class Key, class T>
Q_INLINE_TEMPLATE const T UnorderedMap<Key, T>::value(const Key &akey) const
{
    Node *node;
    if (d->size == 0 || (node = *findNode(akey)) == e) {
        return T();
    } else {
        return node->value;
    }
}

template <class Key, class T>
Q_INLINE_TEMPLATE const T UnorderedMap<Key, T>::value(const Key &akey, const T &adefaultValue) const
{
    Node *node;
    if (d->size == 0 || (node = *findNode(akey)) == e) {
        return adefaultValue;
    } else {
        return node->value;
    }
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE Vector<Key> UnorderedMap<Key, T>::uniqueKeys() const
{
    Vector<Key> res;
    res.reserve(size()); // May be too much, but assume short lifetime
    const_iterator i = begin();
    if (i != end()) {
        for (;;) {
            const Key &aKey = i.key();
            res.append(aKey);
            do {
                if (++i == end())
                    goto break_out_of_outer_loop;
            } while (aKey == i.key());
        }
    }
break_out_of_outer_loop:
    return res;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE Vector<Key> UnorderedMap<Key, T>::keys() const
{
    Vector<Key> res(allocator);
    res.reserve(size());
    const_iterator i = begin();
    while (i != end()) {
        res << i.key();
        ++i;
    }
    return res;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE Vector<Key> UnorderedMap<Key, T>::keys(const T &avalue) const
{
    Vector<Key> res;
    const_iterator i = begin();
    while (i != end()) {
        if (i.value() == avalue)
            res << i.key();
        ++i;
    }
    return res;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE const Key UnorderedMap<Key, T>::key(const T &avalue) const
{
    return key(avalue, Key());
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE const Key UnorderedMap<Key, T>::key(const T &avalue, const Key &defaultValue) const
{
    const_iterator i = begin();
    while (i != end()) {
        if (i.value() == avalue)
            return i.key();
        ++i;
    }

    return defaultValue;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE Vector<T> UnorderedMap<Key, T>::values() const
{
    Vector<T> res;
    res.reserve(size());
    const_iterator i = begin();
    while (i != end()) {
        res << i.value();
        ++i;
    }
    return res;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE Vector<T> UnorderedMap<Key, T>::values(const Key &akey) const
{
    Vector<T> res;
    Node *node = *findNode(akey);
    if (node != e) {
        do {
            res.append(node->value);
        } while ((node = node->next) != e && node->key == akey);
    }
    return res;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE int UnorderedMap<Key, T>::count(const Key &akey) const
{
    int cnt = 0;
    Node *node = *findNode(akey);
    if (node != e) {
        do {
            ++cnt;
        } while ((node = node->next) != e && node->key == akey);
    }
    return cnt;
}

template <class Key, class T>
Q_INLINE_TEMPLATE const T UnorderedMap<Key, T>::operator[](const Key &akey) const
{
    return value(akey);
}

template <class Key, class T>
Q_INLINE_TEMPLATE T &UnorderedMap<Key, T>::operator[](const Key &akey)
{
    detach();

    uint h;
    Node **node = findNode(akey, &h);
    if (*node == e) {
        if (d->willGrow())
            node = findNode(akey, &h);
        return createNode(h, akey, T(), node)->value;
    }
    return (*node)->value;
}

template <class Key, class T>
Q_INLINE_TEMPLATE typename UnorderedMap<Key, T>::iterator UnorderedMap<Key, T>::insert(const Key &akey,
                                                                         const T &avalue)
{
    detach();

    uint h;
    Node **node = findNode(akey, &h);
    if (*node == e) {
        if (d->willGrow())
            node = findNode(akey, &h);
        return iterator(createNode(h, akey, avalue, node));
    }

    if (!QTypeInfo<T>::isDummy)
        (*node)->value = avalue;
    return iterator(*node);
}

template <class Key, class T>
Q_INLINE_TEMPLATE typename UnorderedMap<Key, T>::iterator UnorderedMap<Key, T>::insertMulti(const Key &akey,
                                                                              const T &avalue)
{
    detach();
    d->willGrow();

    uint h;
    Node **nextNode = findNode(akey, &h);
    return iterator(createNode(h, akey, avalue, nextNode));
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE int UnorderedMap<Key, T>::remove(const Key &akey)
{
    if (isEmpty()) // prevents detaching shared null
        return 0;
    detach();

    int oldSize = d->size;
    Node **node = findNode(akey);
    if (*node != e) {
        bool deleteNext = true;
        do {
            Node *next = (*node)->next;
            deleteNext = (next != e && next->key == (*node)->key);
            deleteNode(*node);
            *node = next;
            --d->size;
        } while (deleteNext);
        d->hasShrunk();
    }
    return oldSize - d->size;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE T UnorderedMap<Key, T>::take(const Key &akey)
{
    if (isEmpty()) // prevents detaching shared null
        return T();
    detach();

    Node **node = findNode(akey);
    if (*node != e) {
        T t = (*node)->value;
        Node *next = (*node)->next;
        deleteNode(*node);
        *node = next;
        --d->size;
        d->hasShrunk();
        return t;
    }
    return T();
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE typename UnorderedMap<Key, T>::iterator UnorderedMap<Key, T>::erase(iterator it)
{
    if (it == iterator(e))
        return it;

    iterator ret = it;
    ++ret;

    Node *node = it;
    Node **node_ptr = reinterpret_cast<Node **>(&d->buckets[node->h % d->numBuckets]);
    while (*node_ptr != node)
        node_ptr = &(*node_ptr)->next;
    *node_ptr = node->next;
    deleteNode(node);
    --d->size;
    return ret;
}

template <class Key, class T>
Q_INLINE_TEMPLATE void UnorderedMap<Key, T>::reserve(int asize)
{
    detach();
    d->rehash(-qMax(asize, 1));
}

template <class Key, class T>
Q_INLINE_TEMPLATE typename UnorderedMap<Key, T>::const_iterator UnorderedMap<Key, T>::find(const Key &akey) const
{
    return const_iterator(*findNode(akey));
}

template <class Key, class T>
Q_INLINE_TEMPLATE typename UnorderedMap<Key, T>::const_iterator UnorderedMap<Key, T>::constFind(const Key &akey) const
{
    return const_iterator(*findNode(akey));
}

template <class Key, class T>
Q_INLINE_TEMPLATE typename UnorderedMap<Key, T>::iterator UnorderedMap<Key, T>::find(const Key &akey)
{
    detach();
    return iterator(*findNode(akey));
}

template <class Key, class T>
Q_INLINE_TEMPLATE bool UnorderedMap<Key, T>::contains(const Key &akey) const
{
    return *findNode(akey) != e;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE typename UnorderedMap<Key, T>::Node **UnorderedMap<Key, T>::findNode(const Key &akey,
                                                                            uint *ahp) const
{
    Node **node;
    uint h = ::qHash(akey);

    if (d->numBuckets) {
        node = reinterpret_cast<Node **>(&d->buckets[h % d->numBuckets]);
        xAssert(*node == e || (*node)->next);
        while (*node != e && !(*node)->same_key(h, akey))
            node = &(*node)->next;
    } else {
        node = const_cast<Node **>(reinterpret_cast<const Node * const *>(&e));
    }
    if (ahp)
        *ahp = h;
    return node;
}

template <class Key, class T>
Q_OUTOFLINE_TEMPLATE bool UnorderedMap<Key, T>::operator==(const UnorderedMap<Key, T> &other) const
{
    if (size() != other.size())
        return false;
    if (d == other.d)
        return true;

    const_iterator it = begin();

    while (it != end()) {
        const Key &akey = it.key();

        const_iterator it2 = other.find(akey);
        do {
            if (it2 == other.end() || !(it2.key() == akey))
                return false;
            if (!QTypeInfo<T>::isDummy && !(it.value() == it2.value()))
                return false;
            ++it;
            ++it2;
        } while (it != end() && it.key() == akey);
    }
    return true;
}

}

#endif // XUNORDERERMAP_H