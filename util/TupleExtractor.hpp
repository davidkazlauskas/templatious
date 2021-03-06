//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  TupleExtractor.hpp
 *
 *    Description:  Extract tuple from collections
 *
 *        Version:  1.0
 *        Created:  05/25/2014 03:55:33 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef TUPLEEXTRACTOR_8TT4LUA0
#define TUPLEEXTRACTOR_8TT4LUA0

#include <tuple>
#include <templatious/CollectionAdapter.hpp>

namespace templatious {
namespace util {

template <class ...Args>
struct TupleExtractor;

template <class A,class ...Args>
struct TupleExtractor<A,Args...> {
    typedef typename templatious::adapters::CollectionAdapter<A>::ValueType
        ValueType;
    typedef std::tuple<ValueType> ThisTuple;

    TupleExtractor<Args...> _next;

    auto getTuple() -> decltype(std::tuple_cat(ThisTuple(),_next.getTuple())) {
        return std::tuple_cat(ThisTuple(),_next.getTuple());
    }

    TupleExtractor(A& a,Args& ... args) : _next(args...) {}

};

template <class A>
struct TupleExtractor<A> {
    typedef typename templatious::adapters::CollectionAdapter<A>::ValueType
        ValueType;
    typedef std::tuple<ValueType> ThisTuple;

    ThisTuple getTuple() {
        return ThisTuple();
    }

    TupleExtractor(A& a) {}

};

template <bool additional = false,class ...Args>
struct ExtractionSelector;

template <class A,class ...Args>
struct ExtractionSelector<false,A,Args...> {
    typedef TupleExtractor<Args...> ThisExtractor;

    auto getTuple(Args&... args)
        -> decltype(ThisExtractor(args...).getTuple()) 
    {
        return ThisExtractor(args...).getTuple();
    }
};

template <class A,class ...Args>
struct ExtractionSelector<true,A,Args...> {
    typedef TupleExtractor<Args...> ThisExtractor;

    auto getTuple(Args&... args)
        -> decltype(std::tuple_cat(std::tuple<A>(),ThisExtractor(args...).getTuple()))
    {
        return std::tuple_cat(std::tuple<A>(),ThisExtractor(args...).getTuple());
    }
};

}
}

#endif /* end of include guard: TUPLEEXTRACTOR_8TT4LUA0 */

