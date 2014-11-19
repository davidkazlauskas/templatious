/*
 * =====================================================================================
 *
 *       Filename:  Compterator.h
 *
 *    Description:  Composite iterator container for few collections
 *
 *        Version:  1.0
 *        Created:  09/22/2014 05:19:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef COMPTERATOR_64DY8BKD
#define COMPTERATOR_64DY8BKD

#include <templatious/util/Selectors.h>

#include <templatious/CollectionAdapter.h>
#include <templatious/Pack.h>

namespace templatious {
namespace util {

template <class T>
struct IteratorPair {
    IteratorPair(const IteratorPair& c)
     : _a(c._a), _b(c._b) {}

    IteratorPair(T a,T b) : _a(a), _b(b) {}

    T _a;
    T _b;
};

template <class>
struct IsIteratorPair : std::false_type {};

template <class T>
struct IsIteratorPair< IteratorPair<T> > : std::true_type {};

template <class... T>
struct Compterator;

template <class T,class... Tail>
struct Compterator< T,Tail... > {
    typedef Compterator< T,Tail... > ThisComp;
    typedef Compterator<Tail...> TailComp;
    typedef T Container;

    static const bool last = false;

    Compterator(Container p,Tail... tail)
     : _p(p), _t(tail...) {}

    Container _p;
    TailComp _t;
};

template <class T>
struct Compterator<T> {
    typedef Compterator<T> ThisComp;
    typedef T Container;

    static const bool last = true;

    Compterator(Container p) : _p(p) {}

    Container _p;
};

struct AdapterPairMaker {

    template <class T>
    static auto make(T&& t)
     -> IteratorPair<
     typename templatious::adapters::CollectionAdapter<T>::Iterator >
    {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::Iterator Iter;
        return IteratorPair<Iter>(
                Ad::begin(std::forward<T>(t)),
                Ad::end(std::forward<T>(t)) );
    }

};

struct ValCopyMaker {

    template <class T>
    static auto make(T&& t)
     -> T
    {
        return T(std::forward<T>(t));
    }

};

template <class T>
auto begEndPair(T&& t)
 -> typename std::conditional<
        templatious::adapters::CollectionAdapter<T>::is_valid,
        IteratorPair< typename templatious::adapters::CollectionAdapter<T>::Iterator >,
        T
    >::type
{
    typedef templatious::adapters::CollectionAdapter<T> Ad;

    typedef typename std::conditional< Ad::is_valid,
            AdapterPairMaker,
            ValCopyMaker
        >::type Maker;
    return Maker::make(std::forward<T>(t));
}


template <class... T>
auto makeCompteratorBegEnd(T&&... t)
 -> Compterator< decltype( begEndPair(std::forward<T>(t)) )... >
{
    typedef Compterator< decltype( begEndPair(std::forward<T>(t)) )... >
        RetType;
    return RetType( begEndPair(std::forward<T>(t))... );
}

namespace detail {

    struct TerminateStepper {
        template<int i,class C,class A,class... T>
        static auto forward(C&& c,A&& a,T&&... t)
         -> decltype( makeCompteratorBegEnd( std::forward<T>(t)... ) )
        {
            return makeCompteratorBegEnd( std::forward<T>(t)... );
        }
    };

    struct ValGetAlg {
        template <int i,class T>
        static auto get(T&& t)
         -> T
        {
            return T(std::forward<T>(t));
        }
    };

    struct IterPairGetAlg {
        template <int i,class T>
        static auto get(T&& t)
         -> decltype( templatious::util::begEndPair(std::forward<T>(t)) )
        {
            return templatious::util::begEndPair(std::forward<T>(t));
        }
    };

    struct PackGetAlg {
        template <int i,class T>
        static auto get(T&& t)
         -> decltype( t.template get<i>() )
        {
            return t.template get<i>();
        }
    };

    struct HaltType {};
    struct FwdStepper;
    struct SameStepper;
    struct TerminateStepper;

    template <int i,class C,class... T>
    struct FwdResolver;

    template <int i,class C,class... T>
    static auto compteratorFwd(C&& c,T&&... t)
     -> decltype(
         FwdResolver<i,C,T...>::dispatch(
             std::forward<C>(c),
             std::forward<T>(t)...
         )
     )
    {
        typedef FwdResolver<i,C,T...> Res;
        return Res::dispatch(
            std::forward<C>(c),
            std::forward<T>(t)...
        );
    }

    template <int i,class C,class... T>
    struct FwdResolver {
        typedef templatious::detail::IsPack<C> IP;
        typedef templatious::adapters::CollectionAdapter<C> Ad;

        static const bool terminate = std::is_same<
            HaltType, typename std::decay<C>::type >::value;
        static const bool isP = IP::val;
        static const bool isC = Ad::is_valid;
        static const int sz = IP::size;

        static const int thisI = (i == -1 && isP ? 0 : i);
        static const bool isNotLast = thisI < sz - 1;
        static const bool isLast = !isNotLast;
        static const int passI = (isP && isNotLast? thisI + 1 : -1);
        static const bool moveForward = isLast || !isP;

        typedef typename std::conditional<
                isP,
                PackGetAlg,
                typename std::conditional<
                    isC,
                    IterPairGetAlg,
                    ValGetAlg
                >::type
            >::type GetType;

        typedef typename std::conditional<
                terminate,
                TerminateStepper,
                typename std::conditional<
                    moveForward,
                    FwdStepper,
                    SameStepper
                >::type
            >::type Stepper;

        template <class Cn,class... Tn>
        static auto dispatch(Cn&& c,Tn&&... t)
         -> decltype(
                Stepper::template forward<passI>(std::forward<Cn>(c),
                    GetType::template get<thisI>(std::forward<Cn>(c)),
                    std::forward<Tn>(t)...)
            )
        {
            return Stepper::template forward<passI>(std::forward<Cn>(c),
                    GetType::template get<thisI>(std::forward<Cn>(c)),
                    std::forward<Tn>(t)...);
        }
    };

    struct FwdStepper {
        template<int i,class C,class A,class... T>
        static auto forward(C&& c,A&& a,T&&... t)
         -> decltype( compteratorFwd<i>(std::forward<T>(t)...,
                     std::forward<A>(a)) )
        {
            return compteratorFwd<i>(std::forward<T>(t)...,
                    std::forward<A>(a));
        }
    };

    struct SameStepper {
        template<int i,class C,class A,class... T>
        static auto forward(C&& c,A&& a,T&&... t)
         -> decltype( compteratorFwd<i>(
                 std::forward<C>(c),
                 std::forward<T>(t)...,
                 std::forward<A>(a)) )
        {
            return compteratorFwd<i>(
                std::forward<C>(c),
                std::forward<T>(t)...,
                std::forward<A>(a));
        }
    };

    template <class... T>
    static auto makeCompteratorV2(T&&... t)
     -> decltype( compteratorFwd<-1>(std::forward<T>(t)..., HaltType()) )
    {
        return compteratorFwd<-1>(std::forward<T>(t)..., HaltType());
    }

}

}
}


#endif /* end of include guard: COMPTERATOR_64DY8BKD */
