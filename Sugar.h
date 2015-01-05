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
#include <templatious/adapters/PtrAdapter.h>
#include <templatious/StaticFactory.h>
#include <templatious/StaticManipulator.h>

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
    for (::templatious::__ForeachCounter<decltype(::templatious::StaticAdapter::begin(col))>     \
            __tmp_i(::templatious::StaticAdapter::begin(col));                                   \
            __tmp_i._i != ::templatious::StaticAdapter::end(col)                                 \
            && __tmp_i._keepGoing;                                                               \
            ++__tmp_i)                                                                           \
        for (var = *__tmp_i._i; ; ({__tmp_i.flipGoing();break;}))

#define TEMPLATIOUS_REPEAT(n) \
    TEMPLATIOUS_FOREACH(auto __tmp_var,::templatious::StaticFactory::seqL(n))

#define TEMPLATIOUS_TRIPLET(AdName,FactName,ManipName) \
    typedef templatious::StaticAdapter AdName;\
    typedef templatious::StaticFactory FactName;\
    typedef templatious::StaticManipulator ManipName;

#define TEMPLATIOUS_TRIPLET_STD TEMPLATIOUS_TRIPLET(SA,SF,SM)

#define TEMPLATIOUS_CALLEACH_FCTOR(name, expr) \
    TEMPLATIOUS_TRIPLET_STD;\
    struct name {\
        template <class T>\
        void operator()(T&& i) {\
            expr;\
        }\
    };

#define TEMPLATIOUS_CALLEACH_FCTOR_WSTOR(name, expr) \
    TEMPLATIOUS_TRIPLET_STD;\
    template <class StorType>\
    struct name {\
        name(const StorType& t) : _c(t) {}\
        name() {}\
        template <class T>\
        bool operator()(T&& i) {\
            expr;\
            return true;\
        }\
        StorType _c;\
    };

}


#endif /* end of include guard: SUGAR_RISZR4GH */
