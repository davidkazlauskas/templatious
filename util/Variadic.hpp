//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Variadic.hpp
 *
 *    Description:  Variadic utilities
 *
 *        Version:  1.0
 *        Created:  05/25/2014 03:11:34 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef VARIADIC_HXND7XSS
#define VARIADIC_HXND7XSS

#include <utility>

namespace templatious {
namespace util {

template <class T, class... Args>
T&& getFirst(T&& t, Args&&... args) {
    return t;
}

template <class... T>
struct GetFrist;

template <class T>
struct GetFrist<T> {
    typedef T type;
};

template <class T,class... Tail>
struct GetFrist<T,Tail...> {
    typedef T type;
};

template <int i>
struct GetNth;

template <int n,class... Args>
auto getNth(Args&&... args)
 -> decltype( GetNth<n>::get(
             std::forward<Args>(args)...) )
{
    static_assert(n >= 0,"Index cannot be negative.");
    static_assert(n < sizeof...(Args),
            "Asked element is out of bounds");
    return GetNth<n>::get(
            std::forward<Args>(args)...);
}

template <int i>
struct GetNth {
    template <class T,class... Args>
    static auto get(T&& first,Args&&... args)
     -> decltype( GetNth<i - 1>::get(
            std::forward<Args>(args)...))
    {
        return GetNth<i - 1>::get(
            std::forward<Args>(args)...);
    }
};

template <>
struct GetNth<0> {
    template <class T,class... Args>
    static auto get(T&& first,Args&&... args)
     -> decltype( std::forward<T>(first) )
    {
        return std::forward<T>(first);
    }
};

}
}

#endif /* end of include guard: VARIADIC_HXND7XSS */
