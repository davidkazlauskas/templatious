/*
 * =====================================================================================
 *
 *       Filename:  Range.h
 *
 *    Description:  Range class
 *
 *        Version:  1.0
 *        Created:  07/30/2014 06:01:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef RANGE_KFIVLD23
#define RANGE_KFIVLD23

#include <templatious/CollectionAdapter.h>

namespace templatious {

template <class T>
struct Range {

    typedef typename adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::iterator iterator;
    typedef typename Ad::const_iterator const_iterator;

    static_assert(Ad::is_valid,"Adapter is invalid.");
    T&& _c;
    iterator _b;
    iterator _e;

    template <class V>
    Range(V&& v,const iterator& b,const iterator& e) :
        _c(v), _b(b), _e(e) {}

    iterator begin() {
        return _b;
    }

    iterator end() {
        return _e;
    }

    const_iterator cbegin() {
        return _b;
    }

    const_iterator cend() {
        return _e;
    }
};

}

#endif /* end of include guard: RANGE_KFIVLD23 */
