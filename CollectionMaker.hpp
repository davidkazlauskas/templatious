//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  CollectionMaker.hpp
 *
 *    Description:  Uniform interface to make collections
 *
 *        Version:  1.0
 *        Created:  06/30/2014 07:01:38 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef COLLECTIONMAKER_OL3AS5ML
#define COLLECTIONMAKER_OL3AS5ML

#include <cstddef>
#include <utility>

namespace templatious {
namespace adapters {

template <
    class Val,
    template <class...> class Coll,
    template <class> class Alloc
>
struct CollectionMaker {
    typedef int Collection;

    static const bool is_maker_valid = false;

    static Collection make();
    static Collection make(size_t size);
    static Collection* makeHeap();
    static Collection* makeHeap(size_t size);

};


}
}

#endif /* end of include guard: COLLECTIONMAKER_OL3AS5ML */
