//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Functions.hpp
 *
 *    Description:  Function utilites
 *
 *        Version:  1.0
 *        Created:  05/25/2014 01:45:42 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef FUNCTIONS_48QL99VC
#define FUNCTIONS_48QL99VC

#include <functional>

namespace templatious {
namespace util {

template <class T, class... Args>
auto makeFunction(T (*t)(Args...)) -> decltype(std::function<T(Args...)>(t)) {
    return std::function<T(Args...)>(t);
}

struct DoNothingFunctor {
    template <class... T>
    void operator()(T&&... t) {}
};

template <class U>
struct ForwardFunctor {
    template <class T>
    U operator()(T&& t)
    {
        return std::forward<T>(t);
    }
};

template <>
struct ForwardFunctor<void> {
    template <class T>
    auto operator()(T&& t)
     -> decltype(std::forward<T>(t))
    {
        return std::forward<T>(t);
    }
};

}
}

#endif /* end of include guard: FUNCTIONS_48QL99VC */
