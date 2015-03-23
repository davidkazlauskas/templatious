//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  TypeList.hpp
 *
 *    Description:  Simple typelist to contain types
 *
 *        Version:  1.0
 *        Created:  10/04/2014 02:26:02 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef TYPELIST_LIB0J9WJ
#define TYPELIST_LIB0J9WJ

#include <type_traits>

#include <templatious/util/Selectors.hpp>

namespace templatious {

struct NullType;

template <class T,class U>
struct ComparatorSame {
    static const bool value =
        std::is_same<T,U>::value;
};

template <class... T>
struct TypeList;

// for ByIndex function
template <class List,int sel>
struct IndexSelector {
    typedef typename List::Tail::template ByIndex<sel-1>::type type;
};

template <class List>
struct IndexSelector<List,0> {
    typedef typename List::Head type;
};

template <class A,class... T>
struct TypeList<A,T...> {
    typedef A Head;
    typedef TypeList<T...> Tail;
    static const int size = sizeof...(T) + 1;

    typedef TypeList<A,T...> ThisList;

    template <
        class U,
        template <class,class> class Comparator = ComparatorSame
    >
    struct Contains {
        static const bool thisEquals = Comparator<A,U>::value;
        static const bool value = thisEquals
            || Tail::template Contains<U,Comparator>::value;
    };

    template <int i>
    struct ByIndex {
        static_assert(i >= 0,"Index has to be non negative.");

        typedef typename IndexSelector<ThisList,i>::type type;
    };
};

template <class A>
struct TypeList<A> {
    static const int size = 1;
    typedef A Head;
    typedef NullType Tail;

    typedef TypeList<A> ThisList;

    template <
        class U,
        template <class,class> class Comparator = ComparatorSame
    >
    struct Contains {
        static const bool thisEquals = Comparator<A,U>::value;
        static const bool value = thisEquals;
    };

    template <int i>
    struct ByIndex {
        static_assert(i >= 0,"Index has to be non negative.");

        static const bool moreThanOne = i > 0;

        typedef typename std::conditional<
            moreThanOne, NullType, Head >::type type;
    };
};

namespace detail {

template <
    class A,class B,
    template <class,class> class Comparator = ComparatorSame
>
struct TypelistsEqual : std::false_type {
    static_assert( templatious::util::DummyResolver<A,false>::val,
            "Only typelists can be passed to typelist comparison structure." );
};


template <
    template <class,class> class Comparator,
    class... A,class... B
>
struct TypelistsEqual<
    TypeList<A...>,TypeList<B...>,
    Comparator
>
{
    typedef TypeList<A...> AList;
    typedef TypeList<B...> BList;

    static const bool sizesEqual =
        sizeof...(A) == sizeof...(B);

    static const bool aNull =
        std::is_same<NullType,typename AList::Tail>::value;

    static const bool bNull =
        std::is_same<NullType,typename BList::Tail>::value;

    static const bool nullTailMismatch =
        aNull != bNull;

    static const bool terminated =
        aNull && bNull;

    static const bool currentEqual =
        Comparator<typename AList::Head,typename BList::Head>::value;

    struct FalseResult : std::false_type {};
    struct TrueResult : std::true_type {};

    struct Iterate {
        static const bool value = TypelistsEqual<
            typename AList::Tail,
            typename BList::Tail,
            Comparator
        >::value;
    };

    typedef typename std::conditional<
        !sizesEqual || nullTailMismatch || !currentEqual,
        FalseResult,
        typename std::conditional<
            terminated,
            TrueResult,
            Iterate
        >::type
    >::type ResultResolver;

    static const bool value = ResultResolver::value;

};

// Check if A contains B
template <
    class A,class B,
    template <class,class> class Comparator = ComparatorSame
>
struct TypelistContains : std::false_type {
    static_assert( templatious::util::DummyResolver<A,false>::val,
            "Only typelists can be passed to typelist comparison structure." );
};

template <
    template <class,class> class Comparator,
    class... A,class... B
>
struct TypelistContains<
    TypeList<A...>,TypeList<B...>,
    Comparator
>
{
    typedef TypeList<A...> AList;
    typedef TypeList<B...> BList;

    static const bool aNull =
        std::is_same<NullType,typename AList::Tail>::value;

    static const bool bNull =
        std::is_same<NullType,typename BList::Tail>::value;

    static const bool terminated = bNull;

    static const bool bLarger = aNull && !bNull;

    static const bool currentEqual =
        !bLarger &&
        Comparator<typename AList::Head,typename BList::Head>::value;

    struct FalseResult : std::false_type {};
    struct TrueResult : std::true_type {};

    struct Iterate {
        static const bool value = TypelistContains<
            typename AList::Tail,
            typename BList::Tail,
            Comparator
        >::value;
    };

    typedef typename std::conditional<
        !currentEqual,
        FalseResult,
        typename std::conditional<
            terminated,
            TrueResult,
            Iterate
        >::type
    >::type ResultResolver;

    static const bool value = ResultResolver::value;

};

}
}

#endif /* end of include guard: TYPELIST_LIB0J9WJ */
