//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  StaticFactory.hpp
 *
 *    Description:  Static factory to instantiate collections/maps
 *
 *        Version:  1.0
 *        Created:  06/30/2014 07:17:08 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef STATICFACTORY_ABQ8DGTQ
#define STATICFACTORY_ABQ8DGTQ

#include <templatious/CollectionMaker.hpp>
#include <templatious/Sequence.hpp>
#include <templatious/Pack.hpp>
#include <templatious/Proxy.hpp>
#include <templatious/Virtual.hpp>
#include <templatious/virtual/VCollectionFactory.hpp>
#include <templatious/detail/MatchFunctor.hpp>
#include <templatious/detail/UserUtil.hpp>
#include <templatious/detail/ChainFunctor.hpp>
#include <templatious/detail/OnceTraversable.hpp>
#include <templatious/detail/PackFunctor.hpp>
#include <templatious/detail/CollectionRepeater.hpp>
#include <templatious/detail/ItemRepeater.hpp>
#include <templatious/detail/SelectCollection.hpp>
#include <templatious/detail/VirtualPack.hpp>
#include <templatious/detail/VirtualMatchFunctor.hpp>
#include <templatious/util/Functions.hpp>

namespace templatious {

/**
 * A class which holds static methods that
 * mainly have to do with object creation.
 */
struct StaticFactory {
    static const int DefVpackSettings = VPACK_DEF_MASK;

    /**
     * Uniform function to make collection of any type.
     * @param[in] Val Value type for collection.
     * @param[in] Collection Collection template.
     * @param[in] Collection allocator template, defaults
     * to std::allocator (ignored if collection doesn't
     * use allocators).
     */
    template <
        class Val,
        template <class...> class Collection,
        template <class> class Allocator = std::allocator
    >
    static auto makeCollection()
    -> decltype(templatious::adapters::CollectionMaker<
            Val,Collection,Allocator>().make())
    {
        typedef typename templatious::adapters::
            CollectionMaker<Val,Collection,Allocator> Maker;
        static_assert(Maker::is_maker_valid,
                "Collection maker is invalid.");
        Maker mk;
        return mk.make();
    }

    /**
     * Uniform function to make collection of any type with size hint.
     * @param[in] size Size hint for collection.
     * @param[in] Val Value type for collection.
     * @param[in] Collection Collection template.
     * @param[in] Collection allocator template, defaults
     * to std::allocator (ignored if collection doesn't
     * use allocators).
     */
    template <
        class Val,
        template <class...> class Collection,
        template <class> class Allocator = std::allocator
    >
    static auto makeCollection(long size)
    -> decltype(templatious::adapters::CollectionMaker<
            Val,Collection,Allocator>().make(size))
    {
        templatious::adapters::CollectionMaker<
            Val,Collection,Allocator> mk;
        return mk.make(size);
    }

    /**
     * Sequence Less.
     * Create numeric sequence from
     * 0 until end by step of 1.
     * @param[in] end Value to specify end
     * of collection.
     */
    template <class T>
    static auto seqL(const T& end)
     -> templatious::SeqL<T>
    {
        return templatious::SeqL<T>(0,end,1);
    }

    /**
     * Sequence Less.
     * Create numeric sequence from
     * start until end by step of 1.
     * @param[in] start Value to specify start
     * of collection.
     * @param[in] end Value to specify end
     * of collection.
     */
    template <class T>
    static auto seqL(const T& start,const T& end)
     -> templatious::SeqL<T>
    {
        return templatious::SeqL<T>(start,end,1);
    }

    /**
     * Sequence Less.
     * Create numeric sequence from
     * start until end by step of step.
     * @param[in] start Value to specify start
     * of collection.
     * @param[in] end Value to specify end
     * of collection.
     * @param[in] step Value to specify step
     * of collection. Has to be always positive,
     * even if start > end.
     */
    template <class T>
    static auto seqL(const T& start,const T& end,const T& step)
     -> templatious::SeqL<T>
    {
        return templatious::SeqL<T>(start,end,step);
    }

    /**
     * Sequence Include.
     * Create numeric sequence from
     * 0 to end by step of 1.
     * @param[in] start Value to specify start
     * of collection.
     */
    template <class T>
    static auto seqI(const T& end)
     -> templatious::SeqL<T>
    {
        if (0 <= end) {
            return templatious::SeqL<T>(0,end + 1,1);
        } else {
            return templatious::SeqL<T>(0,end - 1,1);
        }
    }

    /**
     * Sequence Include.
     * Create numeric sequence from
     * start to end by step of 1.
     * @param[in] start Value to specify start
     * of collection.
     * @param[in] end Value to specify end
     * of collection.
     */
    template <class T>
    static auto seqI(const T& start,const T& end)
     -> templatious::SeqL<T>
    {
        if (start <= end) {
            return templatious::SeqL<T>(start,end + 1,1);
        } else {
            return templatious::SeqL<T>(start,end - 1,1);
        }
    }

    /**
     * Sequence Include.
     * Create numeric sequence from
     * start to end by step of step.
     * Throws in case (end - start) % step != 0.
     * @param[in] start Value to specify start
     * of collection.
     * @param[in] end Value to specify end
     * of collection.
     * @param[in] step Value to specify step
     * of collection. Has to be always positive,
     * even if start > end.
     */
    template <class T>
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

    /**
     * Repeater Absolute.
     * Returns reapeater to repeat
     * collection specified amount
     * of times using absolute repeating.
     * For instance, if collection {1,2,3}
     * is repeated 7 times result is collection
     * {1,2,3,1,2,3,1}.
     * @param[in] n Times to repeat.
     * @param[in] c Collection to repeat.
     * @param[in] StoragePolicy
     * Storage policy to specify how to
     * store collection c in itself.
     * Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto repA(long n,T&& c)
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

    /**
     * Repeater Multiply.
     * Returns reapeater to repeat
     * collection specified amount
     * of times using multiple repeating.
     * For instance, if collection {1,2}
     * is repeated 3 times result is collection
     * {1,2,1,2,1,2}.
     * @param[in] n Times to repeat.
     * @param[in] c Collection to repeat.
     * @param[in] StoragePolicy
     * Storage policy to specify how to
     * store collection c in itself.
     * Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto repM(long n,T&& c)
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

    /**
     * Repeater Same.
     * Collection which repeats same
     * element specified amount
     * of times.
     * For instance, if element 55
     * is repeated 7 times result collection
     * is {55,55,55,55,55,55,55}.
     * @param[in] n Times to repeat.
     * @param[in] t Element to repeat.
     * @param[in] StoragePolicy
     * Storage policy to specify how to
     * store element t in itself.
     * Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto repS(long n,T&& t)
     -> detail::ItemRepeater<
        decltype(std::forward<T>(t)),
        StoragePolicy
     >
    {
        return detail::ItemRepeater<
           decltype(std::forward<T>(t)),
           StoragePolicy
        >(std::forward<T>(t),n);
    }

    /**
     * Return range from existing collection
     * specifying begin and end iterators.
     * Passing b iterator which is greater than
     * e iterator causes undefined behaviour.
     * Modifying underlying collection
     * (adding or removing elements) and
     * trying to use this one causes
     * undefined behaviour.
     * @param[in] t Collection to return range from.
     * @param[in] b Collection begin iterator.
     * @param[in] e Collection end iterator.
     * @param[in] StoragePolicy Storage policy
     * to specify how to store t. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto range(T&& t,
        typename adapters::CollectionAdapter<T>::Iterator b,
        typename adapters::CollectionAdapter<T>::Iterator e)
            -> templatious::Range<T,StoragePolicy>
    {
        return Range<T,StoragePolicy>(std::forward<T>(t),b,e);
    }

    /**
     * Return range from existing collection from
     * begin to end only begin iterator.
     * Modifying underlying collection
     * (adding or removing elements) and
     * trying to use this one causes
     * undefined behaviour.
     * @param[in] t Collection to return range from.
     * @param[in] b Collection begin iterator.
     * @param[in] StoragePolicy Storage policy
     * to specify how to store t. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto range(T&& t,
        typename adapters::CollectionAdapter<T>::Iterator b)
            -> templatious::Range<decltype(
                    std::forward<T>(t)),StoragePolicy>
    {
        return Range<decltype(std::forward<T>(t)),StoragePolicy>(
            std::forward<T>(t),b);
    }

    /**
     * Return range from existing collection from
     * begin index to end index.
     * Modifying underlying collection
     * (adding or removing elements) and
     * trying to use this one causes
     * undefined behaviour.
     * @param[in] t Collection to return range from.
     * @param[in] b Collection begin index.
     * @param[in] e Collection end index.
     * @param[in] StoragePolicy Storage policy
     * to specify how to store t. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto range(T&& t, long b, long e)
        -> templatious::Range<decltype(std::forward<T>(t)),StoragePolicy>
    {
        if (b > e) {
            throw RangeBeginMoreThanEndException();
        }

        typedef adapters::CollectionAdapter<T> Ad;
        return Range<decltype(std::forward<T>(t)),StoragePolicy>(
            std::forward<T>(t),
            Ad::iterAt(t,b),
            Ad::iterAt(t,e));
    }

    /**
     * Return range from existing collection from
     * begin index until end.
     * Modifying underlying collection
     * (adding or removing elements) and
     * trying to use this one causes
     * undefined behaviour.
     * @param[in] t Collection to return range from.
     * @param[in] b Collection begin index.
     * @param[in] StoragePolicy Storage policy
     * to specify how to store t. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto range(T&& t, long b)
        -> Range<decltype(std::forward<T>(t)),StoragePolicy>
    {
        typedef adapters::CollectionAdapter<T> Ad;
        return Range<decltype(std::forward<T>(t)),StoragePolicy>(
            std::forward<T>(t),
            Ad::iterAt(std::forward<T>(t),b));
    }

    /**
     * Copy range from existing collection from
     * begin iterator until end iterator.
     * @param[in] t Collection to return range from.
     * @param[in] args Arguments which would be passed
     * to simple range function (iterators or indexes).
     * @param[in] Collection Collection type to return.
     * Defaults to std::vector.
     * @param[in] Allocator Collection allocator.
     * Defaults to std::allocator (is ignored
     * if collection doesn't use allocators).
     */
    template <
        template <class...> class Collection = std::vector,
        template <class> class Allocator = std::allocator,
        class T,
        class... Args
    >
    static auto rangeC(T&& t,Args&&... args) ->
        typename templatious::adapters::CollectionMaker<
            typename templatious::adapters::
                CollectionAdapter<T>::ValueType,
            Collection, Allocator
        >::Collection
    {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        auto rTemp = StaticFactory::range(
            std::forward<T>(t),
            std::forward<Args>(args)...
        );
        typedef templatious::adapters::CollectionMaker<
            typename Ad::ValueType, Collection, Allocator > Mk;
        auto res = Mk::make();
        templatious::StaticAdapter::add(res,rTemp);
        return std::move(res);
    }

