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
#include <templatious/Loop.h>

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

    template <class T = int>
    static templatious::LoopL<T> loopL(const T& end) {
        return templatious::LoopL<T>(0,end,1);
    }

    template <class T = int>
    static templatious::LoopL<T> loopL(const T& start,const T& end) {
        return templatious::LoopL<T>(start,end,1);
    }

    template <class T = int>
    static templatious::LoopL<T> loopL(const T& start,const T& end,const T& step) {
        return templatious::LoopL<T>(start,end,step);
    }


    template <class T = int>
    static templatious::LoopL<T> loopI(const T& end) {
        if (0 <= end) {
            return templatious::LoopL<T>(0,end + 1,1);
        } else {
            return templatious::LoopL<T>(0,end - 1,1);
        }
    }

    template <class T = int>
    static templatious::LoopL<T> loopI(const T& start,const T& end) {
        if (start <= end) {
            return templatious::LoopL<T>(start,end + 1,1);
        } else {
            return templatious::LoopL<T>(start,end - 1,1);
        }
    }

    template <class T = int>
    static templatious::LoopL<T> loopI(const T& start,const T& end,const T& step) {
        assert((end - start) % step == 0 && "Ending of loop is not included with this step.");
        if (start <= end) {
            return templatious::LoopL<T>(start,end + step,step);
        } else {
            return templatious::LoopL<T>(start,end - step,step);
        }
    }

};

}

#endif /* end of include guard: STATICFACTORY_ABQ8DGTQ */
