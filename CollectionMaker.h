/*
 * =====================================================================================
 *
 *       Filename:  CollectionMaker.h
 *
 *    Description:  Uniform interface to make collections
 *
 *        Version:  1.0
 *        Created:  06/30/2014 07:01:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef COLLECTIONMAKER_OL3AS5ML
#define COLLECTIONMAKER_OL3AS5ML

namespace templatious {
namespace adapters {

#include <utility>

template <
    class Val,
    template <class...> class Coll,
    template <class> class Alloc
>
struct CollectionMaker {
    typedef Coll< Val,Alloc<Val> > Collection;

    static const bool is_maker_valid = false;

    Collection make() {
        return std::move(Collection());
    }

    Collection make(size_t size) {
        return std::move(Collection());
    }

};


}
}

#endif /* end of include guard: COLLECTIONMAKER_OL3AS5ML */
