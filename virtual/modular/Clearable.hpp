//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Clearable.hpp
 *
 *    Description:  Clearable virtual collection
 *
 *        Version:  1.0
 *        Created:  08/23/2014 05:28:21 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef CLEARABLE_D3H5AWSJ
#define CLEARABLE_D3H5AWSJ

#include <templatious/util/Exceptions.hpp>
#include <templatious/util/Selectors.hpp>

namespace templatious {
namespace vmodular {

template <class T>
struct Clearable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    void erase(const Iter& i) {
        Ad::erase(getRef(),i);
    }

    void erase(const Iter& beg,const Iter& end) {
        Ad::erase(getRef(),beg,end);
    }

    void clear() {
        Ad::clear(getRef());
    }

    Clearable(ThisCol& t) : T(t) {}

protected:
    ThisCol& getRef() {
        return T::getRef();
    }

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

template <class T>
struct ClearableFake: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    void erase(const Iter& i) {}
    void erase(const Iter& beg,const Iter& end) {}
    void clear() {}

    ClearableFake(ThisCol& t) : T(t) {}

protected:
    ThisCol& getRef() {
        return T::getRef();
    }

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

template <class T>
struct ClearableThrow: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    void erase(const Iter& i) {
        throw templatious::util::FeatureDisabled(
            "Erasing is disabled in current collection.");
    }

    void erase(const Iter& beg,const Iter& end) {
        throw templatious::util::FeatureDisabled(
            "Erasing is disabled in current collection.");
    }

    void clear() {
        throw templatious::util::FeatureDisabled(
            "Erasing is disabled in current collection.");
    }

    ClearableThrow(ThisCol& t) : T(t) {}

protected:
    ThisCol& getRef() {
        return T::getRef();
    }

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

template <class T>
struct ClearablePrevent: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    template <class U = int>
    void erase(const Iter& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Clearable feature is disabled.");
    }

    template <class U = int>
    void erase(const Iter& beg,const Iter& end) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Clearable feature is disabled.");
    }

    template <class U = int>
    void clear() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Clearable feature is disabled.");
    }

    ClearablePrevent(ThisCol& t) : T(t) {}

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

#endif /* end of include guard: CLEARABLE_D3H5AWSJ */
