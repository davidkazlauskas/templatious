/*
 * =====================================================================================
 *
 *       Filename:  Traversable.h
 *
 *    Description:  Traversable trait
 *
 *        Version:  1.0
 *        Created:  08/22/2014 04:20:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef TRAVERSABLE_9YOIEPI9
#define TRAVERSABLE_9YOIEPI9

#include <cstddef>
#include <templatious/util/Exceptions.h>

namespace templatious {

// Standard traversable
template <class T>
struct Traversable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    Traversable(ThisCol& t) : T(t) {}

    Iter begin() {
        return Ad::begin(getRef());
    }

    Iter end() {
        return Ad::end(getRef());
    }

    Iter iterAt(size_t idx) {
        return Ad::iter_at(getRef(),idx);
    }

    CIter cbegin() const {
        return Ad::cbegin(getRef());
    }

    CIter cend() const {
        return Ad::cend(getRef());
    }

    CIter citerAt(size_t idx) const {
        return Ad::iter_at(
            static_cast<ConstCol&>(getRef()),idx);
    }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }
};

// Traversable which always returns collection end
template <class T>
struct TraversableFakeEnd: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    TraversableFakeEnd(ThisCol& t) : T(t) {}

    Iter begin() {
        return Ad::end(getRef());
    }

    Iter end() {
        return Ad::end(getRef());
    }

    Iter iterAt(size_t idx) {
        return Ad::end(getRef());
    }

    CIter cbegin() const {
        return Ad::cend(getRef());
    }

    CIter cend() const {
        return Ad::cend(getRef());
    }

    CIter citerAt(size_t idx) const {
        return Ad::cend(getRef());
    }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }
};

// Traversable throwing exception
template <class T>
struct TraversableThrow: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
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

    CIter iterAt(size_t idx) {
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

    CIter citerAt(size_t idx) const {
        throw templatious::util::FeatureDisabled(
            "Traversal is disabled in current collection.");
    }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }
};

}

#endif /* end of include guard: TRAVERSABLE_9YOIEPI9 */
