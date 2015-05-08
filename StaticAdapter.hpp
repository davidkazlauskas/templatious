//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  StaticAdapter.hpp
 *
 *    Description:  Static adapter for convenience
 *
 *        Version:  1.0
 *        Created:  08/18/2014 07:38:12 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef STATICADAPTER_ONGU27R7
#define STATICADAPTER_ONGU27R7

#include <assert.h>

#include <templatious/CollectionAdapter.hpp>
#include <templatious/virtual/Iterator.hpp>
#include <templatious/Pack.hpp>
#include <templatious/detail/MatchFunctor.hpp>

namespace templatious {
namespace sa_spec {

template <bool tryPull,class BadVal,class Func,class Arg>
struct PullForwardType {
    typedef templatious::util::IsCallableWith< Func, Arg > ICVal;
    typedef typename ICVal::type FwdType;
};

template <class BadVal,class Func,class Arg>
struct PullForwardType<false,BadVal,Func,Arg> {
    typedef BadVal FwdType;
};

template <class Func,class Arg>
struct IsCallableWithInternal {
    typedef typename PullForwardType<
        true,void,Func,Arg>::FwdType type;
};

// we don't want to trigger MatchFunctor
// static assert if it doesn't match anything
template <
    template <class> class StoragePolicy,
    class Arg,
    class... T
> struct IsCallableWithInternal< templatious::detail::
    MatchFunctor< StoragePolicy, T... >, Arg >
{
    typedef templatious::detail::
        MatchFunctor< StoragePolicy, T... > MatchFunctor;
    static const bool does_match = MatchFunctor::
        template DoesMatch< Arg >::value;

    typedef typename PullForwardType<
        does_match,templatious::util::InvalidType,
        MatchFunctor, Arg >::FwdType type;
};

enum AdditionVariant { Data, Collection, Pack, Invalid };

template <class T, class U, class Function>
struct AdditionSelector {
    typedef templatious::adapters::CollectionAdapter<T> AdT;
    typedef templatious::adapters::CollectionAdapter<U> AdU;

    typedef typename std::decay<Function>::type FuncDec;
    typedef IsCallableWithInternal< FuncDec, U > ICval;
    typedef typename ICval::type FwdType;

    static const bool areAdaptable =
        AdT::is_valid && AdU::is_valid;

    static const bool isConvertable =
        std::is_convertible<
            FwdType, typename AdT::ValueType
        >::value;

    template <bool validAdapter,class ValType>
    struct IsInnerConvertable {
        typedef templatious::adapters::CollectionAdapter<ValType> Ad;
        typedef typename Ad::ValueType VType;
        typedef templatious::util::IsCallableWith<
            Function, VType > ICInner;
        typedef typename ICInner::type FwdType;
    };

    template <class ValType>
    struct IsInnerConvertable<false,ValType> {
        typedef templatious::util::InvalidType FwdType;
    };

    typedef IsInnerConvertable<
        AdU::is_valid,U> InnerConv;

    static const bool innerConv =
        std::is_convertible<
            typename InnerConv::FwdType, typename AdT::ValueType
        >::value;

    enum {
        val = templatious::util::IntSelector<
            isConvertable,
            AdditionVariant::Data,
            templatious::util::IntSelector<
                innerConv,
                AdditionVariant::Collection,
                templatious::util::IntSelector<
                    templatious::detail::IsPack<U>::val,
                    AdditionVariant::Pack,
                    AdditionVariant::Invalid
                >::val
            >::val
        >::val
    };
};

struct ForwardFunctor {
    template <class T>
    auto operator()(T&& t)
     -> decltype(std::forward<T>(t))
    {
        return std::forward<T>(t);
    }
};

template <int var>
struct add_custom;

template <>  // add data one by one
struct add_custom< AdditionVariant::Data > {
    template <class T,class F,class U>
    static void add(T& c,F&& f,U&& i) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::add(c, f(std::forward<U>(i)));
    }
};

template <>  // add collections
struct add_custom< AdditionVariant::Collection > {
    template <class T, class F, class U>
    static void add(T& t, F&& f, U&& u) {
        typedef templatious::adapters::CollectionAdapter<T> AdT;
        typedef templatious::adapters::CollectionAdapter<const U> AdU;
        static_assert(AdT::is_valid, "Adapter not supported.");
        static_assert(AdU::is_valid, "Adapter not supported.");
        // cache end because compiler doesn't
        // seem to optimize out adapter end calls
        // on rvalue case.
        auto e = AdU::end(u);
        for (auto i = AdU::begin(u); i != e; ++i) {
            AdT::add(t, f(*i));
        }
    }
};

template <>  // add invalid overload for friendly user message
struct add_custom< AdditionVariant::Invalid > {
    template <class T, class F, class... Any>
    static void add(T& c, F&& f, Any&&... anything) {
        static_assert(templatious::util::DummyResolver<T,false>::val,
            "Type could not be added to collection.");
    }
};

template <>  // add static arrays
struct add_custom< AdditionVariant::Pack > {
    typedef add_custom< AdditionVariant::Pack > ThisAddition;

