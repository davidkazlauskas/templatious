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

#include <cstddef>
#include <utility>

namespace templatious {
namespace detail {

template <class StorType>
struct CallEachStreamFunctor {
    CallEachStreamFunctor(StorType& t) : _c(t) {}

    template <class T>
    void operator()(T&& i) {
        _c << std::forward<T>(i);
    }

    template <class T,class... Args>
    void operator()(T&& i,Args&&... args) {
        (*this)(std::forward<T>(i));
        (*this)(std::forward<Args>(args)...);
    }

    // we're modifying value so we know
    // we need non-const lvalue reference
    StorType& _c;
};

//------- Sum Functor
template <class StorType,bool countAlso = false>
struct SumFunctor {
    SumFunctor(StorType& s) : _c(s) {}

    template <class T>
    void operator()(T&& i) {
        _c += std::forward<T>(i);
        if (countAlso) {
            ++_cnt;
        }
    }

    StorType& _c;
    size_t _cnt;
};

template <class StorType,class FStorType,bool countAlso = false>
struct SumFunctorCustom {

    template <class V>
    SumFunctorCustom(StorType& s,V&& v) :
        _c(s), _fn(std::forward<V>(v)) {}

    template <class T>
    void operator()(T&& i) {
        _c += _fn(std::forward<T>(i));
        if (countAlso) {
            ++_cnt;
        }
    }

    StorType& _c;
    FStorType _fn;
    size_t _cnt;
};

struct DefaultComparator {
    template <class A,class B>
    bool operator()(A&& a,B&& b) const {
        return a == b;
    }
};

//------- DummyVar

struct DummyVar {

    template <class T>
    auto operator=(T&& rhs) const
     -> decltype(std::forward<T>(rhs))
    {
        return std::forward<T>(rhs);
    }
};

static DummyVar dummyVar = DummyVar();

}
}

#endif /* end of include guard: USERUTIL_2646KXF5 */
