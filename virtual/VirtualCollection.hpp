//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  VirtualCollection.hpp
 *
 *    Description:  Virtual collection to be passed around translation units.
 *
 *        Version:  1.0
 *        Created:  08/19/2014 06:44:42 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef VIRTUALCOLLECTION_4I04BTF7
#define VIRTUALCOLLECTION_4I04BTF7

#include <templatious/CollectionAdapter.hpp>
#include <templatious/StaticAdapter.hpp>
#include <templatious/virtual/Iterator.hpp>

namespace templatious {

template <class T>
struct VCollectionBase {

    typedef VIterator<T> Iter;
    typedef VIterator<const T> CIter;
    typedef VCollectionBase<T> ThisCol;
    typedef T ValType;

    virtual ~VCollectionBase() {}

    virtual void add(const T& e) = 0;
    virtual void add(T&& e) = 0;
    virtual void insert(const Iter& i,const T& t) = 0;
    virtual void insert(const Iter& i,T&& t) = 0;
    virtual void erase(const Iter& i) = 0;
    virtual void erase(const Iter& beg,const Iter& end) = 0;
    virtual void clear() = 0;

    virtual T& getByIndex(long idx) = 0;
    virtual const T& cgetByIndex(long idx) const = 0;

    virtual Iter begin() = 0;
    virtual Iter end() = 0;
    virtual CIter cbegin() const = 0;
    virtual CIter cend() const = 0;

    virtual Iter iterAt(long idx) = 0;
    virtual CIter citerAt(long idx) const = 0;

    virtual T& first() = 0;
    virtual const T& cfirst() const = 0;
    virtual T& last() = 0;
    virtual const T& clast() const = 0;

    virtual bool canAdd() const = 0;
    virtual long size() const = 0;

    virtual bool equals(ThisCol* c) const = 0;
protected:

    template <class V>
    static VIteratorBase<T>* getInternal(V&& v) {
        return v._i;
    }

};

template <
    class T,
    template <class> class StoragePolicy
>
struct VCollectionContainerRaw {
    typedef typename StoragePolicy<T>::Container ContCol;

    template <class A>
    VCollectionContainerRaw(A&& col):
        _cc(std::forward<A>(col))
    {}

    auto getColRef()
     -> decltype(std::declval<ContCol>().getRef())
    {
        return _cc.getRef();
    }

    auto cgetColRef() const
     -> decltype(std::declval<ContCol>().cgetRef())
    {
        return _cc.cgetRef();
    }

    auto constCpy() const
     -> decltype(std::declval<ContCol>().constCpy())
    {
        return _cc.constCpy();
    }

private:
    ContCol _cc;
};

template <
    class T,class Dtor,
    template <class> class StoragePolicy
>
struct VCollectionContainerWDtor {
    typedef typename StoragePolicy<T>::Container ContCol;
    typedef typename StoragePolicy<Dtor>::Container ContDtor;

    template <class A,class B>
    VCollectionContainerWDtor(A&& col,B&& dtor):
        _cc(std::forward<A>(col)),
        _cd(std::forward<B>(dtor))
    {}

    ~VCollectionContainerWDtor() {
        // call the destructor
        _cd.getRef()();
    }

    auto getColRef()
     -> decltype(std::declval<ContCol>().getRef())
    {
        return _cc.getRef();
    }

    auto cgetColRef() const
     -> decltype(std::declval<ContCol>().cgetRef())
    {
        return _cc.cgetRef();
    }

