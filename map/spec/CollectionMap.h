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
#include <templatious/CollectionMaker.h>

namespace templatious {

template <
    class Key, class Value,
    template <class...> class Coll,
    class Comparator = templatious::util::ComparatorEq<Key,Key>,
    template <class> class Alloc = std::allocator
>
struct CollectionMap {
    typedef templatious::StaticFactory SF;
    typedef templatious::StaticAdapter SA;

    typedef Key KeyType;
    typedef Value ValueType;
    typedef std::pair<KeyType,ValueType> NodeType;
    typedef typename templatious::adapters::CollectionMaker<
        NodeType, Coll, Alloc> ColMaker;
    typedef typename ColMaker::Collection CollectionType;

    Comparator _comp;
    CollectionType _col;

    CollectionMap(Comparator c) :
        _comp(c),
        _col(std::move(ColMaker::make()))
    { }

    CollectionMap() :
        _col(std::move(ColMaker::make()))
    { }

    bool keyExists(const KeyType& k) const {
        for (auto i = SA::cbegin(_col); i != SA::cend(_col); ++i) {
            if (_comp((*i).first,k)) {
                return true;
            }
        }

        return false;
    }

    bool get(const KeyType& k,ValueType& v) const {
        for (auto i = SA::begin(_col); i != SA::end(_col); ++i) {
            if (_comp((*i).first,k)) {
                v = (*i).second;
                return true;
            }
        }
        return false;
    }
    
    ValueType& get(const KeyType& k) {
        for (auto i = SA::begin(_col); i != SA::end(_col); ++i) {
            if (_comp((*i).first,k)) {
                return (*i).second;
            }
        }
        
        assert(false);
    }

    bool put(const KeyType& k,const ValueType& v) {
        for (auto i = SA::begin(_col); i != SA::end(_col); ++i) {
            if (_comp((*i).first,k)) {
                i->second = v;
                return false;
            }
        }
        
        return SA::add(_col,std::make_pair(k,v));
    }

    void clear() {
        SA::clear(_col);
    }

    size_t getSize() const {
        return SA::getSize(_col);
    }
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

    bool keyExists(const ThisMap& h,const KeyType& k) {
        return h.keyExists(k);
    }

    bool get(const ThisMap& h,const KeyType& k,ValueType& v) {
        return h.get(k,v);
    }

    ValueType& get(ThisMap& h,const KeyType& k) {
        return h.get(k);
    }

    bool put(ThisMap& h,const KeyType& k,const ValueType& v) {
        return h.put(k,v);
    }

    void clear(ThisMap& h) {
        h.clear();
    }

    static size_t getSize(const ThisMap& h) {
        return h.getSize();
    }

};

}

}


#endif /* end of include guard: COLLECTIONMAP_SDEAKTEN */
