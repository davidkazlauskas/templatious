//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  HybridVector.hpp
 *
 *    Description:  Static vector with additional storage
 *
 *        Version:  1.0
 *        Created:  07/14/2014 07:17:25 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef HYBRIDVECTOR_5CFAJMGM
#define HYBRIDVECTOR_5CFAJMGM

#include <cstddef>
#include <vector>

#include <templatious/CollectionMaker.hpp>
#include <templatious/CollectionAdapter.hpp>
#include <templatious/StaticVector.hpp>

namespace templatious {

template <class T,long sz,
         template <class...> class Additional = std::vector,
         template <class> class Alloc = std::allocator
>
struct HybridVector {

    typedef HybridVector<T,sz,Additional,Alloc> ThisVector;
    typedef typename templatious::StaticVector<T> StatVector;
    typedef typename templatious::adapters::CollectionMaker<T,Additional,Alloc> ColMaker;
    typedef typename ColMaker::Collection Collection;
    typedef typename templatious::adapters::CollectionAdapter<Collection*> Ad;
    typedef T ValType;

    template <bool isConst>
    struct HvIterator;

    typedef HvIterator<false> Iterator;
    typedef HvIterator<true> ConstIterator;

    friend struct HvIterator<false>;
    friend struct HvIterator<true>;

    static const long static_size = sz;

    static_assert(Ad::is_valid,
            "Adapter is invalid.");

    StatVector _s;

    HybridVector(T (&c)[static_size]) : _s(c,static_size), _a(nullptr) { }
    HybridVector(const ThisVector& other) = delete;
    HybridVector(ThisVector&& other) :
        _s(std::move(other._s)), _a(other._a)
    {
        other._a = nullptr;
    }

    ~HybridVector() {
        delete _a;
    }

    template <class U>
    void push(U&& e) {
        if (!_s.isFull()) {
            _s.push(std::forward<U>(e));
        }

        Ad::add(extra(),
            std::forward<U>(e));
    }

    template <class U>
    void insert(ulong at,U&& e) {
        if (!_s.isFull()) {
            _s.insert(at,e);
        }

        if (at < static_size) {
            Ad::insertAt(
                    extra(),
                    Ad::begin(extra()),
                    _s.pop());
            _s.insert(at,e);
        } else {
            auto i = Ad::iterAt(extra(),at - static_size);
            Ad::insertAt(extra(),i,e);
        }
    }

    template <class U>
    void insert(const Iterator& at,U&& u) {
        insert(at._pos,u);
    }

    Iterator iterAt(ulong i) {
        assert(i <= size());
        return Iterator(*this,i);
    }

    ConstIterator citerAt(ulong i) {
        assert(i <= size());
        return ConstIterator(*this,i);
    }

    ValType& getByIndex(ulong i) {
        if (i < _s.size()) {
            return _s.getByIndex(i);
        }

        return Ad::getByIndex(extra(),
                i - _s.size());
    }

    void erase(const Iterator& beg,const Iterator& end) {
        assert(beg._pos < end._pos
                && "Beggining iterator is greater than end.");
        assert(end._pos <= size()
                && "End iterator is out of bounds.");

        bool statTrimmed = false;
        if (end._pos < _s.size()) {
            _s.erase(_s.iterAt(beg._pos),_s.iterAt(end._pos));

            statTrimmed = true;
        } else if (beg._pos < _s.size()) {
            assert(extraSize() > 0
                && "Dynamic collection has to contain something.");
            _s.erase(_s.iterAt(beg._pos),_s.end());
            Ad::erase(extra(), Ad::begin(extra()),
                    Ad::iterAt(extra(),end._pos - static_size));

            statTrimmed = true;
        } else {
            Ad::erase(extra(),
                      Ad::iterAt(extra(), beg._pos - static_size),
                      Ad::iterAt(extra(), end._pos - static_size));
        }

        if (statTrimmed && extraSize() > 0) {
            auto i = Ad::begin(extra());
            auto e = Ad::end(extra());
            while (!_s.isFull() && i != e) {
                _s.push(std::move(*i));
                ++i;
            }

            Ad::erase(extra(),Ad::begin(extra()),i);
        }
    }

    void erase(const Iterator& pos) {
        erase(pos,++Iterator(pos));
    }

    Iterator begin() {
        return Iterator(*this,0);
    }

    Iterator end() {
        return Iterator(*this,size());
    }

    ulong size() const {
        return _s.size() + extraSize();
    }

    void clear() {
        _s.clear();
        if (extraExists()) {
            Ad::clear(_a);
        }
    }

    template <bool isConst>
    struct HvIterator {
        typedef typename std::conditional<
            isConst,const T,T>::type ValType;
        typedef HvIterator<isConst> Iterator;