    /**
     * Filter collection using predicate.
     * Modifying underlying collection
     * (adding or removing elements) and
     * trying to use this one causes
     * undefined behaviour.
     * @param[in] t Collection to return range from.
     * @param[in] f Predicate function which decides
     * what elements to expose in collection.
     * Should take in one element of collection
     * and return true if element should be kept
     * in the filter, false if element should be ignored.
     * @param[in] StoragePolicy Storage policy
     * to specify how to store the t collection
     * in the filter. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T, class Fun
    >
    static auto filter(T&& t,Fun&& f)
        -> Filter<decltype(std::forward<T>(t)),Fun,StoragePolicy>
    {
        return Filter<
                decltype(std::forward<T>(t)),
                Fun,StoragePolicy
        >(  std::forward<T>(t),
            std::forward<Fun>(f));
    }

    /**
     * Copy filter collection using predicate.
     * @param[in] t Collection to return range from.
     * @param[in] args Arguments which would be
     * passed to normal filter function.
     * @param[in] Collection Collection type to return.
     * Defaults to std::vector.
     * @param[in] Allocator Collection allocator.
     * Defaults to std::allocator (is ignored
     * if collection doesn't use allocators).
     */
    template <
        template <class...> class Collection = std::vector,
        template <class> class Allocator = std::allocator,
        class T,
        class... Args
    >
    static auto filterC(T&& t,Args&&... args) ->
        typename templatious::adapters::CollectionMaker<
            typename templatious::adapters::
                CollectionAdapter<T>::ValueType,
            Collection, Allocator
        >::Collection
    {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        auto fTemp = StaticFactory::filter(
            std::forward<T>(t),
            std::forward<Args>(args)...
        );
        typedef templatious::adapters::CollectionMaker<
            typename Ad::ValueType, Collection, Allocator > Mk;
        auto res = Mk::make();
        templatious::StaticAdapter::add(res,fTemp);
        return std::move(res);
    }

    /**
     * Skip collection each sz elements.
     * Modifying underlying collection
     * (adding or removing elements) and
     * trying to use this one causes
     * undefined behaviour.
     * @param[in] t Collection to return range from.
     * @param[in] sz Skip size.
     * @param[in] StoragePolicy Storage policy
     * to specify how to store t. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto skip(T&& t,long sz)
        -> Skipper<decltype(std::forward<T>(t)),StoragePolicy>
    {
        return Skipper<decltype(std::forward<T>(t)),StoragePolicy>(
                std::forward<T>(t),
                sz);
    }

    /**
     * Copy skip collection each sz elements.
     * @param[in] t Collection to return range from.
     * @param[in] sz Skip size.
     * @param[in] args Arguments which would be passed
     * to simple range function (iterators or indexes).
     * @param[in] Collection Collection type to return.
     * Defaults to std::vector.
     * @param[in] Allocator Collection allocator.
     * Defaults to std::allocator (is ignored
     * if collection doesn't use allocators).
     */
    template <
        template <class...> class Collection = std::vector,
        template <class> class Allocator = std::allocator,
        class T,
        class... Args
    >
    static auto skipC(T&& t,Args&&... args) ->
        typename templatious::adapters::CollectionMaker<
            typename templatious::adapters::
                CollectionAdapter<T>::ValueType,
            Collection, Allocator
        >::Collection
    {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        auto fTemp = StaticFactory::skip(
            std::forward<T>(t),
            std::forward<Args>(args)...
        );
        typedef templatious::adapters::CollectionMaker<
            typename Ad::ValueType, Collection, Allocator > Mk;
        auto res = Mk::make();
        templatious::StaticAdapter::add(res,fTemp);
        return std::move(res);
    }


    /**
     * Select something from collection
     * according to predicate function.
     * Returns handle with elements selected.
     * @param[in] t Collection to select from.
     * @param[in] f Function to use on one
     * collection element when selecting.
     * @param[in] ColType Type of the resulting
     * collection. If void it is inferred from
     * predicate function passed. Defaults to void.
     */
    template <
        class ColType = void, // infer from function if void
        template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy,
        class T,class F
    >
    static auto select(T&& t,F&& f)
     -> decltype(
         SelectCollectionMaker<
            std::is_same<ColType,void>::value,
            ColType,
            StoragePolicy
         >::Alg::make(std::forward<T>(t),std::forward<F>(f))
     )
    {
        return SelectCollectionMaker<
           std::is_same<ColType,void>::value,
           ColType,
           StoragePolicy
        >::Alg::make(std::forward<T>(t),std::forward<F>(f));
    }

    /**
     * Select something from collection
     * according to predicate function.
     * Returns copy of selected elements.
     * @param[in] t Collection to select from.
     * @param[in] f Function to use on one
     * collection element when selecting.
     * @param[in] ColType Type of the resulting
     * collection. If void it is inferred from
     * predicate function passed. Defaults to void.
     */
    template <
        class ColType = void, // infer from function if void
        template <class...> class Collection = std::vector,
        template <class> class Allocator = std::allocator,
        template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy,
        class T,
        class... Args
    >
    static auto selectC(T&& t,Args&&... args) ->
        typename templatious::adapters::CollectionMaker<
            typename templatious::adapters::
                CollectionAdapter<
                    decltype(
                        StaticFactory::select<ColType>(
                            std::forward<T>(t),
                            std::forward<Args>(args)...
                        )
                    )
                >::ValueType,
            Collection, Allocator
        >::Collection
    {
        auto sTemp = StaticFactory::select<ColType>(
            std::forward<T>(t),
            std::forward<Args>(args)...
        );

        typedef typename templatious::adapters::CollectionAdapter<
            decltype(sTemp)
        >::ValueType OutType;

        typedef templatious::adapters::CollectionMaker<
            OutType, Collection, Allocator > Mk;
        auto sz = sTemp.size();
        // size unknown if -1
        if (sz == -1) {
            auto res = Mk::make();
            templatious::StaticAdapter::add(res,sTemp);
            return std::move(res);
        } else {
            auto res = Mk::make(sz);
            templatious::StaticAdapter::add(res,sTemp);
            return std::move(res);
        }
    }

    /**
     * Get virtual collection handle with all
     * access to collection while hiding the
     * original collection type.
     * This is useful for exposing collection
     * across translation units.
     * Handle doesn't take ownership of it's
     * collection (unless it is passed as rvalue
     * reference) nor is responsible of
     * freeing it. Handle assumes that collection
     * is always valid and not freed.
     * @param[in] t Collection to virtualize.
     */
    template <
        class T,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy
    >
    static auto vcollection(T&& t)
     -> VCollection< typename adapters::CollectionAdapter<
         decltype(std::forward<T>(t))
     >::ValueType >
    {
        typedef adapters::CollectionAdapter< T > Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::ValueType ValType;

        // for lvalue references
        typedef VCollectionImpl<
            decltype(std::forward<T>(t)),
            void,
            StoragePolicy> VImpl;
        VImpl *v = new VImpl( std::forward<T>(t) );
        return VCollection< ValType >(v);
    }

