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
T& getFirst(T& t, Args&... args) {
    return t;
}

}
}

#endif /* end of include guard: VARIADIC_HXND7XSS */
