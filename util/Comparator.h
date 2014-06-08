/*
 * =====================================================================================
 *
 *       Filename:  Comparator.h
 *
 *    Description:  Item comparator
 *
 *        Version:  1.0
 *        Created:  06/08/2014 11:01:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef COMPARATOR_CIFG4Q23
#define COMPARATOR_CIFG4Q23

namespace templatious {
namespace util {

template <class T,class U>
struct ComparatorEq {

    // default equality test
    static bool eq(const T& t,const U& u) {
        return t == u;
    }

};

template <class T,class U>
struct ComparatorLm {

    // default comparator
    static auto lm(const T& t,const U& u) -> decltype(t - u) {
        return t - u;
    }

};


}
}

#endif /* end of include guard: COMPARATOR_CIFG4Q23 */
