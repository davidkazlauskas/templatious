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

    typedef typename templatious::StaticVector<T,sz> StatVector;
    typedef typename templatious::adapters::CollectionMaker<T,Additional,Alloc> ColMaker;
    typedef typename ColMaker::Collection Collection;
    typedef typename templatious::adapters::CollectionAdapter<Collection*> Ad;

    static const size_t static_size = sz;

    StatVector _s;
    Collection* _a;

    static_assert(Ad::is_valid,
            "Adapter is invalid.");

    HybridVector(T (&c)[static_size]) : _s(c), _a(nullptr) { }

    ~HybridVector() {
        delete _a;
    }

    bool push(const T& e) {
        if (!_s.isFull()) {
            return _s.push(e);
        }

        return Ad::add(extra());
    }

    bool push(T&& e) {
        if (!_s.isFull()) {
            return _s.push(e);
        }

        return Ad::add(extra(),e);
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

private:

    Collection* extra() {
        if (nullptr == _a) {
            _a = ColMaker::makeHeap(static_size);
        }

        return _a;
    }

};

}

#endif /* end of include guard: HYBRIDVECTOR_5CFAJMGM */
