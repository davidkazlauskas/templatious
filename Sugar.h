/*
 * =====================================================================================
 *
 *       Filename:  Sugar.h
 *
 *    Description:  Syntax sugar
 *
 *        Version:  1.0
 *        Created:  07/07/2014 07:13:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef SUGAR_RISZR4GH
#define SUGAR_RISZR4GH

#include <templatious/CollectionAdapter.h>

namespace templatious {

template <class T>
struct __ForeachCounter {
    typedef T Iter;

    Iter _i;
    bool _keepGoing;

    __ForeachCounter(const Iter i) : _i(i), _keepGoing(true) {}

    Iter operator++() {
        flipGoing();
        return ++_i;
    }

    void flipGoing() {
        _keepGoing = !_keepGoing;
    }

    Iter operator--() {
        flipGoing();
        return --_i;
    }

};

#define TEMPLATIOUS_FOREACH(var,col) \
    for (templatious::__ForeachCounter<decltype(templatious::StaticAdapter::begin(col))>    \
            _tmp_i(templatious::StaticAdapter::begin(col));                                 \
            _tmp_i._i != templatious::StaticAdapter::end(col)                               \
            && _tmp_i._keepGoing;                                                           \
            ++_tmp_i)                                                                       \
        for (var = *_tmp_i._i; ; ({_tmp_i.flipGoing();break;}))

}

#endif /* end of include guard: SUGAR_RISZR4GH */