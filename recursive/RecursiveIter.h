/*
 * =====================================================================================
 *
 *       Filename:  RecursiveIter.h
 *
 *    Description:  Recursive Collection Iterator
 *
 *        Version:  1.0
 *        Created:  05/25/2014 08:25:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef RECURSIVEITER_4VX65VTM
#define RECURSIVEITER_4VX65VTM

#include <type_traits>
#include <tuple>

#include <templatious/CollectionAdapter.h>

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

    //void print() {
        //std::cout << *_a << std::endl;
    //}

    //void print_enum() { std::cout << num << std::endl; }

    void inc() { ++_a; }

    ThisIter& operator++() {
        inc();
        return *this;
    }

    template <int i = 0, class T>
    void setTuple(T& c) const {
        std::get<i>(c) = *_a;
    }

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

    //void print() {
        //std::cout << *_a << std::endl;
        //_t.print();
    //}

    //void print_enum() {
        //std::cout << num << std::endl;
        //_t.print_enum();
    //}

    template <int i = 0, class T>
    void setTuple(T& c) const {
        std::get<i>(c) = *_a;
        _t.setTuple<i + 1>(c);
    }

    //template <int i = 0, class T>
    //void print_tuple(T& c) const {
        //std::cout << std::get<i>(c) << std::endl;
        //_t.print_tuple<i + 1>(c);
    //}

    void inc() {
        ++_a;
        _t.inc();
    }

    ThisIter& operator++() {
        inc();
        return *this;
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
