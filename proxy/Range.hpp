//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Range.hpp
 *
 *    Description:  Range class
 *
 *        Version:  1.0
 *        Created:  07/30/2014 06:01:51 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef RANGE_KFIVLD23
#define RANGE_KFIVLD23

#include <utility>

#include <templatious/util/IteratorTagExtractor.hpp>
#include <templatious/CollectionAdapter.hpp>
#include <templatious/proxy/Picker.hpp>

namespace templatious {

TEMPLATIOUS_BOILERPLATE_EXCEPTION( RangeBeginMoreThanEndException,
    "Range beginning cannot be greater than end.");

template <class T, template <class> class StoragePolicy>
struct Range {

    template <class I>
    struct PIterator;

    typedef typename adapters::CollectionAdapter<T> Ad;
    typedef PIterator<typename Ad::Iterator> Iterator;
    typedef PIterator<typename Ad::ConstIterator> ConstIterator;
    typedef detail::IsProxy<T> ProxUtil;
    typedef typename ProxUtil::ICollection ICollection;
    typedef Range<T,StoragePolicy> ThisRange;

    static const bool proxy_inside = ProxUtil::val;
    static const bool random_access_iterator = ProxUtil::random_access_iterator;
    static const bool floating_iterator = Ad::floating_iterator;

    static_assert(Ad::is_valid,"Adapter is invalid.");

    typedef typename StoragePolicy<T>::Container Ref;

    template <class V>
    friend struct detail::IsProxy;

#ifndef TEMPLATIOUS_TESTING
private:
#endif

    Ref _c;
    Iterator _b;
    Iterator _e;
    bool _cleared;


    void assertUncleared() const {
        if (_cleared) {
            throw detail::ProxyClearedUsageException();
        }
    }

    void tagCleared() {
        _cleared = true;
    }

#ifndef TEMPLATIOUS_TESTING
public:
#endif

    template <class V>
    Range(V&& v,const Iterator& b,const Iterator& e) :
        _c(std::forward<V>(v)), _b(b), _e(e), _cleared(false)
    { }

    template <class V>
    Range(V&& v,const Iterator& b) :
        _c(std::forward<V>(v)), _b(b),
        _e(Ad::end(_c.getRef())), _cleared(false) {}

    Range(ThisRange&& r) :
        _c(r._c.cpy()),
        _b(r._b),
        _e(r._e),
        _cleared(r._cleared)
    { }

    Iterator begin() {
        assertUncleared();
        return _b;
    }

    Iterator end() {
        assertUncleared();
        return _e;
    }

    ConstIterator cbegin() const {
        assertUncleared();
        return ConstIterator(
            ProxUtil::const_iter_cast(_b._i)
        );
    }

    ConstIterator cend() const {
        assertUncleared();
        return ConstIterator(
            ProxUtil::const_iter_cast(_e._i)
        );
    }

    Iterator iterAt(long n) {
        assertUncleared();
        if (!random_access_iterator) {
            Iterator res(_b);
            detail::naiveIterAdvance(res,_e,n);
            return res;
        } else {
            auto i = iterUnwrap(_b);
            auto e = iterUnwrap(_e);
            static const bool isNaiveAdvance =
                !util::IsRandomAccessIteratorTagged<decltype(i)>::value;
            typedef detail::AdvancePicker<isNaiveAdvance> A;
            long mul = ProxUtil::get_mul(_c.getRef());
            A::adv(i,e,mul * n);
            return Iterator(i);
        }
    }

    long size() const {
        if (!_cleared) {
            return -1;
        } else {
            return 0;
        }
    }

    template <class I>
    struct PIterator {
    private:
        I _i;

        template <class A1, template <class> class A2>
        friend struct Range;

        template <class V>
        friend struct detail::IsProxy;
    public:

        typedef PIterator<I> ThisIter;
        typedef decltype(*_i) IVal;
        static const bool is_rvalue = std::is_rvalue_reference<IVal>::value;

        PIterator(const I& i) : _i(i) {}

        PIterator& operator=(const PIterator& rhs) {
            _i = rhs._i;
            return *this;
        }

        ThisIter& operator++() {
            ++_i;
            return *this;
        }

        ThisIter& operator--() {
            --_i;
            return *this;
        }

        bool operator==(const ThisIter& rhs) const {
            return _i == rhs._i;
        }

