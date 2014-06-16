/*
 * =====================================================================================
 *
 *       Filename:  CollectionMap.h
 *
 *    Description:  Map adapter for simple collections
 *
 *        Version:  1.0
 *        Created:  06/16/2014 08:48:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef COLLECTIONMAP_SDEAKTEN
#define COLLECTIONMAP_SDEAKTEN

#include <utility>

#include <templatious/CollectionManipulator.h>
#include <templatious/MapAdapter.h>

namespace templatious {
namespace adapters {

template <template <class,class> class T,class Key,class Value,template <class> class Alloc>
struct MapAdapter< T<std::pair<Key,Value>,Alloc< std::pair<Key,Value> > > > {

    typedef Key KeyType;
    typedef Value ValueType;
    typedef T<std::pair<Key,Value>,Alloc< std::pair<Key,Value> > > ThisMap;
    typedef typename templatious::manip::StaticManipulator SM;
    typedef typename templatious::StaticAdapter SA;

    static_assert( templatious::adapters::CollectionAdapter<ThisMap>::is_valid, "Map is invalid." );

    bool keyExists(const ThisMap& h,const KeyType& k) {
        for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            if ((*i).first == k) {
                return true;
            }
        }

        return false;
    }

    bool get(const ThisMap& h,const KeyType& k,ValueType& v);
    ValueType& get(const ThisMap& h,const KeyType& k);
    bool put(ThisMap& h,const KeyType& k,const ValueType& v);
    void clear(ThisMap& h);

};

}
}


#endif /* end of include guard: COLLECTIONMAP_SDEAKTEN */