    /**
     * Same as vcollection but user can specify destructor
     * function when the handle (not the collection)
     * is destroyed.
     * @param[in] t Collection to virtualize
     * @param[in] d Destructor function. Should be callable
     * as d() (without parameters).
     * @param[in] StoragePolicy storage policy for collection
     * as well as function. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        class T,
        class Dtor,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy
    >
    static auto vcollectionWDtor(T&& t,Dtor&& d)
     -> VCollection< typename adapters::CollectionAdapter<
         decltype(std::forward<T>(t))
     >::ValueType >
    {
        typedef adapters::CollectionAdapter< T > Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::ValueType ValType;

        // for lvalue references
        typedef VCollectionImpl<
            decltype(std::forward<T>(t)),
            decltype(std::forward<Dtor>(d)),
            StoragePolicy> VImpl;
        VImpl *v = new VImpl(
            std::forward<T>(t),
            std::forward<Dtor>(d) );
        return VCollection< ValType >(v);
    }

    /**
     * Make custom virtual collection with
     * custom access to specific functions.
     * Every policy value defaults to THROW.
     * Handle doesn't take ownership of it's
     * collection (unless it is passed as rvalue
     * reference) nor is responsible of
     * freeing it. Handle assumes that collection
     * is always valid and not freed.
     * @param[in] t Collection to virtualize.
     * @param[in] ap Addition policy. Specifies
     * whether user is allowed to add or insert
     * elements in collection.
     * Possible enums:
     * ~~~~~~~
     * // Throw whenever function
     * // belonging to the addition section is called.
     * templatious::AP_THROW
     *
     * // Fake addition (users
     * // may try to add elements but
     * // there would be no effect).
     * templatious::AP_FAKE
     *
     * // Enable addition (what would normally happen)
     * templatious::AP_ENABLED
     * ~~~~~~~
     * Functions that belong to this part of the interface:
     * ~~~~~~~
     * CollectionAdapter::add
     * CollectionAdapter::insert
     * ~~~~~~~
     * @param[in] cp Clearable policy. Specifies how
     * collection should react to attempts to erase elements.
     * Possible enums:
     * ~~~~~~~
     * // Throw whenever function
     * // belonging to the clear section is called.
     * templatious::CP_THROW
     *
     * // Fake removal (users
     * // may try to erase elements but
     * // there would be no effect).
     * templatious::CP_FAKE
     *
     * // Enable removal (what would normally happen)
     * templatious::CP_ENABLED
     * ~~~~~~~
     * Functions that belong to this part of the interface:
     * ~~~~~~~
     * CollectionAdapter::erase
     * CollectionAdapter::clear
     * ~~~~~~~
     * @param[in] tp Traversible policy. Specifies
     * how collection should react to traverse attempts.
     * Possible enums:
     * ~~~~~~~
     * // Throw whenever function
     * // belonging to the traversal section is called.
     * templatious::TP_THROW
     *
     * // Fake traversal. Implemented by
     * // returning end iterator for both
     * // begin and end.
     * templatious::TP_FAKE
     *
     * // Enable traversal (what would normally happen)
     * templatious::TP_ENABLED
     * ~~~~~~~
     * Functions that belong to this part of the interface:
     * ~~~~~~~
     * CollectionAdapter::begin
     * CollectionAdapter::end
     * CollectionAdapter::iterAt
     *
     * CollectionAdapter::cbegin
     * CollectionAdapter::cend
     * CollectionAdapter::citerAt
     * ~~~~~~~
     * @param[in] acp Access policy. Specifies
     * how collection should react to random
     * element access.
     * Possible enums:
     * ~~~~~~~
     * // Throw whenever function
     * // belonging to the access section is called.
     * templatious::ACP_THROW
     *
     * // Enable random access (what would normally happen)
     * templatious::ACP_ENABLED
     * ~~~~~~~
     * Functions that belong to this part of the interface:
     * ~~~~~~~
     * CollectionAdapter::first
     * CollectionAdapter::last
     * CollectionAdapter::getByIndex
     *
     * CollectionAdapter::cfirst
     * CollectionAdapter::clast
     * CollectionAdapter::cgetByIndex
     * ~~~~~~~
     * @param[in] sp Sizable policy. Specifies
     * whether user is allowed to know collection
     * size.
     * Possible enums:
     * ~~~~~~~
     * // Throw whenever user asks for
     * // collection size.
     * templatious::SP_THROW
     *
     * // Fake size (always return 0 when asked)
     * templatious::SP_FAKE
     *
     * // Enable size (what would normally happen)
     * templatious::SP_ENABLED
     * ~~~~~~~
     * Functions that belong to this part of the interface:
     * ~~~~~~~
     * CollectionAdapter::size
     * ~~~~~~~
     * @param[in] StoragePolicy Storage policy to be used
     * for storing collection. Defaults to
     * templatious::util::DefaultStoragePolicy
     *
     * Example:
     * ~~~~~~~
     * std::vector<int> v;
     * SA::add(v,1,2,3,4,5,6,7);
     *
     * auto vcol = SF::vcollectionCustom<
     *     templatious::AP_THROW,
     *     templatious::CP_FAKE,
     *     templatious::TP_FAKE,
     *     templatious::ACP_ENABLED,
     *     templatious::SP_FAKE
     * >(v);
     *
     * SA::add(vcol,7); // addition throws
     * SA::clear(vcol); // fakes clear (no effect)
     *
     * int sum = 0;
     * TEMPLATIOUS_FOREACH(auto i,vcol) {
     *     ++sum;
     * }
     * // traversal faked
     * assert( sum == 0 );
     *
     * // access enabled
     * assert( SA::getByIndex(vcol,6) == 7 );
     *
     * // size faked
     * assert( SA::size(vcol) == 0 );
     * ~~~~~~~
     */
    template <
        templatious::AddablePolicy ap = templatious::AP_THROW,
        templatious::ClearablePolicy cp = templatious::CP_THROW,
        templatious::TraversablePolicy tp = templatious::TP_THROW,
        templatious::AccessPolicy acp = templatious::ACP_THROW,
        templatious::SizablePolicy sp = templatious::SP_THROW,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto vcollectionCustom(T&& t)
     -> VCollection< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<
            decltype(std::forward<T>(t))> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::ValueType ValType;

        typedef VCollectionFactory<
            decltype(std::forward<T>(t)),
            StoragePolicy,
            sp,
            acp,
            ap,
            cp,
            tp
        > Factory;

        typedef typename Factory::Type Wrap;

        typedef VCollectionImpl<
            Wrap&&,
            void,
            StoragePolicy
        > VImpl;
        VImpl *v = new VImpl( Factory::make(std::forward<T>(t)) );
        return VCollection< ValType >(v);
    }

