/*
 * =====================================================================================
 *
 *       Filename:  Filter.h
 *
 *    Description:  Filter class
 *
 *        Version:  1.0
 *        Created:  08/04/2014 07:37:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef FILTER_O8Y22ICC
#define FILTER_O8Y22ICC

#include <utility>

#include <templatious/CollectionAdapter.h>
#include <templatious/proxy/Picker.h>

namespace templatious {

template <class T,class Fn>
struct Filter {

    template <class I,class Fun>
    struct PIterator;

    typedef typename adapters::CollectionAdapter<T> Ad;
    typedef PIterator<typename Ad::iterator,Fn> iterator;
    typedef PIterator<typename Ad::const_iterator,Fn> const_iterator;
    typedef IsProxy<T> ProxUtil;
    typedef typename ProxUtil::ICollection ICollection;

    static const bool proxy_inside = ProxUtil::val;
    static const bool floating_iterator = Ad::floating_iterator;
    static const bool random_access_iterator = false;

    static_assert(Ad::is_valid,"Adapter is invalid.");
    T&& _c;
    Fn&& _fn;
    iterator _b;
    iterator _e;

    template <class V,class FnRef>
    Filter(V&& v,FnRef&& fn) :
        _c(v),
        _fn(std::forward<FnRef>(fn)),
        _b(Ad::begin(std::forward<V>(v)),
            Ad::end(std::forward<V>(v)),
            std::forward<FnRef>(fn)),
        _e(Ad::end(std::forward<V>(v)),
            Ad::end(std::forward<V>(v)),
            std::forward<FnRef>(fn))
     {
         if (!fn(*_b)) {
             ++_b;
         }
     }

    iterator begin() {
        return _b;
    }

    iterator end() {
        return _e;
    }

    const_iterator cbegin() {
        return _b;
    }

    const_iterator cend() {
        return _e;
    }

    iterator iterAt(size_t i) {
        auto res(_b);
        naiveIterAdvance(res,_e,i);
        return res;
    }

    template <class I,class Fun>
    struct PIterator {
    private:
        I _i;
        I _e;
        Fun&& _fn;

    public:
        friend class Filter<T,Fun>;

        typedef PIterator<I,Fun> ThisIter;
        typedef decltype(*_i) IVal;

        //template <class V>
        //PIterator(V&& i) : _i(std::forward<V>(i)) {}

        template <class V>
        PIterator(const I& i,const I& e,V&& fn) :
            _i(i), _e(e), _fn(std::forward<V>(fn)) {}

        ThisIter& operator++() {
            assert(_e != _i && "Trying to iterate past end of filter.");
            do {
                ++_i;
            } while (_e != _i && !_fn(*_i));
            return *this;
        }

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
            -> decltype(ProxUtil::iter_unwrap(_i))&
        {
            return ProxUtil::iter_unwrap(_i);
        }

    };

    void clear() {
        clearRoutine<floating_iterator>(*this);
        _b._i = _e._i;
    }

    auto getInternal()
        -> decltype(ProxUtil::unwrap(_c))&
    {
        return ProxUtil::unwrap(_c);
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

template <class T,class Fn>
struct IsProxy< Filter< T,Fn > > {
    static const bool val = true;

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

    int get_mul() {
        return -1;
    }
};

namespace adapters {
template <class T,class Fn>
struct CollectionAdapter< Filter<T,Fn> > {

    static const bool is_valid = true;

    typedef Filter<T,Fn> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::iterator iterator;
    typedef typename ThisCol::const_iterator const_iterator;
    typedef typename ThisCol::Ad::value_type value_type;
    typedef typename ThisCol::Ad::const_value_type const_value_type;

    static const bool floating_iterator = ThisCol::Ad::floating_iterator;

    static iterator begin(ThisCol& c) {
        return c.begin();
    }

    static iterator end(ThisCol& c) {
        return c.end();
    }

    static iterator cbegin(ThisCol& c) {
        return c.cbegin();
    }

    static iterator cend(ThisCol& c) {
        return c.cend();
    }

    static iterator iter_at(ThisCol& c,size_t i) {
        return c.iterAt(i);
    }

    template <class V>
    static void insert_at(ThisCol& c,iterator i,V&& v) {
        c.insert(i,std::forward<V>(v));
    }

    static void erase(ThisCol& c, iterator i) {
        c.erase(i);
    }

    static void erase(ThisCol& c, iterator beg, iterator end) {
        c.erase(beg,end);
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

#endif /* end of include guard: FILTER_O8Y22ICC */

