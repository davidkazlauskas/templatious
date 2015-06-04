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

#include <climits>
#include <cstddef>
#include <utility>
#include <typeindex>

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

// no warning about shift overflow
template <int i>
struct RightShiftSide {
    static size_t leftShift(size_t hash) {
        return (hash << (sizeof(hash)*CHAR_BIT - i));
    }
};

template <>
struct RightShiftSide<0> {
    static size_t leftShift(size_t) {
        return 0;
    }
};

template <int i,class Type>
size_t hashType() {
    size_t toShift = std::type_index(typeid(Type)).hash_code();
    typedef RightShiftSide<i> Shifter;
    return (toShift >> i) | Shifter::leftShift(toShift);
}

template <int i,class... Args>
struct HashCounter;

template <int i,class A,class... Tail>
struct HashCounter<i,A,Tail...> {
    typedef HashCounter<i+1,Tail...> TailCounter;

    static size_t hash() {
        return hashType<i,A>() ^ TailCounter::hash();
    }
};

template <int i,class A>
struct HashCounter<i,A> {
    static size_t hash() {
        return hashType<i,A>();
    }
};

template <class... Args>
size_t hashTypes() {
    return HashCounter<0,Args...>::hash();
}

}
}

#endif /* end of include guard: VARIADIC_HXND7XSS */
