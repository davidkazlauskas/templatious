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
#include <templatious/util/Selectors.h>
#include <templatious/util/Container.h>

namespace templatious {

template <class... Args>
struct Pack;

template <class T>
struct IsPack {
    static const bool val = false;
    typedef void ConstDropped;
};


template <class A,class... Tail>
struct Pack<A,Tail...> {

    typedef IsPack<A> IsP;

    typedef typename templatious::util::TypeSelector<
            std::is_lvalue_reference<A>::value && !IsPack<A>::val ,
            templatious::util::RefContainer<A>,
            typename templatious::util::TypeSelector<
                    IsP::val,
                    templatious::util::CopyContainer<typename IsP::ConstDropped>,
                    templatious::util::CopyContainer<A>
                >::val
        >::val Container;

    typedef Pack<A,Tail...> ThisPack;
    typedef Pack<Tail...> TailPack;

    template <class Ins,class... Args>
    struct InsertType {
        typedef typename TailPack::template InsertType<
            Ins,Args...,Ins,A >::value value;
    };

    template <class Ins>
    struct FirstInsertType {
        typedef typename TailPack::template InsertType<
            Ins,A>::value value;
    };

    template <class ARef,class... TailRef>
    explicit Pack(ARef&& r,TailRef&&... t)
    : _r(std::forward<ARef>(r)),
      _t(std::forward<TailRef>(t)...) {}

    Pack(const ThisPack& p) : _r(p._r), _t(p._t) {}

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        _t.call(std::forward<F>(f),
                std::forward<Args>(args)...,
                _r.getRef());
    }

    template <class F>
    void call(F&& f) {
        _t.call(std::forward<F>(f),_r.getRef());
    }

    template <class T>
    auto insert(T&& t)
     -> typename FirstInsertType<T>::value
    const {
         return _t.insert(std::forward<T>(t),_r.cpy());
    }

    template <class T,class... Args>
    auto insert(T&& t,Args&&... args)
     -> typename InsertType<T,Args...>::value
    const {
         return _t.insert(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 std::forward<T>(t),
                 _r.cpy());
    }

private:
    Container _r;
    Pack<Tail...> _t;
};

template <class A>
struct Pack<A> {

    typedef IsPack<A> IsP;

    typedef typename templatious::util::TypeSelector<
            std::is_lvalue_reference<A>::value && !IsPack<A>::val ,
            templatious::util::RefContainer<A>,
            typename templatious::util::TypeSelector<
                    IsP::val,
                    templatious::util::CopyContainer<typename IsP::ConstDropped>,
                    templatious::util::CopyContainer<A>
                >::val
        >::val Container;

    typedef Pack<A> ThisPack;

    template <class Ins,class... Args>
    struct InsertType {
        typedef Pack<Args...,Ins,A> value;
    };

    template <class Ins>
    struct FirstInsertType {
        typedef ThisPack value;
    };

    template <class ARef>
    explicit Pack(ARef&& r)
    : _r(std::forward<ARef>(r)) {}

    Pack(const ThisPack& p) : _r(p._r) {}

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        f(std::forward<Args>(args)...,_r.getRef());
    }

    template <class T>
    ThisPack insert(T&& t) const {
        return ThisPack(_r.cpy());
    }

    template <class T,class... Args>
    auto insert(T&& t,Args&&... args)
     -> typename InsertType<T,Args...>::value
    const {
        typedef typename InsertType<T,Args...>::value RetType;
        return RetType(std::forward<Args>(args)...,
                std::forward<T>(t),_r.cpy());
    }

private:
    Container _r;
};

template <class... T>
struct IsPack< Pack<T...> > {
    static const bool val = true;
    typedef Pack<T...> ConstDropped;
};

template <class... T>
struct IsPack< Pack<T...>& > {
    static const bool val = true;
    typedef Pack<T...> ConstDropped;
};

template <class... T>
struct IsPack< const Pack<T...> > {
    static const bool val = true;
    typedef Pack<T...> ConstDropped;
};

template <class... T>
struct IsPack< const Pack<T...>& > {
    static const bool val = true;
    typedef Pack<T...> ConstDropped;
};

}

#endif /* end of include guard: PACK_APF8NI0N */
