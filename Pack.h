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
#include <templatious/util/ArgumentDelimiter.h>

namespace templatious {

template <class... Args>
struct Pack;

namespace detail {

struct TransformDelimiter {};

template <class T>
struct IsPack {
    static const bool val = false;
    typedef T ConstDropped;

    enum { size = 1 };

    template <class V>
    static auto forward(V&& val)
     -> decltype(std::forward<V>(val))
    {
        return std::forward<V>(val);
    }
};

struct PackTransformInsertWithin {
    template <class... T,class U>
    static auto call(Pack<T...> p,U&& u)
     -> decltype(p.insert(std::forward<U>(u)))
    {
        return p.insert(std::forward<U>(u));
    }

    template <class U>
    static auto call(U&& u)
     -> decltype(std::forward<U>(u))
    {
        return std::forward<U>(u);
    }
};

struct PackAccess {

    template <class TrPol,class P,class... T>
    static auto packTransformWithin(P p,T&&... t)
     -> decltype(
             p.template transform<TrPol>(
                    std::forward<T>(t)...,
                    detail::TransformDelimiter()
             )
        )
    {
        return p.template transform<TrPol>(
                std::forward<T>(t)...,
                detail::TransformDelimiter());
    }

    template <class P,class T>
    static auto packInsert(P p,T&& t)
     -> decltype(p.template insert<T>(std::forward<T>(t)))
    {
        return p.template insert<T>(std::forward<T>(t));
    }

    template <class P,class T>
    static auto packInsertWithin(P&& p,T&& t)
     -> decltype( packTransformWithin<PackTransformInsertWithin>(
                 std::forward<P>(p),std::forward<T>(t)) )
    {
        return packTransformWithin<PackTransformInsertWithin>(
                std::forward<P>(p),std::forward<T>(t) );
    }

    template <class... T>
    static auto packUp(T&&... t)
      -> Pack< typename detail::IsPack<T>::ConstDropped... >
    {
        return Pack< typename detail::IsPack<T>::ConstDropped... >(
                detail::IsPack<T>::forward(t)... );
    }

};

struct Packer {
    template <class... Args>
    static auto call(Args&&... args)
     -> decltype( PackAccess::packUp(std::forward<Args>(args)...) )
    {
        return PackAccess::packUp(std::forward<Args>(args)...);
    }
};

}


template <class A,class... Tail>
struct Pack<A,Tail...> {
    friend struct PackAccess;

    typedef detail::IsPack<A> IsP;

    typedef typename templatious::util::TypeSelector<
            std::is_lvalue_reference<A>::value && !detail::IsPack<A>::val ,
            templatious::util::RefContainer<A>,
            typename templatious::util::TypeSelector<
                    IsP::val,
                    templatious::util::CopyContainer<typename IsP::ConstDropped>,
                    templatious::util::CopyContainer<A>
                >::val
        >::val Container;

    typedef Pack<A,Tail...> ThisPack;
    typedef Pack<Tail...> TailPack;

    enum { size = detail::IsPack<A>::size + Pack<Tail...>::size, flatSize = sizeof...(Tail) + 1 };

    struct InnerPackInsertReturn {
        template <class T,class Ins>
        static auto pass(T&& t,Ins&& i)
         -> decltype(t.insert(std::forward<Ins>(i)))
        {
            return t.insert(std::forward<Ins>(i));
        }
    };

    struct ValReturn {
        template <class T,class Ins>
        static auto pass(T&& t,Ins&& i)
         -> decltype(t)
        {
            return t;
        }
    };

    typedef typename templatious::util::TypeSelector<
        IsP::val,
        InnerPackInsertReturn,
        ValReturn
    >::val InnerPasser;

    template <class ARef,class... TailRef>
    explicit Pack(ARef&& r,TailRef&&... t)
    : _r(std::forward<ARef>(r)),
      _t(std::forward<TailRef>(t)...) {}

    Pack(const ThisPack& p) : _r(p._r), _t(p._t) {}

    struct ThisValGetter {
        template <int i>
        static auto get(ThisPack& p)
         -> decltype( std::declval<Container>().cpy() )
        {
            return p._r.cpy();
        }
    };

    struct TailValGetterFlat {
        template <int i>
        static auto get(ThisPack& p)
         -> decltype( std::declval<TailPack>().template flatGet<i - 1>() )
        {
            return p._t.template flatGet<i - 1>();
        }
    };

