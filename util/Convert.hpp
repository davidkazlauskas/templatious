//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Convert.hpp
 *
 *    Description:  Common conversions
 *
 *        Version:  1.0
 *        Created:  07/20/2014 11:23:46 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef CONVERT_V0QJ1P2G
#define CONVERT_V0QJ1P2G

namespace templatious {
namespace util {

    template <class T>
    struct ReferenceMaker {
        static T& ref(T& r) {
            return r;
        }
    };

    template <class T>
    struct ReferenceMaker<T*> {
        static T& ref(T* r) {
            return *r;
        }
    };

    template <class T>
    struct ReferenceMaker<T*&> {
        static T& ref(T*& r) {
            return *r;
        }
    };

    template <class T>
    struct ReferenceMaker<const T> {
        static const T& ref(const T& r) {
            return r;
        }
    };

    template <class T>
    struct ReferenceMaker<const T*> {
        static const T& ref(const T* r) {
            return *r;
        }
    };

    template <class T>
    struct ReferenceMaker<const T*&> {
        static const T& ref(const T*& r) {
            return *r;
        }
    };

}

template <class T>
auto ref(T&& r)
    -> decltype(templatious::util::
            ReferenceMaker<decltype(r)>::ref(r))
{
    typedef decltype(r) ValType;
    typedef typename templatious::util::ReferenceMaker<ValType> RM;
    return RM::ref(r);
}

}

#endif /* end of include guard: CONVERT_V0QJ1P2G */
