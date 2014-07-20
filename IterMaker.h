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

#include <templatious/tuple/RecursiveIter.h>
#include <templatious/TupleCaller.h>

namespace templatious {
namespace recursive {

struct IteratorMaker {
    template <class... Args>
    static auto makeIter(Args&&... args) -> RecursiveIterator<Args...>
    {
        return RecursiveIterator<Args...>(std::forward<Args>(args)...);
    }
};

}
}

#endif /* end of include guard: ITERMAKER_W1WSUFYD */

