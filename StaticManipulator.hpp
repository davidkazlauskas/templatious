//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  StaticManipulator.hpp
 *
 *    Description:  Static monipulator with generic operations
 *
 *        Version:  1.0
 *        Created:  08/18/2014 07:38:12 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <utility>
#include <type_traits>
#include <vector>
#include <algorithm>

#include <templatious/CollectionAdapter.hpp>
#include <templatious/CollectionMaker.hpp>

#include <templatious/util/SizeVerifier.hpp>
#include <templatious/util/CallCountFunctor.hpp>
#include <templatious/util/Exceptions.hpp>
#include <templatious/util/IteratorTagExtractor.hpp>

#include <templatious/IterMaker.hpp>
#include <templatious/Pack.hpp>
#include <templatious/detail/Distributor.hpp>
#include <templatious/detail/UserUtil.hpp>
#include <templatious/detail/VectoratorItem.hpp>

namespace templatious {
namespace detail {

    enum Variant { Item, Pack, Collection };

    template <
        int variant,
        template <class> class Decider
    >
    struct CallHandler;

    /** \mainpage Templatious documentation
     *
     * Those three classes are likely
     * what you care about:
     *
     * - templatious::StaticAdapter
     * - templatious::StaticFactory
     * - templatious::StaticManipulator
     *
     * And this could be also interesting:
     * - templatious::StaticBuffer
     * - templatious::StaticVector
     *
     * Kinda nice too:
     * - templatious::VirtualPack
     * - templatious::VirtualMatchFunctor
     * - templatious::DynamicVMatchFunctor
     *
     *
     * From time to time:
     * - templatious::ChainFunctor
     */


    // Difference between callEach and forEach:
    // callEach treats packs as composite
    // forEach treats collections as composite
    //
    // Why not all in one?
    // Because of static arrays
    // Static arrays are collections for templatious
    // But literal strings like "hello world"
    // are desired to be considered like one item
    // in callEach, not a collection of chars.
    //
    // Maybe there will be a better solution
    // in the future to treat all three uniformily?
    //
    // We'll see.
    template <class T>
    struct DeciderCallEach {
        static const int TheVar = templatious::util::IntSelector<
            IsPack<T>::val,
            Variant::Pack,
            Variant::Item
        >::val;
    };

    template <class T>
    struct DeciderForEach {
        static const int TheVar = templatious::util::IntSelector<
            templatious::adapters::CollectionAdapter<T>::is_valid,
            Variant::Collection,
            Variant::Item
        >::val;
    };

    template <class T>
    struct DeciderAllUniform {
        static const int TheVar = templatious::util::IntSelector<
            templatious::adapters::CollectionAdapter<T>::is_valid,
            Variant::Collection,
            templatious::util::IntSelector<
                IsPack<T>::val,
                Variant::Pack,
                Variant::Item
            >::val
        >::val;
    };

    struct SetImplCollection;
    struct SetImplVariable;
    struct SetImplPack;

    template <class T,class F>
    struct FoldFunctor {
        template <class Func>
        FoldFunctor(T& t,Func&& f)
            : _c(t), _f(std::forward<Func>(f)) {}

        template <class V>
        void operator()(V&& arg) {
            _c = _f.getRef()(_c,std::forward<V>(arg));
        }

        typedef typename templatious::util::
            DefaultStoragePolicy<F>::Container Container;
    private:
        T& _c;
        Container _f;
    };

    // Sort algorithms
    // random access iterator
    template <bool randTag>
    struct SortAlg {
        template <class T,class Comparator>
        static void sort(T& t,Comparator&& c) {
            typedef templatious::adapters::CollectionAdapter<T> Ad;
            std::sort(Ad::begin(t),Ad::end(t),
                std::forward<Comparator>(c));
        }

        template <class T>
        static void sort(T& t) {
            sort(t,templatious::detail
                ::DefaultLessComparator());
        }
    };
}

TEMPLATIOUS_BOILERPLATE_EXCEPTION( MapFunctionNotEqualException,
    "Collections passed to map are not all equal in size.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION( TraverseFunctionNotEqualException,
    "Collections passed to traverse are not all equal in size.");

/**
 * A class which holds static methods that
 * mainly have to do with element/collection
 * manipulation.
 */
struct StaticManipulator {
private:
    template <
        int var,
        template <class> class Decider
    >
    friend struct detail::CallHandler;

    template <class F, class ITER,bool callWithIndex = false,typename Index = long>
    struct IteratorCaller;

    template <class F,class ITER,class Index>
    struct IteratorCaller<F, ITER, false, Index> {

        template <class FInt,class IterInt>
        auto operator()(FInt&& f, Index idx, IterInt&& i)
            -> decltype(i.callFunction(std::forward<FInt>(f)))
        {
            return i.callFunction(std::forward<FInt>(f));
        }
    };

    template <class F,class ITER,class Index>
    struct IteratorCaller<F, ITER, true, Index> {

        template <class FInt,class IterInt>
        auto operator()(FInt&& f, Index idx, IterInt&& i)
            -> decltype(i.callFunction(std::forward<FInt>(f),
                        idx))
        {
            return i.callFunction(std::forward<FInt>(f),idx);
        }
    };

    template <
        bool ignoreBooleanReturn = false,
        class Func,class T,class... Args
    >
    static long distributeInternal(Func&& f,T&& t,Args&&... args) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        typedef templatious::detail::IsPack<T> IP;

