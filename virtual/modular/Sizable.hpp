//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Sizable.hpp
 *
 *    Description:  Module to find out collection size
 *
 *        Version:  1.0
 *        Created:  08/23/2014 08:49:33 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef SIZABLE_T81BW4IT
#define SIZABLE_T81BW4IT

#include <cstddef>

#include <templatious/util/Exceptions.hpp>
#include <templatious/util/Selectors.hpp>

namespace templatious {
namespace vmodular {

template <class T>
struct Sizable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    Sizable(ThisCol& t) : T(t) {}

    long size() const {
        return Ad::size(static_cast<ConstCol&>(cgetRef()));
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
struct SizableFake: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    SizableFake(ThisCol& t) : T(t) {}

    long size() const {
        return 0;
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
struct SizableThrow: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    SizableThrow(ThisCol& t) : T(t) {}

    long size() const {
        throw templatious::util::FeatureDisabled(
            "Size info is disabled in current collection.");
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
struct SizablePrevent: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    SizablePrevent(ThisCol& t) : T(t) {}

    template <class U = int>
    long size() const {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Sizable feature is disabled.");
        return -1;
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

#endif /* end of include guard: SIZABLE_T81BW4IT */
