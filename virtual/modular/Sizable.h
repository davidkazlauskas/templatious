/*
 * =====================================================================================
 *
 *       Filename:  Sizable.h
 *
 *    Description:  Module to find out collection size
 *
 *        Version:  1.0
 *        Created:  08/23/2014 08:49:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef SIZABLE_T81BW4IT
#define SIZABLE_T81BW4IT

#include <cstddef>

#include <templatious/util/Exceptions.h>

namespace templatious {
namespace vmodular {

template <class T>
struct Sizable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    Sizable(ThisCol& t) : T(t) {}

    size_t size() const {
        return Ad::getSize(static_cast<ConstCol&>(cgetRef()));
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

    size_t size() const {
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

    size_t size() const {
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

}
}

#endif /* end of include guard: SIZABLE_T81BW4IT */
