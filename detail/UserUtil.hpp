//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  UserUtil.hpp
 *
 *    Description:  Common user structures/classes/utilities
 *
 *        Version:  1.0
 *        Created:  10/13/2014 08:08:42 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
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
    long _cnt;
};

//------- Forall functor.
template <
    class StorType,
    template <class> class StoragePolicy
>
struct ForallFunctor {

    template <class V>
    ForallFunctor(V&& v) :
        _st(std::forward<V>(v)), _state(true) {}

    template <class T>
    bool operator()(T&& i) {
        bool curr = _st.getRef()(std::forward<T>(i));
        _state &= curr;
        return curr;
    }

    typedef typename StoragePolicy<StorType>
        ::Container Storage;
    Storage _st;
    bool _state;
};

//------- Exists functor.
template <
    class StorType,
    template <class> class StoragePolicy
>
struct ExistsFunctor {

    template <class V>
    ExistsFunctor(V&& v) :
        _st(std::forward<V>(v)), _state(false) {}

    template <class T>
    bool operator()(T&& i) {
        bool curr = _st.getRef()(std::forward<T>(i));
        _state |= curr;
        return !curr;
    }

    typedef typename StoragePolicy<StorType>
        ::Container Storage;
    Storage _st;
    bool _state;
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
    long _cnt;
};

template <
    class StorType,class Comparator,
    template <class> class ComparatorStoragePolicy
>
struct CompFunctor {
    typedef typename ComparatorStoragePolicy<
        Comparator>::Container Cont;

    template <class V>
    CompFunctor(V&& v) :
        _comp(std::forward<V>(v)),
        _isCompared(false) {}

    template <class T>
    void operator()(T&& i) {
        if (_isCompared) {
            if (_comp.getRef()(
                std::forward<T>(i),_c.getRef()))
            {
                _c.assign(std::forward<T>(i));
            }
        } else {
            _c.assign(std::forward<T>(i));
            _isCompared = true;
        }
    }

    StorType _c;
    Cont _comp;
    bool _isCompared;
};

struct DefaultComparator {
    template <class A,class B>
    bool operator()(A&& a,B&& b) const {
        return a == b;
    }
};

struct DefaultLessComparator {
    template <class A,class B>
    bool operator()(A&& a,B&& b) const {
        return a < b;
    }
};

struct DefaultMoreComparator {
    template <class A,class B>
    bool operator()(A&& a,B&& b) const {
        return a > b;
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
