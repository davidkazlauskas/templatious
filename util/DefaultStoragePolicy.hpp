//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  DefaultStoragePolicy.hpp
 *
 *    Description:  Default storage policy
 *
 *        Version:  1.0
 *        Created:  10/21/2014 07:13:14 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef DEFAULTSTORAGEPOLICY_D0BU0G4Y
#define DEFAULTSTORAGEPOLICY_D0BU0G4Y

#include <utility>

#include <templatious/util/Container.hpp>

namespace templatious {
namespace util {

// default storage policy:
// Ref when available
// Copy when no other way
// Copy when rvalue reference
template <class T>
struct DefaultStoragePolicy {
    typedef typename std::remove_const<T>::type TNoConst;
    typedef typename std::decay<T>::type TDecay;

    typedef typename std::conditional<
        std::is_lvalue_reference<T>::value,
        templatious::util::RefContainer<T>,
        typename std::conditional<
            std::is_function<T>::value,
            templatious::util::StaticPointerContainer<T>,
            typename std::conditional<
                std::is_rvalue_reference<T>::value,
                typename templatious::util::RvalueCopyContainer<
                    TDecay
                >,
                templatious::util::CopyContainer<
                    TNoConst
                >
            >::type
        >::type
    >::type Container;

    template <class U>
    static auto make(U&& u)
     -> Container
    {
        return Container(std::forward<U>(u));
    }

    struct CopyMaker {
        typedef typename std::remove_const<T>::type Result;

        template <class U>
        Result make(U&& u) {
            return Result(std::forward<U>(u));
        }
    };

    struct RefMaker {
        typedef typename std::add_lvalue_reference<T>::type Result;

        template <class U>
        Result make(U&& u) {
            return Result(std::forward<U>(u));
        }
    };
};

}
}


#endif /* end of include guard: DEFAULTSTORAGEPOLICY_D0BU0G4Y */
