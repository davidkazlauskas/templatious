/*
 * =====================================================================================
 *
 *       Filename:  Functions.h
 *
 *    Description:  Function utilites
 *
 *        Version:  1.0
 *        Created:  05/25/2014 01:45:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
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

}
}

#endif /* end of include guard: FUNCTIONS_48QL99VC */