        static const bool isCollection = Ad::is_valid;
        static const bool isPack = IP::val;

        namespace TD = templatious::detail;

        typedef typename std::conditional<
                isPack,
                TD::PackToRestDistribution,
                typename std::conditional<
                    isCollection,
                    TD::CollectionToRestDistribution,
                    TD::DummyErrorDistributor
                >::type
            >::type Distrubutor;

        return Distrubutor::template distribute<ignoreBooleanReturn>(
                std::forward<Func>(f),
                std::forward<T>(t),
                std::forward<Args>(args)...);
    }

    template <
        template <class> class DeciderAlg,
        bool ignoreBooleanReturn = false,
        class T,class U,class... V>
    static bool genericCallAll(T&& f,U&& arg,V&&... args) {
        bool res = detail::CallHandler< DeciderAlg<U>::TheVar, DeciderAlg >
            ::template call<ignoreBooleanReturn>(
                    std::forward<T>(f),std::forward<U>(arg));
        if (!ignoreBooleanReturn && !res) {
            return false;
        }

        return genericCallAll<DeciderAlg,ignoreBooleanReturn>(
                std::forward<T>(f),std::forward<V>(args)...);
    }

    template <
        template <class> class DeciderAlg,
        bool ignoreBooleanReturn = false,
        class T,class U>
    static bool genericCallAll(T&& f,U&& arg) {
        return detail::CallHandler< DeciderAlg<U>::TheVar, DeciderAlg >
            ::template call<ignoreBooleanReturn>(
                    std::forward<T>(f),
                    std::forward<U>(arg));
    }

public:

