/*
 * =====================================================================================
 *
 *       Filename:  Match.h
 *
 *    Description:  Match header for matching one typeset
 *
 *        Version:  1.0
 *        Created:  10/04/2014 02:24:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef MATCH_8FK3O73W
#define MATCH_8FK3O73W

#include <type_traits>

#include <templatious/detail/TypeList.h>

namespace templatious {

struct AnyType;

namespace detail {

template <class A,class B>
struct TightComparison {
    static const bool value = std::is_same<A,B>::value;
};

template <class A>
struct TightComparison<A,AnyType> : std::true_type {};

template <class A>
struct TightComparison<AnyType,A> : std::true_type {};

template <class A,class B>
struct LooseComparison {
    typedef typename std::decay<A>::type DecayA;
    typedef typename std::decay<B>::type DecayB;

    static const bool decay_same = std::is_same<
        DecayA, DecayB
    >::value;

    static const bool subtype_of = std::is_base_of<
        DecayA, DecayB
    >::value;

    static const bool value = decay_same || subtype_of;
};

template <class A>
struct LooseComparison<A,AnyType> : std::true_type {};

template <class A>
struct LooseComparison<AnyType,A> : std::true_type {};

template <class T,class U>
struct TightRecursiveComparison {
    static const bool value =
        TightComparison<T,U>::value;
};

template <class... T,class U>
struct TightRecursiveComparison<
    U,templatious::TypeList<T...>
>
{
    typedef templatious::TypeList<T...> Inner;
    static const bool value =
        Inner::template Contains<
            U, TightRecursiveComparison
        >::value;
};

// reverse
template <class... T,class U>
struct TightRecursiveComparison<
    templatious::TypeList<T...>,U
>
{
    static const bool value =
        TightRecursiveComparison<
            U,
            templatious::TypeList<T...>
        >::value;
};

template <class T,class U>
struct LooseRecursiveComparison {
    static const bool value =
        LooseComparison<T,U>::value;
};

template <class... T,class U>
struct LooseRecursiveComparison<
    U,templatious::TypeList<T...>
>
{
    typedef templatious::TypeList<T...> Inner;
    static const bool value =
        Inner::template Contains<
            U, LooseRecursiveComparison
        >::value;
};

// reverse
template <class... T,class U>
struct LooseRecursiveComparison<
    templatious::TypeList<T...>,U
>
{
    static const bool value =
        LooseRecursiveComparison<
            U,
            templatious::TypeList<T...>
        >::value;
};

template <class T,class Func,
    template <class> class StoragePolicy,
    template <class,class> class ComparisonPolicy = TightComparison,
    template <class,class,
        template <class,class> class
    > class TypelistComparisonPolicy = templatious::detail::TypelistsEqual
>
struct Match {
    static_assert( templatious::util::DummyResolver<T,false>::val,
            "Match can be only used with typelists." );
};

template <class... Args,class Func,
    template <class> class StoragePolicy,
    template <class,class> class ComparisonPolicy,
    template <class,class,
        template <class,class> class
    > class TypelistComparisonPolicy
>
struct Match<
    templatious::TypeList<Args...>,
    Func,
    StoragePolicy,
    ComparisonPolicy,
    TypelistComparisonPolicy
>
{
    typedef StoragePolicy<Func> RefMaker;

    typedef typename RefMaker::Container Container;

    typedef templatious::TypeList<Args...> MatchList;

    template <class V>
    Match(V&& v) :
        _c(RefMaker::make(std::forward<V>(v))) {}

    template <class U>
    struct DoesMatch {
        static const bool value =
            TypelistComparisonPolicy<
                MatchList, U, ComparisonPolicy
            >::value;
    };

    template <class... FArgs>
    auto operator()(FArgs&&... args)
     -> decltype(
            std::declval<Container>().getRef()(
                std::forward<FArgs>(args)...
            )
        )
    {
        return _c.getRef()(std::forward<FArgs>(args)...);
    }

private:
    Container _c;
};


}
}

#endif /* end of include guard: MATCH_8FK3O73W */
