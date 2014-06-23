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

#include <templatious/util/Hasher.h>

namespace templatious {
namespace adapters {

template <
    template <class...> class Map,
    class Key,
    class Value,
    class Hash = templatious::util::Hasher<Key> >
struct MapMaker {

    static const bool is_maker_valid = false;
    typedef void MapType;
    typedef typename templatious::util::HashKit<Hash> Kit;

    Kit _k;

    MapMaker(const Hash& h) : _k(h) {}

    static MapType make(size_t size);

};

}
}


#endif /* end of include guard: MAPMAKER_OQGTEX4F */
