/*
 * =====================================================================================
 *
 *       Filename:  IterMaker.h
 *
 *    Description:  Iterator maker
 *
 *        Version:  1.0
 *        Created:  05/25/2014 10:12:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ITERMAKER_W1WSUFYD
#define ITERMAKER_W1WSUFYD

#include <utility>

#include <templatious/recursive/RecursiveIter.hpp>
#include <templatious/recursive/QuadroIter.hpp>

namespace templatious {
namespace recursive {

struct IteratorMaker {
    template <class... Args>
    static auto makeIter(Args&&... args) -> RecursiveIterator<Args...>
    {
        return RecursiveIterator<Args...>(std::forward<Args>(args)...);
    }

    template <class... Args>
    static auto makeQuadro(Args&&... args)
        -> QuadroIterator<
            decltype(std::forward<Args>(args))...
        >
    {
        return QuadroIterator<
            decltype(std::forward<Args>(args))...
        >(std::forward<Args>(args)...);
    }
};

}
}

#endif /* end of include guard: ITERMAKER_W1WSUFYD */