    /**
     * Map function. Accepts function which takes n amount of
     * args from n collections which are traversed linearly and
     * new collection is made containing elements which are
     * results of specified function.
     * @param[in] fn Function to use when mapping
     * @param[in] args Arguments, collections to traverse
     * in linear way. Assertion is made that they all
     * contain same amount of elements.
     * @param[out] T the collection element type to be returned.
     * Inferred by default
     * @param[in] passIndex Flag wether to pass current iteration
     * index while traversing. Index is passed first to function.
     * Defaults to false.
     * @param[out] ResCollection Collection type to return from
     * this function. Defaults to std::vector. Needed amount of
     * memory for collection is preallocated if applicable.
     * @param[out] Allocator Allocator to use for new collection
     * if applicable. Is ignored if collection is allocator
     * indifferent.
     *
     * Example:
     * ~~~~~~~
     * auto s1 = SF::seqL(10); // {0..9}
     * auto s2 = SF::seqL(10,20); // {10..19}
     *
     * auto func = [](int a,int b) { return a * b; };
     * auto out = SM::map<int>(func,s1,s2);
     * // type of out: std::vector<int>
     * // out contains:
     * // {0,11,24,39,56,75,96,119,144,171}
     * ~~~~~~~
     *
     * Example using prebuilt passIndex variable:
     * ~~~~~~~
     * auto s = SF::seqL(10,20); // {10..19}
     *
     * auto func = [](int i,int b) { return i * b; };
     * // passing only one sequence, first lambda
     * // argument is iteration number 0..n
     * auto out = SM::map<int,true>(func,s);
     * // type of out: std::vector<int>
     * // out contains:
     * // {0,11,24,39,56,75,96,119,144,171}
     * ~~~~~~~
     */
    template <
        class T,bool passIndex = false,
        template <class...> class ResCollection = std::vector,
        template <class> class Allocator = std::allocator,
        class U,class... Args>
    static auto map(U&& fn,Args&&... args)
     -> decltype(
         templatious::adapters::CollectionMaker<
             T,ResCollection,Allocator>::
             make(0)
     )
    {
        if (!templatious::util::SizeVerifier<Args...>(
            std::forward<Args>(args)...).areAllEqual())
        {
            throw MapFunctionNotEqualException();
        }

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace ut = templatious::util;

        auto it = ItMk::makeIter(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,long> ICall;
        ICall call;

        long size = SA::size(ut::getFirst(std::forward<Args>(args)...));
        auto e = SA::end(ut::getFirst(std::forward<Args>(args)...));
        typedef templatious::adapters::CollectionMaker<
            T,ResCollection,Allocator> Mk;

        auto result = Mk::make(size);
        long idx = 0;

        // hopefully, static_assert is more readable
        typedef decltype(call(
            std::forward<U>(fn),
            idx,it)) RetType;
        static const bool is_void =
            std::is_same< void, RetType >::value;
        static_assert( !is_void,
            "Function passed to map has to return non-void value." );

        for (; it._a != e; it.inc()) {
            SA::add(result,
                call(std::forward<U>(fn),
                idx,it));

            if (passIndex) {
                ++idx;
            }
        }

        return result;
    }

    /**
     * Linear traversal function
     * @param[in] fn Function to be used on traversal.
     * Should take arguments from args collection value types.
     * @param[in] args Collection arguments to be traversed
     * linearly. Must all have the same count of elements.
     * @param[in] passIndex Boolean value wether to pass index
     * of current traversal (from 0) to a function. If passed,
     * index is passed first. Defaults to false.
     *
     * Example:
     * ~~~~~~~
     * auto s1 = SF::seqL(10); // {0..9}
     * auto s2 = SF::seqL(10,20); // {10..19}
     *
     * int sum = 0;
     * auto func = [&](int a,int b) { sum += a * b; };
     * SM::traverse(func,s1,s2);
     *
     * assert( sum == 735 );
     * ~~~~~~~
     *
     * Example using prebuilt passIndex variable:
     * ~~~~~~~
     * auto s = SF::seqL(10,20); // {10..19}
     *
     * int sum = 0;
     * auto func = [&](int i,int b) { sum += i * b; };
     * // passing only one sequence, first lambda
     * // argument is iteration number 0..n
     * SM::traverse<true>(func,s);
     *
     * assert( sum == 735 );
     * ~~~~~~~
     */
    template <bool passIndex = false, class U, class... Args>
    static void traverse(U&& fn, Args&&... args) {
        if (!templatious::util::SizeVerifier<Args...>(
            args...).areAllEqual())
        {
            throw TraverseFunctionNotEqualException();
        }

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace ut = templatious::util;

        auto it = ItMk::makeIter(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,long> ICall;
        ICall call;

        auto e = SA::end(ut::getFirst(std::forward<Args>(args)...));
        long idx;
        if (passIndex) {
            idx = 0;
        }

        for (; it._a != e; it.inc()) {
            typedef typename ut::RetValSelector<
                decltype(
                    call(std::forward<U>(fn),
                    idx,it)
                ) > Sel;

            if (!Sel::callAndEval(
                call,
                std::forward<U>(fn),
                idx,it))
            { return; }

            if (passIndex) {
                ++idx;
            }
        }
    }

    /**
     * Quadratic traversal function.
     * ***Removing or adding elements
     * to traversed collections during
     * traversal is undefined behaviour***.
     * Example:
     * ~~~~~~~
     * std::vector<int> v;
     * SA::add(v,1,2,3);
     * SM::quadro(
     *     [](int i,int j,int k) {
     *         std::cout << i << j << k << std::endl;
     *     },
     *     v,v,v
     * );
     * // Should output:
     * // 1,1,1
     * // 1,1,2
     * // 1,1,3
     * // 1,2,1
     * // 1,2,2
     * // 1,2,3
     * // ...
     * // 3,3,2
     * // 3,3,3
     * ~~~~~~~
     * @param[in] fn Function to be used on each iteration.
     * Should take in args collection value types.
     * @param[in] args Collection arguments to be traversed
     * in quadratic manner.
     * @param[in] passIndex Sets whether to pass traversal
     * index (starting with 0) to function. If passed
     * it is passed first. Defaults to false.
     */
    template <bool passIndex = false, class U, class... Args>
    static long quadro(U&& fn, Args&&... args) {
        typedef typename templatious::recursive::IteratorMaker ItMk;
        namespace ut = templatious::util;

        auto it = ItMk::makeQuadro(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,long> ICall;
        ICall call;

        long idx = 0;

        do {
            typedef typename ut::RetValSelector<
                decltype(
                    call(std::forward<U>(fn),idx,it)
                ) > Sel;
            if (!Sel::callAndEval(
                call,
                std::forward<U>(fn),
                idx,it))
            { return ++idx; }

            ++idx;
        } while (!it.inc());

        return idx;
    }

    /**
     * Variadic set function overload.
     * @param[in] t Value used for setting.
     * @param[in,out] v One entity to set.
     * Can be collection, pack or a single
     * variable.
     * @param[in,out] args Rest of the entities
     * to use set function on.
     *
     * Example:
     * ~~~~~~~
     * int a,b,c;
     * std::vector<int> v(3);
     * int p1,p2,p3;
     * auto p = SF::pack(p1,p2,p3);
     *
     * SM::set(7,a,b,c,v,p);
     *
     * assert( a == 7 );
     * assert( b == 7 );
     * assert( c == 7 );
     * assert( v[0] == 7 );
     * assert( v[1] == 7 );
     * assert( v[2] == 7 );
     * assert( p1 == 7 );
     * assert( p2 == 7 );
     * assert( p3 == 7 );
     * ~~~~~~~
     */
    template <class T,class V,class... Col>
    static void set(T&& t,V& v,Col&... args) {
        set(std::forward<T>(t),v);
        set(std::forward<T>(t),args...);
    }

    /**
     * Value set function.
     * @param[in] t Value used for setting
     * @param[in,out] v Entity to set. Can be collection,
     * pack or a single variable.
     *
     * Example:
     * ~~~~~~~
     * int a,b,c;
     * std::vector<int> v(3);
     * auto p = SF::pack(1,2,3);
     * SM::set(7,a,b,c,v,p);
     * // a,b,c = 7
     * // v[0],v[1],v[2] = 7
     * // p.get<0>(),p.get<1>().p.get<2>() = 7
     * ~~~~~~~
     */
    template <class T,class V>
    static void set(T&& t,V& col) {
        typedef typename templatious::adapters::CollectionAdapter<V> Ad;
        typedef typename std::conditional< Ad::is_valid,
                detail::SetImplCollection,
                typename std::conditional< detail::IsPack<V>::val,
                detail::SetImplPack,
                detail::SetImplVariable
            >::type
        >::type Impl;

        Impl::impl(std::forward<T>(t),col);
    }

    /**
     * Generic call each function for multiple elements
     * (packs are considered composite)
     * @param[in] f Function to call on each argument.
     * Function may return false to stop traversal
     * (will work if ignoreBooleanReturn is set to false).
     * @param[in,out] args Arguments to call function on.
     * Can be variables or packs. Packs are processed
     * recursively calling function on each element
     * of the pack.
     * @param[in] ignoreBooleanReturn Value which determines
     * whether to interpret function return value (if it
     * happens to be boolean) as a signal to stop traversal.
     * If set to true boolean return value is ignored and
     * traversal always iterates through every element.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::pack(1,2,3);
     *
     * std::stringstream ss;
     * auto prnt = [&](int i) { ss << i; };
     * SM::callEach(prnt,7,p,7);
     *
     * assert( ss.str() == "71237" );
     *
     * std::vector<int> v(3);
     * // ILLEGAL, collections are not
     * // processed by callEach (see forEach)
     * //SM::callEach(prnt,v);
     * ~~~~~~~
     */
    template <
        bool ignoreBooleanReturn = false,
        class T,class... V
    >
    static long callEach(T&& f,V&&... args) {
        templatious::util::CallCountFunctor<T>
            func(std::forward<T>(f));

        genericCallAll< detail::DeciderCallEach, ignoreBooleanReturn >(
            func,std::forward<V>(args)...
        );

        return func.getCount();
    }

    /**
     * Generic foreach function for multiple elements
     * (collections are considered composite)
     * @param[in] f Function to call on each argument.
     * Function may return false to stop traversal
     * (will work if ignoreBooleanReturn is set to false).
     * @param[in,out] args Arguments to call function on.
     * Can be variables or collections. Collections are
     * processed element by element.
     * @param[in] ignoreBooleanReturn Value which determines
     * whether to interpret function return value (if it
     * happens to be boolean) as a signal to stop traversal.
     * If set to true boolean return value is ignored and
     * traversal always iterates through every element.
     *
     * Example:
     * ~~~~~~~
     * std::vector<int> v;
     * SA::add(v,1,2,3);
     *
     * std::stringstream ss;
     * auto prnt = [&](int i) { ss << i; };
     * SM::forEach(prnt,7,v,7);
     *
     * assert( ss.str() == "71237" );
     *
     * auto p = SF::pack(1,2,3);
     * // ILLEGAL, packs are not
     * // processed by forEach (see callEach)
     * //SM::forEach(prnt,p);
     * ~~~~~~~
     */
    template <
        bool ignoreBooleanReturn = false,
        class T,class... V
    >
    static long forEach(T&& f,V&&... args) {
        templatious::util::CallCountFunctor<T>
            func(std::forward<T>(f));

        genericCallAll< detail::DeciderForEach, ignoreBooleanReturn >(
            func,std::forward<V>(args)...
        );

        return func.getCount();
    }

    /**
     * Sum utility function. Returns numeric sum of
     * all elements. Example:
     * ~~~~~~~
     * int a,b,c;
     * a = b = c = 7;
     * auto s = SF::seqL(3);
     * auto p = SF::pack(4,5,6);
     *
     * int sum = SM::sum<int>(a,b,c,s,p);
     * // sum =
     * // 7 + 7 + 7 | a,b,c
     * // + 0 + 1 + 2 | s
     * // + 4 + 5 + 6 | p
     * // = 39
     * ~~~~~~~
     * @param[in] args Elements to sum. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double.
     */
    template <class RetVal = double,class... V>
    static RetVal sum(V&&... args) {
        RetVal r(0);
        templatious::detail::SumFunctor<RetVal> func(r);

        // since we make an assumption that
        // we're dealing with numeric values
        // we can treat values, packs and collections.
        genericCallAll< detail::DeciderAllUniform, true >(
            func,std::forward<V>(args)...
        );

        return r;
    }

    /**
     * Sum using special function. Returns numeric sum of
     * all elements.
     * @param[in] f Function to be used on every element
     * before summing an element to the result.
     * @param[in] args Elements to sum. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double.
     *
     * Example:
     * ~~~~~~~
     * int a,b,c;
     * a = b = c = 7;
     * auto s = SF::seqL(3);
     * auto p = SF::pack(4,5,6);
     *
     * int sum = SM::sumS<int>(
     *     [](int i) { return 2*i; } // multiply each arg by 2
     *     a,b,c,s,p);
     * // sum =
     * // 7*2 + 7*2 + 7*2 | a,b,c
     * // + 0*2 + 1*2 + 2*2 | s
     * // + 4*2 + 5*2 + 6*2 | p
     * // = 78
     * ~~~~~~~
     */
    template <class RetVal = double,class Fn,class... V>
    static RetVal sumS(Fn&& f,V&&... args) {
        RetVal r(0);
        templatious::detail::SumFunctorCustom<
            RetVal,decltype(std::forward<Fn>(f))>
            func(r,std::forward<Fn>(f));
        func._cnt = 0;

        // again, assumption about numeric values
        genericCallAll< detail::DeciderAllUniform >(
            func,std::forward<V>(args)...
        );

        return r;
    }

    /**
     * Average of values. Returns numeric average
     * of elements passed.
     * @param[in] args Elements to sum. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double.
     *
     * Example:
     * ~~~~~~~
     * int a,b,c;
     * a = 1; b = 2; c = 3;
     * auto p = SF::pack(a,b,c);
     * std::vector<int> v;
     * SA::add(v,5,6,7);
     *
     * double avg = SM::avg(p,4,v);
     * assert( avg == 4 );
     * ~~~~~~~
     */
    template <class RetVal = double,class... V>
    static RetVal avg(V&&... args) {
        RetVal r(0);
        templatious::detail::SumFunctor<RetVal,true> func(r);
        func._cnt = 0;

        // again, assumption about numeric values
        genericCallAll< detail::DeciderAllUniform, true >(
            func,std::forward<V>(args)...
        );

        return r / func._cnt;
    }

    /**
     * Average of values using special function.
     * Returns numeric average of elements passed.
     * @param[in] f Function to be used on every element
     * before summing an element to the result.
     * @param[in] args Elements to sum. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double.
     *
     * Example:
     * ~~~~~~~
     * int a,b,c;
     * a = 1; b = 2; c = 3;
     * auto p = SF::pack(a,b,c);
     * std::vector<int> v;
     * SA::add(v,5,6,7);
     *
     * auto trFunc = [](int i) { return 2*i; };
     * double avg = SM::avgS(trFunc,p,4,v);
     * assert( avg == 8 );
     * ~~~~~~~
     */
    template <class RetVal = double,class Fn,class... V>
    static RetVal avgS(Fn&& f,V&&... args) {
        RetVal r(0);
        templatious::detail::SumFunctorCustom<
            RetVal,decltype(std::forward<Fn>(f)),true>
            func(r,std::forward<Fn>(f));
        func._cnt = 0;

        // again, assumption about numeric values
        genericCallAll< detail::DeciderAllUniform, true >(
            func,std::forward<V>(args)...
        );

        return r / func._cnt;
    }

    /**
     * Minimum of values. Returns numeric minimum
     * of elements passed.
     * @param[in] args Elements to traverse. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double. May be specified to return references.
     *
     * Example:
     * ~~~~~~~
     * int a = 2;
     * int b = 3;
     * auto p = SF::pack(4,5,6);
     * auto s = SF::seqL(6,8);
     * int min = SM::min<int>(a,b,p,s,-1);
     * assert(min == -1);
     * ~~~~~~~
     */
    template <class RetVal = double,class... V>
    static RetVal min(V&&... args) {
        typedef templatious::detail::DefaultLessComparator Comp;
        return maxS<RetVal>(Comp(),std::forward<V>(args)...);
    }

    /**
     * Maximum of values. Returns numeric maximum
     * of elements passed.
     * @param[in] args Elements to traverse. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double. May be specified to return references.
     *
     * Example:
     * ~~~~~~~
     * int a = 2;
     * int b = 3;
     * auto p = SF::pack(4,5,6);
     * auto s = SF::seqL(6,8);
     * int max = SM::max<int>(a,b,p,s,-1);
     * assert(max == 7);
     * ~~~~~~~
     */
    template <class RetVal = double,class... V>
    static RetVal max(V&&... args) {
        typedef templatious::detail::DefaultMoreComparator Comp;
        return maxS<RetVal>(Comp(),std::forward<V>(args)...);
    }

    /**
     * Maximum of values using a special function.
     * Returns numeric user specified maximum according
     * to the user specified comparison of elements passed.
     * @param[in] cf Function to be used for comparison.
     * Should take two parameters for comparison. For instance,
     * if maximum is wanted the function is called like cf(a,b)
     * and it should return true if a > b.
     * @param[in] args Elements to traverse. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double. May be specified to return references.
     *
     * Example:
     * ~~~~~~~
     * struct MyPod {
     *     MyPod(int a,int b) :
     *      _a(a), _b(b) {}
     *
     *     int _a;
     *     int _b;
     * };
     *
     * MyPod a(7,8);
     * std::vector<MyPod> v;
     * SA::add(v,MyPod(1,2),MyPod(3,4),MyPod(5,6));
     * MyPod& mp = SM::maxS<MyPod&>(
     *     [](const MyPod& a,const MyPod& b) {
     *         return a._a * a._b > b._a * b._b;
     *     },
     *     v,a
     * );
     *
     * assert(&a == &mp);
     * ~~~~~~~
     */
    template <class RetVal = double,class CompFunc,class... V>
    static RetVal maxS(CompFunc&& cf,V&&... args) {
        typedef typename std::conditional<
            std::is_reference<RetVal>::value,
            templatious::util::MutRefContainer<RetVal>,
            templatious::util::MutCopyContainer<RetVal>
        >::type TheCont;

        typedef templatious::detail::CompFunctor<
            TheCont, decltype(std::forward<CompFunc>(cf)),
            templatious::util::DefaultStoragePolicy
        > TheFctor;

        TheFctor f(std::forward<CompFunc>(cf));

        genericCallAll< detail::DeciderAllUniform, true >(
            f,std::forward<V>(args)...
        );

        return f._c.getRef();
    }

    /**
     * Fold function. Folds multiple values to single.
     * @param[in] start Starting value. Should be copy
     * constructable.
     * @param[in] f Function to apply in every iteration.
     * Should take in two values as arguments,
     * the accumulator value and the value processed
     * through every iteration of a sequence.
     * Should return newly calculated value.
     * @param[in] args Arguments to traverse. Can process
     * packs, collections and single variables.
     *
     * Example:
     * ~~~~~~~
     * auto mulFunc = [](int curr,int next)
     *     { return curr * next; };
     *
     * int res = SM::fold(1,mulFunc,2,3,4,5,6,7);
     * assert( res == 5040 );
     * ~~~~~~~
     */
    template <class T,class F,class... Args>
    static T fold(T&& start,F&& f,Args&&... args) {
        auto var = start;
        detail::FoldFunctor<
            decltype(std::forward<T>(start)),
            decltype(std::forward<F>(f))
        > ff(var,std::forward<F>(f));

        genericCallAll< detail::DeciderAllUniform, true >(
                ff,std::forward<Args>(args)...
        );

        return std::move(var);
    }

    /**
     * Distribute function. Distributes from pack
     * or collection over other collections/packs/variables
     * in a linear fashion. Returns how many
     * distributions were made.
     * @param[in] t Entity to distribute from.
     * Can be pack or collection.
     * @param[out] args Arguments to distribute over.
     * Can be packs, collections or single variables.
     *
     * Example:
     * ~~~~~~~
     * int a,b,c;
     * int p1,p2,p3;
     * auto p = SF::pack(p1,p2,p3);
     * std::vector<int> v(3);
     *
     * auto s = SF::seqL(12);
     * int count = SM::distribute(s,a,b,c,p,v);
     *
     * assert( count == 9 );
     *
     * assert( a == 0 );
     * assert( b == 1 );
     * assert( c == 2 );
     * assert( p1 == 3 );
     * assert( p2 == 4 );
     * assert( p3 == 5 );
     * assert( v[0] == 6 );
     * assert( v[1] == 7 );
     * assert( v[2] == 8 );
     * ~~~~~~~
     */
    template <
        class T,class... Args
    >
    static long distribute(T&& t,Args&&... args) {
        return distributeInternal<false>(
            templatious::detail::AssignDispatcher(),
            std::forward<T>(t),
            std::forward<Args>(args)...);
    }

    /**
     * Reverse distribute function. Distributes
     * over pack or collection from other
     * collections/packs/variables in a linear fashion.
     * Returns how many distributions were made.
     * @param[out] t Entity to distribute to.
     * Can be pack or collection.
     * @param[in] args Arguments to distribute from.
     * Can be packs, collections or single variables.
     *
     * Example:
     * ~~~~~~~
     * std::vector<int> out(9);
     *
     * int a,b,c;
     * a = 0; b = 1; c = 2;
     * auto p = SF::pack(3,4,5);
     * auto s = SF::seqI(6,8);
     *
     * int count = SM::distributeR(out,a,b,c,p,s);
     *
     * assert( count == 9 );
     *
     * assert( out[0] == 0 );
     * assert( out[1] == 1 );
     * assert( out[2] == 2 );
     * assert( out[3] == 3 );
     * assert( out[4] == 4 );
     * assert( out[5] == 5 );
     * assert( out[6] == 6 );
     * assert( out[7] == 7 );
     * assert( out[8] == 8 );
     * ~~~~~~~
     */
    template <
        class T,class... Args
    >
    static long distributeR(T&& t,Args&&... args) {
        return distributeInternal<false>(
            templatious::detail::RevAssignDispatcher(),
            std::forward<T>(t),
            std::forward<Args>(args)...);
    }

    /**
     * Distribute using special function. Returns how
     * many distributions were made.
     * @param[in] f Function for use in distribution.
     * should take two arguments. May return boolean
     * value false to stop distribution at any time
     * (ignoreBooleanReturn has to be set to false).
     * @param[in,out] t First part distribution entity,
     * first argument for the function f.
     * Can be pack or collection.
     * @param[in,out] args Second part of distribution
     * entity. Second argument for the function f.
     * Can be packs, collections or single variables.
     * @param[in] ignoreBooleanReturn Value to determine
     * whether function return value (if it happens to
     * be boolean) should stop traversal if it returns
     * false. Defaults to false.
     */
    template <
        bool ignoreBooleanReturn = false,
        class Func,class T,class... Args
    >
    static long distributeSpecial(Func&& f,T&& t,Args&&... args) {
        return distributeInternal<ignoreBooleanReturn>(
            std::forward<Func>(f),
            std::forward<T>(t),
            std::forward<Args>(args)...);
    }

    /**
     * Function that checks whether collections
     * passed contain identical elements.
     * Functor specifies the way of comparison
     * and should return true when elements are
     * equal. This is intended to use for collections
     * only.
     * @param[in] f Function to be used when comparing.
     * @param[in] a First collection to compare.
     * @param[in] b Second collection to compare.
     */
    template <
        class F,class A,class B
    >
    static bool areCollectionsEqualS(const F& f,const A& a,const B& b) {
        typedef templatious::adapters::
            CollectionAdapter<A> AdapterA;
        typedef templatious::adapters::
            CollectionAdapter<B> AdapterB;

        long aSz = AdapterA::size(a);
        long bSz = AdapterB::size(b);

        bool anyUnknown = aSz == -1 || bSz == -1;

        if (aSz != bSz && !anyUnknown) {
            return false;
        }

        auto ba = AdapterA::cbegin(a);
        auto bb = AdapterB::cbegin(b);

        auto ea = AdapterA::cend(a);
        auto eb = AdapterB::cend(b);

        while (ba != ea && bb != eb) {
            if (!f(*ba,*bb)) {
                return false;
            }

            ++ba;
            ++bb;
        }

        // make sure we traversed all
        if (ba != ea || bb != eb) {
            return false;
        }

        return true;
    }

    /**
     * Same as the two element version but for more collections.
     * @param[in] f Function to be used when comparing.
     * @param[in] a First collection to compare.
     * @param[in] b Second collection to compare.
     */
    template <
        class F,class A,class B,
        class... Tail
    >
    static bool areCollectionsEqualS(const F& f,
            const A& a,const B& b,const Tail&... tail)
    {
        if (!areCollectionsEqualS(f,a,b)) {
            return false;
        }
        return areCollectionsEqualS(f,b,tail...);
    }

    /**
     * Check if passed collections contain identical
     * elements using the default '==' operator.
     * @param[in] args Argument collections to check.
     */
    template <class... Args>
    static bool areCollectionsEqual(const Args&... args) {
        return areCollectionsEqualS(
            detail::DefaultComparator(),args...);
    }

    /**
     * Check if all elements match given predicate
     * function. Works with collections or single variables.
     * Collections are treated as composite.
     * @param[in] f Function that should return true
     * if element satisfies condition.
     * @param[in] args Argument collections/variables to check.
     */
    template <class F,class... Args>
    static bool forAll(F&& f,Args&&... args) {
        templatious::detail::ForallFunctor<
            decltype(std::forward<F>(f)),
            templatious::util::DefaultStoragePolicy
        > fctor(std::forward<F>(f));

        genericCallAll< detail::DeciderForEach, false >(
            fctor,std::forward<Args>(args)...
        );

        return fctor._state;
    }

    /**
     * Check if all elements match given predicate
     * function. Works with packs or single variables.
     * Packs are treated as composite.
     * @param[in] f Function that should return true
     * if element satisfies condition.
     * @param[in] args Argument collections/variables to check.
     */
    template <class F,class... Args>
    static bool forAllP(F&& f,Args&&... args) {
        templatious::detail::ForallFunctor<
            decltype(std::forward<F>(f)),
            templatious::util::DefaultStoragePolicy
        > fctor(std::forward<F>(f));

        genericCallAll< detail::DeciderCallEach, false >(
            fctor,std::forward<Args>(args)...
        );

        return fctor._state;
    }

    /**
     * Check if at least one element matches given predicate
     * function. Works with collections or single variables.
     * Collections are treated as composite.
     * @param[in] f Function that should return true
     * if element satisfies condition.
     * @param[in] args Argument collections/variables to check.
     */
    template <class F,class... Args>
    static bool exists(F&& f,Args&&... args) {
        templatious::detail::ExistsFunctor<
            decltype(std::forward<F>(f)),
            templatious::util::DefaultStoragePolicy
        > fctor(std::forward<F>(f));

        genericCallAll< detail::DeciderForEach, false >(
            fctor,std::forward<Args>(args)...
        );

        return fctor._state;
    }

    /**
     * Check if at least one element matches given predicate
     * function. Works with packs or single variables.
     * Packs are treated as composite.
     * @param[in] f Function that should return true
     * if element satisfies condition.
     * @param[in] args Argument collections/variables to check.
     */
    template <class F,class... Args>
    static bool existsP(F&& f,Args&&... args) {
        templatious::detail::ExistsFunctor<
            decltype(std::forward<F>(f)),
            templatious::util::DefaultStoragePolicy
        > fctor(std::forward<F>(f));

        genericCallAll< detail::DeciderCallEach, false >(
            fctor,std::forward<Args>(args)...
        );

        return fctor._state;
    }

    /**
     * Dump iterators of collection into one
     * collection. Useful for operations like
     * quick sorting of collections without
     * random access iterators, like std::list.
     * @param[in] t Collection to dump iterators
     * from.
     */
    template <
        bool saveIdx = false,
        template <class...> class Collection = std::vector,
        template <class> class Alloc = std::allocator,
        class T
    >
    static auto iterDump(T&& t)
     -> decltype(
         templatious::detail::CollectionIterDumper<
             decltype(std::forward<T>(t)),
             saveIdx,
             Collection,Alloc
         >::dumpIter(std::forward<T>(t))
     )
    {
        return templatious::detail::CollectionIterDumper<
            decltype(std::forward<T>(t)),
            saveIdx,
            Collection,Alloc
        >::dumpIter(std::forward<T>(t));
    }

    /**
     * Sort elements in the collection
     * in ascending order.
     * Collection has to be mutable. If
     * collection iterator has
     * random_access_iterator_tag then simply
     * calls std::sort on collection. If
     * iterator doesn't have random access
     * iterator tag then all iterators are
     * dumped to collection with random
     * access iterator tag and are sorted then.
     * @param[in] t Collection to be sorted.
     */
    template <class T>
    static void sort(T&& t) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        static const bool isRand = templatious::util
            ::IsRandomAccessIteratorTagged<
                typename Ad::Iterator>::value;

        templatious::detail::SortAlg<isRand>::sort(t);
    }

    /**
     * Sort elements in the collection using
     * special comparator function which returns
     * true for f(a,b) if a < b by default.
     * Collection has to be mutable. If
     * collection iterator has
     * random_access_iterator_tag then simply
     * calls std::sort on collection. If
     * iterator doesn't have random access
     * iterator tag then all iterators are
     * dumped to collection with random
     * access iterator tag and are sorted then.
     * @param[in] t Collection to be sorted.
     * @param[in] c Comparator function to be used.
     * should take two parameters by default. If
     * returns true on f(a,b) when a < b collection
     * is sorted in ascending order.
     */
    template <class T,class Comparator>
    static void sortS(T&& t,Comparator&& c) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        static const bool isRand = templatious::util
            ::IsRandomAccessIteratorTagged<
                typename Ad::Iterator>::value;

        templatious::detail::SortAlg<isRand>
            ::sort(t,std::forward<Comparator>(c));
    }

    /**
     * Check if collection is sorted using
     * special comparator.
     * @param[in] t Collection to check.
     * @param[in] c Comparator to use. Function
     * which returns true for f(a,b) if a < b.
     */
    template <class T,class Comparator>
    static bool isSortedS(T&& t,Comparator&& c) {
        typedef templatious::adapters::CollectionAdapter<const T> Ad;
        auto b = Ad::cbegin(t);
        auto e = Ad::cend(t);
        if (b == e) return true;

        auto pr = b;
        ++b;

        while (b != e) {
            if (c(*b,*pr)) return false;
            pr = b;
            ++b;
        }

        return true;
    }

    /**
     * Check if collection is sorted using
     * default comparator.
     * @param[in] t Collection to check.
     */
    template <class T>
    static bool isSorted(T&& t) {
        return isSortedS(std::forward<T>(t),
            templatious::detail::DefaultLessComparator());
    }
};

namespace detail {