    struct ThisValGetterRec {
        template <int i,class P>
        static auto get(P& p)
         -> decltype( std::declval<typename P::Container>()
             .cpy().template get<i>() )
        {
            return p._r.cpy().template get<i>();
        }
    };

    struct TailValGetter {
        template <int i,class P>
        static auto get(P& p)
         -> decltype( std::declval<TailPack>()
                 .template get<i - IsP::size>() )
        {
            return p._t.template get<
                i - IsP::size>();
        }
    };

    template <int i>
    auto get()
     -> decltype(
         templatious::util::TypeSelector<!IsP::val && i == 0,
             ThisValGetter,
             typename templatious::util::TypeSelector<
                 i < IsP::size,
                 ThisValGetterRec,
                 TailValGetter
             >::val
         >::val::template get<i>( std::declval<ThisPack&>() )
             )
    {
        static_assert(i >= 0,"Pack access index cannot be less than zero.");

        typedef typename templatious::util::TypeSelector<!IsP::val && i == 0,
            ThisValGetter,
            typename templatious::util::TypeSelector<
                i < IsP::size,
                ThisValGetterRec,
                TailValGetter
            >::val
        >::val Getter;
        return Getter::template get<i>(*this);
    }

    template <int i>
    auto flatGet()
     -> decltype( templatious::util::TypeSelector< i == 0,
        ThisValGetter,
        TailValGetterFlat>::val::template get<i>( std::declval<ThisPack&>() ) )
    {
        static_assert(i >= 0,"Pack access index cannot be less than zero.");
        typedef typename templatious::util::TypeSelector<
            i == 0,
            ThisValGetter,
            TailValGetterFlat>::val Getter;
        return Getter::template get<i>(*this);
    }

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        _t.call(std::forward<F>(f),
                std::forward<Args>(args)...,
                _r.cpy());
    }

    template <class F>
    void call(F&& f) {
        _t.call(std::forward<F>(f),_r.cpy());
    }

    template <class T>
    auto insert(T&& t)
     -> decltype(
             std::declval<TailPack>().insert(
                 std::forward<T>(t),
                 std::declval<Container>().cpy()
             )
         )
    const {
         return _t.insert(std::forward<T>(t),_r.cpy());
    }

    template <class T,class... Args>
    auto insert(T&& t,Args&&... args)
     -> decltype(
             std::declval<TailPack>().insert(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 std::forward<T>(t),
                 std::declval<Container>().cpy()
             )
        )
    const {
         return _t.insert(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 std::forward<T>(t),
                 _r.cpy());
    }

    template <class Tr,class... Args>
    auto transform(Args&&... args)
     -> decltype(
         std::declval< Pack<Tail...> >().template transform<Tr>(
             std::forward<Args>(args)...,
             templatious::util::callGroup<
                detail::TransformDelimiter,
                Tr,
                0
             >( std::declval<Container>().cpy(),
                std::forward<Args>(args)... )
         )
     )
    {
        namespace u = templatious::util;
        return _t.template transform<Tr>(
            std::forward<Args>(args)...,
            u::callGroup<
                detail::TransformDelimiter,
                Tr,
                0
            >( _r.cpy(),
               std::forward<Args>(args)... )
        );
    }

private:
    Container _r;
    Pack<Tail...> _t;
};

template <class A>
struct Pack<A> {
    friend struct PackAccess;

    typedef detail::IsPack<A> IsP;

    typedef typename templatious::util::TypeSelector<
            std::is_lvalue_reference<A>::value && !detail::IsPack<A>::val ,
            templatious::util::RefContainer<A>,
            typename templatious::util::TypeSelector<
                    IsP::val,
                    templatious::util::CopyContainer<typename IsP::ConstDropped>,
                    templatious::util::CopyContainer<A>
                >::val
        >::val Container;

    typedef Pack<A> ThisPack;

    enum { size = detail::IsPack<A>::size, flatSize = 1 };

    struct InnerPackInsertReturn {
        template <class T,class Ins>
        static auto pass(T&& t,Ins&& i)
         -> decltype(t.insert(std::forward<Ins>(i)))
        {
            return t.insert(std::forward<Ins>(i));
        }
    };

    struct ValReturn {
        template <class T,class Ins>
        static auto pass(T&& t,Ins&& i)
         -> decltype(std::forward<T>(t))
        {
            return std::forward<T>(t);
        }
    };

    typedef typename templatious::util::TypeSelector<
        IsP::val,
        InnerPackInsertReturn,
        ValReturn
    >::val InnerPasser;

