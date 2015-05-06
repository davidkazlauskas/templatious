//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  RecursiveIter.hpp
 *
 *    Description:  Recursive Collection Iterator
 *
 *        Version:  1.0
 *        Created:  05/25/2014 08:25:40 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef RECURSIVEITER_4VX65VTM
#define RECURSIVEITER_4VX65VTM

#include <type_traits>
#include <tuple>

#include <templatious/CollectionAdapter.hpp>

namespace templatious {
namespace recursive {

namespace col = templatious::adapters;

template <class... Args>
struct RecursiveIterator;

template <class A>
struct RecursiveIterator<A> {
    enum { num = 1 };

    typedef RecursiveIterator<A> ThisIter;
    typedef col::CollectionAdapter<A> Adapter;
    typedef typename Adapter::Iterator Iterator;
    typedef typename Adapter::ValueType ValType;
    static_assert(Adapter::is_valid,"Adapter is invalid.");

    Iterator _a;

    template <class U>
    RecursiveIterator(U&& a) :
        _a(Adapter::begin(std::forward<U>(a)))
    {
        static_assert(std::is_same<U,A>::value,
                "Collection passed to constructor differs \
                from the collection this iterator should take");
    }

    void inc() { ++_a; }

    template <class F>
    auto callFunction(F&& f)
        -> decltype(f(*_a))
    {
        return f(*_a);
    }

    template <class F, class... Args>
    auto callFunction(F&& f, Args&&... args) ->
    decltype(f(std::forward<Args>(args)..., *_a))
    {
        return f(std::forward<Args>(args)..., *_a);
    }

};

template <class A, class... Tail>
struct RecursiveIterator<A, Tail...> {
    enum { num = RecursiveIterator<Tail...>::num + 1 };

    typedef RecursiveIterator<A, Tail...> ThisIter;
    typedef col::CollectionAdapter<A> Adapter;
    typedef typename Adapter::Iterator Iterator;
    typedef typename Adapter::ValueType ValType;

    static_assert(Adapter::is_valid,"Adapter is invalid.");

    Iterator _a;
    RecursiveIterator<Tail...> _t;

    template <class U,class... TailC>
    RecursiveIterator(U&& a, TailC&&... args) :
        _a(Adapter::begin(std::forward<U>(a))),
        _t(std::forward<TailC>(args)...)
    {
        static_assert(std::is_same<U,A>::value,
                "Collection passed to constructor differs \
                from the collection this iterator should take");
    }

    void inc() {
        ++_a;
        _t.inc();
    }

    template <class F>
    auto callFunction(F&& f) -> decltype(
        _t.callFunction(std::forward<F>(f), *_a))
    {
        return _t.callFunction(std::forward<F>(f), *_a);
    }

    template <class F, class... Args>
    auto callFunction(F&& f, Args&&... args)
        -> decltype(_t.callFunction(
        std::forward<F>(f),
        std::forward<Args>(args)...,
        *_a))
    {
        return _t.callFunction(
                std::forward<F>(f),
                std::forward<Args>(args)...,
                *_a);
    }

};

}
}

#endif /* end of include guard: RECURSIVEITER_4VX65VTM */
