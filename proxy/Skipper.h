/*
 * =====================================================================================
 *
 *       Filename:  Skipper.h
 *
 *    Description:  Skip elements in increments
 *
 *        Version:  1.0
 *        Created:  08/07/2014 04:37:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef SKIPPER_8SYZBPCX
#define SKIPPER_8SYZBPCX

#include <utility>

#include <templatious/util/RefMaker.h>
#include <templatious/CollectionAdapter.h>
#include <templatious/proxy/Picker.h>

namespace templatious {

template <class T>
struct Skipper {

    template <class I>
    struct PIterator;

    typedef typename adapters::CollectionAdapter<T> Ad;
    typedef PIterator<typename Ad::Iterator> Iterator;
    typedef PIterator<typename Ad::ConstIterator> ConstIterator;
    typedef IsProxy<T> ProxUtil;
    typedef typename ProxUtil::ICollection ICollection;

    typedef StaticAdapter SA;

    static const bool proxy_inside = ProxUtil::val;
    static const bool random_access_iterator = ProxUtil::random_access_iterator;
    static const bool floating_iterator = Ad::floating_iterator;

    static_assert(Ad::is_valid,"Adapter is invalid.");

    typedef typename templatious::util::RefMaker<T>::val Ref;
    Ref _c;
    Iterator _b;
    Iterator _e;
    size_t _sk; // - skip size

    template <class V>
    Skipper(V&& v,size_t sz) :
        _c(v),
        _b(*this,SA::begin(v),sz),
        _e(*this,SA::end(v),sz),
        _sk(sz)
    { }

    Iterator begin() {
        return _b;
    }

    Iterator end() {
        return _e;
    }

    ConstIterator cbegin() {
        return _b;
    }

    ConstIterator cend() {
        return _e;
    }

    Iterator iterAt(size_t n) {
        if (!random_access_iterator) {
            Iterator res(_b);
            naiveIterAdvance(res,_e,n);
            return res;
        } else {
            auto i = _b._i;
            auto e = _e._i;
            typedef AdvancePicker<!random_access_iterator> A;
            int mul = _sk * ProxUtil::get_mul(_c);
            A::adv(i,e,mul * n);
            return Iterator(*this,i,_sk);
        }
    }

    template <class I>
    struct PIterator {
    private:
        typedef Skipper<T> Parent;
        typedef Parent::ProxUtil ProxUtil;
        friend struct Skipper<T>;

        Parent& _p;
        I _i;
        size_t _sk;

        static const bool random_access_iterator =
            Parent::random_access_iterator;
    public:
        typedef PIterator<I> ThisIter;
        typedef decltype(*_i) IVal;

        PIterator(Parent& p,const I& i,size_t sz) :
            _p(p),
            _i(i),
            _sk(sz)
        {}

        PIterator& operator=(const PIterator& rhs) {
            _i = rhs._i;
            _sk = rhs._sk;

            assert(&_p == &rhs._p);
            return *this;
        }

        ThisIter& operator++() {
            if (!random_access_iterator) {
                naiveIterAdvance(_i,
                    SA::end(_p)._i,
                    _sk);
            } else {
                auto i = iterUnwrap(_i);
                auto e = SA::end(_p).getInternal();
                typedef AdvancePicker<!random_access_iterator> A;
                size_t mul = ProxUtil::get_mul(_p);
                A::adv(i,e,mul * _sk);
                ProxUtil::iter_unwrap(_i) = i;
            }
            return *this;
        }

        //ThisIter& operator--() {
            //--_i;
            //return *this;
        //}

        bool operator==(const ThisIter& rhs) const {
            return _i == rhs._i;
        }

        bool operator!=(const ThisIter& rhs) const {
            return !(*this == rhs);
        }

        IVal& operator*()
        {
            return *(this->_i);
        }

        auto operator->()
            -> decltype(&(this->_i))
        const {
            return &(this->_i);
        }

        auto getInternal()
            -> decltype(ProxUtil::iter_unwrap(_i))
        {
            return ProxUtil::iter_unwrap(_i);
        }

    };

    void clear() {
        clearRoutine<floating_iterator>(*this);
        _b = _e;
    }

    auto getInternal()
        -> decltype(ProxUtil::unwrap(_c))
    {
        return ProxUtil::unwrap(_c);
    }

    int getMul() {
        return ProxUtil::get_mul(_c);
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

template <class T>
struct IsProxy< Skipper< T > > {
    typedef IsProxy<T> Internal;
    static const bool val = true;

    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename
        adapters::CollectionAdapter<ICollection> IAdapter;

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

    typedef int Dist;
    template <class U>
    static Dist get_mul(U&& u) {
        return u.getMul();
    }
};

namespace adapters {

template <class T>
struct CollectionAdapter< Skipper<T> > {

    static const bool is_valid = true;

    typedef Skipper<T> ThisCol;
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

    static Iterator cbegin(ThisCol& c) {
        return c.cbegin();
    }

    static Iterator cend(ThisCol& c) {
        return c.cend();
    }

    //static Iterator iterAt(ThisCol& c,size_t i) {
        //return c.iterAt(i);
    //}

    template <class V>
    static Iterator iterAt(V&& c,size_t i) {
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

    template <class V = int>
    static void instantiate() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<V, false>::val,
                      "loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class V = int>
    static void instantiate(size_t sz) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<V, false>::val,
                      "loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

};

}
}

#endif /* end of include guard: SKIPPER_8SYZBPCX */
