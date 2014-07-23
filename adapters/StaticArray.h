/*
 * =====================================================================================
 *
 *       Filename:  StaticArray.h
 *
 *    Description:  Adapter for static array
 *
 *        Version:  1.0
 *        Created:  07/14/2014 06:15:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef STATICARRAY_XTPYVV10
#define STATICARRAY_XTPYVV10

#include <cstddef>

#include <templatious/util/Selectors.h>
#include <templatious/CollectionAdapter.h>

namespace templatious { 
namespace adapters {

template <class T,size_t sz>
struct CollectionAdapter< T[sz] > {

    static const bool is_valid = true;
    //static const bool is_valid = false;
    static const size_t size = sz;

	typedef T* ThisCol;
    typedef const ThisCol ConstCol;
	typedef T* iterator;
	typedef const iterator const_iterator;
	typedef T value_type;
	typedef const value_type const_value_type;

    template <class U = int>
    static bool add(T c[size], const value_type& i) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Add not supported as static array doesn't hold any state.");
    }

    static value_type& getByIndex(T c[size],size_t i) {
        return c[i];
    }

    static const_value_type& getByIndex(const T c[size],size_t i) {
        return c[i];
    }

    static size_t getSize(const T c[size]) {
        return size;
    }

    template <class U = int>
    static bool erase(T c[size], iterator beg) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Erase not supported as static array doesn't hold any state.");
    }

    template <class U = int>
    static bool erase(T c[size], iterator beg, iterator end) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Erase not supported as static array doesn't hold any state.");
    }

    template <class U = int>
    static ThisCol instantiate() {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Instantiate not supported as static arrays are allocated on the stack.");
    }

    template <class U = int>
    static ThisCol instantiate(int size) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Instantiate not supported as static arrays are allocated on the stack.");
    }

    static iterator begin(T c[size]) {
        return &c[0];
    }

    static iterator end(T c[size]) {
        return &c[size];
    }

    static iterator iter_at(T c[size],size_t i) {
        return &c[i];
    }

    static const_iterator cbegin(T c[size]) {
        return &c[0];
    }

    static const_iterator cend(T c[size]) {
        return &c[size];
    }

    static const_iterator citer_at(T c[size],size_t i) {
        return &c[i];
    }

    static value_type& first(T c[size]) {
        return c[0];
    }

    static const value_type& first(const T c[size]) {
        return c[0];
    }

    static value_type& last(T c[size]) {
        return c[size - 1];
    }

    static const value_type& last(const T c[size]) {
        return c[size - 1];
    }

    template <class U = int>
    static bool insert_at(T c, iterator at, const value_type& i) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Insert not supported as static array doesn't hold any state.");
    }

    template <class U = int>
    static void clear(T c[size]) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Clear not supported as static array doesn't hold any state.");
    }

};

}
}


#endif /* end of include guard: STATICARRAY_XTPYVV10 */
