//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  CallCountFunctor.hpp
 *
 *    Description:  Functor which calls another functor and counts.
 *
 *        Version:  1.0
 *        Created:  10/25/2014 09:48:51 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef CALLCOUNTFUNCTOR_E7XWQK67
#define CALLCOUNTFUNCTOR_E7XWQK67

#include <cstddef> // size_t
#include <utility> // std::forward

#include <templatious/util/DefaultStoragePolicy.hpp>

namespace templatious {
namespace util {

template <class T>
struct CallCountFunctor {
    typedef typename DefaultStoragePolicy<T>::Container Cont;

    template <class V>
    CallCountFunctor(V&& v):
        _c(std::forward<V>(v)), _count(0) {}

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(std::declval<Cont>()
             .getRef()(std::forward<Args>(args)...))
    const
    {
        ++_count;
        return _c.getRef()(
            std::forward<Args>(args)...);
    }

    long getCount() const { return _count; }

private:
    Cont _c;
    long _count;
};

}
}

#endif /* end of include guard: CALLCOUNTFUNCTOR_E7XWQK67 */
