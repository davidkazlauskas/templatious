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
#include <templatious/Sequence.h>
#include <templatious/Pack.h>
#include <templatious/Proxy.h>
#include <templatious/Virtual.h>
#include <templatious/virtual/VCollectionFactory.h>
#include <templatious/detail/MatchFunctor.h>

namespace templatious {

struct StaticFactory {

    template <
        class Val,
        template <class...> class Collection,
        template <class> class Allocator = std::allocator
    >
    static auto makeCollection() 
    -> decltype(templatious::adapters::CollectionMaker<Val,Collection,Allocator>().make()) 
    {
        typedef typename templatious::adapters::CollectionMaker<Val,Collection,Allocator> Maker;
        static_assert(Maker::is_maker_valid,"Collection maker is invalid.");
        Maker mk;
        return mk.make();
    }

    template <
        class Val,
        template <class...> class Collection,
        template <class> class Allocator = std::allocator
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
    static templatious::SeqL<T> seqL(const T& end) {
        return templatious::SeqL<T>(0,end,1);
    }

    template <class T = int>
    static templatious::SeqL<T> seqL(const T& start,const T& end) {
        return templatious::SeqL<T>(start,end,1);
    }

    template <class T = int>
    static templatious::SeqL<T> seqL(const T& start,const T& end,const T& step) {
        return templatious::SeqL<T>(start,end,step);
    }


    template <class T = int>
    static templatious::SeqL<T> seqI(const T& end) {
        if (0 <= end) {
            return templatious::SeqL<T>(0,end + 1,1);
        } else {
            return templatious::SeqL<T>(0,end - 1,1);
        }
    }

    template <class T = int>
    static templatious::SeqL<T> seqI(const T& start,const T& end) {
        if (start <= end) {
            return templatious::SeqL<T>(start,end + 1,1);
        } else {
            return templatious::SeqL<T>(start,end - 1,1);
        }
    }

    template <class T = int>
    static templatious::SeqL<T> seqI(const T& start,const T& end,const T& step) {
        assert((end - start) % step == 0 && "Ending of loop is not included with this step.");
        if (start <= end) {
            return templatious::SeqL<T>(start,end + step,step);
        } else {
            return templatious::SeqL<T>(start,end - step,step);
        }
    }

    template <class T>
    static auto range(T&& t,
        typename adapters::CollectionAdapter<T>::iterator b,
        typename adapters::CollectionAdapter<T>::iterator e)
            -> templatious::Range<T>
    {
        return Range<T>(std::forward<T>(t),b,e);
    }

    template <class T>
    static auto range(T&& t,
        typename adapters::CollectionAdapter<T>::iterator b)
            -> templatious::Range<T>
    {
        return Range<T>(std::forward<T>(t),b);
    }

    template <class T>
    static auto range(T&& t,
        size_t b, size_t e)
            -> templatious::Range<T>
    {
        typedef adapters::CollectionAdapter<T> Ad;
        return Range<T>(std::forward<T>(t),
                Ad::iter_at(std::forward<T>(t),b),
                Ad::iter_at(std::forward<T>(t),e));
    }

    template <class T>
    static auto range(T&& t,
        size_t b)
            -> Range<T>
    {
        typedef adapters::CollectionAdapter<T> Ad;
        return Range<T>(std::forward<T>(t),
                Ad::iter_at(std::forward<T>(t),b));
    }

    template <class T,class Fun>
    static auto filter(T&& t,Fun&& f)
        -> Filter<T,Fun>
    {
        return Filter<T,Fun>(
                std::forward<T>(t),
                std::forward<Fun>(f));
    }

    template <class T>
    static auto skip(T&& t,size_t sz)
        -> Skipper<T>
    {
        return Skipper<T>(
                std::forward<T>(t),
                sz);
    }

    template <class T>
    static auto socket(T& t)
     -> CollectionSocket<T>
    {
        return CollectionSocket<T>(t);
    }

    template <class T>
    static auto vcollection(T& t)
     -> VCollection< typename adapters::CollectionAdapter<T>::value_type >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::value_type ValType;

        typedef VCollectionImpl<T> VImpl;
        VImpl *v = new VImpl( t );
        return VCollection< ValType >(v);
    }

    template <
        bool add = false,
        bool clear = false,
        bool traverse = false,
        bool access = false,
        bool size = false,
        class T
    >
    static auto prevent(T&& t)
     -> typename templatious::VCollectionFactory<
         T,false,
         templatious::util::IntSelector<size,SP_PREVENT,SP_ENABLED>::val,
         templatious::util::IntSelector<access,ACP_PREVENT,ACP_ENABLED>::val,
         templatious::util::IntSelector<add,AP_PREVENT,AP_ENABLED>::val,
         templatious::util::IntSelector<clear,CP_PREVENT,CP_ENABLED>::val,
         templatious::util::IntSelector<traverse,TP_PREVENT,TP_ENABLED>::val
         >::Type
    {

        typedef typename templatious::VCollectionFactory<
             T,false,
             templatious::util::IntSelector<size,SP_PREVENT,SP_ENABLED>::val,
             templatious::util::IntSelector<access,ACP_PREVENT,ACP_ENABLED>::val,
             templatious::util::IntSelector<add,AP_PREVENT,AP_ENABLED>::val,
             templatious::util::IntSelector<clear,CP_PREVENT,CP_ENABLED>::val,
             templatious::util::IntSelector<traverse,TP_PREVENT,TP_ENABLED>::val
             > Maker;

        return Maker::make(std::forward<T>(t));
    }

