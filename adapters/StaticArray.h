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
    static const bool floating_iterator = true;
    static const size_t size_const = sz;

	typedef T* ThisCol;
    typedef const ThisCol ConstCol;
	typedef T* Iterator;
	typedef const Iterator ConstIterator;
	typedef T ValueType;
	typedef const ValueType ConstValueType;

    template <class U = int>
    static bool add(T c[size_const], ConstValueType& i) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Add not supported as static array doesn't hold any state.");
        return false;
    }

    static ValueType& getByIndex(T c[size_const],size_t i) {
        return c[i];
    }

    static ConstValueType& getByIndex(const T c[size_const],size_t i) {
        return c[i];
    }

    static long size(const T c[size_const]) {
        return size_const;
    }

    template <class U = int>
    static bool erase(T c[size_const], Iterator beg) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Erase not supported as static array doesn't hold any state.");
        return false;
    }

    template <class U = int>
    static bool erase(T c[size_const], Iterator beg, Iterator end) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Erase not supported as static array doesn't hold any state.");
        return false;
    }

    static Iterator begin(T c[size_const]) {
        return &c[0];
    }

    static Iterator end(T c[size_const]) {
        return &c[size_const];
    }

    static Iterator iterAt(T c[size_const],size_t i) {
        return &c[i];
    }

    static ConstIterator cbegin(T c[size_const]) {
        return &c[0];
    }

    static ConstIterator cend(T c[size_const]) {
        return &c[size_const];
    }

    static ConstIterator citerAt(T c[size_const],size_t i) {
        return &c[i];
    }

    static ValueType& first(T c[size_const]) {
        return c[0];
    }

    static ConstValueType& first(const T c[size_const]) {
        return c[0];
    }

    static ValueType& last(T c[size_const]) {
        return c[size_const - 1];
    }

    static ConstValueType& last(const T c[size_const]) {
        return c[size_const - 1];
    }

    template <class U = int>
    static bool insertAt(T c, Iterator at, ConstValueType& i) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Insert not supported as static array doesn't hold any state.");
        return false;
    }

    template <class U = int>
    static void clear(T c[size_const]) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Clear not supported as static array doesn't hold any state.");
    }

};

template <class T,size_t sz>
struct CollectionAdapter< const T[sz] > {

    static const bool is_valid = true;
    static const bool floating_iterator = true;
    static const size_t size_const = sz;

	typedef const T* ThisCol;
    typedef ThisCol ConstCol;
	typedef const T* Iterator;
	typedef Iterator ConstIterator;
	typedef const T ValueType;
	typedef ValueType ConstValueType;

    template <class U = int>
    static bool add(T c[size_const], ConstValueType& i) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Add not supported as static array doesn't hold any state.");
        return false;
    }

    static ConstValueType& getByIndex(const T c[size_const],size_t i) {
        return c[i];
    }

    static long size(const T c[size_const]) {
        return size_const;
    }

    template <class U = int>
    static bool erase(T c[size_const], Iterator beg) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Erase not supported as static array doesn't hold any state.");
        return false;
    }

    template <class U = int>
    static bool erase(T c[size_const], Iterator beg, Iterator end) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Erase not supported as static array doesn't hold any state.");
        return false;
    }

    static Iterator begin(const T c[size_const]) {
        return &c[0];
    }

    static Iterator end(const T c[size_const]) {
        return &c[size_const];
    }

    static Iterator iterAt(const T c[size_const],size_t i) {
        return &c[i];
    }

    static ConstIterator cbegin(const T c[size_const]) {
        return &c[0];
    }

    static ConstIterator cend(const T c[size_const]) {
        return &c[size_const];
    }

    static ConstIterator citerAt(const T c[size_const],size_t i) {
        return &c[i];
    }

    static ConstValueType& first(const T c[size_const]) {
        return c[0];
    }

    static ConstValueType& last(const T c[size_const]) {
        return c[size_const - 1];
    }

    template <class U = int>
    static bool insertAt(T c, Iterator at, ConstValueType& i) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Insert not supported as static array doesn't hold any state.");
        return false;
    }

    template <class U = int>
    static void clear(T c[size_const]) {
        // suppress static assert unless method is actually used
        static_assert(templatious::util::DummyResolver<U, false>::val,
            "Clear not supported as static array doesn't hold any state.");
    }

};

template <size_t sz>
struct CollectionAdapter< const char(&)[sz] > {
    // const char array with reference is considered
    // a string, not a collection of chars.
    static const bool is_valid = false;
    static const bool floating_iterator = true;
};

}
}


#endif /* end of include guard: STATICARRAY_XTPYVV10 */
