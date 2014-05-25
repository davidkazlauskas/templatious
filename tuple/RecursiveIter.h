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
typedef col::StaticAdapter SA;

template <class... Args>
struct RecursiveIterator;

template <class A>
struct RecursiveIterator<A> {
    enum { num = 1 };

    typedef col::CollectionAdapter<A> Adapter;
    typedef typename Adapter::iterator Iterator;

    Iterator _a;

    RecursiveIterator(A& a) : _a(SA::begin(a)) {
        //std::cout << "rec iter - " << a << std::endl;
    }

    void print() {
        std::cout << *_a << std::endl;
    }

    void print_enum() { std::cout << num << std::endl; }

    void inc() { ++_a; }

    template <unsigned int i = 0, class T>
    void setTuple(T& c) const {
        std::get<i>(c) = *_a;
    }

};

template <class A, class... Tail>
struct RecursiveIterator<A, Tail...> {
    enum { num = RecursiveIterator<Tail...>::num + 1 };


    typedef col::CollectionAdapter<A> Adapter;
    typedef typename Adapter::iterator Iterator;

    Iterator _a;
    RecursiveIterator<Tail...> _t;

    RecursiveIterator(A& a, Tail & ... args) : _a(SA::begin(a)), _t(args...) { }

    void print() {
        std::cout << *_a << std::endl;
        _t.print();
    }

    void print_enum() {
        std::cout << num << std::endl;
        _t.print_enum();
    }

    template <unsigned int i = 0, class T>
    void setTuple(T& c) const {
        std::get<i>(c) = _a;
        _t.setTuple<i + 1>(c);
    }

    template <unsigned int i = 0, class T>
    void print_tuple(T& c) const {
        std::cout << std::get<i>(c) << std::endl;
        _t.print_tuple<i + 1>(c);
    }

    void inc() {
        ++_a;
        _t.inc();
    }

};
}
};

#endif /* end of include guard: RECURSIVEITER_4VX65VTM */
