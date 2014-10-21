/*
 * =====================================================================================
 *
 *       Filename:  ArgumentMultiplier.h
 *
 *    Description:  Multiply arguments to do something
 *
 *        Version:  1.0
 *        Created:  10/19/2014 09:31:37 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ARGUMENTMULTIPLIER_A41PV7AY
#define ARGUMENTMULTIPLIER_A41PV7AY

#include <utility>

#include <templatious/util/Variadic.h>

namespace templatious {
namespace util {

// sz - passed group size
// int i - iteratation - backwards
// int j - smaller iteration - backwards
template <class Action,int sz,int i,int j>
struct ArgumentMultiplier;
template <class Action,int times,class... CArgs>
struct HighLevelArgumentMultiplier;

template <class Action,int times,class... Args>
auto multiplyArgs(Args&&... args)
 -> decltype(
     HighLevelArgumentMultiplier<Action,times,Args...>::
        call(std::forward<Args>(args)...)
 )
{
    return HighLevelArgumentMultiplier<Action,times,Args...>::
        call(std::forward<Args>(args)...);
}

template <class Action,int times,class... CArgs>
struct HighLevelArgumentMultiplier {
    static_assert(times >= 0,"Times multiplied cannot be negative.");

    static const int aSz = sizeof...(CArgs);
    static_assert(times * aSz < 77,"For sanity reasons total size of arguments created "
            "must be less than 77. Do you really need that many arguments created? "
            "If yes, just remove this static assert.");

    template <class... SArgs>
    struct GetAsIs {
        template <class... Args>
        static auto call(Args&&... args)
         -> decltype( Action::call(std::forward<Args>(args)...) )
        {
            return Action::call(std::forward<Args>(args)...);
        }
    };

    template <class... SArgs>
    struct GetNone {
        template <class... Args>
        static auto call(Args&&... args)
         -> decltype( Action::call() )
        {
            return Action::call();
        }
    };

    template <class... SArgs>
    struct GetMultiply {
        static const int sz = sizeof...(SArgs);
        typedef ArgumentMultiplier<
            Action, sz, times - 2, sz - 1
        > Starter;

        template <class... Args>
        static auto call(Args&&... args)
         -> decltype( Starter::call(std::forward<Args>(args)...) )
        {
            return Starter::call(std::forward<Args>(args)...);
        }
    };

    typedef typename std::conditional<
        times == 1,
        GetAsIs<CArgs...>,
        typename std::conditional<
            times <= 0,
            GetNone<CArgs...>,
            GetMultiply<CArgs...>
        >::type
    >::type Decision;

    template <class... Args>
    static auto call(Args&&... args)
    -> decltype( Decision::call(std::forward<Args>(args)...) )
    {
        return Decision::call(std::forward<Args>(args)...);
    }

};

template <class Action,int sz,int i,int j>
struct ArgumentMultiplier {

    struct CallInternal {
        template <class Caller,int jj,class... Args>
        static auto call(Args&&... args)
         -> decltype(
             Caller::call(
                 std::forward<Args>(args)...,
                 templatious::util::getNth<jj>(
                     std::forward<Args>(args)...
                 )
             )
         )
        {
            return Caller::call(
                 std::forward<Args>(args)...,
                 templatious::util::getNth<jj>(
                     std::forward<Args>(args)...
                 ));
        }
    };

    struct NextLoop {
        typedef ArgumentMultiplier<Action,sz,i - 1,sz - 1>
            NextIter;

        static const int transIdx = sz - j - 1;

        template <class... Args>
        static auto call(Args&&... args)
         -> decltype(
             CallInternal::template call<NextIter,transIdx>(
                 std::forward<Args>(args)...)
         )
        {
            return CallInternal::template call<NextIter,transIdx>(
                    std::forward<Args>(args)...);
        }
    };

    struct SameLoop {
        typedef ArgumentMultiplier<Action,sz,i,j - 1>
            NextIter;

        static const int transIdx = sz - j - 1;

        template <class... Args>
        static auto call(Args&&... args)
         -> decltype(
             NextIter::call(
                 std::forward<Args>(args)...,
                 templatious::util::getNth<transIdx>(
                     std::forward<Args>(args)...
                 ))
         )
        {
            return NextIter::call(
                std::forward<Args>(args)...,
                templatious::util::getNth<transIdx>(
                    std::forward<Args>(args)...
                ));
        }
    };

    typedef typename std::conditional<
        j == 0,
        NextLoop,
        SameLoop
    >::type Decision;

    template <class... Args>
    static auto call(Args&&... args)
     -> decltype(
         Decision::call(
             std::forward<Args>(args)...)
     )
    {
        return Decision::call(
            std::forward<Args>(args)...
        );
    }

};

// final iteration
template <class Action,int sz>
struct ArgumentMultiplier<Action,sz,0,0> {
    template <class... Args>
    static auto call(Args&&... args)
     -> decltype(
            Action::call(std::forward<Args>(args)...,
            templatious::util::getNth<sz - 1>(
                std::forward<Args>(args)...
            )
        )
     )
    {
        return Action::call(
            std::forward<Args>(args)...,
            templatious::util::getNth<sz - 1>(
                std::forward<Args>(args)...
            )
        );
    }
};

}
}

#endif /* end of include guard: ARGUMENTMULTIPLIER_A41PV7AY */