    /**
     * Same as vcollectionCustom but user can specify
     * destructor function when the handle (not the collection)
     * is destroyed.
     * @param[in] t Collection to virtualize
     * @param[in] d Destructor function. Should be callable
     * as d() (without parameters).
     * @param[in] ap Same as vcollectionCustom.
     * @param[in] cp Same as vcollectionCustom.
     * @param[in] tp Same as vcollectionCustom.
     * @param[in] acp Same as vcollectionCustom.
     * @param[in] sp Same as vcollectionCustom.
     * @param[in] StoragePolicy storage policy for collection
     * as well as function. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        templatious::AddablePolicy ap = templatious::AP_THROW,
        templatious::ClearablePolicy cp = templatious::CP_THROW,
        templatious::TraversablePolicy tp = templatious::TP_THROW,
        templatious::AccessPolicy acp = templatious::ACP_THROW,
        templatious::SizablePolicy sp = templatious::SP_THROW,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T,
        class Dtor
    >
    static auto vcollectionCustomWDtor(T&& t,Dtor&& d)
     -> VCollection< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<
            decltype(std::forward<T>(t))> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::ValueType ValType;

        typedef VCollectionFactory<
            decltype(std::forward<T>(t)),
            StoragePolicy,
            sp,
            acp,
            ap,
            cp,
            tp
        > Factory;

        typedef typename Factory::Type Wrap;

        typedef VCollectionImpl<
            Wrap&&,
            decltype(std::forward<Dtor>(d)),
            StoragePolicy
        > VImpl;
        VImpl *v = new VImpl(
            Factory::make( std::forward<T>(t) ),
            std::forward<Dtor>(d)
        );
        return VCollection< ValType >(v);
    }

    /**
     * Get handle to collection with selected
     * permissions. Everything is allowed
     * unless specified otherwise. If action
     * is disallowed on the handle code will
     * not compile. Useful to assert that
     * collection is operated only on
     * the ways intended.
     * @param[in] bitmask Bitmask which specifies
     * compile time permissions for collection.
     * Possible values:
     * ~~~~~~~
     * // prevent addition to collection
     * templatious::VCOL_ADD
     *
     * // prevent erase of elements in collection
     * templatious::VCOL_CLEAR
     *
     * // prevent traversal of collection
     * templatious::VCOL_TRAVERSE
     *
     * // prevent access of collection
     * templatious::VCOL_ACCESS
     *
     * // prevent size info of collection
     * templatious::VCOL_SIZE
     * ~~~~~~~
     * @param[in] t Collection to get handle to.
     *
     * Example:
     * ~~~~~~~
     * std::vector<int> v;
     *
     * // Prevent addition and traversal of collection.
     * // Any other action succeeds.
     * auto h = SF::prevent<
     *     templatious::VCOL_ADD | templatious::VCOL_TRAVERSE
     * >(v);
     *
     * // WRONG, addition prevented, compile time error
     * //SA::add(h,1,2,3);
     *
     * // WRONG, traversal prevented, compile time error
     * //TEMPLATIOUS_FOREACH(auto i,h) {
     * //
     * //}
     *
     * // ok
     * SA::getByIndex(h,0);
     *
     * // ok
     * SA::clear(h);
     *
     * // ok
     * long sz = SA::size(h);
     * ~~~~~~~
     */
    template <
        int bitmask,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto prevent(T&& t)
     -> typename templatious::BitmaskVCollectionFactory<
            bitmask,false,decltype(std::forward<T>(t)),StoragePolicy
        >::Type
    {
        typedef typename templatious::BitmaskVCollectionFactory<
            bitmask,false,decltype(std::forward<T>(t)),StoragePolicy
        >::Maker Maker;

        return Maker::make(std::forward<T>(t));
    }

    /**
     * Get handle to collection with selected
     * permissions. Everything is disallowed
     * unless specified otherwise. If action
     * is disallowed on the handle code will
     * not compile. Useful to assert that
     * collection is operated only on
     * the ways intended.
     * @param[in] bitmask Bitmask which specifies
     * compile time permissions for collection.
     * Possible values:
     * ~~~~~~~
     * // allow addition to collection
     * templatious::VCOL_ADD
     *
     * // allow erase of elements in collection
     * templatious::VCOL_CLEAR
     *
     * // allow traversal of collection
     * templatious::VCOL_TRAVERSE
     *
     * // allow access of collection
     * templatious::VCOL_ACCESS
     *
     * // allow size info of collection
     * templatious::VCOL_SIZE
     * ~~~~~~~
     * @param[in] t Collection to get handle to.
     *
     * Example:
     * ~~~~~~~
     * std::vector<int> v;
     *
     * // Allow addition and traversal of collection.
     * // Any other action fails static assert.
     * auto h = SF::allow<
     *     templatious::VCOL_ADD | templatious::VCOL_TRAVERSE
     * >(v);
     *
     * // ok
     * SA::add(h,1,2,3);
     *
     * // ok
     * TEMPLATIOUS_FOREACH(auto i,h) {
     *
     * }
     *
     * // WRONG, access disabled, compile time error
     * //SA::getByIndex(h,0);
     *
     * // WRONG, clear disabled, compile time error
     * //SA::clear(h);
     *
     * // WRONG, size info disabled, compile time error
     * //long sz = SA::size(h);
     * ~~~~~~~
     */
    template <
        int bitmask,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class T
    >
    static auto allow(T&& t)
     -> typename templatious::BitmaskVCollectionFactory<
            bitmask,true,decltype(std::forward<T>(t)),StoragePolicy
        >::Type
    {
        typedef typename templatious::BitmaskVCollectionFactory<
            bitmask,true,decltype(std::forward<T>(t)),StoragePolicy
        >::Maker Maker;

        return Maker::make(std::forward<T>(t));
    }

    /**
     * Create a pack structure. Pack is like a
     * tuple but with more features. By default
     * pack stores references to objects if
     * they are lvalues and copies objects
     * using rvalue constructor if they are
     * rvalues.
     * @param[in] t Variables to save in pack.
     * Can contain other packs.
     * @param[in] StoragePolicy Pack storage
     * policy for saving elements. Defaults
     * to DefaultPackStoragePolicy.
     */
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

    /**
     * Create a new pack inserting passed
     * element between elements of the pack.
     * Does not insert into all packs contained
     * within pack recursively. Returns new pack
     * created.
     * For instance, if pack contains elements
     * ~~~~~~
     * [7,'7',55]
     * ~~~~~~
     * and pack insert is called with
     * ' ' (space symbol), then the created pack
     * will contain
     * ~~~~~~
     * [7,' ','7',' ',55]
     * ~~~~~~
     * *BUT NOT*
     * ~~~~~~
     * [7,' ','7',' ',55,' ']
     * ~~~~~~
     * *OR*
     * ~~~~~~
     * [' ',7,' ','7',' ',55,' ']
     * ~~~~~~
     * @param[in] p Pack to use for creating new one.
     * @param[in] t Element to insert. Can be another pack.
     */
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

    /**
     * Create a new pack inserting element t
     * into top level packs (if there are any).
     * Returns new pack.
     * For instance, if there is a pack of packs
     * ~~~~~~
     *     [
     *       ['a','b'],
     *       "some string",
     *       ['c','d']
     *     ]
     * ~~~~~~
     * And if ' ' (space) is inserted, then
     * the resulting pack is
     * ~~~~~~
     *     [
     *       ['a',' ','b'],
     *       "some string",
     *       ['c',' ','d']
     *     ]
     * ~~~~~~
     *
     * *BUT NOT*
     * ~~~~~~
     *     [
     *       ['a',' ','b'],
     *       ' ',
     *       "some string",
     *       ' ',
     *       ['c',' ','d']
     *     ]
     * ~~~~~~
     * @param[in] p Pack to use for creating new one.
     * @param[in] t Element to insert.
     */
    template <class P,class T>
    static auto packInsertWithin(P&& p,T&& t)
     -> decltype(detail::PackAccess::packInsertWithin(
                 std::forward<P>(p),std::forward<T>(t)))
    {
        return detail::PackAccess::packInsertWithin(
                std::forward<P>(p),std::forward<T>(t));
    }

    /**
     * Transform pack with a predicate function
     * (usually this is a match functor which
     * selects appropriate types and forwards
     * the rest). Returns the transformed
     * pack.
     * This is useful for breaking down POD
     * types into smaller chunks, for instance,
     * if there is a struct
     * ~~~~~~
     * struct Person {
     *   std::string _firstName;
     *   std::string _lastName;
     * };
     * ~~~~~~
     * And a pack
     * ~~~~~~
     * [
     *   "Person is: ",
     *   Person("Abraham","Isaac")
     * ]
     * ~~~~~~
     * Pack transform function can be used
     * to break this down to
     * ~~~~~~
     * [
     *   "Person is: ",
     *   [ // <- another pack created
     *     "Abraham",
     *     ' ',
     *     "Isaaac"
     *   ]
     * ]
     * ~~~~~~
     * Thus eliminating the POD type.
     * @param[in] p Pack to transform
     *
     */
    template <class F,class P,class... T>
    static auto packTransformWithin(F&& f,P&& p,T&&... t)
     -> decltype(
            detail::PackAccess::packTransformWithin(
                std::forward<F>(f),
                std::forward<P>(p),
                std::forward<T>(t)...)
        )
    {
        return detail::PackAccess::packTransformWithin(
            std::forward<F>(f),
            std::forward<P>(p),
            std::forward<T>(t)...
        );
    }

    /**
     * Make a pack of repeated element(s).
     * For instance
     * ~~~~~~~
     * auto p = StaticFactory::packRepeat<7>(55);
     * ~~~~~~~
     * Will result into pack:
     * ~~~~~~~
     * [55,55,55,55,55,55,55]
     * ~~~~~~~
     * And
     * ~~~~~~~
     * auto p = StaticFactory::packRepeat<3>(2,2);
     * ~~~~~~~
     * Will result into:
     * ~~~~~~~
     * [2,2,2,2,2,2]
     * ~~~~~~~
     * @param[in] args Arguments to make pack from
     * @param[in] n Times to repeat pack.
     */
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

    /**
     * Use packRepeat function with dummy variable.
     * The variable returned is a pack full of
     * dummy object that overloads = operator
     * and can be assigned to from any class.
     * It does nothing. This is useful with
     * distribute function to skip iterators
     * when assigning to this variable.
     * Since = operator for this class does
     * nothing it generates 0 instructions
     * and only advances iterator.
     * @param[in] n Amount of dummy variables to create.
     */
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

