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

    template <int i,int gr,int target,class Func,class U>
    struct Decider {
        typedef typename std::conditional<
                gr == target,
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
               i,gr,target,Func
           >(std::forward<T>(t),std::forward<Args>(args)...))
        {
           return Alg::template callInternal<
               i,gr,target,Func
           >(std::forward<T>(t),std::forward<Args>(args)...);
        }
    };

    template <int i,int gr,int target,class Func,class T,class... Args>
    static auto call(T&& t,Args&&... args)
    -> decltype(
           Decider<i + 1,gr,target,Func,T>::
               callInternal(
                   std::forward<T>(t),
                   std::forward<Args>(args)...
               )
       )
    {
        typedef Decider<i + 1,gr,target,Func,T> Dec;
        return Dec::callInternal(
                std::forward<T>(t),
                std::forward<Args>(args)...);
    }

public:
    typedef UniGroupCaller<Delim> ThisCaller;

    template <int target,class Func,class... Args>
    static auto firstCall(Args&&... args)
    -> decltype(
        call<0,0,target,Func>(
            std::forward<Args>(args)...,
            CustomDelimiter()
        )
    )
    {
        return call<0,0,target,Func>(
            std::forward<Args>(args)...,
            CustomDelimiter()
        );
    }

private:
    struct Include {
        template <int i,int gr,int target,class Func,class Loner,class... Args>
        static auto callInternal(Loner&& l,Args&&... args)
        -> decltype(
            ThisCaller::template call<i,gr,target,Func>(
                std::forward<Args>(args)...,
                std::forward<Loner>(l)
            )
        )
        {
            return ThisCaller::template call<i,gr,target,Func>(
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
