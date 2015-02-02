/*
 * =====================================================================================
 *
 *       Filename:  SelectCollection.h
 *
 *    Description:  Collection that selects something out of other collection
 *
 *        Version:  1.0
 *        Created:  12/14/2014 06:44:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef SELECTCOLLECTION_GQQGQR7T
#define SELECTCOLLECTION_GQQGQR7T

#include <utility>

#include <templatious/CollectionAdapter.h>
#include <templatious/util/Selectors.h>

namespace templatious {

// T - collection to transform
// F - function that selects
// TrType - this collection value type
// StoragePolicy - well... Storage policy
template <class T,class F,class TrType,template <class> class StoragePolicy>
struct SelectCollection {
    template <class InternalIter,class ColType>
    struct IntIterator;

    typedef typename StoragePolicy<T>::Container StorCol;
    typedef typename StoragePolicy<F>::Container StorFunc;
    typedef TrType ValueType;
    typedef const ValueType ConstValueType;
    typedef SelectCollection<T,F,TrType,StoragePolicy> ThisCol;
    typedef const ThisCol ConstCol;
    typedef templatious::adapters::CollectionAdapter<T> Ad;
    typedef IntIterator< typename Ad::Iterator, ThisCol > Iterator;
    typedef IntIterator< typename Ad::ConstIterator, ConstCol > ConstIterator;

    template <class Tc,class Fc>
    SelectCollection(Tc&& c,Fc&& f):
        _c(std::forward<Tc>(c)),
        _f(std::forward<Fc>(f)) {}

    template <class InternalIter,class ColType>
    struct IntIterator {
        typedef IntIterator< InternalIter,ColType > ThisIter;

        // constructor using internal iterator
        template <class V>
        IntIterator(ColType& c,V&& v):
            _ref(c),
            _it(std::forward<V>(v)) {}

        ThisIter& operator++() {
            ++_it;
            return *this;
        }

        ThisIter& operator--() {
            --_it;
            return *this;
        }

        ValueType operator*() {
            return _ref._f.getRef()(*_it);
        }

        std::enable_if<!std::is_reference<ValueType>::value,ValueType>*
        operator->() {
            return std::addressof(_ref._f.getRef()(*_it));
        }

        bool operator==(const ThisIter& rhs) const {
            return _it == rhs._it;
        }

        bool operator!=(const ThisIter& rhs) const {
            return !(*this == rhs);
        }

    private:
        ColType& _ref;
        InternalIter _it;
    };

    Iterator begin() {
        return Iterator(*this,Ad::begin(_c.getRef()));
    }

    Iterator end() {
        return Iterator(*this,Ad::end(_c.getRef()));
    }

    Iterator iterAt(size_t i) {
        return Iterator(*this,Ad::iterAt(_c.getRef(),i));
    }

    ConstIterator cbegin() const {
        return ConstIterator(*this,Ad::cbegin(_c.cgetRef()));
    }

    ConstIterator cend() const {
        return ConstIterator(*this,Ad::cend(_c.cgetRef()));
    }

    Iterator citerAt(size_t i) const {
        return ConstIterator(*this,Ad::citerAt(_c.cgetRef(),i));
    }

    long size() const {
        return Ad::size(_c.cgetRef());
    }

private:
    StorCol _c;
    StorFunc _f;
};

template <bool inferType,class ExplicitType,template <class> class StoragePolicy>
struct SelectCollectionMaker {

    struct InferTypeMaker {

        template <
            class T,class F
        >
        static auto make(T&& t,F&& f)
        -> decltype(
            SelectCollection<
                decltype(std::forward<T>(t)),
                decltype(std::forward<F>(f)),
                decltype(
                    f(
                        *templatious::adapters::
                            CollectionAdapter<T>::begin(
                                std::forward<T>(t)
                            )
                    )
                ),
                StoragePolicy
            >(std::forward<T>(t),std::forward<F>(f))
        )
        {
            return SelectCollection<
                decltype(std::forward<T>(t)),
                decltype(std::forward<F>(f)),
                decltype(
                    f(
                        *templatious::adapters::
                            CollectionAdapter<T>::begin(
                                std::forward<T>(t)
                            )
                    )
                ),
                StoragePolicy
            >(std::forward<T>(t),std::forward<F>(f));
        }

    };

    struct ExplicitTypeMaker {

        template <
            class T,class F
        >
        static auto make(T&& t,F&& f)
        -> decltype(
            SelectCollection<
                decltype(std::forward<T>(t)),
                decltype(std::forward<F>(f)),
                ExplicitType,
                StoragePolicy
            >(std::forward<T>(t),std::forward<F>(f))
        )
        {
            return SelectCollection<
                decltype(std::forward<T>(t)),
                decltype(std::forward<F>(f)),
                ExplicitType,
                StoragePolicy
            >(std::forward<T>(t),std::forward<F>(f));
        }
    };

    typedef typename std::conditional<
        inferType,
        InferTypeMaker,
        ExplicitTypeMaker
    >::type Alg;

};

namespace adapters {

template <class T,class F,class TrType,template <class> class StoragePolicy>
struct CollectionAdapter<
    SelectCollection<T,F,TrType,StoragePolicy>
>{
    static const bool is_valid = true;

    typedef SelectCollection<T,F,TrType,StoragePolicy> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::Iterator Iterator;
    typedef typename ThisCol::ConstIterator ConstIterator;
    typedef typename ThisCol::ValueType ValueType;
    typedef const ValueType ConstValueType;

    static const bool floating_iterator = ThisCol::Ad::floating_iterator;

    template <class V,class U = int>
    static void add(ThisCol& c, V&& i) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "SelectCollection doesn't support addition.");
    }

    template <class V,class U = int>
    static void insertAt(ThisCol& c, Iterator at, V&& i) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "SelectCollection doesn't support insertion.");
    }

    static ValueType getByIndex(ThisCol& c, size_t i) {
        return *c.iterAt(i);
    }

    static ConstValueType getByIndex(ConstCol& c, size_t i) {
        return *c.citerAt(i);
    }

    static long size(ConstCol& c) {
        return c.size();
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator i) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "SelectCollection doesn't support erase operations.");
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "SelectCollection doesn't support erase operations.");
    }

    static Iterator begin(ThisCol& c) {
        return c.begin();
    }

    static Iterator end(ThisCol& c) {
        return c.end();
    }

    static Iterator iterAt(ThisCol& c, size_t i) {
        return c.iterAt(i);
    }

    static ConstIterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator end(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator iterAt(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator citerAt(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    template <class U = int>
    static ValueType first(ThisCol& c) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Getting first element is not supported in this collection.");
    }

    template <class U = int>
    static ConstValueType first(ConstCol& c) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Getting first element is not supported in this collection.");
    }

    template <class U = int>
    static ValueType last(ThisCol& c) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Getting last element is not supported in this collection.");
    }

    template <class U = int>
    static ConstValueType last(ConstCol& c) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Getting last element is not supported in this collection.");
    }

    template <class U = int>
    static void clear(ThisCol& c) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Clearance is not suppoerted in SelectCollection.");
    }

    static bool canAdd(ConstCol& c) {
        return false;
    }
};

template <class T,class F,class TrType,template <class> class StoragePolicy>
struct CollectionAdapter<
    const SelectCollection<T,F,TrType,StoragePolicy>
>{
    static const bool is_valid = true;

    typedef const SelectCollection<T,F,TrType,StoragePolicy> ThisCol;
    typedef ThisCol ConstCol;
    typedef typename ConstCol::ConstIterator Iterator;
    typedef typename ConstCol::ConstIterator ConstIterator;
    typedef typename ConstCol::ConstValueType ValueType;
    typedef ValueType ConstValueType;

    static const bool floating_iterator = ConstCol::Ad::floating_iterator;

    template <class V,class U = int>
    static void add(ConstCol& c, V&& i) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "SelectCollection doesn't support addition.");
    }

    template <class V,class U = int>
    static void insertAt(ConstCol& c, Iterator at, V&& i) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "SelectCollection doesn't support insertion.");
    }

    static ValueType getByIndex(ConstCol& c, size_t i) {
        return *c.citerAt(i);
    }

    static long size(ConstCol& c) {
        return c.size();
    }

    template <class U = int>
    static void erase(ConstCol& c, Iterator i) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "SelectCollection doesn't support erase operations.");
    }

    template <class U = int>
    static void erase(ConstCol& c, Iterator beg, Iterator end) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "SelectCollection doesn't support erase operations.");
    }

    static ConstIterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator end(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator iterAt(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator citerAt(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    template <class U = int>
    static ConstValueType first(ConstCol& c) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Getting first element is not supported in this collection.");
    }

    template <class U = int>
    static ConstValueType last(ConstCol& c) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Getting last element is not supported in this collection.");
    }

    template <class U = int>
    static void clear(ConstCol& c) {
        // suppress static assert unless method is actually called
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Clearance is not suppoerted in SelectCollection.");
    }

    static bool canAdd(ConstCol& c) {
        return false;
    }
};

}
}

#endif /* end of include guard: SELECTCOLLECTION_GQQGQR7T */
