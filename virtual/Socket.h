/*
 * =====================================================================================
 *
 *       Filename:  Socket.h
 *
 *    Description:  Socket to which user may only add files
 *
 *        Version:  1.0
 *        Created:  08/17/2014 02:11:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef SOCKET_81ASFS7
#define SOCKET_81ASFS7

#include <templatious/CollectionAdapter.h>

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

    typedef void* iterator;
    typedef const void* const_iterator;
    typedef void* value_type;
    typedef const void* const_value_type;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.add(std::forward<V>(i));
    }

    template <class V,class U = int>
    static void insert_at(ThisCol& c, iterator at, V&& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
    }

    template <class U = int>
    static value_type& getByIndex(ThisCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<value_type&>(); // no compiler warning
    }

    template <class U = int>
    static const_value_type& getByIndex(ConstCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_value_type&>(); // no compiler warning
    }

    template <class U = int>
    static size_t getSize(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return -1;
    }

    template <class U = int>
    static void erase(ThisCol& c, iterator i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
    }

    template <class U = int>
    static void erase(ThisCol& c, iterator beg, iterator end) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
    }

    template <class U = int>
    static ThisCol instantiate() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Abstract class Socket cannot be instantiated.");
    }

    template <class U>
    static ThisCol instantiate(size_t size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Abstract class Socket cannot be instantiated.");
    }

    template <class U>
    static ThisCol* instHeap() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Abstract class Socket cannot be instantiated.");
    }

    template <class U>
    static ThisCol* instHeap(size_t size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Abstract class Socket cannot be instantiated.");
    }

    template <class U>
    static iterator begin(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<iterator>(); // no compiler warning
    }

    template <class U>
    static iterator end(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<iterator>(); // no compiler warning
    }

    template <class U>
    static iterator iter_at(ThisCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<iterator>(); // no compiler warning
    }

    template <class U>
    static const_iterator begin(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_iterator>(); // no compiler warning
    }
    template <class U>
    static const_iterator end(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_iterator>(); // no compiler warning
    }

    template <class U>
    static const_iterator iter_at(ConstCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_iterator>(); // no compiler warning
    }

    template <class U>
    static const_iterator cbegin(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_iterator>(); // no compiler warning
    }

    template <class U>
    static const_iterator cend(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_iterator>(); // no compiler warning
    }

    template <class U>
    static const_iterator citer_at(ConstCol& c, size_t i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_iterator>(); // no compiler warning
    }

    template <class U>
    static value_type& first(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<value_type&>(); // no compiler warning
    }

    template <class U>
    static const_value_type& first(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_value_type&>(); // no compiler warning
    }

    template <class U>
    static value_type& last(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<value_type&>(); // no compiler warning
    }

    template <class U>
    static const_value_type& last(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Socket class can only be added to.");
        return std::declval<const_value_type&>(); // no compiler warning
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