    /**
     * Pack passed variables to a pack
     * to be used as arguments to a
     * function. Does not have all the features of
     * std::bind, but rather maps exact amount
     * of variables contained in a pack to a
     * function. Returns a functor.
     * @param[in] f Function to execute.
     * @param[in] args arguments to pack.
     * @param[in] StoragePolicy storage policy
     * to store arguments. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
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

    /**
     * Tight match to be used in matchFunctor.
     * Tight match matches
     * ~~~~~~~
     *       int& and       int& // OK
     * const int& and       int& // Doesn't match
     * const int& and const int& // OK
     *       int& and       int  // Doesn't match
     * ~~~~~~~
     * Tight match has to have exact number
     * of arguments to match.
     * Tight match will always
     * match templatious::AnyType.
     * @param[in] f Predicate function to hold.
     * @param[in] T... Type arguments to match.
     */
    template <class... T,class Func>
    static auto matchTight(Func&& f)
     -> detail::Match<
        templatious::TypeList<T...>,
        Func,
        util::DefaultStoragePolicy,
        detail::TightRecursiveComparison,
        templatious::detail::TypelistsEqual
     >
    {
        typedef detail::Match<
            templatious::TypeList<T...>,
            Func,
            util::DefaultStoragePolicy,
            detail::TightRecursiveComparison,
            templatious::detail::TypelistsEqual
        > TheMatch;

        return TheMatch(std::forward<Func>(f));
    }

    /**
     * Same as tight match but with special
     * storage policy.
     * @param[in] f Function to use.
     * @param[in] T... Typelist to match.
     * @param[in] StoragePolicy Storage
     * policy used for saving function.
     */
    template <
        template <class> class StoragePolicy,
        class... T,
        class Func
    >
    static auto matchTight(Func&& f)
     -> detail::Match<
        templatious::TypeList<T...>,
        Func,
        StoragePolicy,
        detail::TightRecursiveComparison,
        templatious::detail::TypelistsEqual
     >
    {
        typedef detail::Match<
            templatious::TypeList<T...>,
            Func,
            StoragePolicy,
            detail::TightRecursiveComparison,
            templatious::detail::TypelistsEqual
        > TheMatch;

        return TheMatch(std::forward<Func>(f));
    }

    /**
     * Special match which uses user supplied
     * template function to test whether
     * this match succeeds.
     * @param[in] f Function to use.
     * @param[in] MatchAlgorithm User supplied
     * template function for matching.
     *
     * Example:
     * ~~~~~~~
     * // CurrentMatch - current argument type
     * // being matched.
     * // pos - current argument position,
     * // starting with 1
     * template <class CurrentMatch,int pos>
     * struct UserMatcher {
     *
     *     // does_match - result of this function.
     *     // Must be true for all arguments to use
     *     // this match.
     *     static const bool does_match = true;
     *     // num_args - the amount of arguments
     *     // function should take. 0 for any amount
     *     // of arguments.
     *     static const int num_args = 0;
     * };
     *
     * // If function f is called with arguments
     * // a,b,c like f(A a,B b,C c) UserMatcher will
     * // be called like:
     * // if (UserMatcher<A,1>::does_match &&
     * // UserMatcher<B,2>::does_match &&
     * // UserMatcher<C,3>::does_match) { MATCHED }
     * ~~~~~~~
     */
    template <
        template <class,int> class MatchAlgorithm,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class Func
    >
    static auto matchSpecial(Func&& f)
     -> detail::MatchSpecial<
         MatchAlgorithm,
         Func,
         StoragePolicy
     >
    {
        typedef detail::MatchSpecial<
            MatchAlgorithm,
            Func,
            StoragePolicy
        > TheMatch;
        return TheMatch(std::forward<Func>(f));
    }

    /**
     * Special match extended which uses user
     * supplied template function to test whether
     * this match succeeds. TypeList is passed to
     * user template.
     * @param[in] f Function to use.
     * @param[in] MatchAlgorithm User supplied
     * template function for matching.
     *
     * Example:
     * ~~~~~~~
     * // TypeList - typelist which contains
     * // all the types current call has.
     * template <class TypeList>
     * struct UserMatcher {
     *     // does_match - result. Did this
     *     // TypeList satisfy user requirements?
     *     static const bool does_match = ?;
     * };
     *
     * // If function f is called with arguments
     * // a,b,c like f(A a,B b,C c) UserMatcher will
     * // be called like:
     * // if (UserMatcher< TypeList<A,B,C> >::does_match )
     * //     { MATCHED }
     * ~~~~~~~
     */
    template <
        template <class> class MatchAlgorithm,
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class Func
    >
    static auto matchSpecialExt(Func&& f)
     -> detail::MatchSpecialExt<
         MatchAlgorithm,
         Func,
         StoragePolicy
     >
    {
        typedef detail::MatchSpecialExt<
            MatchAlgorithm,
            Func,
            StoragePolicy
        > TheMatch;
        return TheMatch(std::forward<Func>(f));
    }

    /**
     * Loose match to be used in matchFunctor.
     * This match applied std::decay on both
     * types when comparing.
     * Loose match matches
     * ~~~~~~~
     *       int& and       int& // OK
     * const int& and       int& // OK
     * const int& and const int& // OK
     *       int& and       int  // OK
     * ~~~~~~~
     * Loose match only matches at the start
     * of arguments in typelist to match,
     * therefore, it may match having less
     * types in typelist than actual arguments.
     * Loose match will always
     * match templatious::AnyType.
     * @param[in] f Predicate function to hold.
     * @param[in] T... Type arguments to match.
     */
    template <class... T,class Func>
    static auto matchLoose(Func&& f)
     -> detail::Match<
        templatious::TypeList<T...>,
        Func,
        util::DefaultStoragePolicy,
        detail::LooseRecursiveComparison,
        templatious::detail::TypelistContains
     >
    {
        typedef detail::Match<
            templatious::TypeList<T...>,
            Func,
            util::DefaultStoragePolicy,
            detail::LooseRecursiveComparison,
            templatious::detail::TypelistContains
        > TheMatch;

        return TheMatch(std::forward<Func>(f));
    }

    /**
     * Same as loose match but with special
     * storage policy.
     * @param[in] f Function to use.
     * @param[in] T... Typelist to match.
     * @param[in] StoragePolicy Storage
     * policy used for saving function.
     */
    template <
        template <class> class StoragePolicy,
        class... T,
        class Func
    >
    static auto matchLoose(Func&& f)
     -> detail::Match<
        templatious::TypeList<T...>,
        Func,
        StoragePolicy,
        detail::LooseRecursiveComparison,
        templatious::detail::TypelistContains
     >
    {
        typedef detail::Match<
            templatious::TypeList<T...>,
            Func,
            StoragePolicy,
            detail::LooseRecursiveComparison,
            templatious::detail::TypelistContains
        > TheMatch;

        return TheMatch(std::forward<Func>(f));
    }

    /**
     * Match anything. This is usually
     * the last match in match functor.
     * @param[in] f Function to use when
     * matched.
     * @param[in] StoragePolicy
     * Storage policy. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class Func
    >
    static auto matchAny(Func&& f)
     -> detail::Match<
        templatious::TypeList< AnyType >,
        Func,
        StoragePolicy,
        detail::LooseRecursiveComparison,
        templatious::detail::TypelistContains
     >
    {
        typedef detail::Match<
            templatious::TypeList< AnyType >,
            Func,
            StoragePolicy,
            detail::LooseRecursiveComparison,
            templatious::detail::TypelistContains
        > TheMatch;
        return TheMatch(std::forward<Func>(f));
    }

    /**
     * Match anything and do nothing.
     * This is usually the last match
     * in match functor.
     */
    static auto matchAnyDoNothing()
     -> detail::Match<
        templatious::TypeList< AnyType >,
        templatious::util::DoNothingFunctor,
        templatious::util::DefaultStoragePolicy,
        detail::LooseRecursiveComparison,
        templatious::detail::TypelistContains
     >
    {
        typedef detail::Match<
            templatious::TypeList< AnyType >,
            templatious::util::DoNothingFunctor,
            templatious::util::DefaultStoragePolicy,
            detail::LooseRecursiveComparison,
            templatious::detail::TypelistContains
        > TheMatch;
        return TheMatch(templatious::util::DoNothingFunctor());
    }

    /**
     * Match anything and forward the argument.
     * Usually the last match in the match functor.
     * @param[in] T Type to forward as. If
     * void result forwarded will be returned as
     * ~~~~~~~
     * return std::forward<T>(arg);
     * ~~~~~~~
     */
    template <class T = void>
    static auto matchAnyForward()
     -> detail::Match<
        templatious::TypeList< AnyType >,
        templatious::util::ForwardFunctor<T>,
        templatious::util::DefaultStoragePolicy,
        detail::LooseRecursiveComparison,
        templatious::detail::TypelistContains
     >
    {
        typedef detail::Match<
            templatious::TypeList< AnyType >,
            templatious::util::ForwardFunctor<T>,
            templatious::util::DefaultStoragePolicy,
            detail::LooseRecursiveComparison,
            templatious::detail::TypelistContains
        > TheMatch;
        return TheMatch(templatious::util::ForwardFunctor<T>());
    }

