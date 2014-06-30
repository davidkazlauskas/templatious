/*
 * =====================================================================================
 *
 *       Filename:  StaticFactory.h
 *
 *    Description:  Static factory to instantiate collections/maps
 *
 *        Version:  1.0
 *        Created:  06/30/2014 07:17:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef STATICFACTORY_ABQ8DGTQ
#define STATICFACTORY_ABQ8DGTQ

#include <templatious/map/MapMaker.h>
#include <templatious/CollectionMaker.h>

namespace templatious {

struct StaticFactory {

    template <
        class Val,
        template <class...> class Collection,
        template <class> class Allocator 
    >
    static auto makeCollection() 
    -> decltype(templatious::adapters::CollectionMaker<Val,Collection,Allocator>().make()) 
    {
        templatious::adapters::CollectionMaker<Val,Collection,Allocator> mk;
        return mk.make();
    }

    template <
        class Val,
        template <class...> class Collection,
        template <class> class Allocator 
    >
    static auto makeCollection(size_t size) 
    -> decltype(templatious::adapters::CollectionMaker<Val,Collection,Allocator>().make(size)) 
    {
        templatious::adapters::CollectionMaker<Val,Collection,Allocator> mk;
        return mk.make(size);
    }

    template <
        class Key,
        class Value,
        template <class...> class Map,
        class Hash = templatious::util::Hasher<Key>,
        template <class> class Allocator = std::allocator
    >
    static auto makeMap()
    -> decltype(templatious::adapters::MapMaker<Key,Value,Map,Hash,Allocator>().make()) 
    {
        templatious::adapters::MapMaker<Key,Value,Map,Hash,Allocator> mk;
        return mk.make();
    }

    template <
        class Key,
        class Value,
        template <class...> class Map,
        class Hash = templatious::util::Hasher<Key>,
        template <class> class Allocator = std::allocator
    >
    static auto makeMap(const Hash& h) 
    -> decltype(templatious::adapters::MapMaker<Key,Value,Map,Hash,Allocator>(h).make()) 
    {
        templatious::adapters::MapMaker<Key,Value,Map,Hash,Allocator> mk(h);
        return mk.make();
    }

    template <
        class Key,
        class Value,
        template <class...> class Map,
        class Hash = templatious::util::Hasher<Key>,
        template <class> class Allocator = std::allocator
    >
    static auto makeMap(size_t size) 
    -> decltype(templatious::adapters::MapMaker<Key,Value,Map,Hash,Allocator>().make(size)) 
    {
        templatious::adapters::MapMaker<Key,Value,Map,Hash,Allocator> mk;
        return mk.make(size);
    }

    template <
        class Key,
        class Value,
        template <class...> class Map,
        class Hash = templatious::util::Hasher<Key>,
        template <class> class Allocator = std::allocator
    >
    static auto makeMap(const Hash& h,size_t size) 
    -> decltype(templatious::adapters::MapMaker<Key,Value,Map,Hash,Allocator>(h).make(size)) 
    {
        templatious::adapters::MapMaker<Key,Value,Map,Hash,Allocator> mk(h);
        return mk.make(size);
    }

};

}

#endif /* end of include guard: STATICFACTORY_ABQ8DGTQ */
