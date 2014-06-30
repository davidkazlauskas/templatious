/*
 * =====================================================================================
 *
 *       Filename:  MapMaker.h
 *
 *    Description:  Make maps according to three template parameters
 *
 *        Version:  1.0
 *        Created:  06/20/2014 03:58:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef MAPMAKER_OQGTEX4F
#define MAPMAKER_OQGTEX4F

#include <memory>

#include <templatious/util/Hasher.h>

namespace templatious {
namespace adapters {

template <
    class Key,
    class Value,
    template <class...> class Map,
    class Hash = templatious::util::Hasher<Key>,
    template <class> class Allocator = std::allocator >
struct MapMaker {

    static const bool is_maker_valid = false;
    typedef void MapType;
    typedef typename templatious::util::HashKit<Hash> Kit;

    Kit _k;

    MapMaker(const Hash& h) : _k(h) {}
    MapMaker() : _k(Hash()) {}

    static MapType make(size_t size);
    static MapType make();

};


}
}


#endif /* end of include guard: MAPMAKER_OQGTEX4F */
