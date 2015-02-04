/*
 * =====================================================================================
 *
 *       Filename:  ArgumentDelimiter.h
 *
 *    Description:  Divide arguments by delimiters and call groups
 *
 *        Version:  1.0
 *        Created:  10/08/2014 08:26:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ARGUMENTDELIMITER_1WLMJP97
#define ARGUMENTDELIMITER_1WLMJP97

#include <utility>

#include <templatious/util/Selectors.h>

namespace templatious {
namespace util {

template <class Delim>
struct UniGroupCaller {
private:
    struct CustomDelimiter {};

    struct Include;
    struct Exclude;
    struct Terminate;
    struct Advance;

    // small utility for checking if int list
    // contains target
    template <int i,int... target>
    struct Contains;

    template <int i,int... target>
    struct Contains<i,i,target...> {
        static const bool result = true;
    };

    template <int i,int j,int... target>
    struct Contains<i,j,target...> {
        static const bool result =
            Contains<i,target...>::result;
    };

    template <int i,int j>
    struct Contains<i,j> {
        static const bool result = false;
    };

    template <int i>
    struct Contains<i,i> {
        static const bool result = true;
    };
    // end

    template <int i,int gr,class Func,class U,int... target>
    struct Decider {
        typedef typename std::conditional<
                Contains<gr,target...>::result,
                Include,
                Exclude
        >::type IncOrExc;

        typedef typename std::conditional<
            std::is_same<
                typename std::decay<U>::type,
                typename std::decay<Delim>::type
            >::value,
            Advance,
            IncOrExc
        >::type AdvOrInExc;

        typedef typename std::conditional<
            std::is_same< typename std::decay<U>::type, CustomDelimiter >::value,
            Terminate,
            AdvOrInExc
        >::type Alg;

        template <class T,class... Args>
        static auto callInternal(T&& t,Args&&... args)
        -> decltype( Alg::template callInternal<
               i,gr,Func,target...
           >(std::forward<T>(t),std::forward<Args>(args)...))
        {
           return Alg::template callInternal<
               i,gr,Func,target...
           >(std::forward<T>(t),std::forward<Args>(args)...);
        }
    };

    template <int i,int gr,class Func,int... target,class T,class... Args>
    static auto call(T&& t,Args&&... args)
    -> decltype(
           Decider<i + 1,gr,Func,T,target...>::
               callInternal(
                   std::forward<T>(t),
                   std::forward<Args>(args)...
               )
       )
    {
        typedef Decider<i + 1,gr,Func,T,target...> Dec;
        return Dec::callInternal(
                std::forward<T>(t),
                std::forward<Args>(args)...);
    }

public:
    typedef UniGroupCaller<Delim> ThisCaller;

    template <class Func,int... target,class... Args>
    static auto firstCall(Args&&... args)
    -> decltype(
        call<0,0,Func,target...>(
            std::forward<Args>(args)...,
            CustomDelimiter()
        )
    )
    {
        return call<0,0,Func,target...>(
            std::forward<Args>(args)...,
            CustomDelimiter()
        );
    }

private:
    struct Include {
        template <int i,int gr,class Func,int... target,class Loner,class... Args>
        static auto callInternal(Loner&& l,Args&&... args)
        -> decltype(
            ThisCaller::template call<i,gr,Func,target...>(
                std::forward<Args>(args)...,
                std::forward<Loner>(l)
            )
        )
        {
            return ThisCaller::template call<i,gr,Func,target...>(
                    std::forward<Args>(args)...,
                    std::forward<Loner>(l));
        }
    };

    struct Exclude {
        template <int i,int gr,int target,class Func,class Loner,class... Args>
        static auto callInternal(Loner&& l,Args&&... args)
        -> decltype(
            ThisCaller::template call<i,gr,target,Func>(
                std::forward<Args>(args)...
            )
        )
        {
            return call<i,gr,target,Func>(
                    std::forward<Args>(args)...);
        }
    };

    struct Terminate {
        template <int i,int gr,int target,class Func,class Loner,class... Args>
        static auto callInternal(Loner&& l,Args&&... args)
        -> decltype(
            Func::call(std::forward<Args>(args)...)
        )
        {
            static_assert(sizeof...(Args) > 0,"Group has zero members.");
            return Func::call(std::forward<Args>(args)...);
        }

        template <int i,int gr,int target,class Func,class Loner>
        static auto callInternal(Loner&& l)
        -> void
        {
            static_assert(templatious::util::DummyResolver<Loner,false>::val,
                    "Group is empty and cannot be called.");
        }
    };

    struct Advance {
        template <int i,int gr,int target,class Func,class Loner,class... Args>
        static auto callInternal(Loner&& l,Args&&... args)
        -> decltype(
            ThisCaller::template call<i,gr + 1,target,Func>(
                std::forward<Args>(args)...
            )
        )
        {
            return call<i,gr + 1,target,Func>(
                    std::forward<Args>(args)...);
        }
    };

};

template <class Delimiter,class Func,int... gr,class... Args>
auto callGroup(Args&&... args)
 -> decltype (
    UniGroupCaller<Delimiter>::template firstCall<gr...,Func>(
        std::forward<Args>(args)...
    )
 )
{
    typedef UniGroupCaller<Delimiter> Caller;
    return Caller::template firstCall<gr...,Func>(
        std::forward<Args>(args)...
    );
}

}
}

#endif /* end of include guard: ARGUMENTDELIMITER_1WLMJP97 */