    template <
        bool add = false,
        bool clear = false,
        bool traverse = false,
        bool access = false,
        bool size = false,
        class T
    >
    static auto allow(T&& t)
     -> typename templatious::VCollectionFactory<
         T,false,
         templatious::util::IntSelector<size,SP_ENABLED,SP_PREVENT>::val,
         templatious::util::IntSelector<access,ACP_ENABLED,ACP_PREVENT>::val,
         templatious::util::IntSelector<add,AP_ENABLED,AP_PREVENT>::val,
         templatious::util::IntSelector<clear,CP_ENABLED,CP_PREVENT>::val,
         templatious::util::IntSelector<traverse,TP_ENABLED,TP_PREVENT>::val
         >::Type
    {

        typedef typename templatious::VCollectionFactory<
             T,false,
             templatious::util::IntSelector<size,SP_ENABLED,SP_PREVENT>::val,
             templatious::util::IntSelector<access,ACP_ENABLED,ACP_PREVENT>::val,
             templatious::util::IntSelector<add,AP_ENABLED,AP_PREVENT>::val,
             templatious::util::IntSelector<clear,CP_ENABLED,CP_PREVENT>::val,
             templatious::util::IntSelector<traverse,TP_ENABLED,TP_PREVENT>::val
             > Maker;

        return Maker::make(std::forward<T>(t));
    }

    template <class... T>
    static auto pack(T&&... t)
     -> decltype( detail::packUp(std::forward<T>(t)...) ) {
        return detail::packUp(std::forward<T>(t)...);
    }

    template <class P,class T>
    static auto packInsert(P p,T&& t)
     -> decltype(p.template insert<T>(std::forward<T>(t)))
    {
        return p.template insert<T>(std::forward<T>(t));
    }

    template <class P,class T>
    static auto packInsertWithin(P p,T&& t)
     -> decltype(p.insertWithin(std::forward<T>(t)))
    {
        return p.insertWithin(std::forward<T>(t));
    }

    template <class... T,class Func>
    static auto matchTight(Func&& f)
     -> detail::Match<
        templatious::TypeList<T...>,
        Func,
        detail::TightRecursiveComparison,
        templatious::detail::TypelistsEqual
     >
    {
        typedef detail::Match<
            templatious::TypeList<T...>,
            Func,
            detail::TightRecursiveComparison,
            templatious::detail::TypelistsEqual
        > TheMatch;

        return TheMatch(std::forward<Func>(f));
    }

    template <class... T,class Func>
    static auto matchLoose(Func&& f)
     -> detail::Match<
        templatious::TypeList<T...>,
        Func,
        detail::LooseRecursiveComparison,
        templatious::detail::TypelistContains
     >
    {
        typedef detail::Match<
            templatious::TypeList<T...>,
            Func,
            detail::LooseRecursiveComparison,
            templatious::detail::TypelistContains
        > TheMatch;

        return TheMatch(std::forward<Func>(f));
    }

    template <class Func>
    static auto matchAny(Func&& f)
     -> detail::Match<
        templatious::TypeList< AnyType >,
        Func,
        detail::LooseRecursiveComparison,
        templatious::detail::TypelistContains
     >
    {
        typedef detail::Match<
            templatious::TypeList< AnyType >,
            Func,
            detail::LooseRecursiveComparison,
            templatious::detail::TypelistContains
        > TheMatch;
        return TheMatch(std::forward<Func>(f));
    }

    static auto matchAnyDoNothing()
     -> detail::Match<
        templatious::TypeList< AnyType >,
        templatious::util::DoNothingFunctor,
        detail::LooseRecursiveComparison,
        templatious::detail::TypelistContains
     >
    {
        typedef detail::Match<
            templatious::TypeList< AnyType >,
            templatious::util::DoNothingFunctor,
            detail::LooseRecursiveComparison,
            templatious::detail::TypelistContains
        > TheMatch;
        return TheMatch(templatious::util::DoNothingFunctor());
    }

    template <class... T>
    static auto matchFunctor(T&&... t)
     -> detail::MatchFunctor<T...>
    {
        typedef detail::MatchFunctor<T...> Fctor;
        return Fctor(std::forward<T>(t)...);
    }

};

}

#endif /* end of include guard: STATICFACTORY_ABQ8DGTQ */
