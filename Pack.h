/*
 * =====================================================================================
 *
 *       Filename:  Pack.h
 *
 *    Description:  Pack for packing references to variables
 *
 *        Version:  1.0
 *        Created:  09/13/2014 08:42:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef PACK_APF8NI0N
#define PACK_APF8NI0N

#include <utility>
#include <templatious/util/Container.h>

namespace templatious {

template <class... Args>
struct Pack;

template <class A,class... Tail>
struct Pack<A,Tail...> {

    template <class ARef,class... TailRef>
    Pack(ARef&& r,TailRef&&... t)
    : _r(std::forward<ARef>(r)),
      _t(std::forward<TailRef>(t)...) {}

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        _t.call(std::forward<F>(f),std::forward<Args>(args)...,_r.getRef());
    }

    template <class F>
    void call(F&& f) {
        _t.call(std::forward<F>(f),_r.getRef());
    }

private:
    templatious::util::RefContainer<A> _r;
    Pack<Tail...> _t;
};

template <class A>
struct Pack<A> {

    template <class ARef>
    Pack(ARef&& r)
    : _r(std::forward<ARef>(r)) {}

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        f(std::forward<Args>(args)...,_r.getRef());
    }

private:
    templatious::util::RefContainer<A> _r;
};

template <class T>
struct IsPack {
    static const bool val = false;
};

template <class... T>
struct IsPack< Pack<T...> > {
    static const bool val = true;
};

template <class... T>
struct IsPack< Pack<T...>& > {
    static const bool val = true;
};

template <class... T>
struct IsPack< const Pack<T...> > {
    static const bool val = true;
};

template <class... T>
struct IsPack< const Pack<T...>& > {
    static const bool val = true;
};

}

#endif /* end of include guard: PACK_APF8NI0N */