    template <
         int which = 0,
         class T, class F,class P
    >
    static void add(
        T& c, F&& f,
        P&& p)
    {
        typedef typename std::conditional<
            which < std::decay<P>::type::size,
            AddNextVar,
            EmptyVar
        >::type Var;

        Var::template addInternal<
            which
        >(
            c,
            std::forward<F>(f),
            std::forward<P>(p)
        );
    }

private:
    struct AddNextVar {
        template <
            int which,
            class T, class F,
            class P
        >
        static void addInternal(
            T& c,F&& f,
            P&& p)
        {
            typedef templatious::adapters::CollectionAdapter<T> Ad;
            Ad::add(c,f(p.template get<which>()));
            ThisAddition::add< which + 1 >(
                c,
                std::forward<F>(f),
                std::forward<P>(p)
            );
        }
    };

    struct EmptyVar {
        // do nothing
        template <int which,class... Args>
        static void addInternal(Args&&... args) { }
    };
};

}

/**
 * A class which holds static methods that
 * mainly have to do with interacting with
 * a single collection.
 */
struct StaticAdapter {
    /**
     * Returns begin iterator for any collection.
     * @param[in] c Collection
     */
    template <class T>
    static auto begin(T&& c)
        -> decltype(adapters::CollectionAdapter<T>::begin(c)) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::begin(c);
    }

    /**
     * Returns end iterator for any collection.
     * @param[in] c Collection
     */
    template <class T>
    static auto end(T&& c) -> decltype(adapters::CollectionAdapter<T>::end(c)) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::end(c);
    }

    /**
     * Returns const begin iterator for any collection.
     * @param[in] c Collection
     */
    template <class T>
    static auto cbegin(const T& c)
        -> typename adapters::CollectionAdapter<T>::ConstIterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::cbegin(c);
    }

    /**
     * Returns const end iterator for any collection.
     * @param[in] c Collection
     */
    template <class T>
    static auto cend(const T& c)
        -> typename adapters::CollectionAdapter<T>::ConstIterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::cend(c);
    }

    /**
     * Adds element to any collection with initializer list.
     * @param[in,out] c Collection to be added to.
     * @param[in] o Initializer list.
     */
    template <class T, class U>
    static void add(T& c, const std::initializer_list<U>& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        sa_spec::ForwardFunctor fwd;
        auto end = o.end();
        for (auto i = o.begin(); i != end; ++i) {
            Ad::add(c,fwd(*i));
        }
    }

    /**
     * Adds elements to any collection.
     * @param[in,out] c Collection to be added to.
     * @param[in] o Value to be added.
     * Can be pack, collection or a simple variable.
     * Individual elements in packs or collections
     * are added.
     */
    template <class T, class U>
    static void add(T& c, U&& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef sa_spec::ForwardFunctor Fwd;
        Fwd fwd;
        typedef decltype(std::forward<U>(o)) UTrue;
        sa_spec::add_custom<
            sa_spec::AdditionSelector<T, UTrue, Fwd>::val
        >::add(c,fwd,std::forward<U>(o));
    }

    /**
     * Variadic add method overload.
     * @param[in,out] c Collection to be added to.
     * @param[in] o Value to be added.
     * Can be pack, collection or a simple variable.
     * Individual elements in packs or collections
     * are added.
     * @param[in] args The rest of the values to be processed.
     */
    template <class T, class U, class... Args>
    static void add(T& c, U&& o, Args&&... args) {
        sa_spec::ForwardFunctor fwd;
        addCustom(c, fwd, std::forward<U>(o));
        addCustom(c, fwd, std::forward<Args>(args)...);
    }

    /**
     * Custom add function.
     * @param[in,out] c Collection to be added to.
     * @param[in] f Function to be applied for each added
     * element. Should return final addable type.
     * @param[in] o Value to be added.
     * Can be pack, collection or a simple variable.
     * Individual elements in packs or collections
     * are added.
     */
    template <class T, class F, class U>
    static void addCustom(T& c, F&& f, U&& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef decltype(std::forward<F>(f)) Fwd;
        typedef decltype(std::forward<U>(o)) UTrue;
        sa_spec::add_custom<
            sa_spec::AdditionSelector<T, UTrue, Fwd>::val
        >::add(c, std::forward<F>(f), std::forward<U>(o));
    }

    /**
     * Custom add function.
     * @param[in,out] c Collection to be added to.
     * @param[in] f Function to be applied for each added
     * element. Should return final addable type.
     * @param[in] o Value to be added.
     * Can be pack, collection or a simple variable.
     * Individual elements in packs or collections
     * are added.
     * @param[in] args The rest of the values to be processed.
     */
    template <class T, class F, class U, class... Args>
    static void addCustom(T& c, F&& f, U&& o, Args&&... args) {
        addCustom(c, std::forward<F>(f), std::forward<U>(o));
        addCustom(c, std::forward<F>(f), std::forward<Args>(args)...);
    }

    /**
     * Size getter for collection. Returns size
     * or -1 if size cannot be determined.
     * @param[in] c Collection to lookup size for.
     */
    template <class T>
    static long size(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::size(c);
    }

    /**
     * Size getter for collection. Returns what
     * would simple size function return, but if
     * simple size function returns -1 collection
     * is traversed to find it's final size.
     * @param[in] c Collection to lookup size for.
     */
    template <class T>
    static long trueSize(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        long initial = size(c);
        if (initial != -1) {
            return initial;
        }

        long count = 0;
        auto e = cend(c);
        for (auto i = cbegin(c); i != e; ++i) {
            ++count;
        }

        return count;
    }

    /**
     * Determines if collection is eligible for
     * element addition.
     * @param[in] c Collection to query.
     */
    template <class T>
    static bool canAdd(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::canAdd(c);
    }

    /**
     * Get element from collection by index.
     * @param[in,out] c Collection to get element from.
     * @param[in] i Element index to get.
     */
    template <class T>
    static auto getByIndex(T& c,long i)
        -> typename adapters::CollectionAdapter<T>::ValueType& {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::getByIndex(c, i);
    }

    /**
     * Erase elements from collection by beginning
     * and end iterators.
     * @param[in,out] c Collection to erase from.
     * @param[in] beg Beginning iterator.
     * @param[in] end End iterator.
     */
    template <class T>
    static void erase(T& c,
                      typename adapters::CollectionAdapter<T>::Iterator beg,
                      typename adapters::CollectionAdapter<T>::Iterator end)
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, beg, end);
    }

    /**
     * Erase single element from a collection.
     * @param[in,out] c Collection to erase from.
     * @param[in] pos Iterator to erase.
     */
    template <class T>
    static void erase(T& c,
                      typename adapters::CollectionAdapter<T>::Iterator pos) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, pos);
    }

    /**
     * Erase elements from a collection from
     * position until end.
     * @param[in,out] c Collection to erase from.
     * @param[in] pos Iterator to start erasing from.
     */
    template <class T>
    static void eraseTillEnd(T& c,
         typename adapters::CollectionAdapter<T>::Iterator pos)
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, pos, Ad::end(c));
    }

    /**
     * Get iterator from a collection by index.
     * Requesting iterator at a position of size
     * should return end iterator. However, anything
     * more than that throws an exception. Requesting
     * iterator at negative position throws an exception.
     * @param[in] c Collection to get iterator from.
     * @param[in] i Index of iterator to get.
     */
    template <class T>
    static auto iterAt(T& c,long i)
        -> typename adapters::CollectionAdapter<T>::Iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::iterAt(c, i);
    }

    /**
     * Get constant iterator from a collection by index.
     * Requesting iterator at a position of size
     * should return end iterator. However, anything
     * more than that throws an exception. Requesting
     * iterator at negative position throws an exception.
     * @param[in] c Collection to get iterator from.
     * @param[in] i Index of iterator to get.
     */
    template <class T>
    static auto citerAt(T& c,long i)
        -> typename adapters::CollectionAdapter<T>::ConstIterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::citerAt(c, i);
    }

    /**
     * Insert element to collection.
     * @param[in,out] c Collection to insert to.
     * @param[in] at Position iterator where to insert.
     * Inserted element replaces element pointed to this iterator.
     * @param[in] val Value to insert.
     */
    template <class T,class V>
    static void insert(
        T& c, typename adapters::CollectionAdapter<T>::Iterator at,
        V&& val)
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::insertAt(c,at,std::forward<V>(val));
    }

    /**
     * Clear collection.
     * @param[out] c Collection to clear.
     */
    template <class T>
    static void clear(T&& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::clear(std::forward<T>(c));
    }

    /**
     * Clear multiple collections.
     * @param[out] c Collection to clear.
     * @param[out] t Rest of collections to clear.
     */
    template <class T,class... Tail>
    static void clear(T&& c,Tail&&... t) {
        clear(std::forward<T>(c));
        clear(std::forward<Tail>(t)...);
    }

    /**
     * Virtual begin iterator. Can be used
     * for exposing elements across
     * translation units.
     * @param[in,out] c Collection to expose.
     */
    template <class T>
    static auto vbegin(T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::ValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        // creation of VImpl should never throw
        VImpl *v = new VImpl( Ad::begin(c) );
        return VIterator< ValType >(v);
    }

    /**
     * Virtual end iterator. Can be used
     * for exposing elements across
     * translation units.
     * @param[in,out] c Collection to expose.
     */
    template <class T>
    static auto vend(T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::ValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        // creation of VImpl should never throw
        VImpl *v = new VImpl( Ad::end(c) );
        return VIterator< ValType >(v);
    }

    /**
     * Virtual const begin iterator. Can be used
     * for exposing elements across
     * translation units.
     * @param[in,out] c Collection to expose.
     */
    template <class T>
    static auto vcbegin(const T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::ConstValueType >
    {
        typedef adapters::CollectionAdapter<const T> Ad;
        typedef typename Ad::ConstValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        // creation of VImpl should never throw
        VImpl *v = new VImpl( Ad::cbegin(c) );
        return VIterator< ValType >(v);
    }

    /**
     * Virtual const end iterator. Can be used
     * for exposing elements across
     * translation units.
     * @param[in,out] c Collection to expose.
     */
    template <class T>
    static auto vcend(const T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::ConstValueType >
    {
        typedef adapters::CollectionAdapter<const T> Ad;
        typedef typename Ad::ConstValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        // creation of VImpl should never throw
        VImpl *v = new VImpl( Ad::cend(c) );
        return VIterator< ValType >(v);
    }

    /**
     * Virtual iterator by index. Can be used
     * for exposing elements across
     * translation units.
     * @param[in,out] c Collection to expose.
     */
    template <class T>
    static auto viterAt(T& c,long s)
    -> VIterator< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::ValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        // creation of VImpl should never throw
        VImpl *v = new VImpl( Ad::iterAt(c,s) );
        return VIterator< ValType >(v);
    }

    /**
     * Virtual const iterator by index. Can be used
     * for exposing elements across
     * translation units.
     * @param[in,out] c Collection to expose.
     */
    template <class T>
    static auto vciterAt(const T& c,long s)
    -> VIterator< typename adapters::CollectionAdapter<T>::ConstValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::ConstValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        // creation of VImpl should never throw
        VImpl *v = new VImpl( Ad::citerAt(c,s) );
        return VIterator< ValType >(v);
    }

};

}

#endif /* end of include guard: STATICADAPTER_ONGU27R7 */
