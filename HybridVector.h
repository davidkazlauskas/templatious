/*
 * =====================================================================================
 *
 *       Filename:  HybridVector.h
 *
 *    Description:  Static vector with additional storage
 *
 *        Version:  1.0
 *        Created:  07/14/2014 07:17:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#include <cstddef>
#include <vector>

#include <templatious/CollectionMaker.h>
#include <templatious/CollectionAdapter.h>
#include <templatious/StaticVector.h>

#ifndef HYBRIDVECTOR_5CFAJMGM
#define HYBRIDVECTOR_5CFAJMGM

namespace templatious {

template <class T,size_t sz,
         template <class...> class Additional = std::vector,
         template <class> class Alloc = std::allocator
>
struct HybridVector {

    typedef HybridVector<T,sz,Additional,Alloc> ThisVector;
    typedef typename templatious::StaticVector<T,sz> StatVector;
    typedef typename templatious::adapters::CollectionMaker<T,Additional,Alloc> ColMaker;
    typedef typename ColMaker::Collection Collection;
    typedef typename templatious::adapters::CollectionAdapter<Collection*> Ad;

    template <bool isConst>
    struct HvIterator;

    typedef HvIterator<false> Iterator;
    typedef HvIterator<true> ConstIterator;

    friend struct HvIterator<false>;
    friend struct HvIterator<true>;

    static const size_t static_size = sz;

    static_assert(Ad::is_valid,
            "Adapter is invalid.");

    StatVector _s;

    HybridVector(T (&c)[static_size]) : _s(c), _a(nullptr) { }

    ~HybridVector() {
        delete _a;
    }

    template <class U>
    bool push(U&& e) {
        if (!_s.isFull()) {
            return _s.push(
                std::forward<U>(e));
        }

        return Ad::add(
            extra(),
            std::forward<U>(e));
    }

    bool insert(ulong at,const T& e) {
        if (!_s.isFull()) {
            return _s.insert(at,e);
        }

        if (at < static_size) {
            Ad::insert_at(
                    extra(),
                    Ad::begin(extra()),
                    _s.pop());
            return _s.insert(at,e);
        } else {
            auto i = Ad::iter_at(extra(),at - static_size);
            return Ad::insert_at(extra(),i,e);
        }
    }

    bool insert(ulong at,T&& e) {
        if (!_s.isFull()) {
            return _s.insert(at,e);
        }

        if (at < static_size) {
            Ad::insert_at(
                    extra(),
                    Ad::begin(extra()),
                    _s.pop());
            return _s.insert(at,e);
        } else {
            auto i = Ad::iter_at(extra(),at - static_size);
            return Ad::insert_at(extra(),i,e);
        }
    }

    void erase(const Iterator& beg,const Iterator& end) {
        assert(beg._pos < end._pos
                && "Beggining iterator is greater than end.");
        assert(end._pos <= getSize()
                && "End iterator is out of bounds.");

        bool statTrimmed = false;
        if (end._pos < _s.getSize()) {
            _s.erase(_s.iterAt(beg._pos),_s.iterAt(end._pos));

            statTrimmed = true;
        } else if (beg._pos < _s.getSize()) {
            assert(extraSize() > 0
                && "Dynamic collection has to contain something.");
            _s.erase(_s.iterAt(beg._pos),_s.end());
            Ad::erase(extra(), Ad::begin(extra()),
                    Ad::iter_at(extra(),end._pos - static_size));

            statTrimmed = true;
        } else {
            Ad::erase(extra(),
                      Ad::iter_at(extra(), beg._pos - static_size),
                      Ad::iter_at(extra(), end._pos - static_size));
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

    void erase(const Iterator& beg) {
        erase(beg,end());
    }

    Iterator begin() {
        return Iterator(*this,0);
    }

    Iterator end() {
        return Iterator(*this,getSize());
    }

    ulong getSize() const {
        return _s.getSize() + extraSize();
    }

    template <bool isConst>
    struct HvIterator {
        typedef typename templatious::util::TypeSelector<
            isConst,const T,T>::val ValType;
        typedef HvIterator<isConst> Iterator;

        friend class HybridVector<T,sz,Additional,Alloc>;

        HvIterator(ThisVector& v,ulong pos) {
            _v = &v;
            _pos = pos;
            if (nullptr != _v->_a) {
                _i = Ad::begin(_v->_a);
            }
        }

        HvIterator& operator++() {
            ++_pos;
            if (_pos > _v->_s.getSize()) {
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
            if (_pos < _v->_s.getSize()) {
                return _v->_s.at(_pos);
            }

            return *_i;
        }

        ValType* operator->() const {
            if (_pos < _v->_s.getSize()) {
                return &(_v->_s.at(_pos));
            }

            return &(*_i);
        }

    private:
        ThisVector* _v;
        ulong _pos;
        typename Ad::iterator _i;

    };

private:
    Collection* _a;

    Collection* extra() {
        if (nullptr == _a) {
            _a = ColMaker::makeHeap(static_size);
        }

        return _a;
    }

    ulong extraSize() const {
        if (nullptr == _a) {
            return 0;
        }

        return Ad::getSize(_a);
    }

};

}

#endif /* end of include guard: HYBRIDVECTOR_5CFAJMGM */
