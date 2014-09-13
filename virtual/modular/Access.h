/*
 * =====================================================================================
 *
 *       Filename:  Access.h
 *
 *    Description:  Access collection utilities
 *
 *        Version:  1.0
 *        Created:  08/23/2014 08:20:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef GENERAL_1NN4MGYY
#define GENERAL_1NN4MGYY

#include <cstddef>

#include <templatious/util/Exceptions.h>
#include <templatious/util/Selectors.h>

namespace templatious {
namespace vmodular {

template <class T>
struct Access: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::value_type ValType;
    typedef typename Ad::const_value_type CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    Access(ThisCol& t) : T(t) {}

    ValType& first() {
        return Ad::first(getRef());
    }

    CValType& cfirst() const {
        return Ad::first(static_cast<ConstCol&>(getRef()));
    }

    ValType& last() {
        return Ad::last(getRef());
    }

    CValType& clast() const {
        return Ad::last(static_cast<ConstCol&>(getRef()));
    }

    ValType& getByIndex(size_t idx) {
        return Ad::getByIndex(getRef(),idx);
    }

    CValType& cgetByIndex(size_t idx) const {
        return Ad::getByIndex(static_cast<ConstCol&>(getRef()),idx);
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
struct AccessThrow: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::value_type ValType;
    typedef typename Ad::const_value_type CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    AccessThrow(ThisCol& t) : T(t) {}

    ValType& first() {
        throw templatious::util::FeatureDisabled(
            "Access operations are disabled in current collection.");
    }

    CValType& cfirst() const {
        throw templatious::util::FeatureDisabled(
            "Access operations are disabled in current collection.");
    }

    ValType& last() {
        throw templatious::util::FeatureDisabled(
            "Access operations are disabled in current collection.");
    }

    CValType& clast() const {
        throw templatious::util::FeatureDisabled(
            "Access operations are disabled in current collection.");
    }

    ValType& getByIndex(size_t idx) {
        throw templatious::util::FeatureDisabled(
            "Access operations are disabled in current collection.");
    }

    CValType& cgetByIndex(size_t idx) const {
        throw templatious::util::FeatureDisabled(
            "Access operations are disabled in current collection.");
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
struct AccessPrevent: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::value_type ValType;
    typedef typename Ad::const_value_type CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    AccessPrevent(ThisCol& t) : T(t) {}

    template <class U = int>
    ValType& first() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Access feature is disabled.");
    }

    template <class U = int>
    CValType& cfirst() const {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Access feature is disabled.");
    }

    template <class U = int>
    ValType& last() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Access feature is disabled.");
    }

    template <class U = int>
    CValType& clast() const {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Access feature is disabled.");
    }

    template <class U = int>
    ValType& getByIndex(size_t idx) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Access feature is disabled.");
    }

    template <class U = int>
    CValType& cgetByIndex(size_t idx) const {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Access feature is disabled.");
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

#endif /* end of include guard: GENERAL_1NN4MGYY */