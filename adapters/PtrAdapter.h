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

#include <templatious/util/Selectors.h>
#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter<T*> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef T* ThisCol;
    typedef const T* ConstCol;
    typedef typename Inner::iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V>
    static void add(ThisCol c, V&& i) {
        Inner::add(*c,i);
    }

    template <class V>
    static void insert_at(ThisCol c,iterator at,V&& i) {
        Inner::insert_at(*c,at,i);
    }

    static value_type& getByIndex(ThisCol c,size_t i) {
        return Inner::getByIndex(*c,i);
    }

    static const_value_type& getByIndex(ConstCol c, size_t i) {
        return Inner::getByIndex(*c,i);
    }

    static size_t getSize(ThisCol c) {
        return Inner::getSize(*c);
    }

    static void erase(ThisCol c, iterator beg) {
        Inner::erase(*c,beg);
    }

    static void erase(ThisCol c, iterator beg, iterator end) {
        Inner::erase(*c,beg,end);
    }

    static ThisCol instantiate() {
        return Inner::instHeap();
    }
    static ThisCol instantiate(size_t size) {
        return Inner::instHeap(size);
    }

    static iterator begin(ThisCol c) {
        return Inner::begin(*c);
    }

    static iterator end(ThisCol c) {
        return Inner::end(*c);
    }

    static iterator iter_at(ThisCol c,size_t i) {
        return Inner::iter_at(*c,i);
    }

    static const_iterator cbegin(ThisCol c) {
        return Inner::cbegin(*c);
    }

    static const_iterator cend(ThisCol c) {
        return Inner::cend(*c);
    }

    static const_iterator citer_at(ThisCol c,size_t i) {
        return Inner::citer_at(*c,i);
    }

    static value_type& first(ThisCol c) {
        return Inner::first(*c);
    }

    static const value_type& first(ConstCol c) {
        return Inner::first(*c);
    }

    static value_type& last(ThisCol c) {
        return Inner::last(*c);
    }

    static const value_type& last(ConstCol c) {
        return Inner::last(*c);
    }

    static void clear(ThisCol c) {
        return Inner::clear(*c);
    }

};

template <class T>
struct CollectionAdapter<const T*> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef const T* ThisCol;
    typedef const T* ConstCol;
    typedef typename Inner::const_iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::const_value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V,class U = int>
    static void add(ThisCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class V,class U = int>
    static void insert_at(ThisCol c,iterator at,V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static const_value_type& getByIndex(ConstCol c, size_t i) {
        return Inner::getByIndex(*c,i);
    }

    static size_t getSize(ConstCol c) {
        return Inner::getSize(*c);
    }

    template <class U = int>
    static void erase(ThisCol c, iterator beg) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U = int>
    static void erase(ThisCol c, iterator beg, iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static ThisCol instantiate() {
        return Inner::instHeap();
    }
    static ThisCol instantiate(size_t size) {
        return Inner::instHeap(size);
    }

    static iterator begin(ThisCol c) {
        return Inner::begin(*c);
    }

    static iterator end(ThisCol c) {
        return Inner::end(*c);
    }

    static iterator iter_at(ThisCol c,size_t i) {
        return Inner::iter_at(*c,i);
    }

    static const_iterator cbegin(ThisCol c) {
        return Inner::cbegin(*c);
    }

    static const_iterator cend(ThisCol c) {
        return Inner::cend(*c);
    }

    static const_iterator citer_at(ThisCol c,size_t i) {
        return Inner::citer_at(*c,i);
    }

    static const_value_type& first(ConstCol c) {
        return Inner::first(*c);
    }

    static const_value_type& last(ConstCol c) {
        return Inner::last(*c);
    }

    template <class U = int>
    static void clear(ThisCol c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
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
    typedef typename Inner::iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V>
    static void add(RCol c, V&& i) {
        Inner::add(c,i);
    }

    template <class V>
    static void insert_at(RCol c,iterator at,V&& i) {
        Inner::insert_at(c,at,i);
    }

    static value_type& getByIndex(RCol c,size_t i) {
        return Inner::getByIndex(c,i);
    }

    static const_value_type& getByIndex(CRCol c, size_t i) {
        return Inner::getByIndex(c,i);
    }

    static size_t getSize(RCol c) {
        return Inner::getSize(c);
    }

    static void erase(RCol c, iterator beg) {
        Inner::erase(c,beg);
    }

    static void erase(RCol c, iterator beg, iterator end) {
        Inner::erase(c,beg,end);
    }

    static auto instantiate()
        -> decltype(Inner::instantiate())
    {
        return Inner::instantiate();
    }

    static auto instantiate(size_t size)
        -> decltype(Inner::instantiate())
    {
        return Inner::instantiate(size);
    }

    static iterator begin(RCol c) {
        return Inner::begin(c);
    }

    static iterator end(RCol c) {
        return Inner::end(c);
    }

    static iterator iter_at(RCol c,size_t i) {
        return Inner::iter_at(c,i);
    }

    static const_iterator cbegin(RCol c) {
        return Inner::cbegin(c);
    }

    static const_iterator cend(RCol c) {
        return Inner::cend(c);
    }

    static const_iterator citer_at(RCol c,size_t i) {
        return Inner::citer_at(c,i);
    }

    static value_type& first(RCol c) {
        return Inner::first(c);
    }

    static const value_type& first(CRCol c) {
        return Inner::first(c);
    }

    static value_type& last(RCol c) {
        return Inner::last(c);
    }

    static const value_type& last(CRCol c) {
        return Inner::last(c);
    }

    static void clear(RCol c) {
        return Inner::clear(c);
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
    typedef typename Inner::const_iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::const_value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V,class U = int>
    static void add(CRCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class V,class U = int>
    static void insert_at(CRCol c,iterator at,V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static const_value_type& getByIndex(CRCol c, size_t i) {
        return Inner::getByIndex(c,i);
    }

    static size_t getSize(CRCol c) {
        return Inner::getSize(c);
    }

    template <class U = int>
    static void erase(CRCol c, iterator beg) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U = int>
    static void erase(CRCol c, iterator beg, iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static ThisCol instantiate() {
        return Inner::instHeap();
    }
    static ThisCol instantiate(size_t size) {
        return Inner::instHeap(size);
    }

    static iterator begin(CRCol c) {
        return Inner::cbegin(c);
    }

    static iterator end(CRCol c) {
        return Inner::cend(c);
    }

    static iterator iter_at(CRCol c,size_t i) {
        return Inner::iter_at(c,i);
    }

    static const_iterator cbegin(CRCol c) {
        return Inner::cbegin(c);
    }

    static const_iterator cend(CRCol c) {
        return Inner::cend(c);
    }

    static const_iterator citer_at(CRCol c,size_t i) {
        return Inner::citer_at(c,i);
    }

    static const_value_type& first(CRCol c) {
        return Inner::first(c);
    }

    static const_value_type& last(CRCol c) {
        return Inner::last(c);
    }

    template <class U = int>
    static void clear(CRCol c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

};

}
}

#endif /* end of include guard: PTRADAPTER_YWAZ6V0Z */
