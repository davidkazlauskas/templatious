//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Iterator.hpp
 *
 *    Description:  Virtual iterator to be passed around translation units
 *
 *        Version:  1.0
 *        Created:  08/18/2014 06:21:22 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef ITERATOR_ODMVVSMD
#define ITERATOR_ODMVVSMD

#include <memory>
#include <utility>

#include <templatious/CollectionAdapter.hpp>
#include <templatious/util/Selectors.hpp>
#include <templatious/util/Exceptions.hpp>

namespace templatious {

TEMPLATIOUS_BOILERPLATE_EXCEPTION(
    VirtualIteratorNoDecrementException,
    "This virtual iterator doesn't support decrement"
    " operation.");

template <class T>
struct VIteratorBase {
    typedef VIteratorBase<T> ThisIter;
    typedef T ValType;

    virtual ~VIteratorBase() {}

    virtual ThisIter& operator++() = 0;
    virtual ThisIter& operator--() = 0;
    virtual ValType& operator*() = 0;
    virtual ValType* operator->() = 0;
    virtual bool operator==(const VIteratorBase& rhs) const = 0;
    virtual ThisIter* clone() const = 0;
};

template <class T>
struct VIteratorImpl:
    public VIteratorBase< typename adapters::CollectionAdapter<T>::ValueType >
{
    typedef typename adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::Iterator Iterator;

    typedef VIteratorImpl<T> ThisIter;
    typedef typename Ad::ValueType ValType;
    typedef VIteratorBase<ValType> Base;

    static const bool supports_decrement =
        templatious::util::
            SupportsDecrementOperator<Iterator>::value;

    template <class V>
    VIteratorImpl(V&& v) : _i(std::forward<V>(v)) {}

    virtual ~VIteratorImpl() {}

    virtual ThisIter& operator++() {
        ++_i;
        return *this;
    }

    virtual ThisIter& operator--() {
        return decrement();
    }

    template <class Dummy = void>
    ThisIter& decrement(
        typename std::enable_if<
            supports_decrement,
            Dummy
        >::type* ptr = nullptr
    ) {
        --_i;
        return *this;
    }

    template <class Dummy = void>
    ThisIter& decrement(
        typename std::enable_if<
            !supports_decrement,
            Dummy
        >::type* ptr = nullptr
    ) {
        throw VirtualIteratorNoDecrementException();
    }

    virtual ValType& operator*() {
        return *_i;
    }

    virtual ValType* operator->() {
        return std::addressof(*_i);
    }

    virtual bool operator==(const Base& rhs) const {
        if (typeid(*this) == typeid(rhs)) {
            const ThisIter& rhRef =
                static_cast<const ThisIter&>(rhs);
            return _i == rhRef._i;
        }

        return false;
    }

    virtual ThisIter* clone() const {
        return new ThisIter(
            static_cast<const Iterator>(_i));
    }

    Iterator& internal() {
        return _i;
    }

private:
    Iterator _i;
};

template <class T>
struct VCollection;

template <class T>
struct VIterator {
    typedef VIteratorBase<T> Base;
    typedef VIterator<T> ThisIter;
    typedef T ValType;

    VIterator(Base* b) : _i(b) {}

    VIterator(const ThisIter& i) {
        _i = i._i->clone();
    }

    VIterator(ThisIter&& i) {
        _i = i._i;
        i._i = nullptr;
    }

    ~VIterator() {
        freePtr();
    }

    ThisIter& operator++() {
        _i->operator++();
        return *this;
    }

    ThisIter& operator--() {
        _i->operator--();
        return *this;
    }

    ValType& operator*() const {
        return _i->operator*();
    }

    ValType* operator->() const {
        return _i->operator->();
    }

    bool operator==(const ThisIter& rhs) const {
        return (*_i) == (*rhs._i);
    }

    bool operator!=(const ThisIter& rhs) const {
        return !(*this == rhs);
    }

    ThisIter& operator=(const ThisIter& rhs) {
        freePtr();
        _i = rhs._i->clone();
        return *this;
    }

    ThisIter& operator=(ThisIter&& rhs) {
        freePtr();
        _i = rhs._i;
        rhs._i = nullptr;
        return *this;
    }

private:
    template <
        class Tm,
        class Dtor,
        template <class> class StoragePolicy
    >
    friend struct VCollectionImpl;

    Base* getBase() const {
        return _i;
    }

    void freePtr() { delete _i; }

    Base* _i;
};

}

#endif /* end of include guard: ITERATOR_ODMVVSMD */

