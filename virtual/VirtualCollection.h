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

    virtual ~VCollectionBase() {}

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
    virtual size_t size() const = 0;

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

    virtual size_t size() const {
        return Ad::getSize(_ref);
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

    virtual ~VCollection() { delete _b; }

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

    size_t size() const {
        return _b->size();
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

namespace adapters {

template <class T>
struct CollectionAdapter< VCollection<T> > {

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef VCollection<T> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::Iter iterator;
    typedef typename ThisCol::CIter const_iterator;
    typedef T value_type;
    typedef const T const_value_type;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.add(i);
    }

    template <class V>
    static void insert_at(ThisCol& c, iterator at, V&& i) {
        // TO IMPLEMENT
    }

    // TO IMPLEMENT
    static value_type& getByIndex(ThisCol& c, size_t i);
    static const_value_type& getByIndex(ConstCol& c, size_t i);

    static size_t getSize(ConstCol& c) {
        return c.size();
    }

    // TO IMPLEMENT
    static void erase(ThisCol& c, iterator i);
    static void erase(ThisCol& c, iterator beg, iterator end);

    template <class U = int>
    static ThisCol instantiate() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Virtual collection should be wrapper \
                      around actual collection, therefore, \
                      cannot be instantiated.");
    }

    template <class U = int>
    static ThisCol instantiate(size_t size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Virtual collection should be wrapper \
                      around actual collection, therefore, \
                      cannot be instantiated.");
    }

    template <class U = int>
    static ThisCol* instHeap() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Virtual collection should be wrapper \
                      around actual collection, therefore, \
                      cannot be instantiated.");
    }

    template <class U = int>
    static ThisCol* instHeap(size_t size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Virtual collection should be wrapper \
                      around actual collection, therefore, \
                      cannot be instantiated.");
    }

    static iterator begin(ThisCol& c) {
        return c.begin();
    }

    static iterator end(ThisCol& c) {
        return c.end();
    }

    static iterator iter_at(ThisCol& c, size_t i) {
        return c.iterAt(i);
    }

    static iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static iterator end(ConstCol& c) {
        return c.cend();
    }

    static iterator iter_at(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static const_iterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator cend(ConstCol& c) {
        return c.cend();
    }

    static const_iterator citer_at(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    // TO IMPLEMENT
    static value_type& first(ThisCol& c);
    static const value_type& first(ConstCol& c);
    static value_type& last(ThisCol& c);
    static const value_type& last(ConstCol& c);

    static void clear(ThisCol& c) {
        c.clear();
    }

    static bool canAdd(ConstCol& c) {
        return c.canAdd();
    }
};

}
}

#endif /* end of include guard: VIRTUALCOLLECTION_4I04BTF7 */