    /**
     * Function to create match functor.
     * Intended to be composed of match
     * methods in StaticFactory which
     * start as "match".
     * @param[in] t Match arguments to use.
     * @param[in] StoragePolicy
     * Storage policy to store matches.
     * Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::DefaultStoragePolicy,
        class... T
    >
    static auto matchFunctor(T&&... t)
     -> detail::MatchFunctor<StoragePolicy,T...>
    {
        typedef detail::MatchFunctor<StoragePolicy,T...> Fctor;
        return Fctor(std::forward<T>(t)...);
    }

    /**
     * Create stack allocated virtual match
     * functor.
     * This match functor matches only
     * one type of argument -
     * templatious::VirtualPack. Can be
     * used for convenient unpacking of
     * virtual packs directly into function
     * calls.
     *
     * Virtual match functor can be composed
     * out of virtual matches ( created with
     * templatious::StaticFactory::virtualMatch or
     * templatious::StaticFactory::virtualTransparentMatch )
     * or out of other virtual match functors.
     *
     * Function to call for a specific virtual
     * pack is resolved at runtime.
     *
     * @param[in] t Arguments to compose virtual
     * match functor from.
     *
     * Example:
     * ~~~~~~~
     * auto vmfA = SF::virtualMatchFunctor(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "MATCHED INT" << std::endl;
     *         }
     *     )
     * );
     *
     * auto vmfB = SF::virtualMatchFunctor(
     *     vmfA, // compose first into second
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "MATCHED CHAR" << std::endl;
     *         }
     *     )
     * );
     *
     * auto pInt = SF::vpack<int>(7);
     * auto pChar = SF::vpack<char>('7');
     *
     * bool matchedInt  = vmfB.tryMatch(pInt);
     * bool matchedChar = vmfB.tryMatch(pChar);
     * // prints out:
     * // MATCHED INT
     * // MATCHED CHAR
     *
     * assert( matchedInt );
     * assert( matchedChar );
     * ~~~~~~~
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::CopyOnlyStoragePolicy,
        class... T
    >
    static auto virtualMatchFunctor(T&&... t)
     -> VirtualMatchFunctorImpl< StoragePolicy, T... >
    {
        typedef VirtualMatchFunctorImpl< StoragePolicy, T... > Fctor;
        return Fctor(std::forward<T>(t)...);
    }


    /**
     * Create heap allocated virtual match functor
     * wrapped inside std::unique_ptr.
     * This match functor matches only
     * one type of argument -
     * templatious::VirtualPack. Can be
     * used for convenient unpacking of
     * virtual packs directly into function
     * calls.
     *
     * Virtual match functor can be composed
     * out of virtual matches ( created with
     * templatious::StaticFactory::virtualMatch or
     * templatious::StaticFactory::virtualTransparentMatch )
     * or out of other virtual match functors.
     *
     * Function to call for a specific virtual
     * pack is resolved at runtime.
     *
     * @param[in] t Arguments to compose virtual
     * match functor from.
     *
     * Example:
     * ~~~~~~~
     * auto vmfA = SF::virtualMatchFunctor(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "MATCHED INT" << std::endl;
     *         }
     *     )
     * );
     *
     * std::unique_ptr< templatious::VirtualMatchFunctor > vmfB =
     *     SF::virtualMatchFunctorPtr(
     *         vmfA, // compose first into second
     *         SF::virtualMatch<char>(
     *             [](char i) {
     *                 std::cout << "MATCHED CHAR" << std::endl;
     *             }
     *         )
     *     );
     *
     * auto pInt = SF::vpack<int>(7);
     * auto pChar = SF::vpack<char>('7');
     *
     * bool matchedInt  = vmfB->tryMatch(pInt);
     * bool matchedChar = vmfB->tryMatch(pChar);
     * // prints out:
     * // MATCHED INT
     * // MATCHED CHAR
     *
     * assert( matchedInt );
     * assert( matchedChar );
     * ~~~~~~~
     */
    template <
        template <class> class StoragePolicy =
            templatious::util::CopyOnlyStoragePolicy,
        class... T
    >
    static auto virtualMatchFunctorPtr(T&&... t)
     -> std::unique_ptr< VirtualMatchFunctor >
    {
        typedef VirtualMatchFunctorVImpl<
            StoragePolicy,
            T...
        > TheImpl;

        return std::unique_ptr< VirtualMatchFunctor >(
            new TheImpl( std::forward<T>(t)... )
        );
    }

    /**
     * Create virtual match with
     * default storage policy.
     * This match returns true if it matched,
     * indicating that this match handled the object
     * and stopping further object matching down
     * the line.
     * @param[in] f Function to call when matched.
     * @param[in] Args Virtual pack signature.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<int>(7);
     *
     * auto vmf = SF::virtualMatchFunctor(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "A MATCHED" << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "NEVER REACHED" << std::endl;
     *         }
     *     )
     * );
     *
     * bool matched = vmf.tryMatch(p);
     * // prints out:
     * // A MATCHED
     *
     * assert( matched );
     * ~~~~~~~
     */
    template <
        class... Args,
        class F
    >
    static auto virtualMatch(F&& f)
     -> VirtualMatch<
         decltype(std::forward<F>(f)),
         false,
         templatious::util::CopyOnlyStoragePolicy,
         Args...
     >
    {
        typedef VirtualMatch<
            decltype(std::forward<F>(f)),
            false,
            templatious::util::CopyOnlyStoragePolicy,
            Args...
        > TheMatch;
        return TheMatch(std::forward<F>(f));
    }

    /**
     * Create virtual match with
     * custom storage policy.
     * This match returns true if it matched,
     * indicating that this match handled the object
     * and stopping further object matching down
     * the line.
     * @param[in] f Function to call when matched.
     * @param[in] Args Virtual pack signature.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<int>(7);
     *
     * auto vmf = SF::virtualMatchFunctor(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "A MATCHED" << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "NEVER REACHED" << std::endl;
     *         }
     *     )
     * );
     *
     * bool matched = vmf.tryMatch(p);
     * // prints out:
     * // A MATCHED
     *
     * assert( matched );
     * ~~~~~~~
     */
    template <
        template <class> class StoragePolicy,
        class... Args,
        class F
    >
    static auto virtualMatch(F&& f)
     -> VirtualMatch<
         decltype(std::forward<F>(f)),
         false,
         StoragePolicy,
         Args...
     >
    {
        typedef VirtualMatch<
            decltype(std::forward<F>(f)),
            false,
            StoragePolicy,
            Args...
        > TheMatch;
        return TheMatch(std::forward<F>(f));
    }

    /**
     * Create virtual transparent match with default
     * strorage policy.
     * This match returns false even if it matched,
     * passing virtual pack to subsequent matches.
     * @param[in] f Function to call when matched.
     * @param[in] Args Virtual pack signature.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<int>(7);
     *
     * auto vmf = SF::virtualMatchFunctor(
     *     SF::virtualTransparentMatch<int>(
     *         [](int i) {
     *             std::cout << "A MATCHED" << std::endl;
     *         }
     *     ),
     *     SF::virtualTransparentMatch<int>(
     *         [](int i) {
     *             std::cout << "B MATCHED" << std::endl;
     *         }
     *     )
     * );
     *
     * bool matched = vmf.tryMatch(p);
     * // prints out:
     * // A MATCHED
     * // B MATCHED
     *
     * assert( !matched );
     * ~~~~~~~
     */
    template <
        class... Args,
        class F
    >
    static auto virtualTransparentMatch(F&& f)
     -> VirtualMatch<
         decltype(std::forward<F>(f)),
         true,
         templatious::util::CopyOnlyStoragePolicy,
         Args...
     >
    {
        typedef VirtualMatch<
            decltype(std::forward<F>(f)),
            true,
            templatious::util::CopyOnlyStoragePolicy,
            Args...
        > TheMatch;
        return TheMatch(std::forward<F>(f));
    }

