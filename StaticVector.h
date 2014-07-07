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

namespace templatious {

template <class T,unsigned long sz>
struct StaticVector {

    typedef unsigned long ulong;
    static const ulong size = sz;
    static_assert(size > 0,"Static vector cannot be of negative size.");

    StaticVector(T vct[size]) : _vct(vct), _cnt(0) { }
    StaticVector(T vct[size],ulong currCnt) :
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

    T pop() {
        assert(_cnt > 0);
        return std::move(_vct[--_cnt]);
    }

    bool isFull() {
        return _cnt >= size;
    }

    bool isEmpty() {
        return _cnt == 0;
    }

private:
    T* _vct;
    ulong _cnt;


};

}


#endif /* end of include guard: STATICVECTOR_IIC8KCSO */
