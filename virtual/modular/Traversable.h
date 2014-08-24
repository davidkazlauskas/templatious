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
namespace vmodular {

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
        return Ad::cbegin(cgetRef());
    }

    CIter cend() const {
        return Ad::cend(cgetRef());
    }

    CIter citerAt(size_t idx) const {
        return Ad::iter_at(
            static_cast<ConstCol&>(getRef()),idx);
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

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    TraversableFake(ThisCol& t) : T(t) {}

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

    ConstCol& cgetRef() const {
        return T::cgetRef();
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

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

}
}

#endif /* end of include guard: TRAVERSABLE_9YOIEPI9 */