    /**
     * Create virtual transparent match with custom
     * strorage policy.
     * This match returns false even if it matched,
     * passing virtual pack to subsequent matches.
     * @param[in] f Function to call when matched.
     * @param[in] StoragePolicy Storage policy to use
     * for storing function.
     * @param[in] Args Virtual pack signature.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<int>(7);
     *
     * auto vmf = SF::virtualMatchFunctor(
     *     SF::virtualTransparentMatch<int>(
     *         [](int i) {
     *             std::cout << "A MATCHED" << std::endl;
     *         }
     *     ),
     *     SF::virtualTransparentMatch<int>(
     *         [](int i) {
     *             std::cout << "B MATCHED" << std::endl;
     *         }
     *     )
     * );
     *
     * bool matched = vmf.tryMatch(p);
     * // prints out:
     * // A MATCHED
     * // B MATCHED
     *
     * assert( !matched );
     * ~~~~~~~
     */
    template <
        template <class> class StoragePolicy,
        class... Args,
        class F
    >
    static auto virtualTransparentMatch(F&& f)
     -> VirtualMatch<
         decltype(std::forward<F>(f)),
         true,
         StoragePolicy,
         Args...
     >
    {
        typedef VirtualMatch<
            decltype(std::forward<F>(f)),
            true,
            StoragePolicy,
            Args...
        > TheMatch;
        return TheMatch(std::forward<F>(f));
    }

    /**
     * Create virtual pack which resides on the stack.
     * @param[in] Signature Signature of the pack. Should
     * only contain raw types without references. Const
     * qualifiers allowed.
     * @param[in] vars Variables to use to initialize
     * pack.
     * @note Signature and var count must be the same.
     * @note Pointer to this function is a polymorphic
     * pointer of base class templatious::VirtualPack.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<long,long>(1,2);
     *
     * bool aSucc = p.tryCallFunction<long,long>(
     *     [](long a,long b) {
     *         std::cout << a << " " << b << std::endl;
     *     }
     * );
     *
     * bool bSucc = p.tryCallFunction<int,long>(
     *     [](long a,long b) {
     *         std::cout << a << " " << b << std::endl;
     *     }
     * );
     *
     * assert( aSucc );
     * assert( !bSucc );
     * ~~~~~~~
     */
    template <
        class... Signature,
        class... Init
    >
    static auto vpack(Init&&... vars)
     -> templatious::VirtualPackImpl<
         DefVpackSettings, Signature...
     >
    {
        const bool sameSize =
            sizeof...(Signature) == sizeof...(Init);
        static_assert(sameSize,
            "Type count in signature has to be the"
            " same as the count of arguments passed.");
        return templatious::VirtualPackImpl<
            DefVpackSettings,Signature...
        >(
            std::forward<Init>(vars)...
        );
    }

    /**
     * Create heap allocated virtual pack wrapped inside
     * std::shared_ptr which could be shared among threads.
     * @param[in] Signature Signature of the pack. Should
     * only contain raw types without references. Const
     * qualifiers allowed.
     * @param[in] vars Variables to use to initialize
     * pack.
     * @note Signature and var count must be the same.
     * @note This function returns a polymorphic
     * pointer of base class templatious::VirtualPack.
     *
     * Example:
     * ~~~~~~~
     * std::shared_Ptr< templatious::VirtualPack > p =
     *     SF::vpackPtr<long,long>(1,2);
     *
     * bool aSucc = p->tryCallFunction<long,long>(
     *     [](long a,long b) {
     *         std::cout << a << " " << b << std::endl;
     *     }
     * );
     *
     * bool bSucc = p->tryCallFunction<int,long>(
     *     [](long a,long b) {
     *         std::cout << a << " " << b << std::endl;
     *     }
     * );
     *
     * assert( aSucc );
     * assert( !bSucc );
     * ~~~~~~~
     */
    template <
        class... Signature,
        class... Init
    >
    static auto vpackPtr(Init&&... vars)
     -> std::shared_ptr< VirtualPackImpl<
         DefVpackSettings,Signature...
         > >
    {
        typedef VirtualPackImpl<DefVpackSettings,Signature...>
            TheVal;
        const bool sameSize =
            sizeof...(Signature) == sizeof...(Init);
        static_assert(sameSize,
            "Type count in signature has to be the"
            " same as the count of arguments passed.");
        return std::make_shared< TheVal >(
            std::forward<Init>(vars)...
        );
    }

    /**
     * Create heap allocated virtual pack with callback
     * wrapped inside std::shared_ptr which could be
     * shared among threads. Callback is called each
     * time this pack is used (after the usage).
     * @param[in] Signature Signature of the pack. Should
     * only contain raw types without references. Const
     * qualifiers allowed.
     * @param[in] f Function to be used as callback. Should
     * take in const lvalue reference to templatious::VirtualPackCore
     * with it's signature. Always stored as a copy.
     * @param[in] vars Variables to use to initialize
     * pack.
     * @note Signature and var count must be the same.
     * @note This function returns a polymorphic
     * pointer of base class templatious::VirtualPack.
     *
     * Example:
     * ~~~~~~~
     * volatile long sum = 0;
     *
     * auto p =
     *     SF::vpackPtrWCallback<long,long>(
     *     [&](const TEMPLATIOUS_VPCORE<long,long>& p) {
     *         sum += p.fGet<0>();
     *         sum += p.fGet<1>();
     *     },
     *     1,2);
     *
     * bool aSucc = p->tryCallFunction<long,long>(
     *     [](long& a,long& b) {
     *         a *= 2;
     *         b *= 2;
     *     }
     * );
     *
     * assert( aSucc );
     * assert( sum == 6 );
     * ~~~~~~~
     */
    template <
        class... Signature,
        class Function,
        class... Init
    >
    static auto vpackPtrWCallback(Function&& f,Init&&... vars)
     -> std::shared_ptr< VirtualPackImpl<
         DefVpackSettings | VPACK_WCALLBACK,
         decltype(std::forward<Function>(f)), Signature...
         > >
    {
        typedef VirtualPackImpl<
            DefVpackSettings | VPACK_WCALLBACK,
            decltype(std::forward<Function>(f)), Signature...
        > TheVal;
        const bool sameSize =
            sizeof...(Signature) == sizeof...(Init);
        static_assert(sameSize,
            "Type count in signature has to be the"
            " same as the count of arguments passed.");
        return std::make_shared< TheVal >(
            std::forward<Function>(f),std::forward<Init>(vars)...
        );
    }

    /**
     * Create heap allocated virtual pack with custom
     * flags indicating the behaviour of the pack.
     * @param[in] vpackFlags Bitmask of the configuration
     * of the pack.
     * Possible values
     * ~~~~~~~
     * // does pack count ivocations?
     * templatious::VPACK_COUNT
     *
     * // is pack eligible for waiting
     * // for the fisr invocation?
     * templatious::VPACK_WAIT
     *
     * // is pack thread safe? (if yes, only
     * // one thread can invoke pack at a time)
     * templatious::VPACK_SYNCED
     *
     * // does pack have callback?
     * // you never need to set this value,
     * // as this is determined by the function
     * // you call. If you call vpackPtrWCallback
     * // or vpackPtrCustomWCallback this is always true,
     * // if this flag is used on vpackPtrCustom
     * // static assertion is triggered.
     * templatious::VPACK_WCALLBACK
     * ~~~~~~~
     *
     * @param[in] Signature Signature of the pack. Should
     * only contain raw types without references. Const
     * qualifiers allowed.
     * @param[in] vars Variables to use to initialize
     * pack.
     * @note Signature and var count must be the same.
     * @note This function returns a polymorphic
     * pointer of base class templatious::VirtualPack.
     *
     * Example (synchronization):
     * ~~~~~~~
     * auto pack = SF::vpackPtrCustom<
     *     templatious::VPACK_SYNCED,
     *     int,int
     * >(1,2);
     *
     * const int ROUNDS = 100000;
     *
     * auto handleA = std::async(std::launch::async,
     *     [=]() {
     *         TEMPLATIOUS_REPEAT( ROUNDS ) {
     *             pack->tryCallFunction<int,int>(
     *                 [](int& a,int& b) {
     *                     ++a;
     *                     ++b;
     *                 }
     *             );
     *         }
     *     });
     *
     * auto handleB = std::async(std::launch::async,
     *     [=]() {
     *         TEMPLATIOUS_REPEAT( ROUNDS ) {
     *             pack->tryCallFunction<int,int>(
     *                 [](int& a,int& b) {
     *                     ++a;
     *                     ++b;
     *                 }
     *             );
     *         }
     *     });
     *
     * handleA.wait();
     * handleB.wait();
     *
     * int outA = pack->fGet<0>();
     * int outB = pack->fGet<1>();
     *
     * int expA = 1 + ROUNDS * 2;
     * int expB = 2 + ROUNDS * 2;
     *
     * assert( outA == expA );
     * assert( outB == expB );
     * ~~~~~~~
     *
     * Example (waiting):
     * ~~~~~~~
     * auto pack = SF::vpackPtrCustom<
     *     tt::t::VPACK_WAIT,
     *     int,int
     * >(1,2);
     *
     * auto handle = std::async(std::launch::async,
     *     [=]() {
     *         std::this_thread::sleep_for(std::chrono::milliseconds(100));
     *         pack->tryCallFunction<int,int>(
     *             [](int& a,int& b) {
     *                 a *= 2;
     *                 b *= 2;
     *             }
     *         );
     *     });
     *
     * pack->wait();
     * int outA = pack->fGet<0>();
     * int outB = pack->fGet<1>();
     *
     * assert( outA == 2 );
     * assert( outB == 4 );
     * ~~~~~~~
     */
    template <
        int vpackFlags,
        class... Signature,
        class... Init
    >
    static auto vpackPtrCustom(Init&&... vars)
     -> std::shared_ptr< VirtualPackImpl<
         vpackFlags, Signature...
         > >
    {
        typedef VirtualPackImpl<
            vpackFlags,
            Signature...
        > TheVal;
        const bool hasCallback = (vpackFlags & VPACK_WCALLBACK) != 0;
        static_assert( !hasCallback,
            "Specified bitmask contains VPACK_WCALLBACK flag,"
            " however, this is unappropriate method to create"
            " a pack with a callback. Use vpackPtrWCallback or"
            " vpackPtrCustomWCallback instead."
        );

        const bool sameSize =
            sizeof...(Signature) == sizeof...(Init);
        static_assert(sameSize,
            "Type count in signature has to be the"
            " same as the count of arguments passed.");
        return std::make_shared< TheVal >(
            std::forward<Init>(vars)...
        );
    }

