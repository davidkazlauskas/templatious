/*
 * =====================================================================================
 *
 *       Filename:  MatchFunctor.h
 *
 *    Description:  Match functor
 *
 *        Version:  1.0
 *        Created:  10/06/2014 05:39:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef MATCHFUNCTOR_2OYV4S30
#define MATCHFUNCTOR_2OYV4S30

#include <templatious/util/Selectors.h>
#include <templatious/util/Variadic.h>
#include <templatious/detail/TypeList.h>

#include <templatious/detail/Match.h>

namespace templatious {
namespace detail {

template <
    template <class> class StoragePolicy,
    class... T
>
struct MatchFunctor {

    typedef typename templatious::util::GetFrist<T...>::type First;

    static_assert(
            templatious::util::DummyResolver<First,false>::val,
            "MatchFunctor can only be used with Match structures.");

};

template <class M,class... Args>
struct DoesMatchExt {

    typedef typename templatious::util::GetFrist<Args...>::type First;
    static_assert(
            templatious::util::DummyResolver<First,false>::val,
            "MatchFunctor can only be used with Match structures.");

    static const bool value = false;
};

template <
    class T,class Func,
    template <class,class> class ComparisonPolicy,
    template <class,class,
       template <class,class> class
    > class TypelistComparisonPolicy,
    template <class> class MatchStoragePolicy,
    class... Args
>
struct DoesMatchExt<
    Match<
        T,Func,
        MatchStoragePolicy,
        ComparisonPolicy,
        TypelistComparisonPolicy
    >,
    Args...
>
{
    typedef Match<
        T,Func,
        MatchStoragePolicy,
        ComparisonPolicy,
        TypelistComparisonPolicy
    > ThisMatch;

    typedef templatious::TypeList<Args...> TheList;

    static const bool value =
        ThisMatch::template DoesMatch< TheList >::value;
};

template <
    template <class> class StoragePolicy,
    class... T,
    class... Args
>
struct DoesMatchExt<
    MatchFunctor< StoragePolicy, T... >,
    Args...
>
{
    typedef MatchFunctor< StoragePolicy, T... > ThisMatch;

    static const bool value =
        ThisMatch::template DoesMatch< Args... >::value;
};

template <
    class T,
    class... Tail,
    template <class> class StoragePolicy
>
struct MatchFunctor<
    StoragePolicy,
    T,
    Tail...
>
{
    typedef T ThisMatch;
    typedef MatchFunctor<StoragePolicy,Tail...> TailMatch;
    typedef typename StoragePolicy<ThisMatch>::Container Container;

    typedef typename std::decay<ThisMatch>::type DThisMatch;
    typedef typename std::decay<TailMatch>::type DTailMatch;

    template <class M,class... TailArgs>
    MatchFunctor(M&& m,TailArgs&&... args) :
        _m(std::forward<M>(m)), _t(std::forward<TailArgs>(args)...) {}

    template <class... Args>
    struct Resolver;

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(Resolver<Args...>::call(
            std::declval<ThisMatch&>(),
            std::declval<TailMatch&>(),
            std::forward<Args>(args)...)
        )
    {
        return Resolver<Args...>::call(
                _m.getRef(),_t,
                std::forward<Args>(args)...);
    }

    struct ThisCall {
        template <class A,class B,class... Args>
        static auto call(A&& m,B&& tm,Args&&... args)
            -> decltype(m(std::forward<Args>(args)...))
        {
            return m(std::forward<Args>(args)...);
        }
    };

    struct TailCall {
        template <class A,class B,class... Args>
        static auto call(A&& m,B&& tm,Args&&... args)
            -> decltype(tm(std::forward<Args>(args)...))
        {
            return tm(std::forward<Args>(args)...);
        }
    };

    template <class... Args>
    struct Resolver {
        typedef typename std::conditional<
            DoesMatchExt< DThisMatch, Args... >::value,
            ThisCall,
            TailCall
        >::type Call;

        template <class A,class B>
        static auto call(A&& m,B&& tm,Args&&... args)
            -> decltype(Call::call(
                std::forward<A>(m),
                std::forward<B>(tm),
                std::forward<Args>(args)...))
        {
            return Call::call(
                std::forward<A>(m),
                std::forward<B>(tm),
                std::forward<Args>(args)...);
        }
    };

    template <class... Args>
    struct DoesMatch {
        static const bool thisMatches =
            DoesMatchExt< DThisMatch, Args... >::value;

        static const bool tailMatches =
            DoesMatchExt< DTailMatch, Args... >::value;

        static const bool value = thisMatches || tailMatches;
    };

private:
    Container _m;
    TailMatch _t;
};

template <
    class T,
    template <class> class StoragePolicy
>
struct MatchFunctor< StoragePolicy, T >
{
    typedef T ThisMatch;
    typedef typename StoragePolicy<ThisMatch>::Container Container;

    typedef typename std::decay<ThisMatch>::type DThisMatch;

    template <class M>
    MatchFunctor(M&& m) :
        _m(std::forward<M>(m)) {}

    // have to reroute request to trigger static_assert
    template <class... Args>
    struct ThisCall {
        static const bool matches =
            DoesMatchExt< DThisMatch, Args... >::value;
        static_assert(matches,
            "Last match in the MatchFunctor has to always match.");

        template <class M>
        static auto call(M&& m,Args&&... args)
            -> decltype(m(std::forward<Args>(args)...))
        {
            return m(std::forward<Args>(args)...);
        }
    };

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(ThisCall<
             decltype(std::forward<Args>(args))...
        >::call(
         std::declval<ThisMatch&>(),
         std::forward<Args>(args)...)
        )
    {
        return ThisCall<
            decltype(std::forward<Args>(args))...
        >::call(
            _m.getRef(),
            std::forward<Args>(args)...);
    }

    template <class... Args>
    struct DoesMatch {
        static const bool thisMatches =
            DoesMatchExt< DThisMatch, Args... >::value;

        static const bool value = thisMatches;
    };

private:
    Container _m;
};

}
}

#endif /* end of include guard: MATCHFUNCTOR_2OYV4S30 */
