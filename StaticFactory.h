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
#include <templatious/detail/UserUtil.h>
#include <templatious/detail/ChainFunctor.h>
#include <templatious/detail/OnceTraversable.h>
#include <templatious/detail/PackFunctor.h>
#include <templatious/detail/CollectionRepeater.h>
#include <templatious/util/Functions.h>

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
    static auto seqL(const T& end)
     -> templatious::SeqL<T>
    {
        return templatious::SeqL<T>(0,end,1);
    }

    template <class T = int>
    static auto seqL(const T& start,const T& end)
     -> templatious::SeqL<T>
    {
        return templatious::SeqL<T>(start,end,1);
    }

    template <class T = int>
    static auto seqL(const T& start,const T& end,const T& step)
     -> templatious::SeqL<T>
    {
        return templatious::SeqL<T>(start,end,step);
    }

    template <class T = int>
    static auto seqI(const T& end)
     -> templatious::SeqL<T>
    {
        if (0 <= end) {
            return templatious::SeqL<T>(0,end + 1,1);
        } else {
            return templatious::SeqL<T>(0,end - 1,1);
        }
    }

    template <class T = int>
    static auto seqI(const T& start,const T& end)
     -> templatious::SeqL<T>
    {
        if (start <= end) {
            return templatious::SeqL<T>(start,end + 1,1);
        } else {
            return templatious::SeqL<T>(start,end - 1,1);
        }
    }

    template <class T = int>
    static auto seqI(const T& start,const T& end,const T& step)
     -> templatious::SeqL<T>
    {
        if ((end - start) % step != 0) {
            throw IncorrectBoundsException();
        }

        if (start <= end) {
            return templatious::SeqL<T>(start,end + step,step);
        } else {
            return templatious::SeqL<T>(start,end - step,step);
        }
    }

    // Repeater absolute
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto repA(size_t n,T&& c)
     -> detail::Repeater<
            false,
            StoragePolicy,
            decltype(std::forward<T>(c))
     >
    {
        return detail::Repeater<
           false,
           StoragePolicy,
           decltype(std::forward<T>(c))
        >(n,std::forward<T>(c));
    }

    // Repeater multiply
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto repM(size_t n,T&& c)
     -> detail::Repeater<
            true,
            StoragePolicy,
            decltype(std::forward<T>(c))
     >
    {
        return detail::Repeater<
           true,
           StoragePolicy,
           decltype(std::forward<T>(c))
        >(n,std::forward<T>(c));
    }

    template <class T,template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy>
    static auto range(T&& t,
        typename adapters::CollectionAdapter<T>::Iterator b,
        typename adapters::CollectionAdapter<T>::Iterator e)
            -> templatious::Range<T,StoragePolicy>
    {
        return Range<T,StoragePolicy>(std::forward<T>(t),b,e);
    }

    template <class T,template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy>
    static auto range(T&& t,
        typename adapters::CollectionAdapter<T>::Iterator b)
            -> templatious::Range<decltype(std::forward<T>(t)),StoragePolicy>
    {
        return Range<decltype(std::forward<T>(t)),StoragePolicy>(
            std::forward<T>(t),b);
    }

    template <class T,template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy>
    static auto range(T&& t,
        size_t b, size_t e)
            -> templatious::Range<decltype(std::forward<T>(t)),StoragePolicy>
    {
        typedef adapters::CollectionAdapter<T> Ad;
        return Range<decltype(std::forward<T>(t)),StoragePolicy>(
            std::forward<T>(t),
            Ad::iterAt(std::forward<T>(t),b),
            Ad::iterAt(std::forward<T>(t),e));
    }

    template <class T,template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy>
    static auto range(T&& t,
        size_t b)
            -> Range<decltype(std::forward<T>(t)),StoragePolicy>
    {
        typedef adapters::CollectionAdapter<T> Ad;
        return Range<decltype(std::forward<T>(t)),StoragePolicy>(
            std::forward<T>(t),
            Ad::iterAt(std::forward<T>(t),b));
    }

    template <class T,class Fun,template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy>
    static auto filter(T&& t,Fun&& f)
        -> Filter<decltype(std::forward<T>(t)),Fun,StoragePolicy>
    {
        return Filter<
                decltype(std::forward<T>(t)),
                Fun,StoragePolicy
        >(  std::forward<T>(t),
            std::forward<Fun>(f));
    }

    template <class T,template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy>
    static auto skip(T&& t,size_t sz)
        -> Skipper<decltype(std::forward<T>(t)),StoragePolicy>
    {
        return Skipper<decltype(std::forward<T>(t)),StoragePolicy>(
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
     -> VCollection< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::ValueType ValType;

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

    template <
        template <class> class StoragePolicy =
            DefaultPackStoragePolicy,
        class... T
    >
    static auto pack(T&&... t)
     -> decltype(
            detail::PackAccess::packUp<StoragePolicy>(
                std::forward<T>(t)...)
        )
    {
        return detail::PackAccess::packUp<StoragePolicy>(
                std::forward<T>(t)...);
    }

    template <
        template <class> class StoragePolicy =
            DefaultPackStoragePolicy,
        class P,class T
    >
    static auto packInsert(P&& p,T&& t)
     -> decltype(detail::PackAccess::packInsert<StoragePolicy>(
            std::forward<P>(p),std::forward<T>(t)))
    {
        return detail::PackAccess::packInsert<StoragePolicy>(
            std::forward<P>(p),std::forward<T>(t));
    }

    template <class P,class T>
    static auto packInsertWithin(P&& p,T&& t)
     -> decltype(detail::PackAccess::packInsertWithin(
                 std::forward<P>(p),std::forward<T>(t)))
    {
        return detail::PackAccess::packInsertWithin(
                std::forward<P>(p),std::forward<T>(t));
    }

    template <class TrPol,class P,class... T>
    static auto packTransformWithin(P&& p,T&&... t)
     -> decltype(
            detail::PackAccess::packTransformWithin<TrPol>(
                std::forward<P>(p),
                std::forward<T>(t)...)
        )
    {
        return detail::PackAccess::packTransformWithin<TrPol>(
            std::forward<P>(p),
            std::forward<T>(t)...
        );
    }

    template <int n,class... Args>
    static auto packRepeat(Args&&... args)
     -> decltype(
         detail::PackAccess::packRepeat<n>(
             std::forward<Args>(args)...)
     )
    {
        return detail::PackAccess::packRepeat<n>(
             std::forward<Args>(args)...);
    }

    template <int n = 1>
    static auto dummyVar()
     -> decltype(
         detail::PackAccess::packRepeat<n>(
             detail::dummyVar
         )
     )
    {
        return detail::PackAccess::packRepeat<n>(
            detail::dummyVar
        );
    }

    template <
        template <class> class StoragePolicy =
            ::templatious::util::DefaultStoragePolicy,
        class F,class... Args>
    static auto packFunctor(F&& f,Args&&... args)
     -> detail::PackFunctor<
         StoragePolicy,
         decltype(std::forward<F>(f)),
         decltype(pack(std::forward<Args>(args)...))
     >
    {
        return detail::PackFunctor<
            StoragePolicy,
            decltype(std::forward<F>(f)),
            decltype(pack(std::forward<Args>(args)...))
        >(
            std::forward<F>(f),
            pack(std::forward<Args>(args)...)
        );
    };

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

    template <class T>
    static auto streamFunctor(T& t)
     -> detail::CallEachStreamFunctor<T&>
    {
        return detail::CallEachStreamFunctor<T&>(t);
    }

    template <
        template <class> class T,
        class Stor
    >
    static auto storageFunctor(Stor&& s)
     -> T< decltype(std::forward<Stor>(s)) >
    {
        return T< decltype(std::forward<Stor>(s)) >(
                std::forward<Stor>(s));
    }

    template <
        bool statefulDefault = false,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class... Args
    >
    static auto chainFunctor(Args&&... args)
     -> decltype(
        detail::makeChainFunctor< statefulDefault, false, StoragePolicy >(
            detail::makeFunctorPair< StoragePolicy >(
                std::forward<Args>(args)
            )...
        )
     )
    {
        return detail::makeChainFunctor< statefulDefault, false, StoragePolicy >(
            detail::makeFunctorPair< StoragePolicy >(
                std::forward<Args>(args)
            )...
        );
    }

    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T,class U
    >
    static auto functorPair(T&& t,U&& u)
     -> detail::FunctorPair< StoragePolicy, true, T, U >
    {
        return detail::FunctorPair< StoragePolicy, true, T, U >(
            std::forward<T>(t), std::forward<U>(u)
        );
    }

    template <class T>
    static auto onceTraversable(T&& t)
     -> decltype(
        detail::makeOnceTraversable<
            templatious::util::DefaultStoragePolicy
        >(std::forward<T>(t))
     )
    {
        return detail::makeOnceTraversable<
            templatious::util::DefaultStoragePolicy
        >(std::forward<T>(t));
    }
};

}

#endif /* end of include guard: STATICFACTORY_ABQ8DGTQ */