    // not random access iterator
    template <>
    struct SortAlg<false> {
        template <class T,class Comparator>
        static void sort(T& t,Comparator&& c) {
            auto d = templatious::StaticManipulator::iterDump<true>(t);
            typedef templatious::adapters::CollectionAdapter<T> Ad;
            typedef templatious::adapters::CollectionAdapter<
                decltype(d)> AdD;
            typedef templatious::adapters::CollectionMaker<
                typename Ad::Iterator,std::vector,std::allocator> Maker;

            auto rawDump = Maker::make(AdD::size(d));
            typedef templatious::adapters::CollectionAdapter<
                decltype(rawDump)> AdR;

            auto e = Ad::end(t);
            for (auto i = Ad::begin(t); i != e; ++i) {
                AdR::add(rawDump,i);
            }

            std::sort(AdD::begin(d),AdD::end(d),
                std::forward<Comparator>(c));

            templatious::detail::VectoratorAlgs
                ::vectoratorSortExp(rawDump,d,e);
        }

        template <class T>
        static void sort(T& t) {
            sort(t,templatious::detail
                ::DefaultLessComparator());
        }
    };

    template <
        template <class> class Decider
    >
    struct CallHandler< Variant::Item, Decider > {
        template <
            bool ignoreBooleanReturn = false,
            class F,class T
        >
        static bool call(F&& f,T&& t) {
            typedef templatious::util::RetValSelector<
                decltype(f(std::forward<T>(t))),
                true> Sel;

            bool res = Sel::callAndEval(
                std::forward<F>(f),
                std::forward<T>(t)
            );

            return ignoreBooleanReturn || res;
        }
    };

