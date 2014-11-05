/*
 * =====================================================================================
 *
 *       Filename:  UserUtil.h
 *
 *    Description:  Common user structures/classes/utilities
 *
 *        Version:  1.0
 *        Created:  10/13/2014 08:08:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef USERUTIL_2646KXF5
#define USERUTIL_2646KXF5

#include <utility>

namespace templatious {
namespace detail {

template <class StorType>
struct CallEachStreamFunctor {
    CallEachStreamFunctor(const StorType& t) : _c(t) {}
    CallEachStreamFunctor() {}

    template <class T>
    void operator()(T&& i) {
        _c << i;
    }

    template <class T,class... Args>
    void operator()(T&& i,Args&&... args) {
        (*this)(std::forward<T>(i));
        (*this)(std::forward<Args>(args)...);
    }

    StorType _c;
};

}
}

#endif /* end of include guard: USERUTIL_2646KXF5 */
