/*
 * =====================================================================================
 *
 *       Filename:  OnceTraversable.h
 *
 *    Description:  Once traversable collection wrapper that mutates iterators
 *
 *        Version:  1.0
 *        Created:  10/31/2014 03:52:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ONCETRAVERSABLE_7K4ACVNE
#define ONCETRAVERSABLE_7K4ACVNE

#include <templatious/CollectionAdapter.h>
#include <templatious/util/Selectors.h>
#include <templatious/util/DefaultStoragePolicy.h>

#include <utility>

namespace templatious {
namespace detail {

template <class T>
struct OnceTraversable {
    typedef OnceTraversable<T> ThisTraversable;

    OnceTraversable(const T& beg,const T& end)
     : _b(beg), _e(end) {}

    struct Iterator {
        Iterator(T& t) : _t(t) {}

        auto operator*()
         -> decltype(*std::declval<T>())
        {
            return *_t;
        }

        Iterator operator++() {
            ++_t;
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return _t == rhs._t;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }
    private:
        T& _t;
    };

    Iterator begin() {
        return Iterator(_b);
    }

    Iterator end() {
        return Iterator(_e);
    }

    T origBegin() {
        return _b;
    }

    T origEnd() {
        return _e;
    }
private:
    T _b;
    T _e;
};

template <
    template <class> class StoragePolicy,
    class T
>
struct OnceTraversableWCol {
    typedef typename StoragePolicy<T>::Container Cont;
    typedef templatious::adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::Iterator ColIterator;
    typedef OnceTraversable< ColIterator > ThisTraversable;
    typedef typename ThisTraversable::Iterator IntIterator;

    template <class V>
    OnceTraversableWCol(V&& c)
        : _c(std::forward<V>(c)),
          _t(Ad::begin(_c.getRef()),
             Ad::end(_c.getRef())) {}

    IntIterator begin() {
        return _t.begin();
    }

    IntIterator end() {
        return _t.end();
    }

private:
    Cont _c;
    ThisTraversable _t;
};

struct OnceTraversableMakerLval {

    template <
        template <class> class StoragePolicy,
        class T
    >
    static auto make(T&& t)
     -> OnceTraversable<
        typename ::templatious::adapters::
            CollectionAdapter<T>::Iterator
     >
    {
        typedef ::templatious::adapters::
            CollectionAdapter<T> Ad;
        typedef OnceTraversable<
            typename Ad::Iterator
        > Res;

        return Res(
            Ad::begin(std::forward<T>(t)),
            Ad::end(std::forward<T>(t))
        );
    }

};

struct OnceTraversableMakerRval {

    template <
        template <class> class StoragePolicy,
        class T
    >
    static auto make(T&& t)
     -> OnceTraversableWCol<
         StoragePolicy,
         decltype(std::forward<T>(t))
     >
    {
        typedef OnceTraversableWCol<
            StoragePolicy,
            decltype(std::forward<T>(t))
        > Res;
        return Res(std::forward<T>(t));
    }

};

template <
    template <class> class StoragePolicy,
    class T
>
struct OnceTraversableMaker {
    typedef typename std::conditional<
        std::is_rvalue_reference<T>::value,
        OnceTraversableMakerRval,
        OnceTraversableMakerLval
    >::type Maker;
};

template <
    template <class> class StoragePolicy,
    class T
>
auto makeOnceTraversable(T&& t)
 -> decltype(
     OnceTraversableMaker<
         StoragePolicy,decltype(std::forward<T>(t))
     >::Maker::template make<StoragePolicy>(std::forward<T>(t))
 )
{
    return OnceTraversableMaker<
        StoragePolicy,decltype(std::forward<T>(t))
    >::Maker::template make<StoragePolicy>(std::forward<T>(t));
}

}

namespace adapters {

template <class T>
struct CollectionAdapter<
    ::templatious::detail::OnceTraversable<T>
>{

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef ::templatious::detail::OnceTraversable<T> ThisCol;
    typedef ::templatious::detail::OnceTraversable<const T> ConstCol;
    typedef typename ThisCol::Iterator Iterator;
    typedef typename ConstCol::Iterator ConstIterator;
    typedef decltype(
        *(std::declval<ThisCol>().begin())
    ) ValueType;
    typedef decltype(
        *(std::declval<ConstCol>().begin())
    ) ConstValueType;

    template <class V,class U = int>
    static void add(ThisCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class V,class U = int>
    static void insert(ThisCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static ValueType& getByIndex(ThisCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static ConstValueType& getByIndex(ConstCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static int size(ConstCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
        return -1;
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static ThisCol instantiate() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol instantiate(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol* instHeap() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol* instHeap(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    static Iterator begin(ThisCol& c) {
        return c.begin();
    }

    static Iterator end(ThisCol& c) {
        return c.end();
    }

    template <class U = int>
    static Iterator iterAt(ThisCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be iterated "
                "from begin to end.");
    }

    static Iterator begin(ConstCol& c) {
        return c.begin();
    }

    static Iterator end(ConstCol& c) {
        return c.end();
    }

    template <class U = int>
    static Iterator iterAt(ConstCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be iterated "
                "from begin to end.");
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.begin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.end();
    }

    template <class U = int>
    static ConstIterator citerAt(ConstCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be iterated "
                "from begin to end.");
    }

    template <class U = int>
    static ValueType& first(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable collection elements "
                "can only be accessed through iterators.");
    }

    template <class U = int>
    static ConstValueType& first(ConstCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable collection elements "
                "can only be accessed through iterators.");
    }

    template <class U = int>
    static ValueType& last(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable collection elements "
                "can only be accessed through iterators.");
    }

    template <class U = int>
    static ConstValueType& last(ConstCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable collection elements "
                "can only be accessed through iterators.");
    }

    template <class U = int>
    static void clear(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable cannot be cleared.");
    }

    static bool canAdd(ConstCol& c) {
        return false;
    }

};

template <
    template <class> class StoragePolicy,
    class T
>
struct CollectionAdapter<
    ::templatious::detail::OnceTraversableWCol< StoragePolicy, T >
>{

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef ::templatious::detail::OnceTraversableWCol<
        StoragePolicy, T > ThisCol;
    typedef ::templatious::detail::OnceTraversableWCol<
        StoragePolicy, const T > ConstCol;
    typedef typename ThisCol::IntIterator Iterator;
    typedef typename ConstCol::IntIterator ConstIterator;
    typedef decltype(
        *(std::declval<ThisCol>().begin())
    ) ValueType;
    typedef decltype(
        *(std::declval<ConstCol>().begin())
    ) ConstValueType;

    template <class V,class U = int>
    static void add(ThisCol& c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class V,class U = int>
    static void insert(ThisCol& c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static ValueType& getByIndex(ThisCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static int size(ConstCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
        return -1;
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static ThisCol instantiate() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol instantiate(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol* instHeap() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol* instHeap(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    static Iterator begin(ThisCol& c) {
        return c.begin();
    }

    static Iterator end(ThisCol& c) {
        return c.end();
    }

    template <class U = int>
    static Iterator iterAt(ThisCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be iterated "
                "from begin to end.");
    }

    template <class U = int>
    static ConstIterator cbegin(ConstCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Not yet implemented...");
    }

    template <class U = int>
    static ConstIterator cend(ConstCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Not yet implemented...");
    }

    template <class U = int>
    static ConstIterator citerAt(ConstCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be iterated "
                "from begin to end.");
    }

    template <class U = int>
    static ValueType& first(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable collection elements "
                "can only be accessed through iterators.");
    }

    template <class U = int>
    static ValueType& last(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable collection elements "
                "can only be accessed through iterators.");
    }

    template <class U = int>
    static void clear(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable cannot be cleared.");
    }

    static bool canAdd(ConstCol& c) {
        return false;
    }

};

template <
    template <class> class StoragePolicy,
    class T
>
struct CollectionAdapter<
    ::templatious::detail::OnceTraversableWCol< StoragePolicy, const T >
>{

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef ::templatious::detail::OnceTraversableWCol<
        StoragePolicy, const T > ThisCol;
    typedef ::templatious::detail::OnceTraversableWCol<
        StoragePolicy, const T > ConstCol;
    typedef typename ThisCol::IntIterator Iterator;
    typedef typename ConstCol::IntIterator ConstIterator;
    typedef decltype(
        *(std::declval<ThisCol>().begin())
    ) ValueType;
    typedef decltype(
        *(std::declval<ConstCol>().begin())
    ) ConstValueType;

    template <class V,class U = int>
    static void add(ThisCol& c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class V,class U = int>
    static void insert(ThisCol& c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static ValueType& getByIndex(ThisCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static int size(ConstCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
        return -1;
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class U = int>
    static ThisCol instantiate() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol instantiate(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol* instHeap() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    template <class U = int>
    static ThisCol* instHeap(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be instantiated"
                " out of another collection.");
    }

    static Iterator begin(ThisCol& c) {
        return c.begin();
    }

    static Iterator end(ThisCol& c) {
        return c.end();
    }

    template <class U = int>
    static Iterator iterAt(ThisCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be iterated "
                "from begin to end.");
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.begin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.end();
    }

    template <class U = int>
    static ConstIterator citerAt(ConstCol& c, size_t i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable can only be iterated "
                "from begin to end.");
    }

    template <class U = int>
    static ValueType& first(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable collection elements "
                "can only be accessed through iterators.");
    }

    template <class U = int>
    static ValueType& last(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable collection elements "
                "can only be accessed through iterators.");
    }

    template <class U = int>
    static void clear(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable cannot be cleared.");
    }

    static bool canAdd(ConstCol& c) {
        return false;
    }

};

}
}

#endif /* end of include guard: ONCETRAVERSABLE_7K4ACVNE */
