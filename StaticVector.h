/*
 * =====================================================================================
 *
 *       Filename:  StaticVector.h
 *
 *    Description:  Static vector to efficiently use stack space
 *
 *        Version:  1.0
 *        Created:  07/07/2014 05:40:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef STATICVECTOR_IIC8KCSO
#define STATICVECTOR_IIC8KCSO

#include <assert.h>

#include <templatious/CollectionAdapter.h>
#include <templatious/Sugar.h>

namespace templatious {

template <class T,size_t sz>
struct StaticVector {

    typedef size_t ulong;
    typedef StaticVector<T,sz> ThisVector;
    struct Iterator;

    static const ulong size = sz;
    static_assert(size > 0,"Static vector cannot be of negative size.");

    StaticVector(T (&vct)[size]) : _vct(vct), _cnt(0) { }
    StaticVector(T (&vct)[size],ulong currCnt) :
        _vct(vct), _cnt(currCnt)
    {
        assert(currCnt <= size
            && "Initial static array size cannot be larger than a capacity.");
    }

    bool push(const T& e) {
        if (isFull()) {
            return false;
        }

        _vct[_cnt++] = e;
        return true;
    }

    bool pop(T& out) {
        if (isEmpty()) {
            return false;
        }

        out = _vct[--_cnt];
        return true;
    }

    T&& pop() {
        assert(_cnt > 0 && "Trying to pop an empty vector.");
        return std::move(_vct[--_cnt]);
    }

    //T&& pop_first() {
        //assert(_cnt > 0 && "Trying to pop an empty vector.");
        //T res = std::move(_vct[0]);
        //TEMPLATIOUS_FOREACH(auto i,templatious::LoopL<size_t>(--_cnt)) {
            //_vct[i] = _vct[i + 1];
        //}
        //return std::move(res);
    //}

    T& at(ulong pos) const {
        assert(pos >= 0 && pos < size && "Requested position out of bounds.");
        return _vct[pos];
    }

    bool isFull() const {
        return _cnt >= size;
    }

    bool isEmpty() const {
        return _cnt == 0;
    }

    Iterator begin() const {
        return Iterator(_vct,size);
    }

    Iterator end() const {
        return Iterator(_vct,size,size);
    }

    const Iterator cbegin() const {
        return Iterator(_vct,size);
    }

    const Iterator cend() const {
        return Iterator(_vct,size,size);
    }

    struct Iterator {
        T* _vct;
        ulong _size;
        ulong _iter;

        Iterator(T* vct,ulong size) :
            _vct(vct), _size(size), _iter(0) {}

        Iterator(T* vct,ulong size,ulong pos) :
            _vct(vct), _size(size), _iter(pos)
        {
            assert(_iter <= _size && "Iterator position cannot be greater than size.");
        }

        Iterator& operator++() {
            ++_iter;
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return _iter == rhs._iter;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

        T& operator*() const {
            assert(_iter < _size && "Iterator out of bounds.");
            return _vct[_iter];
        }

        T* operator->() const {
            assert(_iter < _size && "Iterator out of bounds.");
            return &_vct[_iter];
        }

    };

private:
    T* _vct;
    ulong _cnt;


};

template <class T,size_t sz>
StaticVector<T,sz> makeStaticVector(T (&arr)[sz]) {
    return StaticVector<T,sz>(arr);
}

}


#endif /* end of include guard: STATICVECTOR_IIC8KCSO */
