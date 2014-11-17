/*
 * =====================================================================================
 *
 *       Filename:  Addable.h
 *
 *    Description:  Addable collection trait
 *
 *        Version:  1.0
 *        Created:  08/23/2014 01:19:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ADDABLE_T69BV8TV
#define ADDABLE_T69BV8TV

#include <templatious/util/Exceptions.h>
#include <templatious/util/Selectors.h>

namespace templatious {
namespace vmodular {

template <class T>
struct Addable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    Addable(ThisCol& t) : T(t) {}

    void add(const ValType& i) {
        Ad::add(getRef(),i);
    }

    void insert(const Iter& at,const ValType& i) {
        Ad::insertAt(getRef(),at,i);
    }

    bool canAdd() const {
        return false;
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
struct AddableFake: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    AddableFake(ThisCol& t) : T(t) {}

    void add(const ValType& i) {}
    void insert(const Iter& at,const ValType& i) {}
    bool canAdd() const { return false; }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }

    ConstCol& cgetRef() const {
        return T::cgetRef();
    }
};

template <class T>
struct AddableThrow: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    AddableThrow(ThisCol& t) : T(t) {}

    void add(const ValType& i) {
        throw templatious::util::FeatureDisabled(
            "Adding is disabled in current collection.");
    }

    void insert(const Iter& at,const ValType& i) {
        throw templatious::util::FeatureDisabled(
            "Adding is disabled in current collection.");
    }

    bool canAdd() const {
        throw templatious::util::FeatureDisabled(
            "Adding is disabled in current collection.");
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
struct AddablePrevent: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::Iterator Iter;
    typedef typename Ad::ConstIterator CIter;
    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    AddablePrevent(ThisCol& t) : T(t) {}

    template <class U = int>
    void add(const ValType& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Addable feature is disabled.");
    }

    template <class U = int>
    void insert(const Iter& at,const ValType& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Addable feature is disabled.");
    }

    template <class U = int>
    bool canAdd() const {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Addable feature is disabled.");
        return false;
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

#endif /* end of include guard: ADDABLE_T69BV8TV */
