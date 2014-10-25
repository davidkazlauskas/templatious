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
#include <templatious/util/ArgumentMultiplier.h>

namespace templatious {

namespace detail {

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

}

// default storage policy
// takes by reference when available
// and by copy when unavailable
template <class T>
struct DefaultPackStoragePolicy {
    typedef detail::IsPack<T> IsP;
    typedef typename std::conditional<
            std::is_lvalue_reference<T>::value && !detail::IsPack<T>::val ,
            templatious::util::RefContainer<T>,
            typename std::conditional<
                    IsP::val,
                    templatious::util::CopyContainer<typename IsP::ConstDropped>,
                    templatious::util::CopyContainer<T>
                >::type
        >::type Container;
};

template <
    template <class> class StoragePolicy = DefaultPackStoragePolicy,
    class... Args>
struct Pack;

namespace detail {

struct TransformDelimiter {};

template <
    template <class> class NewStoragePolicy = DefaultPackStoragePolicy
>
struct PackTransformInsertWithin {
    template <
        template <class> class StoragePolicy,
        class... T,class U
    >
    static auto call(Pack<StoragePolicy,T...> p,U&& u)
     -> decltype(p.template insert<NewStoragePolicy>(
                 std::forward<U>(u)))
    {
        return p.template insert<NewStoragePolicy>(
                std::forward<U>(u));
    }

    template <class U>
    static auto call(U&& u)
     -> decltype(std::forward<U>(u))
    {
        return std::forward<U>(u);
    }
};

struct Packer;

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

    template <
        template <class> class StoragePolicy = DefaultPackStoragePolicy,
        class P,class T
    >
    static auto packInsert(P p,T&& t)
     -> decltype(p.template insert<StoragePolicy,T>(std::forward<T>(t)))
    {
        return p.template insert<StoragePolicy,T>(std::forward<T>(t));
    }

    template <
        template <class> class StoragePolicy = DefaultPackStoragePolicy,
        class P,class T
    >
    static auto packInsertWithin(P&& p,T&& t)
     -> decltype( packTransformWithin< PackTransformInsertWithin<StoragePolicy> >(
                 std::forward<P>(p),std::forward<T>(t)) )
    {
        return packTransformWithin< PackTransformInsertWithin<StoragePolicy> >(
                std::forward<P>(p),std::forward<T>(t) );
    }

    template <
        template <class> class StoragePolicy = DefaultPackStoragePolicy,
        class... T
    >
    static auto packUp(T&&... t)
      -> Pack< StoragePolicy, typename detail::IsPack<T>::ConstDropped... >
    {
        return Pack< StoragePolicy, typename detail::IsPack<T>::ConstDropped... >(
                detail::IsPack<T>::forward(t)... );
    }

    template <
        int n,
        class... Args
    >
    static auto packRepeat(Args&&... args)
     -> decltype(
         templatious::util::multiplyArgs<Packer,n>(
             std::forward<Args>(args)...
         )
     )
    {
        return templatious::util::multiplyArgs<Packer,n>(
                std::forward<Args>(args)...);
    }
};

struct Packer {
    template <
        template <class> class StoragePolicy = DefaultPackStoragePolicy,
        class... Args
    >
    static auto call(Args&&... args)
     -> decltype( PackAccess::packUp<StoragePolicy>(
                 std::forward<Args>(args)...) )
    {
        return PackAccess::packUp<StoragePolicy>(
                std::forward<Args>(args)...);
    }
};

}


template <
    template <class> class StoragePolicy,
    class A,
    class... Tail
>
struct Pack<StoragePolicy,A,Tail...> {
    friend struct PackAccess;

    typedef detail::IsPack<A> IsP;

    typedef typename StoragePolicy<A>::Container
        Container;

    typedef Pack<StoragePolicy,A,Tail...> ThisPack;
    typedef Pack<StoragePolicy,Tail...> TailPack;

    enum { size = detail::IsPack<A>::size + Pack<StoragePolicy,Tail...>::size, flatSize = sizeof...(Tail) + 1 };

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

    template <
        bool ignoreBooleanReturn = false,
        class F,class... Args
    >
    bool call(F&& f,Args&&... args) {
        return _t.template call<ignoreBooleanReturn>(
                std::forward<F>(f),
                std::forward<Args>(args)...,
                _r.cpy());
    }

    template <
        bool ignoreBooleanReturn = false,
        class F>
    bool call(F&& f) {
        return _t.template call<ignoreBooleanReturn>(
                std::forward<F>(f),
                _r.cpy());
    }

