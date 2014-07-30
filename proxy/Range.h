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

#include <utility>

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

    template <class V>
    Range(V&& v,const iterator& b) :
        _c(v), _b(b),
        _e(Ad::end(std::forward<V>(v))) {}


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

namespace adapters {

template <class T>
struct CollectionAdapter< Range<T> > {
    static const bool is_valid = true;

    typedef Range<T> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::iterator iterator;
    typedef typename ThisCol::const_iterator const_iterator;
    typedef typename ThisCol::Ad::value_type value_type;
    typedef typename ThisCol::Ad::const_value_type const_value_type;

    static iterator begin(ThisCol& c) {
        return c.begin();
    }

    static iterator end(ThisCol& c) {
        return c.end();
    }

    static iterator cbegin(ThisCol& c) {
        return c.cbegin();
    }

    static iterator cend(ThisCol& c) {
        return c.cend();
    }
};

}
}

#endif /* end of include guard: RANGE_KFIVLD23 */
