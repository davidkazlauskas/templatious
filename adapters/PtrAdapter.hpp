/*
 * =====================================================================================
 *
 *       Filename:  PtrAdapter.h
 *
 *    Description:  Collection adapter specifically for pointers
 *
 *        Version:  1.0
 *        Created:  07/18/2014 03:48:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef PTRADAPTER_YWAZ6V0Z
#define PTRADAPTER_YWAZ6V0Z

#include <templatious/util/Selectors.hpp>
#include <templatious/CollectionAdapter.hpp>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter<T*> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef T* ThisCol;
    typedef const T* ConstCol;
    typedef typename Inner::Iterator Iterator;
    typedef typename Inner::ConstIterator ConstIterator;
    typedef typename Inner::ValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class V>
    static void add(ThisCol c, V&& i) {
        Inner::add(*c,i);
    }

    template <class V>
    static void insertAt(ThisCol c,Iterator at,V&& i) {
        Inner::insertAt(*c,at,i);
    }

    static ValueType& getByIndex(ThisCol c,size_t i) {
        return Inner::getByIndex(*c,i);
    }

    static ConstValueType& getByIndex(ConstCol c, size_t i) {
        return Inner::getByIndex(*c,i);
    }

    static long size(ThisCol c) {
        return Inner::size(*c);
    }

    static void erase(ThisCol c, Iterator beg) {
        Inner::erase(*c,beg);
    }

    static void erase(ThisCol c, Iterator beg, Iterator end) {
        Inner::erase(*c,beg,end);
    }

    static Iterator begin(ThisCol c) {
        return Inner::begin(*c);
    }

    static Iterator end(ThisCol c) {
        return Inner::end(*c);
    }

    static Iterator iterAt(ThisCol c,size_t i) {
        return Inner::iterAt(*c,i);
    }

    static ConstIterator cbegin(ThisCol c) {
        return Inner::cbegin(*c);
    }

    static ConstIterator cend(ThisCol c) {
        return Inner::cend(*c);
    }

    static ConstIterator citerAt(ThisCol c,size_t i) {
        return Inner::citerAt(*c,i);
    }

    static ValueType& first(ThisCol c) {
        return Inner::first(*c);
    }

    static ConstValueType& first(ConstCol c) {
        return Inner::first(*c);
    }

    static ValueType& last(ThisCol c) {
        return Inner::last(*c);
    }

    static ConstValueType& last(ConstCol c) {
        return Inner::last(*c);
    }

    static void clear(ThisCol c) {
        return Inner::clear(*c);
    }

    static bool canAdd(ConstCol c) {
        return Inner::canAdd(*c);
    }

};

template <class T>
struct CollectionAdapter<const T*> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef const T* ThisCol;
    typedef const T* ConstCol;
    typedef typename Inner::ConstIterator Iterator;
    typedef typename Inner::ConstIterator ConstIterator;
    typedef typename Inner::ConstValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class V,class U = int>
    static void add(ThisCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class V,class U = int>
    static void insertAt(ThisCol c,Iterator at,V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static ConstValueType& getByIndex(ConstCol c, size_t i) {
        return Inner::getByIndex(*c,i);
    }

    static long size(ConstCol c) {
        return Inner::size(*c);
    }

    template <class U = int>
    static void erase(ThisCol c, Iterator beg) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U = int>
    static void erase(ThisCol c, Iterator beg, Iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static Iterator begin(ThisCol c) {
        return Inner::begin(*c);
    }

    static Iterator end(ThisCol c) {
        return Inner::end(*c);
    }

    static Iterator iterAt(ThisCol c,size_t i) {
        return Inner::iterAt(*c,i);
    }

    static ConstIterator cbegin(ThisCol c) {
        return Inner::cbegin(*c);
    }

    static ConstIterator cend(ThisCol c) {
        return Inner::cend(*c);
    }

    static ConstIterator citerAt(ThisCol c,size_t i) {
        return Inner::citerAt(*c,i);
    }

    static ConstValueType& first(ConstCol c) {
        return Inner::first(*c);
    }

    static ConstValueType& last(ConstCol c) {
        return Inner::last(*c);
    }

    template <class U = int>
    static void clear(ThisCol c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static bool canAdd(ConstCol c) {
        return Inner::canAdd(*c);
    }

};

template <class T>
struct CollectionAdapter<T&> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef T ThisCol;
    typedef const T ConstCol;
    typedef ThisCol& RCol;
    typedef ConstCol& CRCol;
    typedef typename Inner::Iterator Iterator;
    typedef typename Inner::ConstIterator ConstIterator;
    typedef typename Inner::ValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class V>
    static void add(RCol c, V&& i) {
        Inner::add(c,i);
    }

    template <class V>
    static void insertAt(RCol c,Iterator at,V&& i) {
        Inner::insertAt(c,at,i);
    }

    static ValueType& getByIndex(RCol c,size_t i) {
        return Inner::getByIndex(c,i);
    }

    static ConstValueType& getByIndex(CRCol c, size_t i) {
        return Inner::getByIndex(c,i);
    }

    static long size(RCol c) {
        return Inner::size(c);
    }

    static long size(CRCol c) {
        return Inner::size(c);
    }

    static void erase(RCol c, Iterator beg) {
        Inner::erase(c,beg);
    }

    static void erase(RCol c, Iterator beg, Iterator end) {
        Inner::erase(c,beg,end);
    }

    static Iterator begin(RCol c) {
        return Inner::begin(c);
    }

    static Iterator end(RCol c) {
        return Inner::end(c);
    }

    static Iterator iterAt(RCol c,size_t i) {
        return Inner::iterAt(c,i);
    }

    static ConstIterator cbegin(RCol c) {
        return Inner::cbegin(c);
    }

    static ConstIterator cend(RCol c) {
        return Inner::cend(c);
    }

    static ConstIterator citerAt(RCol c,size_t i) {
        return Inner::citerAt(c,i);
    }

    static ValueType& first(RCol c) {
        return Inner::first(c);
    }

    static ConstValueType& first(CRCol c) {
        return Inner::first(c);
    }

    static ValueType& last(RCol c) {
        return Inner::last(c);
    }

    static ConstValueType& last(CRCol c) {
        return Inner::last(c);
    }

    static void clear(RCol c) {
        return Inner::clear(c);
    }

    static bool canAdd(CRCol c) {
        return Inner::canAdd(c);
    }

};

template <class T>
struct CollectionAdapter<const T&> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef const T ThisCol;
    typedef ThisCol ConstCol;
    typedef ThisCol& RCol;
    typedef ConstCol& CRCol;
    typedef typename Inner::ConstIterator Iterator;
    typedef typename Inner::ConstIterator ConstIterator;
    typedef typename Inner::ConstValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class V,class U = int>
    static void add(CRCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class V,class U = int>
    static void insertAt(CRCol c,Iterator at,V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static ConstValueType& getByIndex(CRCol c, size_t i) {
        return Inner::getByIndex(c,i);
    }

    static long size(CRCol c) {
        return Inner::size(c);
    }

    template <class U = int>
    static void erase(CRCol c, Iterator beg) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U = int>
    static void erase(CRCol c, Iterator beg, Iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static ConstIterator begin(CRCol c) {
        return Inner::cbegin(c);
    }

    static ConstIterator end(CRCol c) {
        return Inner::cend(c);
    }

    static ConstIterator iterAt(CRCol c,size_t i) {
        return Inner::citerAt(c,i);
    }

    static ConstIterator cbegin(CRCol c) {
        return Inner::cbegin(c);
    }

    static ConstIterator cend(CRCol c) {
        return Inner::cend(c);
    }

    static ConstIterator citerAt(CRCol c,size_t i) {
        return Inner::citerAt(c,i);
    }

    static ConstValueType& first(CRCol c) {
        return Inner::first(c);
    }

    static ConstValueType& last(CRCol c) {
        return Inner::last(c);
    }

    template <class U = int>
    static void clear(CRCol c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static bool canAdd(CRCol c) {
        return Inner::canAdd(c);
    }
};

// RVALUE REFERENCES
template <class T>
struct CollectionAdapter<T&&> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef T ThisCol;
    typedef const T ConstCol;
    typedef ThisCol& RCol;
    typedef ConstCol& CRCol;
    typedef ThisCol&& RVCol;
    typedef ConstCol&& CRVCol;
    typedef typename Inner::Iterator Iterator;
    typedef typename Inner::ConstIterator ConstIterator;
    typedef typename Inner::ValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class V>
    static void add(RCol c, V&& i) {
        Inner::add(c,i);
    }

    template <class V>
    static void insertAt(RCol c,Iterator at,V&& i) {
        Inner::insertAt(c,at,i);
    }

    static ValueType& getByIndex(RCol c,size_t i) {
        return Inner::getByIndex(c,i);
    }

    static ConstValueType& getByIndex(CRCol c, size_t i) {
        return Inner::getByIndex(c,i);
    }

    static long size(RCol c) {
        return Inner::size(c);
    }

    static long size(CRCol c) {
        return Inner::size(c);
    }

    static void erase(RCol c, Iterator beg) {
        Inner::erase(c,beg);
    }

    static void erase(RCol c, Iterator beg, Iterator end) {
        Inner::erase(c,beg,end);
    }

    static Iterator begin(RCol c) {
        return Inner::begin(c);
    }

    static Iterator end(RCol c) {
        return Inner::end(c);
    }

    static Iterator begin(RVCol c) {
        return Inner::begin(c);
    }

    static Iterator end(RVCol c) {
        return Inner::end(c);
    }

    static Iterator iterAt(RCol c,size_t i) {
        return Inner::iterAt(c,i);
    }

    static Iterator iterAt(RVCol c,size_t i) {
        return Inner::iterAt(c,i);
    }

    static ConstIterator cbegin(CRCol c) {
        return Inner::cbegin(c);
    }

    static ConstIterator cend(CRCol c) {
        return Inner::cend(c);
    }

    static ConstIterator citerAt(RCol c,size_t i) {
        return Inner::citerAt(c,i);
    }

    static ValueType& first(RCol c) {
        return Inner::first(c);
    }

    static ConstValueType& first(CRCol c) {
        return Inner::first(c);
    }

    static ValueType& last(RCol c) {
        return Inner::last(c);
    }

    static ConstValueType& last(CRCol c) {
        return Inner::last(c);
    }

    static void clear(RCol c) {
        return Inner::clear(c);
    }

    static bool canAdd(CRCol c) {
        return Inner::canAdd(c);
    }
};

// RVALUE REFERENCES
template <class T>
struct CollectionAdapter<const T&&> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef T ThisCol;
    typedef const T ConstCol;
    typedef ThisCol& RCol;
    typedef ConstCol& CRCol;
    typedef typename Inner::Iterator Iterator;
    typedef typename Inner::ConstIterator ConstIterator;
    typedef typename Inner::ValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class V>
    static void add(RCol c, V&& i) {
        Inner::add(c,i);
    }

    template <class V>
    static void insertAt(RCol c,Iterator at,V&& i) {
        Inner::insertAt(c,at,i);
    }

    static ValueType& getByIndex(RCol c,size_t i) {
        return Inner::getByIndex(c,i);
    }

    static ConstValueType& getByIndex(CRCol c, size_t i) {
        return Inner::getByIndex(c,i);
    }

    static long size(RCol c) {
        return Inner::size(c);
    }

    static long size(CRCol c) {
        return Inner::size(c);
    }

    static void erase(RCol c, Iterator beg) {
        Inner::erase(c,beg);
    }

    static void erase(RCol c, Iterator beg, Iterator end) {
        Inner::erase(c,beg,end);
    }

    static Iterator begin(RCol c) {
        return Inner::begin(c);
    }

    static Iterator end(RCol c) {
        return Inner::end(c);
    }

    static Iterator iterAt(RCol c,size_t i) {
        return Inner::iterAt(c,i);
    }

    static ConstIterator cbegin(RCol c) {
        return Inner::cbegin(c);
    }

    static ConstIterator cend(RCol c) {
        return Inner::cend(c);
    }

    static ConstIterator citerAt(RCol c,size_t i) {
        return Inner::citerAt(c,i);
    }

    static ValueType& first(RCol c) {
        return Inner::first(c);
    }

    static ConstValueType& first(CRCol c) {
        return Inner::first(c);
    }

    static ValueType& last(RCol c) {
        return Inner::last(c);
    }

    static ConstValueType& last(CRCol c) {
        return Inner::last(c);
    }

    static void clear(RCol c) {
        return Inner::clear(c);
    }

    static bool canAdd(CRCol c) {
        return Inner::canAdd(c);
    }
};


}
}

#endif /* end of include guard: PTRADAPTER_YWAZ6V0Z */
