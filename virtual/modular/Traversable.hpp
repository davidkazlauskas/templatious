//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Traversable.hpp
 *
 *    Description:  Traversable trait
 *
 *        Version:  1.0
 *        Created:  08/22/2014 04:20:06 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef TRAVERSABLE_9YOIEPI9
#define TRAVERSABLE_9YOIEPI9

#include <cstddef>

#include <templatious/util/Exceptions.hpp>
#include <templatious/util/Selectors.hpp>

namespace templatious {
namespace vmodular {

// Standard traversable
template <class T>
struct Traversable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    Traversable(ThisCol& t) : T(t) {}

    Iter begin() {
        return Ad::begin(getRef());
    }

    Iter end() {
        return Ad::end(getRef());
    }

    Iter iterAt(long idx) {
        return Ad::iterAt(getRef(),idx);
    }

    CIter cbegin() const {
        return Ad::cbegin(cgetRef());
    }

    CIter cend() const {
        return Ad::cend(cgetRef());
    }

    CIter citerAt(long idx) const {
        return Ad::citerAt(cgetRef(),idx);
    }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

// Traversable which always returns collection end
template <class T>
struct TraversableFake: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    TraversableFake(ThisCol& t) : T(t) {}

    Iter begin() {
        return Ad::end(getRef());
    }

    Iter end() {
        return Ad::end(getRef());
    }

    Iter iterAt(long idx) {
        return Ad::end(getRef());
    }

    CIter cbegin() const {
        return Ad::cend(cgetRef());
    }

    CIter cend() const {
        return Ad::cend(cgetRef());
    }

    CIter citerAt(long idx) const {
        return Ad::cend(cgetRef());
    }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

// Traversable throwing exception
template <class T>
struct TraversableThrow: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    TraversableThrow(ThisCol& t) : T(t) {}

    Iter begin() {
        throw templatious::util::FeatureDisabled(
            "Traversal is disabled in current collection.");
    }

    Iter end() {
        throw templatious::util::FeatureDisabled(
            "Traversal is disabled in current collection.");
    }

    Iter iterAt(long idx) {
        throw templatious::util::FeatureDisabled(
            "Traversal is disabled in current collection.");
    }

    CIter cbegin() const {
        throw templatious::util::FeatureDisabled(
            "Traversal is disabled in current collection.");
    }

    CIter cend() const {
        throw templatious::util::FeatureDisabled(
            "Traversal is disabled in current collection.");
    }

    CIter citerAt(long idx) const {
        throw templatious::util::FeatureDisabled(
            "Traversal is disabled in current collection.");
    }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

template <class T>
struct TraversablePrevent: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    TraversablePrevent(ThisCol& t) : T(t) {}

    template <class U = int>
    Iter begin() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Traversable feature is disabled.");
    }

    template <class U = int>
    Iter end() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Traversable feature is disabled.");
    }

    template <class U = int>
    Iter iterAt(long idx) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Traversable feature is disabled.");
    }

    template <class U = int>
    CIter cbegin() const {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Traversable feature is disabled.");
    }

    template <class U = int>
    CIter cend() const {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Traversable feature is disabled.");
    }

    template <class U = int>
    CIter citerAt(long idx) const {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Traversable feature is disabled.");
    }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

}
}

#endif /* end of include guard: TRAVERSABLE_9YOIEPI9 */
