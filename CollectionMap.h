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
#include <templatious/map/MapAdapter.h>

namespace templatious {
namespace adapters {

template <template <class,class> class T,class Key,class Value,template <class> class Alloc>
struct MapAdapter< T<std::pair<Key,Value>,Alloc< std::pair<Key,Value> > > > {

    static const bool is_valid = true;

    typedef Key KeyType;
    typedef Value ValueType;
    typedef T<std::pair<Key,Value>,Alloc< std::pair<Key,Value> > > ThisMap;
    typedef typename templatious::manip::StaticManipulator SM;
    typedef typename templatious::StaticAdapter SA;

    static_assert( templatious::adapters::CollectionAdapter<ThisMap>::is_valid, "Map is invalid." );

    template <class Comp = templatious::util::Default>
    bool keyExists(const ThisMap& h,const KeyType& k) {
        templatious::util::ComparatorEq<KeyType,KeyType,Comp> c;
        for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            if (c((*i).first,k)) {
                return true;
            }
        }

        return false;
    }

    template <class Comp = templatious::util::Default>
    bool get(const ThisMap& h,const KeyType& k,ValueType& v) {
        templatious::util::ComparatorEq<KeyType,KeyType,Comp> c;
        for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            if (c((*i).first,k)) {
                v = (*i).second;
                return true;
            }
        }
        return false;
    }

    template <class Comp = templatious::util::Default>
    ValueType& get(ThisMap& h,const KeyType& k) {
        templatious::util::ComparatorEq<KeyType,KeyType,Comp> c;
        for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            if (c((*i).first,k)) {
                return (*i).second;
            }
        }
        
        assert(false);
    }

    template <class Comp = templatious::util::Default>
    bool put(ThisMap& h,const KeyType& k,const ValueType& v) {
        templatious::util::ComparatorEq<KeyType,KeyType,Comp> c;
        for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            if (c((*i).first,k)) {
                i->second = v;
                return false;
            }
        }
        
        return SA::add(h,std::make_pair(k,v));
    }

    void clear(ThisMap& h) {
        SA::clear(h);
    }

    static size_t getSize(const ThisMap& h) {
        return SA::getSize(h);
    }

};

}
}


#endif /* end of include guard: COLLECTIONMAP_SDEAKTEN */
