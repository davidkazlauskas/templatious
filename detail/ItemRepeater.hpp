//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  ItemRepeater.hpp
 *
 *    Description:  Repeat one item n times
 *
 *        Version:  1.0
 *        Created:  01/02/2015 02:20:04 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef ITEMREPEATER_TOX6MU34
#define ITEMREPEATER_TOX6MU34

#include <utility>

#include <templatious/util/Exceptions.hpp>
#include <templatious/CollectionAdapter.hpp>

namespace templatious {
namespace detail {

TEMPLATIOUS_BOILERPLATE_EXCEPTION( ItemRepeaterOutOfRangeException,
    "Requested item is out of range.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION( ItemRepeaterEmptyException,
    "Item repeater is empty.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION( ItemRepeaterNegativeSizeException,
    "Item repeater cannot be of negative size.");

template <
    class T,
    template <class> class StoragePolicy
>
struct ItemRepeater {
    typedef typename StoragePolicy<T>::Container Container;
    typedef ItemRepeater<T,StoragePolicy> ThisRepeater;

    // user is more likely to pass negative
    // value than a very large integer
    template <class V>
    ItemRepeater(V&& v,long sz)
        : _size(sz), _c(std::forward<V>(v))
    {
        if (_size < 0) {
            throw ItemRepeaterNegativeSizeException();
        }
    }

    struct Iterator {
        Iterator(const ThisRepeater& r,long i)
            : _i(i), _r(std::addressof(r)) {}

        auto operator*() const
         -> decltype(std::declval<Container>().cgetRef())
        {
            return _r->_c.cgetRef();
        }

        Iterator& operator++() {
            ++_i;
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return _i == rhs._i;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }
    private:
        long _i;
        const ThisRepeater* _r;
    };

    // this collection shall return
    // const and only const iterators
    Iterator cbegin() const {
        return Iterator(*this,0);
    }

    Iterator cend() const {
        return Iterator(*this,_size);
    }

    long size() const {
        return _size;
    }

    auto theItem() const
     -> decltype(std::declval<Container>().cgetRef())
    {
        if (isEmpty()) {
            throw ItemRepeaterEmptyException();
        }
        return _c.cgetRef();
    }

    Iterator iterAt(long n) const {
        if (n >= _size) {
            throw ItemRepeaterOutOfRangeException();
        }
        return Iterator(*this,n);
    }

private:
    bool isEmpty() const {
        return _size <= 0;
    }

    long _size;
    Container _c;
};

}

namespace adapters {

template <
    class T,
    template <class> class StoragePolicy
>
struct CollectionAdapter<
    const templatious::detail::ItemRepeater<
        T,
        StoragePolicy
    >
> {
    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef const templatious::detail::ItemRepeater<
        T,
        StoragePolicy
    > ThisCol;

    typedef const ThisCol ConstCol;
    typedef ThisCol& RCol;
    typedef ConstCol& CRCol;
    typedef typename ThisCol::Iterator Iterator;
    typedef typename ConstCol::Iterator ConstIterator;
    typedef const typename std::remove_reference<T>::type ValueType;
    typedef ValueType ConstValueType;

    template <class U = int,class V>
    static void add(RCol c, V&& i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    template <class U = int,class V>
    static void insertAt(RCol c,Iterator at,V&& i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    template <class U = int>
    static ConstValueType& getByIndex(CRCol c, long i) {
        if (i >= c.size()) {
            throw ::templatious::detail::
                ItemRepeaterOutOfRangeException();
        }
        return c.theItem();
    }

    static long size(CRCol c) {
        return c.size();
    }

    template <class U = int>
    static void erase(RCol c, Iterator beg) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    template <class U = int>
    static void erase(RCol c, Iterator beg, Iterator end) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    static ConstIterator begin(CRCol c) {
        return c.cbegin();
    }

    static ConstIterator end(CRCol c) {
        return c.cend();
    }

    template <class U = int>
    static Iterator iterAt(RCol c,long i) {
        return c.iterAt(i);
    }

    static ConstIterator cbegin(CRCol c) {
        return c.cbegin();
    }

    static ConstIterator cend(CRCol c) {
        return c.cend();
    }

    template <class U = int>
    static ConstIterator citerAt(RCol c,long i) {
        return c.iterAt(i);
    }

    template <class U = int>
    static ConstValueType& first(CRCol c) {
        return c.theItem();
    }

    template <class U = int>
    static ConstValueType& last(CRCol c) {
        return c.theItem();
    }

    template <class U = int>
    static void clear(RCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    static bool canAdd(CRCol c) {
        return false;
    }

};

template <
    class T,
    template <class> class StoragePolicy
>
struct CollectionAdapter<
    templatious::detail::ItemRepeater<
        T,
        StoragePolicy
    >
> {
    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef const templatious::detail::ItemRepeater<
        T,
        StoragePolicy
    > ThisCol;

    typedef const ThisCol ConstCol;
    typedef ThisCol& RCol;
    typedef ConstCol& CRCol;
    typedef typename ThisCol::Iterator Iterator;
    typedef typename ConstCol::Iterator ConstIterator;
    typedef const typename std::remove_reference<T>::type ValueType;
    typedef ValueType ConstValueType;

    template <class U = int,class V>
    static void add(RCol c, V&& i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    template <class U = int,class V>
    static void insertAt(RCol c,Iterator at,V&& i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    template <class U = int>
    static ConstValueType& getByIndex(CRCol c, long i) {
        if (i >= c.size()) {
            throw ::templatious::detail::
                ItemRepeaterOutOfRangeException();
        }
        return c.theItem();
    }

    static long size(CRCol c) {
        return c.size();
    }

    template <class U = int>
    static void erase(RCol c, Iterator beg) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    template <class U = int>
    static void erase(RCol c, Iterator beg, Iterator end) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    static ConstIterator begin(CRCol c) {
        return c.cbegin();
    }

    static ConstIterator end(CRCol c) {
        return c.cend();
    }

    template <class U = int>
    static Iterator iterAt(RCol c,long i) {
        return c.iterAt(i);
    }

    static ConstIterator cbegin(CRCol c) {
        return c.cbegin();
    }

    static ConstIterator cend(CRCol c) {
        return c.cend();
    }

    template <class U = int>
    static ConstIterator citerAt(RCol c,long i) {
        return c.iterAt(i);
    }

    template <class U = int>
    static ConstValueType& first(CRCol c) {
        return c.theItem();
    }

    template <class U = int>
    static ConstValueType& last(CRCol c) {
        return c.theItem();
    }

    template <class U = int>
    static void clear(RCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Item repeater can be traversed only.");
    }

    static bool canAdd(CRCol c) {
        return false;
    }

};

}
}

#endif /* end of include guard: ITEMREPEATER_TOX6MU34 */