    template <
        template <class> class Decider
    >
    struct CallHandler< Variant::Collection, Decider > {
        template <
            bool ignoreBooleanReturn = false,
            class F,class T
        >
        static bool call(F&& f,T&& t) {
            typedef templatious::adapters::CollectionAdapter<T> Ad;
            typedef decltype(Ad::begin(t)) It;
            typedef templatious::util::RetValSelector<
                decltype(f(*std::declval<It>())),
                true> Sel;

            auto end = Ad::end(t);
            for (auto i = Ad::begin(t);
                    i != end;
                    ++i)
            {
                bool res = Sel::callAndEval(std::forward<F>(f),*i);
                if (!ignoreBooleanReturn && !res)
                {
                    return false;
                }
            }

            return true;
        }
    };

    template <
        template <class> class Decider
    >
    struct CallHandler< Variant::Pack, Decider > {

        template <bool ignoreBooleanReturn, class Func>
        struct PackCaller {
            PackCaller(Func f) : _r(f) {}

            template <class... T>
            bool operator()(T&&... t) {
                return templatious::StaticManipulator::
                    genericCallAll< Decider, ignoreBooleanReturn >(
                        _r.getRef(),std::forward<T>(t)...);
            }

        private:
            typedef typename templatious::util::
                DefaultStoragePolicy<Func>::Container Cont;