    auto constCpy() const
     -> decltype(std::declval<ContCol>().constCpy())
    {
        return _cc.constCpy();
    }

private:
    ContCol _cc;
    ContDtor _cd;
};

template <
    class T,
    class Dtor,
    template <class> class StoragePolicy
>
struct VCollectionImpl:
    public VCollectionBase< typename templatious::adapters::CollectionAdapter<T>::ValueType >
{
    typedef templatious::adapters::CollectionAdapter<T> Ad;
    typedef templatious::StaticAdapter SA;
    typedef VCollectionBase< typename Ad::ValueType > Super;

    static const bool hasDestructor =
        !std::is_same< Dtor, void >::value;

    typedef typename Ad::ValueType ValType;
    typedef typename Ad::ConstValueType CValType;
    typedef VCollectionImpl< T, Dtor, StoragePolicy > ThisCol;
    typedef typename Super::Iter Iter;
    typedef typename Super::CIter CIter;
    typedef typename std::conditional<
        !hasDestructor,
        VCollectionContainerRaw< T, StoragePolicy >,
        VCollectionContainerWDtor< T, Dtor, StoragePolicy >
    >::type Cont;

    typedef typename std::remove_reference<T>::type Deref;

    typedef typename templatious::VIteratorImpl<
        Deref > IterImpl;
    typedef typename templatious::VIteratorImpl<
        Deref > CIterImpl;

    template <class V, class Enabled = void*>
    VCollectionImpl(
        V&& c,
        typename std::enable_if<
            !hasDestructor, Enabled
        >::type dummy = nullptr
    ) : _ref(std::forward<V>(c)) {
        static_assert(templatious::util::
            DummyResolver<V,!hasDestructor>::val,
            "This VCollection imlementation "
            "must call the two argument constructor "
            "with collection and functor.");
    }

    template <class V,class D, class Enabled = void* >
    VCollectionImpl(
        V&& c,D&& d,
        typename std::enable_if<
            hasDestructor, Enabled
        >::type dummy = nullptr
    ) :
        _ref(
            std::forward<V>(c),
            std::forward<D>(d)
        )
    {
        static_assert(templatious::util::
            DummyResolver<V,hasDestructor>::val,
            "This VCollection imlementation "
            "must call the one argument constructor "
            "with collection only.");
    }

    virtual void add(const ValType& e) {
        Ad::add(getColRef(),e);
    }

    virtual void add(ValType&& e) {
        Ad::add(getColRef(),std::move(e));
    }

    virtual void insert(const Iter& i,const ValType& t) {
        IterImpl* impl = static_cast<IterImpl*>(i.getBase());
        Ad::insertAt(getColRef(),impl->internal(),t);
    }

    virtual void insert(const Iter& i,ValType&& t) {
        IterImpl* impl = static_cast<IterImpl*>(i.getBase());
        Ad::insertAt(getColRef(),impl->internal(),std::move(t));
    }

    virtual void erase(const Iter& i) {
        IterImpl* impl = static_cast<IterImpl*>(i.getBase());
        Ad::erase(getColRef(),impl->internal());
    }

    virtual void erase(const Iter& beg,const Iter& end) {
        IterImpl* iBeg = static_cast<IterImpl*>(beg.getBase());
        IterImpl* iEnd = static_cast<IterImpl*>(end.getBase());
        Ad::erase(getColRef(),iBeg->internal(),iEnd->internal());
    }

    virtual void clear() {
        Ad::clear(getColRef());
    }

    virtual ValType& getByIndex(long idx) {
        return Ad::getByIndex(getColRef(),idx);
    }

    virtual CValType& cgetByIndex(long idx) const {
        return Ad::getByIndex(cgetColRef(),idx);
    }

    virtual ValType& first() {
        return Ad::first(getColRef());
    }

    virtual CValType& cfirst() const {
        return Ad::first(cgetColRef());
    }

    virtual ValType& last() {
        return Ad::last(getColRef());
    }

    virtual CValType& clast() const {
        return Ad::last(cgetColRef());
    }

    virtual Iter begin() {
        return SA::vbegin(getColRef());
    }

    virtual Iter end() {
        return SA::vend(getColRef());
    }

    virtual CIter cbegin() const {
        return SA::vcbegin(cgetColRef());
    }

    virtual CIter cend() const {
        return SA::vcend(cgetColRef());
    }

    virtual Iter iterAt(long idx) {
        return SA::viterAt(getColRef(),idx);
    }

    virtual CIter citerAt(long idx) const {
        return SA::vciterAt(cgetColRef(),idx);
    }

    virtual bool canAdd() const {
        return Ad::canAdd(cgetColRef());
    }

    virtual long size() const {
        return Ad::size(cgetColRef());
    }

    virtual bool equals(Super* c) const {
        if (nullptr == c) {
            return false;
        }
        return comp(*c);
    }

private:

    auto getColRef()
     -> decltype(std::declval<Cont>().getColRef())
    {
        return _ref.getColRef();
    }

    auto cgetColRef() const
     -> decltype(std::declval<Cont>().cgetColRef())
    {
        return _ref.cgetColRef();
    }

    bool comp(Super& c) const {
        if (typeid(c) == typeid(*this)) {
            ThisCol& ref = static_cast<ThisCol&>(c);
            return std::addressof(cgetColRef()) ==
                std::addressof(ref.cgetColRef());
        }

        return false;
    }

    Cont _ref;
};

template <class T>
struct VCollection {
    typedef VCollection<T> ThisCol;
    typedef VCollectionBase<T> Base;
    typedef typename Base::Iter Iter;
    typedef typename Base::CIter CIter;
    typedef T ValType;
    typedef const ValType CValType;

    VCollection(Base* b) : _b(b) {}
    VCollection(ThisCol&& o) {
        _b = o._b;
        o._b = nullptr;
    }

