/*
 * =====================================================================================
 *
 *       Filename:  VirtualCollection.h
 *
 *    Description:  Virtual collection to be passed around translation units.
 *
 *        Version:  1.0
 *        Created:  08/19/2014 06:44:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef VIRTUALCOLLECTION_4I04BTF7
#define VIRTUALCOLLECTION_4I04BTF7

#include <templatious/CollectionAdapter.h>
#include <templatious/StaticAdapter.h>
#include <templatious/virtual/Iterator.h>

namespace templatious {

template <class T>
struct VCollection {

    typedef VIterator<T> Iter;
    typedef VIterator<const T> CIter;

    virtual void add(const T& e) = 0;
    virtual void insert(const Iter& i,const T& t) = 0;
    virtual void clear() = 0;

    virtual Iter begin() = 0;
    virtual Iter end() = 0;
    virtual CIter cbegin() const = 0;
    virtual CIter cend() const = 0;

    virtual Iter iterAt(size_t idx) = 0;
    virtual CIter citerAt(size_t idx) const = 0;

    virtual bool canAdd() const = 0;

protected:

    template <class V>
    static VIteratorBase<T>* getInternal(V&& v) {
        return v._i;
    }

};

template <class T>
struct VCollectionImpl:
    public VCollection< typename templatious::adapters::CollectionAdapter<T>::value_type >
{

    typedef templatious::adapters::CollectionAdapter<T> Ad;
    typedef templatious::StaticAdapter SA;
    typedef VCollection< typename Ad::value_type > Super;

    typedef typename Ad::value_type ValType;
    typedef typename Super::Iter Iter;
    typedef typename Super::CIter CIter;

    typedef typename templatious::VIteratorImpl<
        T > IterImpl;
    typedef typename templatious::VIteratorImpl<
        T > CIterImpl;

    VCollectionImpl(T& c) : _ref(c) {}

    virtual void add(const ValType& e) {
        Ad::add(_ref,e);
    }

    virtual void insert(const Iter& i,const ValType& t) {
        IterImpl* impl = static_cast<IterImpl*>(i.getBase());
        Ad::insert_at(_ref,impl->internal(),t);
    }

    virtual void clear() {
        Ad::clear(_ref);
    }

    virtual Iter begin() {
        return SA::vbegin(_ref);
    }

    virtual Iter end() {
        return SA::vend(_ref);
    }

    virtual CIter cbegin() const {
        return SA::vcbegin(_ref);
    }

    virtual CIter cend() const {
        return SA::vcend(_ref);
    }

    virtual Iter iterAt(size_t idx) {
        return SA::viterAt(_ref,idx);
    }

    virtual CIter citerAt(size_t idx) const {
        return SA::vciterAt(_ref,idx);
    }

    virtual bool canAdd() const {
        return Ad::canAdd(_ref);
    }
private:
    T& _ref;
};

}

#endif /* end of include guard: VIRTUALCOLLECTION_4I04BTF7 */

