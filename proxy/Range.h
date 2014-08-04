/*
 * =====================================================================================
 *
 *       Filename:  Range.h
 *
 *    Description:  Range class
 *
 *        Version:  1.0
 *        Created:  07/30/2014 06:01:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef RANGE_KFIVLD23
#define RANGE_KFIVLD23

#include <utility>

#include <templatious/CollectionAdapter.h>
#include <templatious/proxy/Picker.h>

namespace templatious {

template <class T>
struct Range {

    template <class I>
    struct PIterator;

    typedef typename adapters::CollectionAdapter<T> Ad;
    typedef PIterator<typename Ad::iterator> iterator;
    typedef PIterator<typename Ad::const_iterator> const_iterator;
    typedef IsProxy<T> ProxUtil;
    typedef typename ProxUtil::ICollection ICollection;

    static const bool proxy_inside = ProxUtil::val;
    static const bool floating_iterator = Ad::floating_iterator;

    static_assert(Ad::is_valid,"Adapter is invalid.");
    T&& _c;
    iterator _b;
    iterator _e;

    template <class V>
    Range(V&& v,const iterator& b,const iterator& e) :
        _c(v), _b(b), _e(e)
    { }

    template <class V>
    Range(V&& v,const iterator& b) :
        _c(v), _b(b),
        _e(Ad::end(std::forward<V>(v))) {}


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

    template <class I>
    struct PIterator {
    private:
        I _i;

    public:
        typedef PIterator<I> ThisIter;
        typedef decltype(*_i) IVal;

        //template <class V>
        //PIterator(V&& i) : _i(std::forward<V>(i)) {}

        // FIX THIS DAZLOW ... maybe?
        PIterator(const I& i) : _i(i) {}


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

        IVal& operator*()
        {
            return *(this->_i);
        }

        auto operator->()
            -> decltype(&(this->_i))
        const {
            return &(this->_i);
        }

        I& getInternal() {
            return _i;
        }

    };

    //template <class V,class Iter>
    //void insert(Iter&& i,V&& v) {
        //// we assume iterator passed to
        //// this method belongs to this range
        //Ad::insert(std::forward<T>(_c),
                //std::forward<Iter>(i),
                //std::forward<V>(v));
        //for (int i = 0; i < erase_shift; ++i) {
            //++_e;
        //}
    //}

    //void erase(iterator i) {
        //// we assume iterator passed to
        //// this method belongs to this range
        //Ad::erase(std::forward<T>(_c),i);
        //static_assert(Ad::bidirectional_iterator,
                //"Collection cannot be iterated backwards, therefore, cannot erase.");
        //assert(_b != _e && "Range is empty (start iterator equals end)");
        //for (size_t i = 0; i < erase_shift; ++i) {
            //--_e;
        //}
    //}

    //void erase(iterator beg,iterator end) {
        //// we assume iterator passed to
        //// this method belongs to this range
        //Ad::erase(std::forward<T>(_c),beg,end);
        //static_assert(Ad::bidirectional_iterator,
                //"Collection cannot be iterated backwards, therefore, cannot erase.");
        //if (rand_access) {
            //size_t diff = end - beg;
            //for (size_t i = 0; i < erase_shift * diff; ++i) {
                //--_e;
            //}
        //} else {
            //while (beg != end) {
                //erase(beg);
                //++beg;
            //}
        //}
    //}

    //iterator iterAt(size_t i) {
        //if (rand_access) {
            //assert(i <= _e && "index outside collection range.");
            //assert(i >= _b && "index outside collection range.");
            //return _b + i;
        //}

        //auto b = _b;
        //size_t j = 0;

        //while (j != i) {
            //++b;
        //}

        //return b;
    //}

    void clear() {
        clearRoutine<floating_iterator>(*this);
        _b = _e;
    }

    auto getInternal()
        -> decltype(ProxUtil::unwrap(_c))
    {
        return ProxUtil::unwrap(_c);
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
struct IsProxy< Range< T > > {
    static const bool val = true;

    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename
        adapters::CollectionAdapter<ICollection> IAdapter;

    template <class C>
    static auto unwrap(C&& c)
        -> decltype(c.getInternal())
    {
        return c.getInternal();
    }

    template <class C>
    static auto iter_unwrap(C&& c)
        -> decltype(c.getInternal())
    {
        return c.getInternal();
    }
};

namespace adapters {

template <class T>
struct CollectionAdapter< Range<T> > {

    static const bool is_valid = true;

    typedef Range<T> ThisCol;
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

#endif /* end of include guard: RANGE_KFIVLD23 */
