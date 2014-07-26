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

#include <tuple>

#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace recursive {

namespace col = templatious::adapters;
typedef templatious::StaticAdapter SA;

template <class... Args>
struct RecursiveIterator;

template <class A>
struct RecursiveIterator<A> {
    enum { num = 1 };

    typedef col::CollectionAdapter<A> Adapter;
    typedef typename Adapter::iterator Iterator;
    typedef decltype(*Iterator()) ValType;
    static_assert(Adapter::is_valid,"Adapter is invalid.");

    Iterator _a;

    template <class U>
    RecursiveIterator(U&& a) : _a(SA::begin(std::forward<U>(a))) {
        //std::cout << "rec iter - " << a << std::endl;
    }

    void print() {
        std::cout << *_a << std::endl;
    }

    void print_enum() { std::cout << num << std::endl; }

    void inc() { ++_a; }

    template <int i = 0, class T>
    void setTuple(T& c) const {
        std::get<i>(c) = *_a;
    }

    template <class F>
    auto callFunction(F&& f)
        -> decltype(f(std::forward<ValType>(*_a)))
    {
        return f(std::forward<ValType>(*_a));
    }

    template <class F, class... Args>
    auto callFunction(F&& f, Args&&... args) ->
    decltype(f(std::forward<Args>(args)..., std::forward<ValType>(*_a)))
    {
        return f(std::forward<Args>(args)..., std::forward<ValType>(*_a));
    }

};

template <class A, class... Tail>
struct RecursiveIterator<A, Tail...> {
    enum { num = RecursiveIterator<Tail...>::num + 1 };


    typedef col::CollectionAdapter<A> Adapter;
    typedef typename Adapter::iterator Iterator;
    typedef decltype(*Iterator()) ValType;

    static_assert(Adapter::is_valid,"Adapter is invalid.");

    Iterator _a;
    RecursiveIterator<Tail...> _t;

    template <class U,class... TailC>
    RecursiveIterator(U&& a, TailC&&... args) :
        _a(SA::begin(std::forward<U>(a))),
        _t(std::forward<TailC>(args)...) { }

    void print() {
        std::cout << *_a << std::endl;
        _t.print();
    }

    void print_enum() {
        std::cout << num << std::endl;
        _t.print_enum();
    }

    template <int i = 0, class T>
    void setTuple(T& c) const {
        std::get<i>(c) = *_a;
        _t.setTuple<i + 1>(c);
    }

    template <int i = 0, class T>
    void print_tuple(T& c) const {
        std::cout << std::get<i>(c) << std::endl;
        _t.print_tuple<i + 1>(c);
    }

    void inc() {
        ++_a;
        _t.inc();
    }

    template <class F>
    auto callFunction(F&& f) -> decltype(
        _t.callFunction(std::forward<F>(f), std::forward<ValType>(*_a)))
    {
        return _t.callFunction(std::forward<F>(f), std::forward<ValType>(*_a));
    }

    template <class F, class... Args>
    auto callFunction(F&& f, Args&&... args)
        -> decltype(_t.callFunction(
        std::forward<F>(f), std::forward<Args>(args)..., std::forward<ValType>(*_a)))
    {
        return _t.callFunction(
                std::forward<F>(f),
                std::forward<Args>(args)...,
                std::forward<ValType>(*_a));
    }

};

}
}

#endif /* end of include guard: RECURSIVEITER_4VX65VTM */
