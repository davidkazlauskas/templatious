//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  QuadroIter.hpp
 *
 *    Description:  Iterator for quadro function
 *
 *        Version:  1.0
 *        Created:  07/27/2014 07:31:34 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef QUADROITER_NY9I9DCO
#define QUADROITER_NY9I9DCO

#include <type_traits>

#include <templatious/CollectionAdapter.hpp>
#include <templatious/util/DefaultStoragePolicy.hpp>

namespace templatious {
namespace recursive {

template <class... Args>
struct QuadroIterator;

template <class A>
struct QuadroIterator<A> {
    typedef QuadroIterator<A> ThisIter;
    typedef templatious::adapters::CollectionAdapter<A> Adapter;
    typedef typename Adapter::Iterator Iterator;

    typedef typename templatious::util::
        DefaultStoragePolicy< A >::Container Container;

    Container _c;
    Iterator _i;
    Iterator _e;

    template <class T>
    QuadroIterator(T&& t) :
        _c(t),
        _i(Adapter::begin(t)),
        _e(Adapter::end(t))
    {
        static_assert(std::is_same<T,A>::value,
                "Collection passed to constructor differs \
                from the collection this iterator should take");
    }

    bool inc() {
        ++_i;
        if (_i != _e) {
            return false;
        }

        return true;
    }

    void reset() {
        _i = Adapter::begin(_c.getRef());
    }

    template <class F>
    auto callFunction(F&& f)
        -> decltype(f(*_i))
    {
        return f(*_i);
    }

    template <class F, class... Args>
    auto callFunction(F&& f, Args&&... args) ->
    decltype(f(std::forward<Args>(args)..., *_i))
    {
        return f(std::forward<Args>(args)..., *_i);
    }


};

template <class A, class... Tail>
struct QuadroIterator<A, Tail...> {
    typedef QuadroIterator<A, Tail...> ThisIter;
    typedef QuadroIterator<Tail...> TailIter;
    typedef templatious::adapters::CollectionAdapter<A> Adapter;
    typedef typename Adapter::Iterator Iterator;

    typedef typename templatious::util::
        DefaultStoragePolicy< A >::Container Container;

    Container _c;
    Iterator _i;
    Iterator _e;
    TailIter _t;

    template <class T,class... TailC>
    QuadroIterator(T&& t,TailC&&... tail) :
        _c(t),
        _i(Adapter::begin(std::forward<T>(t))),
        _e(Adapter::end(std::forward<T>(t))),
        _t(std::forward<TailC>(tail)...)
    {
        static_assert(std::is_same<T,A>::value,
                "Collection passed to constructor differs \
                from the collection this iterator should take");
    }

    bool inc() {
        if (_t.inc()) {
            _t.reset();
            ++_i;
            if (_i == _e) {
                return true;
            }
        }
        return false;
    }

    void reset() {
        _i = Adapter::begin(_c.getRef());
        _t.reset();
    }

    template <class F>
    auto callFunction(F&& f) -> decltype(
        _t.callFunction(std::forward<F>(f), *_i))
    {
        return _t.callFunction(std::forward<F>(f), *_i);
    }

    template <class F, class... Args>
    auto callFunction(F&& f, Args&&... args)
        -> decltype(_t.callFunction(
        std::forward<F>(f), std::forward<Args>(args)..., *_i))
    {
        return _t.callFunction(
                std::forward<F>(f),
                std::forward<Args>(args)...,
                *_i);
    }


};

}
}

#endif /* end of include guard: QUADROITER_NY9I9DCO */