        bool operator!=(const ThisIter& rhs) const {
            return !(*this == rhs);
        }

        auto operator*()
            -> decltype(*std::declval<I&>())
        {
            return *(this->_i);
        }

        auto operator*() const
            -> decltype(*std::declval<const I&>())
        {
            return *(this->_i);
        }

        template <bool Rval = is_rvalue>
        auto operator->()
            -> typename std::enable_if<
                !Rval,decltype(std::addressof(*(this->_i)))
            >::type
        const {
            return std::addressof(*(this->_i));
        }

        auto getInternal()
            -> decltype(ProxUtil::iter_unwrap(_i))
        {
            return ProxUtil::iter_unwrap(_i);
        }

    };

    void clear() {
        detail::clearRoutine<floating_iterator>(*this);
        tagCleared();
        ProxUtil::tag_cleared(_c.getRef());
        _b = _e;
    }

    auto getInternal()
        -> decltype(ProxUtil::unwrap(_c.getRef()))
    {
        return ProxUtil::unwrap(_c.getRef());
    }

    long getMul() const {
        return ProxUtil::get_mul(_c.cgetRef());
    }

    template <class V>
    static auto iterUnwrap(V&& v)
        -> decltype(ProxUtil::iter_unwrap(
            std::forward<V>(v)))
    {
        return ProxUtil::iter_unwrap(
            std::forward<V>(v)
        );
    }
};

namespace detail {

template <class T, template <class> class StoragePolicy>
struct IsProxy< Range< T, StoragePolicy > > {
    typedef IsProxy<T> Internal;
    typedef Range<T,StoragePolicy> ThisCol;
    typedef typename ThisCol::ConstIterator ConstIterator;
    static const bool val = true;
    static const bool random_access_iterator =
        IsProxy< T >::random_access_iterator;

    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename
        adapters::CollectionAdapter<ICollection> IAdapter;

    template <class C>
    static auto unwrap(C&& c)
        -> decltype(c.getInternal())&
    {
        return c.getInternal();
    }

    template <class C>
    static auto iter_unwrap(C&& c)
        -> decltype(c.getInternal())&
    {
        return c.getInternal();
    }

    typedef long Dist;
    template <class U>
    static Dist get_mul(U&& u) {
        return u.getMul();
    }

    template <class U>
    static void tag_cleared(U& u) {
        u.tagCleared();
    }

    template <class Iter>
    static auto const_iter_cast(Iter&& i)
     -> decltype(
        ConstIterator(
            Internal::const_iter_cast(i._i)
        )
     )
    {
        return ConstIterator(
            Internal::const_iter_cast(i._i)
        );
    }
};

}

namespace adapters {

template <class T, template <class> class StoragePolicy>
struct CollectionAdapter< Range<T, StoragePolicy> > {

    static const bool is_valid = true;

    typedef Range<T, StoragePolicy> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::Iterator Iterator;
    typedef typename ThisCol::ConstIterator ConstIterator;
    typedef typename ThisCol::Ad::ValueType ValueType;
    typedef typename ThisCol::Ad::ConstValueType ConstValueType;

    static const bool floating_iterator = ThisCol::Ad::floating_iterator;

    template <class C>
    static Iterator begin(C&& c) {
        return c.begin();
    }

    template <class C>
    static Iterator end(C&& c) {
        return c.end();
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    template <class C>
    static Iterator iterAt(C&& c,long i) {
        return c.iterAt(i);
    }

    template <class C,class V>
    static void insertAt(C&& c,Iterator i,V&& v) {
        static_assert(templatious::util::DummyResolver<C,false>::val,
            "Range proxy class is uninsertable.");
    }

    template <class C>
    static void erase(C&& c, Iterator i) {
        static_assert(templatious::util::DummyResolver<C,false>::val,
            "Range proxy class is unerasable.");
    }

    template <class C>
    static void erase(C&& c, Iterator beg, Iterator end) {
        static_assert(templatious::util::DummyResolver<C,false>::val,
            "Range proxy class is unerasable.");
    }

    template <class C>
    static void clear(C&& c) {
        c.clear();
    }

    template <class C>
    static long size(C&& c) {
        return c.size();
    }

};

}
}

#endif /* end of include guard: RANGE_KFIVLD23 */
