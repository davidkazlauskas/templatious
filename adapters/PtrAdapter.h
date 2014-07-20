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

    typedef T* ThisCol;
    typedef const T* ConstCol;
    typedef typename Inner::iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V>
    static bool add(ThisCol c, V&& i) {
        return Inner::add(*c,i);
    }

    template <class V>
    static bool insert_at(ThisCol c,iterator at,V&& i) {
        return Inner::insert_at(*c,at,i);
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

    static bool erase(ThisCol c, iterator beg) {
        return Inner::erase(*c,beg);
    }

    static bool erase(ThisCol c, iterator beg, iterator end) {
        return Inner::erase(*c,beg,end);
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

    typedef const T* ThisCol;
    typedef const T* ConstCol;
    typedef typename Inner::const_iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::const_value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V,class U = int>
    static bool add(ThisCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class V,class U = int>
    static bool insert_at(ThisCol c,iterator at,V&& i) {
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
    static bool erase(ThisCol c, iterator beg) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U = int>
    static bool erase(ThisCol c, iterator beg, iterator end) {
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

    typedef T& ThisCol;
    typedef const T& ConstCol;
    typedef typename Inner::iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V>
    static bool add(ThisCol c, V&& i) {
        return Inner::add(c,i);
    }

    template <class V>
    static bool insert_at(ThisCol c,iterator at,V&& i) {
        return Inner::insert_at(c,at,i);
    }

    static value_type& getByIndex(ThisCol c,size_t i) {
        return Inner::getByIndex(c,i);
    }

    static const_value_type& getByIndex(ConstCol c, size_t i) {
        return Inner::getByIndex(c,i);
    }

    static size_t getSize(ThisCol c) {
        return Inner::getSize(c);
    }

    static bool erase(ThisCol c, iterator beg) {
        return Inner::erase(c,beg);
    }

    static bool erase(ThisCol c, iterator beg, iterator end) {
        return Inner::erase(c,beg,end);
    }

    static ThisCol instantiate() {
        return Inner::instHeap();
    }
    static ThisCol instantiate(size_t size) {
        return Inner::instHeap(size);
    }

    static iterator begin(ThisCol c) {
        return Inner::begin(c);
    }

    static iterator end(ThisCol c) {
        return Inner::end(c);
    }

    static iterator iter_at(ThisCol c,size_t i) {
        return Inner::iter_at(c,i);
    }

    static const_iterator cbegin(ThisCol c) {
        return Inner::cbegin(c);
    }

    static const_iterator cend(ThisCol c) {
        return Inner::cend(c);
    }

    static const_iterator citer_at(ThisCol c,size_t i) {
        return Inner::citer_at(c,i);
    }

    static value_type& first(ThisCol c) {
        return Inner::first(c);
    }

    static const value_type& first(ConstCol c) {
        return Inner::first(c);
    }

    static value_type& last(ThisCol c) {
        return Inner::last(c);
    }

    static const value_type& last(ConstCol c) {
        return Inner::last(c);
    }

    static void clear(ThisCol c) {
        return Inner::clear(c);
    }

};

template <class T>
struct CollectionAdapter<const T&> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;

    typedef const T& ThisCol;
    typedef const T& ConstCol;
    typedef typename Inner::const_iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::const_value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V,class U = int>
    static bool add(ThisCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class V,class U = int>
    static bool insert_at(ThisCol c,iterator at,V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static const_value_type& getByIndex(ConstCol c, size_t i) {
        return Inner::getByIndex(c,i);
    }

    static size_t getSize(ConstCol c) {
        return Inner::getSize(c);
    }

    template <class U = int>
    static bool erase(ThisCol c, iterator beg) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U = int>
    static bool erase(ThisCol c, iterator beg, iterator end) {
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
        return Inner::begin(c);
    }

    static iterator end(ThisCol c) {
        return Inner::end(c);
    }

    static iterator iter_at(ThisCol c,size_t i) {
        return Inner::iter_at(c,i);
    }

    static const_iterator cbegin(ThisCol c) {
        return Inner::cbegin(c);
    }

    static const_iterator cend(ThisCol c) {
        return Inner::cend(c);
    }

    static const_iterator citer_at(ThisCol c,size_t i) {
        return Inner::citer_at(c,i);
    }

    static const_value_type& first(ConstCol c) {
        return Inner::first(c);
    }

    static const_value_type& last(ConstCol c) {
        return Inner::last(c);
    }

    template <class U = int>
    static void clear(ThisCol c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

};

}
}

#endif /* end of include guard: PTRADAPTER_YWAZ6V0Z */