        friend class HybridVector<T,sz,Additional,Alloc>;

        HvIterator(ThisVector& v,ulong pos) {
            _v = &v;
            _pos = pos;
            if (nullptr != _v->_a) {
                if (_pos <= _v->_s.size()) {
                    _i = Ad::begin(_v->_a);
                } else {
                    Ad::iterAt(_v->_a,
                        _pos - _v->_s.size());
                }
            }
        }

        HvIterator& operator++() {
            ++_pos;
            if (_pos > _v->_s.size()) {
                ++_i;
            }

            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return _pos == rhs._pos;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

        ValType& operator*() const {
            if (_pos < _v->_s.size()) {
                return _v->_s.at(_pos);
            }

            return *_i;
        }

        ValType* operator->() const {
            if (_pos < _v->_s.size()) {
                return &(_v->_s.at(_pos));
            }

            return &(*_i);
        }

    private:
        ThisVector* _v;
        ulong _pos;
        typename Ad::Iterator _i;

    };

private:
    Collection* _a;

    Collection* extra() {
        if (nullptr == _a) {
            _a = ColMaker::makeHeap(static_size);
        }

        return _a;
    }

    bool extraExists() const {
        return nullptr != _a;
    }

    ulong extraSize() const {
        if (extraExists()) {
            return 0;
        }

        return Ad::size(_a);
    }

};

template <class T,
         long sz,
         template <class...> class Additional = std::vector,
         template <class> class Alloc = std::allocator
>
auto makeHybridVector(T (&arr)[sz]) ->
HybridVector<T,sz,Additional,Alloc>
{
    return HybridVector<T,sz,Additional,Alloc>(arr);
}


namespace adapters {

template <class T,long sz,
         template <class...> class Additional,
         template <class> class Alloc
>
struct CollectionAdapter< HybridVector<T,sz,Additional,Alloc> > {

    static const bool is_valid = true;

    typedef HybridVector<T,sz,Additional,Alloc> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::Iterator Iterator;
    typedef typename ThisCol::ConstIterator ConstIterator;
    typedef T ValueType;
    typedef const ValueType ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.push(std::forward<V>(i));
    }

    template <class V>
    static void insertAt(ThisCol& c, Iterator at, V&& i) {
        c.insert(at,std::forward<V>(i));
    }

    static ValueType& getByIndex(ThisCol& c, long i) {
        return c.getByIndex(i);
    }

    static ConstValueType& getByIndex(ConstCol& c, long i) {
        return c.getByIndex(i);
    }

    static long size(ConstCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c,const Iterator& pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c, const Iterator& beg,
            const Iterator& end)
    {
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
        return c.getByIndex(0);
    }

    static ConstValueType& first(ConstCol& c) {
        return c.getByIndex(0);
    }

    static ValueType& last(ThisCol& c) {
        return c.getByIndex(c.size() - 1);
    }

    static ConstValueType& last(ConstCol& c) {
        return c.getByIndex(c.size() - 1);
    }

    static void clear(ThisCol& c) {
        return c.clear();
    }

    static bool canAdd(ThisCol& c) {
        return true;
    }

};

template <class T,long sz,
         template <class...> class Additional,
         template <class> class Alloc
>
struct CollectionAdapter< const HybridVector<T,sz,Additional,Alloc> > {

    static const bool is_valid = true;

    typedef const HybridVector<T,sz,Additional,Alloc> ThisCol;
    typedef ThisCol ConstCol;
    typedef typename ThisCol::ConstIterator Iterator;
    typedef typename ThisCol::ConstIterator ConstIterator;
    typedef const T ValueType;
    typedef const T ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<V, false>::val,
                      "Const collection doesn't support add.");
    }

    template <class V>
    static void insertAt(ThisCol& c, Iterator at, V&& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<V, false>::val,
                      "Const collection doesn't support insert.");
    }

    template <class U = int>
    static ConstValueType& getByIndex(ThisCol& c, long i) {
        return c.getByIndex(i);
    }

    static long size(ConstCol& c) {
        return c.size();
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator pos) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Const collection doesn't support erase.");
    }

    template <class U = int>
    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Const collection doesn't support erase.");
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

    static ConstValueType& first(ConstCol& c) {
        return c.getByIndex(0);
    }

    static ConstValueType& last(ConstCol& c) {
        return c.getByIndex(c.size() - 1);
    }

    template <class U = int>
    static void clear(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Const collection doesn't support clear.");
    }

    static bool canAdd(ThisCol& c) {
        return false;
    }

};

}

}

#endif /* end of include guard: HYBRIDVECTOR_5CFAJMGM */
