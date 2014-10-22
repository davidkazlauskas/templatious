/*
 * =====================================================================================
 *
 *       Filename:  DefaultStoragePolicy.h
 *
 *    Description:  Default storage policy
 *
 *        Version:  1.0
 *        Created:  10/21/2014 07:13:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef DEFAULTSTORAGEPOLICY_D0BU0G4Y
#define DEFAULTSTORAGEPOLICY_D0BU0G4Y

#include <utility>

#include <templatious/util/Container.h>

namespace templatious {
namespace util {

// default storage policy:
// Ref when available
// Copy when no other way
template <class T>
struct DefaultStoragePolicy {
    typedef typename std::conditional<
            std::is_lvalue_reference<T>::value,
            templatious::util::RefContainer<T>,
            typename std::conditional<
                std::is_function<T>::value,
                templatious::util::StaticPointerContainer<T>,
                templatious::util::CopyContainer<
                    typename std::remove_const<T>::type
                >
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
