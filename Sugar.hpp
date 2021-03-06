//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Sugar.hpp
 *
 *    Description:  Syntax sugar
 *
 *        Version:  1.0
 *        Created:  07/07/2014 07:13:15 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef SUGAR_RISZR4GH
#define SUGAR_RISZR4GH

#include <templatious/CollectionAdapter.hpp>
#include <templatious/adapters/PtrAdapter.hpp>
#include <templatious/StaticFactory.hpp>
#include <templatious/StaticManipulator.hpp>

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

/**
 * Templatious foreach macro.
 * @param var Loop variable name.
 * @param col Collection to traverse.
 *
 * Example:
 * ~~~~~~~
 * std::vector<int> v;
 * SA::add(v,1,3,5);
 * // v contains {1,3,5}
 *
 * TEMPLATIOUS_FOREACH(auto& i,v) {
 *     std::cout << i << " ";
 *     i *= 2;
 * }
 *
 * // prints out
 * // 1 3 5
 * // v now contains {2,6,10}
 * ~~~~~~~
 */
#define TEMPLATIOUS_FOREACH(var,col) \
    for (::templatious::__ForeachCounter<decltype(::templatious::StaticAdapter::begin(col))>     \
            __tmp_i(::templatious::StaticAdapter::begin(col));                                   \
            __tmp_i._i != ::templatious::StaticAdapter::end(col)                                 \
            && __tmp_i._keepGoing;                                                               \
            ++__tmp_i)                                                                           \
        for (var = *__tmp_i._i; ; ({__tmp_i.flipGoing();break;}))

/**
 * 0 to N traversal.
 * @param var Loop variable name (type is long)
 * @param to  Number to traverse to
 *
 * Example:
 * ~~~~~~~
 * int sumA = 0;
 * int sumB = 0;
 *
 * for (int i = 0; i < 10; ++i) {
 *     sumA += i;
 * }
 *
 * TEMPLATIOUS_0_TO_N(i,10) {
 *     sumB += i;
 * }
 *
 * assert( sumA == sumB );
 * ~~~~~~~
 */
#define TEMPLATIOUS_0_TO_N(var,to)                                                               \
    for (long var = 0; var < to; ++var)

/**
 * Repeat underlying block of code n times.
 *
 * Example:
 * ~~~~~~~
 * int count = 0;
 * TEMPLATIOUS_REPEAT( 7 ) {
 *     ++count;
 * }
 * assert( count == 7 );
 * ~~~~~~~
 */
#define TEMPLATIOUS_REPEAT(n) \
    for (long __tmp_i = 0; __tmp_i < n; ++__tmp_i)

#define TEMPLATIOUS_TRIPLET(AdName,FactName,ManipName) \
    typedef templatious::StaticAdapter AdName;\
    typedef templatious::StaticFactory FactName;\
    typedef templatious::StaticManipulator ManipName;

/**
 * Macro to define main three templatious classes:
 * templatious::StaticAdapter
 * templatious::StaticFactory
 * templatious::StaticManipulator
 *
 * Expands to:
 * ~~~~~~~
 * typedef templatious::StaticAdapter SA;
 * typedef templatious::StaticFactory SF;
 * typedef templatious::StaticManipulator SM;
 * ~~~~~~~
 */
#define TEMPLATIOUS_TRIPLET_STD TEMPLATIOUS_TRIPLET(SA,SF,SM)

#define TEMPLATIOUS_VPCORE templatious::VirtualPackCore

/**
 * Define a functor class to be
 * used with arbitrary type signature when passed into
 * SM::callEach. Needed if packs contain
 * different types of variables and C++11 lambda
 * expression does not suffice.
 * If you happen to be on C++14
 * just use lambda expression with auto
 * parameter type instead.
 *
 * @param name Name of the instantiated struct.
 * @param expr Expression to perform when called,
 * i is the loop variable.
 *
 * Example:
 * ~~~~~~~
 * TEMPLATIOUS_CALLEACH_FCTOR(PrintFctor,{
 *     std::cout << i << " ";
 * });
 *
 * ...
 *
 * auto pack = SF::pack(1,2.2,"3.3",std::string("4.4"));
 *
 * PrintFctor sf;
 * SM::callEach(sf,pack);
 *
 * // prints out
 * // 1 2.2 3.3 4.4
 * ~~~~~~~
 */
#define TEMPLATIOUS_CALLEACH_FCTOR(name, expr) \
    TEMPLATIOUS_TRIPLET_STD;\
    struct name {\
        template <class T>\
        void operator()(T&& i) {\
            expr;\
        }\
    };

/**
 * Define a functor class with storage to be
 * used with arbitrary type signature when passed into
 * SM::callEach. Needed if packs contain
 * different types of variables and C++11 lambda
 * expression does not suffice.
 * If you happen to be on C++14
 * just use lambda expression with auto
 * parameter type instead.
 *
 * @param name Name of the instantiated struct.
 * @param expr Expression to perform when called.
 * _c is the storage variable while i is the
 * loop variable.
 *
 * Example:
 * ~~~~~~~
 * TEMPLATIOUS_CALLEACH_FCTOR_WSTOR(SumFctor, _c += i);
 *
 * ...
 *
 * auto pack = SF::pack(1,2,3,4);
 *
 * SumFctor<int> sf(0);
 * SM::callEach(sf,pack);
 *
 * int expSum = 1 + 2 + 3 + 4;
 * assert( sf._c == expSum );
 * ~~~~~~~
 */
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
