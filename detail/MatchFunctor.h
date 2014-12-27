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

template <
    class T,class Func,
    template <class,class> class ComparisonPolicy,
    template <class,class,
       template <class,class> class
    > class TypelistComparisonPolicy,
    class... Tail,
    template <class> class StoragePolicy,
    template <class> class MatchStoragePolicy
>
struct MatchFunctor<
    StoragePolicy,
    Match<
        T,Func,
        MatchStoragePolicy,
        ComparisonPolicy,
        TypelistComparisonPolicy
    >,
    Tail...
>
{
    typedef Match<T,Func,
        MatchStoragePolicy,
        ComparisonPolicy,
        TypelistComparisonPolicy> ThisMatch;
    typedef MatchFunctor<StoragePolicy,Tail...> TailMatch;
    typedef typename StoragePolicy<ThisMatch>::Container Container;

    template <class... TailArgs>
    MatchFunctor(const ThisMatch& m,TailArgs&&... args) :
        _m(m), _t(std::forward<TailArgs>(args)...) {}

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
        template <class... Args>
        static auto call(ThisMatch& m,TailMatch& tm,Args&&... args)
            -> decltype(m(std::forward<Args>(args)...))
        {
            return m(std::forward<Args>(args)...);
        }
    };

    struct TailCall {
        template <class... Args>
        static auto call(ThisMatch& m,TailMatch& tm,Args&&... args)
            -> decltype(tm(std::forward<Args>(args)...))
        {
            return tm(std::forward<Args>(args)...);
        }
    };

    template <class... Args>
    struct Resolver {
        typedef templatious::TypeList<Args...> TheList;

        typedef typename std::conditional<
            ThisMatch::template DoesMatch< TheList >::value,
            ThisCall,
            TailCall
        >::type Call;

        static auto call(ThisMatch& m,TailMatch& tm,Args&&... args)
            -> decltype(Call::call(m,tm,std::forward<Args>(args)...))
        {
            return Call::call(m,tm,std::forward<Args>(args)...);
        }
    };

private:
    Container _m;
    TailMatch _t;
};

template <
    class T,class Func,
    template <class,class> class ComparisonPolicy,
    template <class,class,
       template <class,class> class
    > class TypelistComparisonPolicy,
    template <class> class StoragePolicy,
    template <class> class MatchStoragePolicy
>
struct MatchFunctor< StoragePolicy, Match<T,Func,
    MatchStoragePolicy, ComparisonPolicy,
    TypelistComparisonPolicy>
>
{
    typedef Match<T,Func,
        MatchStoragePolicy,
        ComparisonPolicy,
        TypelistComparisonPolicy> ThisMatch;
    typedef typename StoragePolicy<ThisMatch>::Container Container;

    MatchFunctor(const ThisMatch& m) :
        _m(m) {}

    // have to reroute request to trigger static_assert
    template <class... Args>
    struct ThisCall {
        typedef templatious::TypeList<Args...> Tpl;
        static_assert(ThisMatch::template DoesMatch<Tpl>::value,
                "Last match in the MatchFunctor has to always match.");

        static auto call(ThisMatch& m,Args&&... args)
            -> decltype(m(std::forward<Args>(args)...))
        {
            return m(std::forward<Args>(args)...);
        }
    };

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(ThisCall<Args...>::call(
         std::declval<ThisMatch&>(),
         std::forward<Args>(args)...)
        )
    {
        return ThisCall<Args...>::call(
            _m.getRef(),
            std::forward<Args>(args)...);
    }

private:
    Container _m;
};

}
}

#endif /* end of include guard: MATCHFUNCTOR_2OYV4S30 */
