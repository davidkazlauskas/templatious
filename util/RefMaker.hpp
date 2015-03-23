//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  RefMaker.hpp
 *
 *    Description:  Make const or simple reference according to type
 *
 *        Version:  1.0
 *        Created:  08/10/2014 08:51:23 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef REFMAKER_1FCODAYU
#define REFMAKER_1FCODAYU

namespace templatious {
namespace util {

template <class T>
struct RefMaker;

template <class T>
struct RefMaker {
    typedef T& val;
};

template <class T>
struct RefMaker<const T> {
    typedef const T& val;
};

template <class T>
struct RefMaker<T&&> {
    typedef const T& val;
};

}
}

#endif /* end of include guard: REFMAKER_1FCODAYU */
