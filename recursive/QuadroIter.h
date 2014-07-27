/*
 * =====================================================================================
 *
 *       Filename:  QuadroIter.h
 *
 *    Description:  Iterator for quadro function
 *
 *        Version:  1.0
 *        Created:  07/27/2014 07:31:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef QUADROITER_NY9I9DCO
#define QUADROITER_NY9I9DCO

#include <type_traits>

#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace recursive {

template <class... Args>
struct QuadroIterator;

template <class A>
struct QuadroIterator<A> {
    typedef QuadroIterator<A> ThisIter;
    typedef col::CollectionAdapter<A> Adapter;
    typedef typename Adapter::iterator Iterator;
    typedef decltype(*Iterator()) ValType;

    A& _c;
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
        _i = Adapter::begin(_c);
    }

    template <class F>
    auto callFunction(F&& f)
        -> decltype(f(std::forward<ValType>(*_i)))
    {
        return f(std::forward<ValType>(*_i));
    }

    template <class F, class... Args>
    auto callFunction(F&& f, Args&&... args) ->
    decltype(f(std::forward<Args>(args)..., std::forward<ValType>(*_i)))
    {
        return f(std::forward<Args>(args)..., std::forward<ValType>(*_i));
    }


};

template <class A, class... Tail>
struct QuadroIterator<A, Tail...> {
    typedef QuadroIterator<A, Tail...> ThisIter;
    typedef QuadroIterator<Tail...> TailIter;
    typedef col::CollectionAdapter<A> Adapter;
    typedef typename Adapter::iterator Iterator;
    typedef decltype(*Iterator()) ValType;

    A& _c;
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
        _i = Adapter::begin(_c);
        _t.reset();
    }

    template <class F>
    auto callFunction(F&& f) -> decltype(
        _t.callFunction(std::forward<F>(f), std::forward<ValType>(*_i)))
    {
        return _t.callFunction(std::forward<F>(f), std::forward<ValType>(*_i));
    }

    template <class F, class... Args>
    auto callFunction(F&& f, Args&&... args)
        -> decltype(_t.callFunction(
        std::forward<F>(f), std::forward<Args>(args)..., std::forward<ValType>(*_i)))
    {
        return _t.callFunction(
                std::forward<F>(f),
                std::forward<Args>(args)...,
                std::forward<ValType>(*_i));
    }


};

}
}

#endif /* end of include guard: QUADROITER_NY9I9DCO */
