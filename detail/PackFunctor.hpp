//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  PackFunctor.hpp
 *
 *    Description:  Functor which gets arguments from pack
 *
 *        Version:  1.0
 *        Created:  11/04/2014 05:27:58 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef PACKFUNCTOR_TB2YBYOH
#define PACKFUNCTOR_TB2YBYOH

#include <utility>

namespace templatious {
namespace detail {

// we make an assumption that U class
// has Pack call function, even though,
// we don't use Pack header here.

template <int i,int end>
struct CallPack {

    struct CallTerminate {
        template <class F,class P,class... Args>
        static auto call(F&& f,P&& p,Args&&... args)
         -> decltype(
             f(std::forward<Args>(args)...)
         )
        {
            return f(std::forward<Args>(args)...);
        }
    };

    struct CallAdvance {
        template <class F,class P,class... Args>
        static auto call(F&& f,P&& p)
         -> decltype(
             CallPack<i + 1,end>::call(
                 std::forward<F>(f),
                 std::forward<P>(p),
                 p.template get<i>()
             )
         )
        {
            return CallPack<i + 1,end>::call(
                std::forward<F>(f),
                std::forward<P>(p),
                p.template get<i>()
            );
        }

        template <class F,class P,class... Args>
        static auto call(F&& f,P&& p,Args&&... args)
         -> decltype(
             CallPack<i + 1,end>::call(
                 std::forward<F>(f),
                 std::forward<P>(p),
                 std::forward<Args>(args)...,
                 p.template get<i>()
             )
         )
        {
            return CallPack<i + 1,end>::call(
                std::forward<F>(f),
                std::forward<P>(p),
                std::forward<Args>(args)...,
                p.template get<i>()
            );
        }
    };

    typedef typename std::conditional<
        i < end,
        CallAdvance,
        CallTerminate
    >::type Decision;

    template <class F,class P,class... Args>
    static auto call(F&& f,P&& p)
     -> decltype(
         Decision::call(
             std::forward<F>(f),
             std::forward<P>(p)
         )
     )
    {
        return Decision::call(
            std::forward<F>(f),
            std::forward<P>(p)
        );
    }

    template <class F,class P,class... Args>
    static auto call(F&& f,P&& p,Args&&... args)
     -> decltype(
         Decision::call(
             std::forward<F>(f),
             std::forward<P>(p),
             std::forward<Args>(args)...
         )
     )
    {
        return Decision::call(
            std::forward<F>(f),
            std::forward<P>(p),
            std::forward<Args>(args)...
        );
    }
};

template <
    template <class> class StoragePolicy,
    class T,class U
>
struct PackFunctor {
    typedef typename StoragePolicy<T>::Container ContF;
    typedef typename StoragePolicy<U>::Container ContP;

    static const int pack_size = U::size;
    static_assert(pack_size > 0,
        "PackFunctor pack has to have"
        " at least one element.");

    template <class TV,class UV>
    PackFunctor(TV&& t,UV&& u) :
        _cf(std::forward<TV>(t)),
        _cp(std::forward<UV>(u)) {}

    auto operator()()
     -> decltype(
         CallPack<0,pack_size>
            ::call(
                std::declval<ContF>().getRef(),
                std::declval<ContP>().getRef()
            )
     )
    {
        return CallPack<0,pack_size>
            ::call(
                _cf.getRef(),
                _cp.getRef()
            );
    }

private:
    ContF _cf;
    ContP _cp;
};

}
}

#endif /* end of include guard: PACKFUNCTOR_TB2YBYOH */

