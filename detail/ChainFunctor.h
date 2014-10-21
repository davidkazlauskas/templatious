/*
 * =====================================================================================
 *
 *       Filename:  ChainFunctor.h
 *
 *    Description:  Chain functor for multiple functor chaining
 *
 *        Version:  1.0
 *        Created:  10/21/2014 06:59:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef CHAINFUNCTOR_JQWE6P11
#define CHAINFUNCTOR_JQWE6P11

#include <utility>

namespace templatious {
namespace detail {

template <
    template <class> class StoragePolicy,
    class... Args
>
struct ChainFunctor;

template <
    template <class> class StoragePolicy,
    class A,class... Tl
>
struct ChainFunctor<StoragePolicy,A,Tl...> {
    typedef typename StoragePolicy<A>::Container Container;
    typedef ChainFunctor<StoragePolicy,Tl...> Tail;

    template <class T,class... Args>
    ChainFunctor(T&& t,Args&&... args)
     : _c(t), _t(std::forward<Args>(args)...) {}

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(
         std::declval<Tail>()(
             std::declval<Container>().getRef()(
                 std::forward<Args>(args)...
             )
         )
     )
    {
        return _t(_c.getRef()(std::forward<Args>(args)...));
    }

private:
    Container _c;
    Tail _t;
};

template <
    template <class> class StoragePolicy,
    class A
>
struct ChainFunctor<StoragePolicy,A> {
    typedef typename StoragePolicy<A>::Container Container;

    template <class T,class... Args>
    ChainFunctor(T&& t)
     : _c(t) {}

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(
         std::declval<Container>().getRef()(
             std::forward<Args>(args)...
         )
     )
    {
        return _c.getRef()(std::forward<Args>(args)...);
    }

private:
    Container _c;
};

}
}

#endif /* end of include guard: CHAINFUNCTOR_JQWE6P11 */
