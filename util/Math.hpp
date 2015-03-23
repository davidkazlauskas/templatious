//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Math.hpp
 *
 *    Description:  Math functions
 *
 *        Version:  1.0
 *        Created:  07/03/2014 05:37:18 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef MATH_X5C027N1
#define MATH_X5C027N1

namespace templatious {
namespace util {

template <class T>
T makeNeg(const T& t) {
    if (t > 0) {
        return -t;
    }

    return t;
}

template <class T>
T makePos(const T& t) {
    if (t < 0) {
        return -t;
    }

    return t;
}

}
}

#endif /* end of include guard: MATH_X5C027N1 */
