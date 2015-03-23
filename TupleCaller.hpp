//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  TupleCaller.hpp
 *
 *    Description:  Call tuples automatically
 *
 *        Version:  1.0
 *        Created:  05/25/2014 07:45:51 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef TUPLECALLER_UNZX3QCI
#define TUPLECALLER_UNZX3QCI

#include <templatious/recursive/TupleMacros.hpp>

namespace templatious {
namespace tuple {

template <class T, class... Args>
auto callTuple(T& f, std::tuple<Args...>& t)
    -> decltype(tuple_caller<sizeof...(Args), T, Args...>().call(f, t)) 
{
    tuple_caller<sizeof...(Args), T, Args...> tc;
    return tc.call(f, t);
}

}
}

#endif /* end of include guard: TUPLECALLER_UNZX3QCI */
