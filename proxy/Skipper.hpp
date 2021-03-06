//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Skipper.hpp
 *
 *    Description:  Skip elements in increments
 *
 *        Version:  1.0
 *        Created:  08/07/2014 04:37:16 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef SKIPPER_8SYZBPCX
#define SKIPPER_8SYZBPCX

#include <utility>

#include <templatious/util/Exceptions.hpp>
#include <templatious/util/IteratorTagExtractor.hpp>
#include <templatious/CollectionAdapter.hpp>
#include <templatious/proxy/Picker.hpp>

namespace templatious {

TEMPLATIOUS_BOILERPLATE_EXCEPTION( SkipperInvalidAssignmentException,
    "Skipper iterator can only be assigned iterator from same original collection.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION( SkipperNonPositiveStepException,
    "Skipper can only be assigned positive step >= 1.");

template <class T,template <class> class StoragePolicy>
struct Skipper {

    template <class I,class Parent>
    struct PIterator;

    typedef typename adapters::CollectionAdapter<T> Ad;
    typedef Skipper<T,StoragePolicy> ThisSkipper;
    typedef const ThisSkipper ConstSkipper;

    typedef PIterator<typename Ad::Iterator,ThisSkipper> Iterator;
    typedef PIterator<typename Ad::ConstIterator,ConstSkipper> ConstIterator;
    typedef detail::IsProxy<T> ProxUtil;
    typedef typename ProxUtil::ICollection ICollection;

    typedef StaticAdapter SA;

    static const bool proxy_inside = ProxUtil::val;
    static const bool random_access_iterator = ProxUtil::random_access_iterator;
    static const bool floating_iterator = Ad::floating_iterator;

    static_assert(Ad::is_valid,"Adapter is invalid.");

    typedef typename StoragePolicy<T>::Container Ref;

#ifndef TEMPLATIOUS_TESTING
private:
#endif

    Ref _c;
    Iterator _b;
    Iterator _e;
    long _sk; // - skip size
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
    Skipper(V&& v,long sz) :
        _c(std::forward<V>(v)),
        _b(*this,SA::begin(_c.getRef()),sz),
        _e(*this,SA::end(_c.getRef()),sz),
        _sk(sz), _cleared(false)
    {
        if (_sk <= 0) {
            throw SkipperNonPositiveStepException();
        }
    }

    Skipper(ThisSkipper&& s) :
        _c(s._c.cpy()),
        _b(*this,SA::begin(_c.getRef()),s._sk),
        _e(*this,SA::end(_c.getRef()),s._sk),
        _sk(s._sk), _cleared(s._cleared)
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
            _b._p,
            ProxUtil::const_iter_cast(_b._i),
            _b._sk
        );
    }

    ConstIterator cend() const {
        assertUncleared();
        return ConstIterator(
            _e._p,
            ProxUtil::const_iter_cast(_e._i),
            _e._sk
        );
    }

    Iterator iterAt(long n) {
        assertUncleared();
        if (!random_access_iterator) {
            Iterator res(_b);
            detail::naiveIterAdvance(res,_e,n);
            return res;
        } else {
            auto i = _b._i;
            auto e = _e._i;
            static const bool isNaiveAdvance =
                !util::IsRandomAccessIteratorTagged<decltype(i)>::value;
            typedef detail::AdvancePicker<isNaiveAdvance> A;
            long mul = _sk * ProxUtil::get_mul(_c.getRef());
            A::adv(i,e,mul * n);
            return Iterator(*this,i,_sk);
        }
    }

    long size() const {
        if (!_cleared) {
            return -1;
        } else {
            return 0;
        }
    }

    template <class I,class Parent>
    struct PIterator {
    private:
        typedef typename Parent::ProxUtil ProxUtil;

        Parent& _p;
        I _i;
        long _sk;

        static const bool random_access_iterator =
            Parent::random_access_iterator;

        friend struct Skipper<T,StoragePolicy>;

        template <class V>
        friend struct detail::IsProxy;
    public:
        typedef PIterator<I,Parent> ThisIter;
        typedef decltype(*_i) IVal;
        static const bool is_rvalue =
            std::is_rvalue_reference<IVal>::value
            || !std::is_reference<IVal>::value;
        typedef typename std::conditional< !is_rvalue,
            I,void**>::type PtrDerefType;

        PIterator(Parent& p,const I& i,long sz) :
            _p(p),
            _i(i),
            _sk(sz)
        {}

        PIterator& operator=(const PIterator& rhs) {
            _i = rhs._i;
            _sk = rhs._sk;

            if (std::addressof(_p) != std::addressof(rhs._p)) {
                throw SkipperInvalidAssignmentException();
            }
            return *this;
        }

        ThisIter& operator++() {
            if (!random_access_iterator) {
                detail::naiveIterAdvance(_i,
                    SA::end(_p)._i,
                    _sk);
            } else {
                auto i = iterUnwrap(_i);
                auto e = SA::end(_p).getInternal();
                typedef detail::AdvancePicker<!random_access_iterator> A;
                long mul = ProxUtil::get_mul(_p);
                A::adv(i,e,mul * _sk);
                ProxUtil::iter_unwrap(_i) = i;
            }
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
                !Rval,decltype(std::addressof(*(std::declval<PtrDerefType>())))
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

template <class T,template <class> class StoragePolicy>
struct IsProxy< Skipper< T,StoragePolicy > > {
    typedef IsProxy<T> Internal;
    static const bool val = true;

    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename
        adapters::CollectionAdapter<ICollection> IAdapter;

    typedef Skipper<T,StoragePolicy> ThisCol;
    typedef typename ThisCol::ConstIterator ConstIterator;

    static const bool random_access_iterator
        = Internal::random_access_iterator;

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
            i._p,
            Internal::const_iter_cast(i._i),
            i._sk
        )
     )
    {
        return ConstIterator(
            i._p,
            Internal::const_iter_cast(i._i),
            i._sk
        );
    }
};

}

namespace adapters {

template <class T,template <class> class StoragePolicy>
struct CollectionAdapter< Skipper<T,StoragePolicy> > {

    static const bool is_valid = true;

    typedef Skipper<T,StoragePolicy> ThisCol;
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

    template <class V>
    static Iterator iterAt(V&& c,long i) {
        return c.iterAt(i);
    }

    template <class C,class V>
    static void insertAt(C&& c,Iterator i,V&& v) {
        c.insert(i,std::forward<V>(v));
    }

    template <class C>
    static void erase(C&& c, Iterator i) {
        c.erase(i);
    }

    template <class C>
    static void erase(C&& c, Iterator beg, Iterator end) {
        c.erase(beg,end);
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

#endif /* end of include guard: SKIPPER_8SYZBPCX */
