/*
 * =====================================================================================
 *
 *       Filename:  Pointerizer.h
 *
 *    Description:  Make pointers out of references/value types
 *
 *        Version:  1.0
 *        Created:  05/25/2014 11:05:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef POINTERIZER_LRA0D1QI
#define POINTERIZER_LRA0D1QI

#include <type_traits>
#include <tuple>

template <bool isPointer,class A>
struct MakePtr;

template <class A>
struct MakePtr<true,A> {
    typedef A PtrType;

    PtrType _val;

    MakePtr(A val) : _val(val) {}
};

template <class A>
struct MakePtr<false,A> {
    typedef A* PtrType;

    PtrType _val;

    MakePtr(A val) : _val(&val) {}
};

template <class ...Args>
struct Pointerizer;

template <int i,class T>
struct ValGetter {
    auto get(T& t)
        -> decltype(ValGetter<i-1, decltype(t._next)>().get(t._next)) 
    {
        ValGetter<i-1, decltype(t._next)> next(t._next);
        return next.get(t._next);
    }
};

template <class T>
struct ValGetter<0,T> {

    auto get(T& t) -> decltype(t._val) {
        return t._val;
    }

};


template <class A, class ...Args>
struct Pointerizer<A, Args...> {
    typedef MakePtr<std::is_pointer<A>::value,A> Converter;
    typedef typename Converter::PtrType ThisPtr;
    typedef std::tuple<ThisPtr> ThisTuple;
    //typedef decltype(Pointerizer<A, Args...>().getTuple()) FullTuple;
    typedef Pointerizer<A, Args...> ThisType;

    ThisPtr _val;
    Pointerizer<Args...> _next;

    auto getTuple() -> decltype( std::tuple_cat(ThisTuple(_val),_next.getTuple()) ) {
        return std::tuple_cat(ThisTuple(_val),_next.getTuple());
    }

    template <int i>
    auto get() -> decltype(ValGetter<i,ThisType>().template get(*this)) {
        return ValGetter<i,ThisType>().template get(*this);
    }

    Pointerizer(A& a,Args & ... args) : _val(Converter<A>(a)._val), _next(args...) {}

};

template <class A>
struct Pointerizer<A> {
    typedef typename MakePtr<std::is_pointer<A>::value,A>::PtrType ThisPtr;
    typedef std::tuple<ThisPtr> ThisTuple;
    //typedef ThisTuple FullTuple;

    ThisPtr _val;

    ThisTuple getTuple() {
        return ThisTuple(_val);
    }

    template <int i>
    ThisPtr get() {
        return _val;
    }

    Pointerizer(A& a) : _val(ThisPtr()._val) {}
};

#endif /* end of include guard: POINTERIZER_LRA0D1QI */
