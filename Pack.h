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

template <class... T>
auto packUp(T&&... t)
  -> Pack< typename detail::IsPack<T>::ConstDropped... >
{
    return Pack< typename detail::IsPack<T>::ConstDropped... >(
            detail::IsPack<T>::forward(t)... );
}

}


template <class A,class... Tail>
struct Pack<A,Tail...> {

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
         -> decltype(i)
        {
            return i;
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
     -> decltype(
             std::declval<TailPack>().insert(
                 std::forward<T>(t),
                 std::declval<Container>().getRef()
             )
         )
    const {
         return _t.insert(std::forward<T>(t),_r.getRef());
    }

    template <class T,class... Args>
    auto insert(T&& t,Args&&... args)
     -> decltype(
             std::declval<TailPack>().insert(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 std::forward<T>(t),
                 std::declval<Container>().getRef()
             )
        )
    const {
         return _t.insert(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 std::forward<T>(t),
                 _r.getRef());
    }

    template <class T,class... Args>
    auto insertWithin(T&& t,Args&&... args)
     -> decltype( std::declval<TailPack>().insertWithin(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 InnerPasser::pass(
                     std::declval<Container>().getRef(),
                     std::forward<T>(t)
                     )) )
    const {
        typedef typename templatious::util::TypeSelector<
            IsP::val,
            InnerPackInsertReturn,
            ValReturn
        >::val Passer;

        return _t.insertWithin(
                std::forward<T>(t),
                std::forward<Args>(args)...,
                Passer::pass(_r.getRef(),
                    std::forward<T>(t))
                );
    }

    template <class T>
    auto insertWithin(T&& t)
     -> decltype( std::declval<TailPack>().insertWithin(
                    std::forward<T>(t),
                    InnerPasser::pass(
                        std::declval<Container>().getRef(),
                        std::forward<T>(t)
                    )
                 ))
    const {
        typedef typename templatious::util::TypeSelector<
            IsP::val,
            InnerPackInsertReturn,
            ValReturn
        >::val Passer;

        return _t.insertWithin(
                std::forward<T>(t),
                Passer::pass(_r.getRef(),
                    std::forward<T>(t))
                );
    }

private:
    Container _r;
    Pack<Tail...> _t;
};

template <class A>
struct Pack<A> {

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
         -> decltype(std::forward<Ins>(i))
        {
            return std::forward<Ins>(i);
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

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        f(std::forward<Args>(args)...,_r.getRef());
    }

    template <class T>
    auto insert(T&& t)
     -> decltype( detail::packUp( std::declval<Container>().getRef() ) )
    const {
        return detail::packUp( _r.getRef() );
    }

    template <class T,class... Args>
    auto insert(T&& t,Args&&... args)
     -> decltype( detail::packUp(
                  std::forward<Args>(args)...,
                  std::forward<T>(t),
                  std::declval<Container>().getRef()
                 ))
    const {
        return detail::packUp(
                  std::forward<Args>(args)...,
                  std::forward<T>(t),
                  _r.getRef()
                );
    }

    template <class T,class... Args>
    auto insertWithin(T&& t,Args&&... args)
      -> decltype( detail::packUp(
                  std::forward<Args>(args)...,
                  InnerPasser::pass(
                      std::declval<Container>().getRef(),
                      std::forward<T>(t)
                  )
              ))
    const {
        return detail::packUp(
                std::forward<Args>(args)...,
                InnerPasser::pass(
                    _r.getRef(),
                    std::forward<T>(t)
                )
            );
    }

    template <class T>
    auto insertWithin(T&& t)
     -> decltype( detail::packUp(
                  InnerPasser::pass(
                      std::declval<Container>().getRef(),
                      std::forward<T>(t))
                  ))
    const {
        return detail::packUp(
                InnerPasser::pass(
                    _r.getRef(),
                    std::forward<T>(t)
                )
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