    /**
     * Create heap allocated virtual pack with callback and
     * with custom flags indicating the behaviour of the pack.
     * @param[in] vpackFlags Bitmask of the configuration
     * of the pack.
     * Possible values
     * ~~~~~~~
     * // does pack count ivocations?
     * templatious::VPACK_COUNT
     *
     * // is pack eligible for waiting
     * // for the fisr invocation?
     * templatious::VPACK_WAIT
     *
     * // is pack thread safe? (if yes, only
     * // one thread can invoke pack at a time)
     * templatious::VPACK_SYNCED
     *
     * // does pack have callback?
     * // you never need to set this value,
     * // as this is determined by the function
     * // you call. If you call vpackPtrWCallback
     * // or vpackPtrCustomWCallback this is always true,
     * // if this flag is used on vpackPtrCustom
     * // static assertion is triggered.
     * templatious::VPACK_WCALLBACK
     * ~~~~~~~
     *
     * @param[in] Signature Signature of the pack. Should
     * only contain raw types without references. Const
     * qualifiers allowed.
     * @param[in] f Function to be used as callback. Should
     * take in const lvalue reference to templatious::VirtualPackCore
     * with it's signature. Always stored as a copy.
     * @param[in] vars Variables to use to initialize
     * pack.
     * @note Signature and var count must be the same.
     * @note This function returns a polymorphic
     * pointer of base class templatious::VirtualPack.
     *
     * Example (synchronization with callback):
     * ~~~~~~~
     * // increment this from threads
     * auto iPtr = std::make_shared< int >( 0 );
     *
     * auto pack = SF::vpackPtrCustomWCallback<
     *     templatious::VPACK_SYNCED,
     *     int,int
     * >([=](const TEMPLATIOUS_VPCORE<int,int>& vp) {
     *     *iPtr += vp.fGet<0>();
     *     *iPtr += vp.fGet<1>();
     * },1,2);
     *
     * const int ROUNDS = 100000;
     *
     * // callbacks must also be synchronized
     * auto handleA = std::async(std::launch::async,
     *     [=]() {
     *         TEMPLATIOUS_REPEAT( ROUNDS ) {
     *             pack->tryCallFunction<int,int>(
     *                 [](int& a,int& b) {}
     *             );
     *         }
     *     });
     *
     * auto handleB = std::async(std::launch::async,
     *     [=]() {
     *         TEMPLATIOUS_REPEAT( ROUNDS ) {
     *             pack->tryCallFunction<int,int>(
     *                 [](int& a,int& b) {}
     *             );
     *         }
     *     });
     *
     * handleA.wait();
     * handleB.wait();
     *
     * int outExpected = (ROUNDS * 2) * (1 + 2);
     * assert( *iPtr == outExpected );
     * ~~~~~~~
     */
    template <
        int vpackFlags,
        class... Signature,
        class Function,
        class... Init
    >
    static auto vpackPtrCustomWCallback(Function&& f,Init&&... vars)
     -> std::shared_ptr< VirtualPackImpl<
         vpackFlags | templatious::VPACK_WCALLBACK,
         decltype(std::forward<Function>(f)), Signature...
        > >
    {
        typedef VirtualPackImpl<
            vpackFlags | templatious::VPACK_WCALLBACK,
            decltype(std::forward<Function>(f)),
            Signature...
        > TheVal;
        const bool sameSize =
            sizeof...(Signature) == sizeof...(Init);
        static_assert(sameSize,
            "Type count in signature has to be the"
            " same as the count of arguments passed.");
        return std::make_shared< TheVal >(
            std::forward<Function>(f), std::forward<Init>(vars)...
        );
    }

    /**
     * Create a stream functor out of object.
     * For instance
     * ~~~~~~~
     * auto sf = StaticFactory::streamOutFunctor(std::cout);
     * sf("ashes","to","ashes","dust","to","dust");
     * // upper expression is the same as:
     * std::cout << "ashes" << "to" << "ashes"
     *           << "dust"  << "to" << "dust";
     * ~~~~~~~
     * @param[in] t Object to create stream functor from.
     */
    template <class T>
    static auto streamOutFunctor(T& t)
     -> detail::CallEachStreamFunctor<T&>
    {
        return detail::CallEachStreamFunctor<T&>(t);
    }

    /**
     * Storage functor to create from special clsses
     * generated with TEMPLATIOUS_CALLEACH_FCTOR_WSTOR
     * macro.
     * @param[in] s Object to use.
     * @param[in] T Template to use.
     */
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

    /**
     * Chain functor encapsulates decorator
     * design pattern. It saves functions
     * to be called in a row (and in reverse
     * if it is created using pairs) order.
     *
     * Functional example:
     * ~~~~~~~
     * int add(int n) { return n + 7; }
     * int mul(int n) { return n * 7; }
     *
     * ..
     *
     * auto cf = StaticFactory::chainFunctor(
     *     add,mul
     * );
     *
     * // this
     * int res1 = cf(7);
     * // is same as
     * int res2 = mul(add(7));
     * ~~~~~~~
     * Stateful example:
     * ~~~~~~~
     * void add(int& n) { n += 7; }
     * void mul(int& n) { n *= 7; }
     *
     * ..
     *
     * auto cf = StaticFactory::chainFunctor<true>(
     *     add,mul
     * );
     *
     * // this
     * int a = 7;
     * int res1 = cf(a);
     * // is same as
     * int b = 7;
     * add(b);
     * mul(b);
     * ~~~~~~~
     * @param[in] args Functions to encapsulate.
     * @param[in] statefulDefault
     * whether to use stateful decorating
     * (passing same exact arguments each iteration)
     * or functional decorating
     * (passing current function return value
     * to the next function)
     * @param[in] StoragePolicy
     * Storage policy to use. Defaults to
     * templatious::util::DefaultStoragePolicy.
     */
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

    /**
     * Create a functor pair consisting of
     * do and undo action. Intented to be
     * used with chainFunctor.
     * @param[in] t Do action
     * @param[in] u Undo action
     * @param[in] StoragePolicy Storage
     * policy to be used when saving actions.
     *
     * Example:
     * ~~~~~~~
     * int addNum(int n) { return n + 7; }
     * int subNum(int n) { return n - 7; }
     *
     * int mulNum(int n) { return n * 7; }
     * int divNum(int n) { return n / 7; }
     *
     * ..
     * auto cf = StaticFactory::chainFunctor(
     *     StaticFactory::functorPair(addNum,subNum),
     *     StaticFactory::functorPair(mulNum,divNum)
     * );
     *
     * int res = cf(7);          // res contains 98
     * int back = cf.doBwd(res); // back contains 7
     *
     * // doBwd method is only applicable if
     * // *EVERY* argument passed to chainFunctor
     * // is functorPair with do and undo actions.
     * ~~~~~~~
     */
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

    /**
     * Create once traversable collection
     * handle. Whenever begin iterator
     * of this collection advances, begin
     * iterator in handle is also advanced,
     * therefore, "consuming" collection.
     * Useful when distributing same collection
     * over local arguments.
     *
     * Example:
     * ~~~~~~~
     * std::vector<int> v;
     * StaticAdapter::add(v,StaticFactory::seqL(100));
     * // v now contains 0,1,2,..,99
     * auto ot = StaticFactory::onceTraversable(v);
     *
     * int a,b,c;
     * while (3 == StaticManipulator
     *     ::distribute(ot,a,b,c))
     * {
     *     ..
     *     // will be executed 33 times
     * }
     * ~~~~~~~
     * @param[in] t Collection to create handle from.
     */
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
