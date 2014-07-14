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
#include <templatious/StaticVector.h>

#ifndef HYBRIDVECTOR_5CFAJMGM
#define HYBRIDVECTOR_5CFAJMGM

namespace templatious {

template <class T,size_t sz,
         template <class...> class Additional = std::vector,
         template <class> class Alloc = std::allocator
>
struct HybridVector {

    typedef StaticVector<T,sz> StatVector;
    typedef templatious::adapters::CollectionMaker<T,Additional,Alloc> ColMaker;
    typedef typename ColMaker::Collection Collection;

    static const size_t static_size = sz;

    StatVector _s;
    Collection* _a;

    HybridVector(T c[static_size]) : _s(c), _a(nullptr) { }

};

}

#endif /* end of include guard: HYBRIDVECTOR_5CFAJMGM */
