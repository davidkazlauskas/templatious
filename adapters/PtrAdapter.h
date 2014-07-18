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

#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter<T*> {
    static const bool is_valid = false;

    typedef CollectionAdapter<T> Inner;

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

}
}

#endif /* end of include guard: PTRADAPTER_YWAZ6V0Z */
