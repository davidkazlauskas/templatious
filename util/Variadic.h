/*
 * =====================================================================================
 *
 *       Filename:  Variadic.h
 *
 *    Description:  Variadic utilities
 *
 *        Version:  1.0
 *        Created:  05/25/2014 03:11:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef VARIADIC_HXND7XSS
#define VARIADIC_HXND7XSS

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

}
}

#endif /* end of include guard: VARIADIC_HXND7XSS */