    // handle cannot be copied, only moved
    VCollection(const ThisCol& o) = delete;
    ThisCol& operator=(const ThisCol& o) = delete;

    ThisCol& operator=(ThisCol&& o) {
        delete _b;
        _b = o._b;
        o._b = nullptr;
        return *this;
    }

    virtual ~VCollection() { delete _b; }

    void add(const T& e) {
        _b->add(e);
    }

    void add(T&& e) {
        _b->add(std::move(e));
    }

    void insert(const Iter& i,const T& t) {
        _b->insert(i,t);
    }

    void insert(const Iter& i,T&& t) {
        _b->insert(i,std::move(t));
    }

    void erase(const Iter& i) {
        _b->erase(i);
    }

    void erase(const Iter& beg,const Iter& end) {
        _b->erase(beg,end);
    }

    void clear() {
        _b->clear();
    }

    T& getByIndex(long idx) {
        return _b->getByIndex(idx);
    }

    const T& cgetByIndex(long idx) const {
        return _b->cgetByIndex(idx);
    }

    ValType& first() {
        return _b->first();
    }

    CValType& cfirst() const {
        return _b->cfirst();
    }

    ValType& last() {
        return _b->last();
    }

    CValType& clast() const {
        return _b->clast();
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

    Iter iterAt(long idx) {
        return _b->iterAt(idx);
    }

    CIter citerAt(long idx) const {
        return _b->citerAt(idx);
    }

    bool canAdd() const {
        return _b->canAdd();
    }

    long size() const {
        return _b->size();
    }

    bool operator==(const VCollection& vc) const {
        if (_b == nullptr) return false;
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
    typedef typename ThisCol::Iter Iterator;
    typedef typename ThisCol::CIter ConstIterator;
    typedef T ValueType;
    typedef const T ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.add(std::forward<V>(i));
    }

    template <class V>
    static void insertAt(ThisCol& c, const Iterator& at, V&& i) {
        c.insert(at,std::forward<V>(i));
    }

    static ValueType& getByIndex(ThisCol& c, long i) {
        return c.getByIndex(i);
    }

    static ConstValueType& getByIndex(ConstCol& c, long i) {
        return c.cgetByIndex(i);
    }

    static long size(ConstCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c, const Iterator& i) {
        c.erase(i);
    }

    static void erase(ThisCol& c, const Iterator& beg, const Iterator& end) {
        c.erase(beg,end);
    }

    static Iterator begin(ThisCol& c) {
        return c.begin();
    }

    static Iterator end(ThisCol& c) {
        return c.end();
    }

    static Iterator iterAt(ThisCol& c, long i) {
        return c.iterAt(i);
    }

    static ConstIterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator end(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator iterAt(ConstCol& c, long i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator citerAt(ConstCol& c, long i) {
        return c.citerAt(i);
    }

    static ValueType& first(ThisCol& c) {
        return c.first();
    }

    static ConstValueType& first(ConstCol& c) {
        return c.cfirst();
    }

    static ValueType& last(ThisCol& c) {
        return c.last();
    }

    static ConstValueType& last(ConstCol& c) {
        return c.clast();
    }

    static void clear(ThisCol& c) {
        c.clear();
    }

    static bool canAdd(ConstCol& c) {
        return c.canAdd();
    }
};

template <class T>
struct CollectionAdapter< const VCollection<T> > {

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef const VCollection<T> ThisCol;
    typedef ThisCol ConstCol;
    typedef typename ThisCol::CIter Iterator;
    typedef Iterator ConstIterator;
    typedef const T ValueType;
    typedef ValueType ConstValueType;

    template <class V,class U = void>
    static void add(ThisCol& c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class V,class U = void>
    static void insertAt(ThisCol& c, const Iterator& at, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static ConstValueType& getByIndex(ConstCol& c, long i) {
        return c.cgetByIndex(i);
    }

    static long size(ConstCol& c) {
        return c.size();
    }

    template <class U = void>
    static void erase(ThisCol& c, const Iterator& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U = void>
    static void erase(ThisCol& c, const Iterator& beg, const Iterator& end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static Iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static Iterator end(ConstCol& c) {
        return c.cend();
    }

    static Iterator iterAt(ConstCol& c, long i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator citerAt(ConstCol& c, long i) {
        return c.citerAt(i);
    }

    static ConstValueType& first(ConstCol& c) {
        return c.cfirst();
    }

    static ConstValueType& last(ConstCol& c) {
        return c.clast();
    }

    template <class U = void>
    static void clear(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static bool canAdd(ConstCol& c) {
        return false;
    }
};

}
}

#endif /* end of include guard: VIRTUALCOLLECTION_4I04BTF7 */

