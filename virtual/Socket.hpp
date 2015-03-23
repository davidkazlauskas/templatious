//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Socket.hpp
 *
 *    Description:  Socket to which user may only add files
 *
 *        Version:  1.0
 *        Created:  08/17/2014 02:11:33 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef SOCKET_81ASFS7
#define SOCKET_81ASFS7

#include <templatious/CollectionAdapter.hpp>

namespace templatious {

template <class T>
struct Socket {
    virtual void add(T& a) = 0;
    virtual void add(const T& a) = 0;
};

namespace adapters {

template <class T>
struct CollectionAdapter< Socket<T> > {

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef Socket<T> ThisCol;
    typedef const ThisCol ConstCol;

    typedef void* Iterator;
    typedef const void* ConstIterator;
    typedef void* ValueType;
    typedef const void* ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.add(std::forward<V>(i));
    }

    template <class V,class U = int>
    static void insertAt(ThisCol& c, Iterator at, V&& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
    }

    template <class U = int>
    static ValueType& getByIndex(ThisCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ValueType&>(); // no compiler warning
    }

    template <class U = int>
    static ConstValueType& getByIndex(ConstCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstValueType&>(); // no compiler warning
    }

    template <class U = int>
    static long size(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return -1;
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
    }

    template <class U>
    static Iterator begin(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<Iterator>(); // no compiler warning
    }

    template <class U>
    static Iterator end(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<Iterator>(); // no compiler warning
    }

    template <class U>
    static Iterator iterAt(ThisCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<Iterator>(); // no compiler warning
    }

    template <class U>
    static ConstIterator begin(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstIterator>(); // no compiler warning
    }
    template <class U>
    static ConstIterator end(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstIterator>(); // no compiler warning
    }

    template <class U>
    static ConstIterator iterAt(ConstCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstIterator>(); // no compiler warning
    }

    template <class U>
    static ConstIterator cbegin(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstIterator>(); // no compiler warning
    }

    template <class U>
    static ConstIterator cend(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstIterator>(); // no compiler warning
    }

    template <class U>
    static ConstIterator citerAt(ConstCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstIterator>(); // no compiler warning
    }

    template <class U>
    static ValueType& first(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ValueType&>(); // no compiler warning
    }

    template <class U>
    static ConstValueType& first(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstValueType&>(); // no compiler warning
    }

    template <class U>
    static ValueType& last(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ValueType&>(); // no compiler warning
    }

    template <class U>
    static ConstValueType& last(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<ConstValueType&>(); // no compiler warning
    }

    template <class U>
    static void clear(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
    }

    template <class U>
    static bool canAdd(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return false;
    }
};

}

}

#endif /* end of include guard: SOCKET_81ASFS7 */
