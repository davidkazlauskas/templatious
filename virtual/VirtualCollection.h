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
struct VCollectionBase {

    typedef VIterator<T> Iter;
    typedef VIterator<const T> CIter;
    typedef VCollectionBase<T> ThisCol;

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

    virtual bool equals(ThisCol& c) const = 0;
    virtual bool equals(ThisCol* c) const = 0;
    virtual ThisCol* clone() const = 0;
protected:

    template <class V>
    static VIteratorBase<T>* getInternal(V&& v) {
        return v._i;
    }

};

template <class T>
struct VCollectionImpl:
    public VCollectionBase< typename templatious::adapters::CollectionAdapter<T>::value_type >
{


    typedef templatious::adapters::CollectionAdapter<T> Ad;
    typedef templatious::StaticAdapter SA;
    typedef VCollectionBase< typename Ad::value_type > Super;

    typedef typename Ad::value_type ValType;
    typedef VCollectionImpl< T > ThisCol;
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

    virtual bool equals(Super& c) const {
        return comp(c);
    }

    virtual bool equals(Super* c) const {
        return comp(*c);
    }

    virtual ThisCol* clone() const {
        return new ThisCol(_ref);
    }

private:

    bool comp(Super& c) const {
        if (typeid(c) == typeid(*this)) {
            ThisCol& ref = static_cast<ThisCol&>(c);
            return std::addressof(_ref) == std::addressof(ref._ref);
        }

        return false;
    }

    T& _ref;
};

template <class T>
struct VCollection {
    typedef VCollection<T> ThisCol;
    typedef VCollectionBase<T> Base;
    typedef typename Base::Iter Iter;
    typedef typename Base::CIter CIter;

    VCollection(Base* b) : _b(b) {}
    VCollection(ThisCol&& o) {
        _b = o._b;
        o._b = nullptr;
    }

    VCollection(const ThisCol& o) : _b(o._b->clone()) {}

    ThisCol& operator=(const ThisCol& o) {
        delete _b;
        _b = o._b->clone();
    }

    ThisCol& operator=(ThisCol&& o) {
        _b = o._b;
        o._b = nullptr;
    }

    ~VCollection() { delete _b; }

    void add(const T& e) {
        _b->add(e);
    }

    void insert(const Iter& i,const T& t) {
        _b->insert(i,t);
    }

    void clear() {
        _b->clear();
    }

    Iter begin() {
        return _b->begin();
    }

    Iter end() {
        return _b->end();
    }

    CIter cbegin() const {
        return _b->cbegin();
    }

    CIter cend() const {
        return _b->cend();
    }

    Iter iterAt(size_t idx) {
        return _b->iterAt(idx);
    }

    CIter citerAt(size_t idx) const {
        return _b->citerAt(idx);
    }

    bool canAdd() const {
        return _b->canAdd();
    }

    bool operator==(const VCollection& vc) const {
        return _b->equals(vc._b);
    }

    bool operator!=(const VCollection& vc) const {
        return !(*this == vc);
    }

private:
    Base* _b;
};

}

#endif /* end of include guard: VIRTUALCOLLECTION_4I04BTF7 */

