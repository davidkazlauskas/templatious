//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Filter.hpp
 *
 *    Description:  Filter class
 *
 *        Version:  1.0
 *        Created:  08/04/2014 07:37:01 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef FILTER_O8Y22ICC
#define FILTER_O8Y22ICC

#include <utility>

#include <templatious/util/Exceptions.hpp>
#include <templatious/CollectionAdapter.hpp>
#include <templatious/proxy/Picker.hpp>

namespace templatious {

TEMPLATIOUS_BOILERPLATE_EXCEPTION( FilterPastEndIterationException,
    "Trying to iterate past end of filter.");

template <class T,class Fn,template <class> class StoragePolicy>
struct Filter {

    template <class I,class Fun>
    struct PIterator;

    typedef typename adapters::CollectionAdapter<T> Ad;
    typedef PIterator<typename Ad::Iterator,Fn> Iterator;
    typedef PIterator<typename Ad::ConstIterator,Fn> ConstIterator;
    typedef detail::IsProxy<T> ProxUtil;
    typedef typename ProxUtil::ICollection ICollection;
    typedef Filter<T,Fn,StoragePolicy> ThisFilter;

    static const bool proxy_inside = ProxUtil::val;
    static const bool floating_iterator = Ad::floating_iterator;
    static const bool random_access_iterator = false;

    static_assert(Ad::is_valid,"Adapter is invalid.");

    typedef typename StoragePolicy<T>::Container Ref;
    typedef typename StoragePolicy<Fn>::Container Fun;

#ifndef TEMPLATIOUS_TESTING
private:
#endif

    Ref _c;
    Fun _fn;
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

    template <class V,class FnRef>
    Filter(V&& v,FnRef&& fn) :
        _c(std::forward<V>(v)),
        _fn(std::forward<FnRef>(fn)),
        _b(Ad::begin(_c.getRef()),
            Ad::end(_c.getRef()),
            std::forward<FnRef>(fn)),
        _e(Ad::end(_c.getRef()),
            Ad::end(_c.getRef()),
            std::forward<FnRef>(fn)),
        _cleared(false)
    {
        bool begEqEnd = _b == _e;
        if (!begEqEnd && !_fn.getRef()(*_b)) {
            ++_b;
        }
    }

    Filter(ThisFilter&& other)
        : _c(other._c.cpy()),
          _fn(other._fn.cpy()),
          _b(Ad::begin(_c.getRef()),
              Ad::end(_c.getRef()),
              _fn.getRef()),
          _e(Ad::end(_c.getRef()),
              Ad::end(_c.getRef()),
              _fn.cpy()),
          _cleared(other._cleared)
    {
        bool begEqEnd = _b == _e;
        if (!begEqEnd && !_fn.getRef()(*_b)) {
            ++_b;
        }
    }

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
            ProxUtil::const_iter_cast(_b._i),
            ProxUtil::const_iter_cast(_b._e),
            _b._fn
        );
    }

    ConstIterator cend() const {
        assertUncleared();
        return ConstIterator(
            ProxUtil::const_iter_cast(_e._i),
            ProxUtil::const_iter_cast(_e._e),
            _e._fn
        );
    }

    Iterator iterAt(size_t i) {
        assertUncleared();
        auto res(_b);
        detail::naiveIterAdvance(res,_e,i);
        return res;
    }

    long size() const {
        if (!_cleared) {
            return -1;
        } else {
            return 0;
        }
    }

    template <class I,class Fun>
    struct PIterator {
    private:
        I _i;
        I _e;
        typedef typename StoragePolicy<Fn>::Container Func;
        Func _fn;

        friend struct Filter<T,Fun,StoragePolicy>;

        template <class V>
        friend struct detail::IsProxy;
    public:

        typedef PIterator<I,Fun> ThisIter;
        typedef decltype(*_i) IVal;

        template <class V>
        PIterator(const I& i,const I& e,V&& fn) :
            _i(i), _e(e), _fn(std::forward<V>(fn)) {}

        PIterator& operator=(const PIterator& rhs) {
            _i = rhs._i;
            _e = rhs._e;
            return *this;
        }

        ThisIter& operator++() {
            if (_e == _i) {
                throw FilterPastEndIterationException();
            }

            do {
                ++_i;
            } while (_e != _i && !_fn.getRef()(*_i));
            return *this;
        }

        bool operator==(const ThisIter& rhs) const {
            return _i == rhs._i;
        }

        bool operator!=(const ThisIter& rhs) const {
            return !(*this == rhs);
        }

        auto operator*() -> decltype(
            *std::declval<ThisIter>()._i)
        {
            return *(this->_i);
        }

        auto operator*() const -> decltype(
            *std::declval<ThisIter>()._i)
        {
            return *(this->_i);
        }

        auto operator->()
            -> decltype(&(this->_i))
        const {
            return &(this->_i);
        }

        auto getInternal()
            -> decltype(ProxUtil::iter_unwrap(_i))&
        {
            return ProxUtil::iter_unwrap(_i);
        }

    };

    void clear() {
        detail::clearRoutine<floating_iterator>(*this);
        tagCleared();
        ProxUtil::tag_cleared(_c.getRef());
        _b._i = _e._i;
    }

    auto getInternal()
        -> decltype(ProxUtil::unwrap(_c.getRef()))&
    {
        return ProxUtil::unwrap(_c.getRef());
    }

    template <class V>
    static auto iterUnwrap(V&& v)
        -> decltype(ProxUtil::iter_unwrap(
            std::forward<V>(v)))&
    {
        return ProxUtil::iter_unwrap(
            std::forward<V>(v)
        );
    }

};

namespace detail {

template <class T,class Fn,template <class> class StoragePolicy>
struct IsProxy< Filter< T,Fn,StoragePolicy > > {
    typedef IsProxy<T> Internal;
    typedef Filter<T,Fn,StoragePolicy> ThisCol;
    typedef typename ThisCol::ConstIterator ConstIterator;
    static const bool val = true;
    static const bool random_access_iterator = false;

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

    template <class C>
    static void iter_advance(C& i,C& e,size_t s) {
        naiveIterAdvance(i,e,s);
    }

    template <class C>
    static long get_mul(C&& a) {
        return -1;
    }

    template <class U>
    static void tag_cleared(U& u) {
        u.tagCleared();
    }

    template <class Iter>
    static auto const_iter_cast(Iter&& i)
     -> decltype(
        ConstIterator(
            Internal::const_iter_cast(i._i),
            Internal::const_iter_cast(i._e),
            i._fn
        )
     )
    {
        return ConstIterator(
            Internal::const_iter_cast(i._i),
            Internal::const_iter_cast(i._e),
            i._fn
        );
    }
};

}

namespace adapters {

template <class T,class Fn,template <class> class StoragePolicy>
struct CollectionAdapter< Filter<T,Fn,StoragePolicy> > {

    static const bool is_valid = true;

    typedef Filter<T,Fn,StoragePolicy> ThisCol;
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
    static Iterator iterAt(C&& c,size_t i) {
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

#endif /* end of include guard: FILTER_O8Y22ICC */