    template <class ARef>
    explicit Pack(ARef&& r)
    : _r(std::forward<ARef>(r)) {}

    Pack(const ThisPack& p) : _r(p._r) {}

    struct ThisValGetter {
        template <int i>
        static auto get(ThisPack& p)
         -> decltype( std::declval<Container>().cpy() )
        {
            return p._r.cpy();
        }
    };

    struct ThisValGetterRec {
        template <int i,class P>
        static auto get(P& p)
         -> decltype( std::declval<P>()
             ._r.cpy().template get<i>() )
        {
            return p._r.cpy().template get<i>();
        }
    };

    template <int i>
    auto get()
     -> decltype(
         templatious::util::TypeSelector<IsP::val,
             ThisValGetterRec,ThisValGetter
         >::val::template get<i>( std::declval<ThisPack&>() )
         )
    {
        static_assert(i >= 0,"Pack access index cannot be less than zero.");
        static_assert(i < IsP::size,"Trying get element past size of a Pack.");

        typedef typename templatious::util::TypeSelector<IsP::val,
            ThisValGetterRec,ThisValGetter>::val Getter;
        return Getter::template get<i>(*this);
    }

    template <int i>
    auto flatGet()
     -> decltype( ThisValGetter::template get<i>( std::declval<ThisPack&>() ) )
    {
        static_assert(i >= 0,"Pack access index cannot be less than zero.");
        static_assert(i == 0,"Trying get element past flat size of a Pack.");
        typedef ThisValGetter Getter;
        return Getter::template get<i>(*this);
    }

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        f(std::forward<Args>(args)...,_r.cpy());
    }

    template <class T>
    auto insert(T&& t)
     -> decltype( detail::PackAccess::packUp(
                 std::declval<Container>().cpy() ) )
    const {
        return detail::PackAccess::packUp( _r.cpy() );
    }

    template <class T,class... Args>
    auto insert(T&& t,Args&&... args)
     -> decltype( detail::PackAccess::packUp(
                  std::forward<Args>(args)...,
                  std::forward<T>(t),
                  std::declval<Container>().cpy()
                 ))
    const {
        return detail::PackAccess::packUp(
                  std::forward<Args>(args)...,
                  std::forward<T>(t),
                  _r.cpy()
                );
    }

    template <class Tr,class... Args>
    auto transform(Args&&... args)
     -> decltype(
         templatious::util::callGroup<
            detail::TransformDelimiter,
            detail::Packer,
            1
         >(
             std::forward<Args>(args)...,
             templatious::util::callGroup<
                detail::TransformDelimiter,
                Tr,
                0
             >( std::declval<Container>().cpy(),
                std::forward<Args>(args)... )
         )
     )
    {
        namespace u = templatious::util;
        return u::callGroup<
            detail::TransformDelimiter,
            detail::Packer,
            1
        >(
            std::forward<Args>(args)...,
            u::callGroup<
                detail::TransformDelimiter,
                Tr,
                0
            >( _r.cpy(),
               std::forward<Args>(args)... )
        );
    }

private:
    Container _r;
};


namespace detail {

template <class... T>
struct IsPack< Pack<T...> > {
    static const bool val = true;
    typedef Pack<T...> ConstDropped;

    enum { size = Pack<T...>::size };

    template <class V>
    static auto forward(const V& val)
     -> Pack<T...>
    {
        return Pack<T...>(val);
    }
};

template <class... T>
struct IsPack< Pack<T...>& > {
    static const bool val = true;
    typedef Pack<T...> ConstDropped;

    enum { size = Pack<T...>::size };

    template <class V>
    static auto forward(const V& val)
     -> Pack<T...>
    {
        return Pack<T...>(val);
    }
};

template <class... T>
struct IsPack< const Pack<T...> > {
    static const bool val = true;
    typedef Pack<T...> ConstDropped;

    enum { size = Pack<T...>::size };

    template <class V>
    static auto forward(const V& val)
     -> Pack<T...>
    {
        return Pack<T...>(val);
    }
};

template <class... T>
struct IsPack< const Pack<T...>& > {
    static const bool val = true;
    typedef Pack<T...> ConstDropped;

    enum { size = Pack<T...>::size };

    template <class V>
    static auto forward(const V& val)
     -> Pack<T...>
    {
        return Pack<T...>(val);
    }
};

}

}

#endif /* end of include guard: PACK_APF8NI0N */