            Cont _r;
        };

        template <
            bool ignoreBooleanReturn = false,
            class F,class T
        >
        static bool call(F&& f,T&& t) {
            PackCaller<ignoreBooleanReturn,F> p(std::forward<F>(f));
            bool res = t.template call<ignoreBooleanReturn>(p);
            return ignoreBooleanReturn || res;
        }
    };

    struct SetImplCollection {

        template <class T,class V>
        static void impl(T&& t,V& c) {
            typedef typename templatious::adapters::CollectionAdapter<V> Ad;

            auto end = Ad::end(c);
            for (auto i = Ad::begin(c);
                      i != end;
                      ++i)
            {
                *i = t;
            }
        }

    };

    struct SetImplVariable {

        template <class T,class V>
        static void impl(T&& t,V& c) {
            c = t;
        }

    };

    template <class T>
    struct SetCalleachFctor {
        const T& _c; // set variable storage
        SetCalleachFctor(const T& t) : _c(t) {}

        template <class V>
        void operator()(V& v) {
            v = _c;
        }
    };


    struct SetImplPack {

        template <class T,class V>
        static void impl(T&& t,V& c) {
            SetCalleachFctor<const T&> f(std::forward<T>(t));
            templatious::StaticManipulator::callEach(f,c);
        }

    };



}
}

#endif
