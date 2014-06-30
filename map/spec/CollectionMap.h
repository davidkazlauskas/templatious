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
#include <memory>

#include <templatious/util/spec/DefaultComparators.h>
#include <templatious/map/MapAdapter.h>
#include <templatious/StaticFactory.h>

namespace templatious {

template <
    class Key, class Value,
    template <class...> class Coll,
    class Comparator = templatious::util::ComparatorEq<Key,Key>,
    template <class> class Alloc = std::allocator
>
struct CollectionMap {
    typedef templatious::StaticFactory SF;

    typedef decltype(SF::makeCollection< std::pair<Key,Value>, Coll, Alloc>()) CollectionType;

};

namespace adapters {

template <
    class Key,class Value,
    template <class...> class Coll,
    class Comparator,
    template <class> class Alloc 
>
struct MapAdapter< templatious::CollectionMap<Key,Value,Coll,Comparator,Alloc> > {

    static const bool is_valid = true;
    static const bool is_comparator_const = false;

    typedef Key KeyType;
    typedef Value ValueType;
    typedef templatious::StaticFactory SF;
    typedef templatious::CollectionMap<Key,Value,Coll,Comparator,Alloc> ThisMap;
    typedef typename templatious::StaticAdapter SA;

    static_assert( templatious::adapters::CollectionAdapter< 
            typename ThisMap::CollectionType >::is_valid, "Map is invalid." );

    //template <class Comp = templatious::util::Default>
    //bool keyExists(const ThisMap& h,const KeyType& k) {
        //templatious::util::ComparatorEq<KeyType,KeyType,Comp> c;
        //for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            //if (c((*i).first,k)) {
                //return true;
            //}
        //}

        //return false;
    //}

    //template <class Comp = templatious::util::Default>
    //bool get(const ThisMap& h,const KeyType& k,ValueType& v) {
        //templatious::util::ComparatorEq<KeyType,KeyType,Comp> c;
        //for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            //if (c((*i).first,k)) {
                //v = (*i).second;
                //return true;
            //}
        //}
        //return false;
    //}

    //template <class Comp = templatious::util::Default>
    //ValueType& get(ThisMap& h,const KeyType& k) {
        //templatious::util::ComparatorEq<KeyType,KeyType,Comp> c;
        //for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            //if (c((*i).first,k)) {
                //return (*i).second;
            //}
        //}
        
        //assert(false);
    //}

    //template <class Comp = templatious::util::Default>
    //bool put(ThisMap& h,const KeyType& k,const ValueType& v) {
        //templatious::util::ComparatorEq<KeyType,KeyType,Comp> c;
        //for (auto i = SA::begin(h); i != SA::end(h); ++i) {
            //if (c((*i).first,k)) {
                //i->second = v;
                //return false;
            //}
        //}
        
        //return SA::add(h,std::make_pair(k,v));
    //}

    //void clear(ThisMap& h) {
        //SA::clear(h);
    //}

    //static size_t getSize(const ThisMap& h) {
        //return SA::getSize(h);
    //}

};

}

}


#endif /* end of include guard: COLLECTIONMAP_SDEAKTEN */
