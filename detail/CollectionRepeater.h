/*
 * =====================================================================================
 *
 *       Filename:  CollectionRepeater.h
 *
 *    Description:  Utility collection to repeat collections
 *
 *        Version:  1.0
 *        Created:  11/05/2014 05:28:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef COLLECTIONREPEATER_20IX8ASF
#define COLLECTIONREPEATER_20IX8ASF

#include <utility> // size_t, std::forward

#include <templatious/CollectionAdapter.h>
#include <templatious/util/Selectors.h>

namespace templatious {
namespace detail {

template <
    bool IsMultiply,
    template <class> class StoragePolicy,
    class T
>
struct Repeater {
    typedef typename StoragePolicy<T>::Container Container;
    typedef Repeater<IsMultiply,StoragePolicy,T> ThisRepeater;
    typedef templatious::adapters::CollectionAdapter<T> Adapter;
    typedef typename Adapter::Iterator InternalIterator;
    typedef typename Adapter::ConstIterator CInternalIterator;

    template <class V>
    Repeater(size_t n,V&& v)
     : _n(n), _c(std::forward<V>(v)) {}

    template <class IIterType,class IRefType>
    struct Iterator {
        typedef templatious::adapters::
            CollectionAdapter<IRefType> IAdapter;

        struct AbsoluteAdvance {
            static void adv(Iterator& i) {
                ++i._i;
                ++i._idx;
                if (IAdapter::end(i._r) == i._i) {
                    i._i = IAdapter::begin(i._r);
                }
            }
        };

        struct MultiplyAdvance {
            static void adv(Iterator& i) {
                ++i._i;
                if (IAdapter::end(i._r) == i._i) {
                    i._i = IAdapter::begin(i._r);
                    ++i._idx;
                }
            }
        };

        typedef typename std::conditional<
            IsMultiply,
            MultiplyAdvance,
            AbsoluteAdvance
        >::type AdvAlg;

        Iterator(IRefType r,
                 const IIterType& i)
            : _r(r), _i(i), _idx(0) {}

        Iterator(IRefType r,
                 const IIterType& i,
                 size_t start)
            : _r(r), _i(i), _idx(start) {}

        Iterator& operator++() {
            AdvAlg::adv(*this);
            return *this;
        }

        auto operator*()
         -> decltype(*std::declval<IIterType>())
        {
            return *_i;
        }

        bool operator==(const Iterator& rhs) const {
            return _idx == rhs._idx;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

    private:
        IRefType _r;
        IIterType _i;
        size_t _idx;
    };

    auto begin()
     -> Iterator<
         typename Adapter::Iterator,
         decltype(std::declval<Container>().getRef())
     >
    {
        return Iterator<
            typename Adapter::Iterator,
            decltype(_c.getRef())
        >(
            _c.getRef(),
            Adapter::begin(_c.getRef()),
            0
        );
    }

    auto end()
     -> Iterator<
         typename Adapter::Iterator,
         decltype(std::declval<Container>().getRef())
     >
    {
        return Iterator<
            typename Adapter::Iterator,
            decltype(_c.getRef())
        >(
            _c.getRef(),
            Adapter::end(_c.getRef()),
            _n
        );
    }

    auto cbegin() const
     -> Iterator<
         decltype(Adapter::cbegin(
             std::declval<Container>().cgetRef())),
         decltype(std::declval<Container>().cgetRef())
     >
    {
        return Iterator<
            decltype(Adapter::cbegin(_c.cgetRef())),
            decltype(_c.cgetRef())
        >(
            _c.cgetRef(),
            Adapter::cbegin(_c.cgetRef()),
            0
        );
    }

    auto cend() const
     -> Iterator<
         decltype(Adapter::cend(
             std::declval<Container>().cgetRef())),
         decltype(std::declval<Container>().cgetRef())
     >
    {
        return Iterator<
            decltype(Adapter::cend(_c.cgetRef())),
            decltype(_c.cgetRef())
        >(
            _c.cgetRef(),
            Adapter::cend(_c.cgetRef()),
            _n
        );
    }

    template <class Dummy = int>
    int size() const {
        static_assert(templatious::util::
            DummyResolver<Dummy,!IsMultiply>::val,
            "Multiply collection repeater cannot"
            " predict exact collection size.");

        return _n;
    }

    typedef Iterator<InternalIterator,
        decltype(std::declval<Container>().getRef())
    > IteratorT;
    typedef Iterator<CInternalIterator,
        decltype(std::declval<Container>().cgetRef())
    > CIteratorT;

private:
    size_t _n;
    Container _c;
};

}

namespace adapters {

template <
    bool IsMultiply,
    template <class> class StoragePolicy,
    class T
>
struct CollectionAdapter<
    templatious::detail::Repeater<
        IsMultiply,
        StoragePolicy,
        T
    >
> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef templatious::detail::Repeater<
        IsMultiply,
        StoragePolicy,
        T
    > ThisCol;

    typedef const ThisCol ConstCol;
    typedef ThisCol& RCol;
    typedef ConstCol& CRCol;
    typedef typename ThisCol::IteratorT Iterator;
    typedef typename ConstCol::CIteratorT ConstIterator;
    typedef typename Inner::ValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class U = int,class V>
    static void add(RCol c, V&& i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int,class V>
    static void insertAt(RCol c,Iterator at,V&& i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ValueType& getByIndex(RCol c,size_t i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ConstValueType& getByIndex(CRCol c, size_t i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    static int size(RCol c) {
        return c.size();
    }

    static int size(CRCol c) {
        return c.size();
    }

    template <class U = int>
    static void erase(RCol c, Iterator beg) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static void erase(RCol c, Iterator beg, Iterator end) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static bool instantiate()
    {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be instantiated"
            " from other collection only.");
        return false;
    }

    template <class U = int>
    static bool instantiate(size_t size)
    {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be instantiated"
            " from other collection only.");
        return false;
    }

    static Iterator begin(RCol c) {
        return c.begin();
    }

    static Iterator end(RCol c) {
        return c.end();
    }

    static ConstIterator begin(CRCol c) {
        return c.cbegin();
    }

    static ConstIterator end(CRCol c) {
        return c.cend();
    }

    template <class U = int>
    static Iterator iterAt(RCol c,size_t i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    static ConstIterator cbegin(CRCol c) {
        return c.cbegin();
    }

    static ConstIterator cend(CRCol c) {
        return c.cend();
    }

    template <class U = int>
    static ConstIterator citerAt(RCol c,size_t i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ValueType& first(RCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ConstValueType& first(CRCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ValueType& last(RCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ConstValueType& last(CRCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static void clear(RCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    static bool canAdd(CRCol c) {
        return false;
    }

};

template <
    bool IsMultiply,
    template <class> class StoragePolicy,
    class T
>
struct CollectionAdapter<
    const templatious::detail::Repeater<
        IsMultiply,
        StoragePolicy,
        T
    >
> {
    typedef CollectionAdapter<T> Inner;
    static const bool is_valid = Inner::is_valid;
    static const bool floating_iterator = Inner::floating_iterator;

    typedef const templatious::detail::Repeater<
        IsMultiply,
        StoragePolicy,
        T
    > ThisCol;

    typedef ThisCol ConstCol;
    typedef ThisCol& RCol;
    typedef ConstCol& CRCol;
    typedef typename ThisCol::IteratorT Iterator;
    typedef typename ConstCol::CIteratorT ConstIterator;
    typedef typename Inner::ValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class U = int,class V>
    static void add(RCol c, V&& i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int,class V>
    static void insertAt(RCol c,Iterator at,V&& i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ValueType& getByIndex(RCol c,size_t i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ConstValueType& getByIndex(CRCol c, size_t i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    static int size(RCol c) {
        return c.size();
    }

    template <class U = int>
    static void erase(RCol c, Iterator beg) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static void erase(RCol c, Iterator beg, Iterator end) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static bool instantiate()
    {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be instantiated"
            " from other collection only.");
        return false;
    }

    template <class U = int>
    static bool instantiate(size_t size)
    {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be instantiated"
            " from other collection only.");
        return false;
    }

    static Iterator begin(RCol c) {
        return c.begin();
    }

    static Iterator end(RCol c) {
        return c.end();
    }

    template <class U = int>
    static Iterator iterAt(RCol c,size_t i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    static ConstIterator cbegin(RCol c) {
        return c.cbegin();
    }

    static ConstIterator cend(RCol c) {
        return c.cend();
    }

    template <class U = int>
    static ConstIterator citerAt(RCol c,size_t i) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ValueType& first(RCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ConstValueType& first(CRCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ValueType& last(RCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static ConstValueType& last(CRCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    template <class U = int>
    static void clear(RCol c) {
        static_assert(templatious::util::
            DummyResolver<U,false>::val,
            "Collection repeater can be traversed only.");
    }

    static bool canAdd(CRCol c) {
        return false;
    }

};

}
}

#endif /* end of include guard: COLLECTIONREPEATER_20IX8ASF */