    template <
        template <class> class InsertStoragePolicy,
        class T
    >
    auto insert(T&& t)
     -> decltype(
             std::declval<TailPack>().template insert<InsertStoragePolicy>(
                 std::forward<T>(t),
                 std::declval<Container>().cpy()
             )
         )
    const {
         return _t.template insert<InsertStoragePolicy>(
                 std::forward<T>(t),_r.cpy());
    }

    template <
        template <class> class InsertStoragePolicy,
        class T,
        class... Args
    >
    auto insert(T&& t,Args&&... args)
     -> decltype(
             std::declval<TailPack>().template insert<InsertStoragePolicy>(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 std::forward<T>(t),
                 std::declval<Container>().cpy()
             )
        )
    const {
         return _t.template insert<InsertStoragePolicy>(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 std::forward<T>(t),
                 _r.cpy());
    }

    template <class Tr,class... Args>
    auto transform(Args&&... args)
     -> decltype(
         std::declval< Pack<StoragePolicy,Tail...> >().template transform<Tr>(
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
    Pack<StoragePolicy,Tail...> _t;
};

template <
    template <class> class StoragePolicy,
    class A
>
struct Pack<StoragePolicy,A> {
    friend struct PackAccess;

    typedef detail::IsPack<A> IsP;

    typedef typename StoragePolicy<A>::Container
        Container;

    typedef Pack<StoragePolicy,A> ThisPack;

    enum { size = detail::IsPack<A>::size, flatSize = 1 };


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

    template <
        bool ignoreBooleanReturn = false,
        class F,class... Args
    >
    bool call(F&& f,Args&&... args) {
        typedef templatious::util::RetValSelector<
            decltype(f(std::forward<Args>(args)...,_r.cpy())),
            true> Sel;

        bool res = Sel::callAndEval(
            f,
            std::forward<Args>(args)...,
            _r.cpy()
        );

        return ignoreBooleanReturn || res;
    }

    template <
        template <class> class InsertStoragePolicy,
        class T
    >
    auto insert(T&& t)
     -> decltype( detail::PackAccess::packUp<InsertStoragePolicy>(
                 std::declval<Container>().cpy() ) )
    const {
        return detail::PackAccess::packUp<InsertStoragePolicy>(
                _r.cpy());
    }

    template <
        template <class> class InsertStoragePolicy,
        class T,class... Args
    >
    auto insert(T&& t,Args&&... args)
     -> decltype( detail::PackAccess::packUp<InsertStoragePolicy>(
                  std::forward<Args>(args)...,
                  std::forward<T>(t),
                  std::declval<Container>().cpy()
                 ))
    const {
        return detail::PackAccess::packUp<InsertStoragePolicy>(
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

template <
    template <class> class StoragePolicy,
    class... T
>
struct IsPack< Pack<StoragePolicy,T...> > {
    static const bool val = true;
    typedef Pack<StoragePolicy,T...> ConstDropped;

    enum { size = Pack<StoragePolicy,T...>::size };

    template <class V>
    static auto forward(const V& val)
     -> Pack<StoragePolicy,T...>
    {
        return Pack<StoragePolicy,T...>(val);
    }
};

template <
    template <class> class StoragePolicy,
    class... T
>
struct IsPack< Pack<StoragePolicy,T...>& > {
    static const bool val = true;
    typedef Pack<StoragePolicy,T...> ConstDropped;

    enum { size = Pack<StoragePolicy,T...>::size };

    template <class V>
    static auto forward(const V& val)
     -> Pack<StoragePolicy,T...>
    {
        return Pack<StoragePolicy,T...>(val);
    }
};

template <
    template <class> class StoragePolicy,
    class... T
>
struct IsPack< const Pack<StoragePolicy,T...> > {
    static const bool val = true;
    typedef Pack<StoragePolicy,T...> ConstDropped;

    enum { size = Pack<StoragePolicy,T...>::size };

    template <class V>
    static auto forward(const V& val)
     -> Pack<StoragePolicy,T...>
    {
        return Pack<StoragePolicy,T...>(val);
    }
};

template <
    template <class> class StoragePolicy,
    class... T
>
struct IsPack< const Pack<StoragePolicy,T...>& > {
    static const bool val = true;
    typedef Pack<StoragePolicy,T...> ConstDropped;

    enum { size = Pack<StoragePolicy,T...>::size };

    template <class V>
    static auto forward(const V& val)
     -> Pack<StoragePolicy,T...>
    {
        return Pack<StoragePolicy,T...>(val);
    }
};

}

}

#endif /* end of include guard: PACK_APF8NI0N */
