//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  ChainFunctor.hpp
 *
 *    Description:  Chain functor for multiple functor chaining
 *
 *        Version:  1.0
 *        Created:  10/21/2014 06:59:40 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef CHAINFUNCTOR_JQWE6P11
#define CHAINFUNCTOR_JQWE6P11

#include <utility>

#include <templatious/util/Selectors.hpp>
#include <templatious/util/DefaultStoragePolicy.hpp>

namespace templatious {
namespace detail {

/**
 * The main purpose of this class is to encapsulate
 * specific actions in the specified order
 * (possibly with their undoable actions)
 */
template <
    bool statefulDefault = false,
    bool reversed = false,
    template <class> class StoragePolicy =
        templatious::util::DefaultStoragePolicy,
    class... Args
>
struct ChainFunctor;

/**
 * Encapsulates do and undo actions.
 */
template <
    template <class> class StoragePolicy,
    bool hasUndo,
    class A,class B
>
struct FunctorPair {
    typedef typename StoragePolicy<A>::Container ContainerA;
    typedef typename StoragePolicy<B>::Container ContainerB;
    typedef FunctorPair< StoragePolicy, hasUndo, A, B > ThisPair;

    static const bool undo = hasUndo;

    template <class AC,class BC>
    FunctorPair(AC&& ac,BC&& bc):
        _a(std::forward<AC>(ac)),
        _b(std::forward<BC>(bc)) {}

    template <class... Args>
    auto callDo(Args&&... args)
     -> decltype(
         std::declval<ContainerA>().getRef()(
             std::forward<Args>(args)...)
     )
    {
        return _a.getRef()(
            std::forward<Args>(args)...);
    }

    template <class... Args>
    auto callUndo(Args&&... args)
     -> decltype(
         std::declval<ContainerB>().getRef()(
             std::forward<Args>(args)...)
     )
    {
        return _b.getRef()(
            std::forward<Args>(args)...);
    }

    auto getDo() const
     -> FunctorPair< StoragePolicy, false, A, void >
    {
        return FunctorPair<
            StoragePolicy, false, A, void
        >(_a.cpy());
    }

    auto getUndo() const
     -> FunctorPair< StoragePolicy, false, B, void >
    {
        return FunctorPair<
            StoragePolicy, false, B, void
        >(_b.cpy());
    }

    auto reverse() const
     -> FunctorPair< StoragePolicy, true, B, A >
    {
        return FunctorPair<
            StoragePolicy, true, B, A
        >(_b.cpy(),_a.cpy());
    }

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(
         std::declval<ThisPair>().callDo(
             std::forward<Args>(args)...)
     )
    {
        return callDo(std::forward<Args>(args)...);
    }

private:
    ContainerA _a;
    ContainerB _b;
};

template <
    template <class> class StoragePolicy,
    class A,class B
>
struct FunctorPair<StoragePolicy,false,A,B> {
    typedef typename StoragePolicy<A>::Container ContainerA;

    typedef FunctorPair<StoragePolicy,false,A,B> ThisPair;

    static const bool undo = false;

    template <class AC>
    FunctorPair(AC&& ac):
        _a(std::forward<AC>(ac)) {}

    template <class... Args>
    auto callDo(Args&&... args)
     -> decltype(
         std::declval<ContainerA>().getRef()(
             std::forward<Args>(args)...)
     )
    {
        return _a.getRef()(
            std::forward<Args>(args)...);
    }

    template <class U,class... Args>
    void callUndo(U&& u,Args&&... args)
    {
        static_assert(templatious::util::DummyResolver<U,false>::val,
            "This functor pair doesn't have an undo operation.");
    }

    template <class U>
    void callUndo(U&& u)
    {
        static_assert(templatious::util::DummyResolver<U,false>::val,
            "This functor pair doesn't have an undo operation.");
    }

    template <class U = int>
    void callUndo()
    {
        static_assert(templatious::util::DummyResolver<U,false>::val,
            "This functor pair doesn't have an undo operation.");
    }

    auto getDo() const
     -> FunctorPair< StoragePolicy, false, A, void >
    {
        return FunctorPair<
            StoragePolicy, false, A, void
        >(_a.cpy());
    }

    template <class U = int>
    bool getUndo()
    {
        static_assert(templatious::util::DummyResolver<U,false>::val,
            "This functor pair doesn't have an undo operation.");
        return false;
    }

    template <class U = int>
    bool reverse() const
    {
        static_assert(templatious::util::DummyResolver<U,false>::val,
            "This functor pair doesn't have an undo operation,"
            " therefore, cannot be reversed.");
        return false;
    }

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(
         std::declval<ThisPair>().callDo(
             std::forward<Args>(args)...)
     )
    {
        return callDo(std::forward<Args>(args)...);
    }

private:
    ContainerA _a;
};

template <
    template <class> class StoragePolicy,
    class T
>
struct PairMaker {
    template <class U>
    static auto make(U&& u)
     -> FunctorPair< StoragePolicy, false, U, void >
    {
        return FunctorPair< StoragePolicy, false, U, void >(
            std::forward<U>(u)
        );
    }
};

template <
    template <class> class StoragePolicy,
    bool hasUndo, class A,class B
>
struct PairMaker<
    StoragePolicy,
    FunctorPair< StoragePolicy, hasUndo, A, B >
>
{
    template <class U>
    static auto make(U&& u)
     -> decltype( std::forward<U>(u) )
    {
        return std::forward<U>(u);
    }
};

template <
    template <class> class StoragePolicy,
    class T
>
auto makeFunctorPair(T&& t)
 -> decltype(
     PairMaker<StoragePolicy, T>::make(std::forward<T>(t))
 )
{
    return PairMaker<StoragePolicy, T>::make(std::forward<T>(t));
}

template <class T>
auto unwrapFunctorPairDo(T&& t)
 -> decltype(t.getDo())
{
    return t.getDo();
}

template <class T>
auto unwrapFunctorPairUndo(T&& t)
 -> decltype(t.getUndo())
{
    return t.getUndo();
}

template <class T>
auto unwrapFunctorPairReverse(T&& t)
 -> decltype(t.reverse())
{
    return t.reverse();
}

template <
    bool statefulDefault = false,
    bool reversed = false,
    template <class> class StoragePolicy,
    class... Args
>
auto makeChainFunctor(Args&&... args)
 -> ChainFunctor< statefulDefault, reversed, StoragePolicy, Args... >
{
    return ChainFunctor< statefulDefault, reversed, StoragePolicy, Args... >(
            std::forward<Args>(args)...);
}

struct DoCaller {
    template <class T,class... Args>
    static auto call(T&& f,Args&&... args)
     -> decltype(
         f.callDo(
             std::forward<Args>(args)...
         )
     )
    {
        return f.callDo(
            std::forward<Args>(args)...
        );
    }
};

struct UndoCaller {
    template <class T,class... Args>
    static auto call(T&& f,Args&&... args)
     -> decltype(
         f.callUndo(
             std::forward<Args>(args)...
         )
     )
    {
        return f.callUndo(
            std::forward<Args>(args)...
        );
    }
};

struct UndoUnavailableProcessor {
    template <class T,class... Args>
    static bool process(T&& t,Args&&... args)
    {
        static_assert(
            templatious::util::DummyResolver<T,false>::val,
            "Backwards opeartion is unavailable because "
            "not all functors have undo operation."
        );
        return false;
    }
};

template <class Caller>
struct TailDoProcessor {
    template <class T,class... Args>
    static auto process(T&& t,Args&&... args)
     -> decltype(
         Caller::call(
             t._c.getRef(),
             std::forward<Args>(args)...
         )
     )
    {
        return Caller::call(
            t._c.getRef(),
            std::forward<Args>(args)...
        );
    }
};

template <class Caller>
struct TailDoStatefulProcessor {
    template <class T,class... Args>
    static void process(T&& t,Args&&... args)
    {
        Caller::call(
            t._c.getRef(),
            std::forward<Args>(args)...
        );
    }
};

template <class Caller>
struct FunctionalProcessor {

    template <class T,class... Args>
    static auto process(T&& t,Args&&... args)
     -> decltype(
         std::conditional<
             !std::decay<T>::type::Tail::is_last,
             FunctionalProcessor<Caller>,
             TailDoProcessor<Caller>
         >::type::process(
             t._t,
             Caller::call(
                 t._c.getRef(),
                 std::forward<Args>(args)...
             )
         )
     )
    {
        typedef typename std::conditional<
            !std::decay<T>::type::Tail::is_last,
            FunctionalProcessor<Caller>,
            TailDoProcessor<Caller>
        >::type Processor;

        return Processor::process(
            t._t,
            Caller::call(
                t._c.getRef(),
                std::forward<Args>(args)...
            )
        );
    }

};

template <class Caller>
struct FunctionalReverseProcessor {

    template <class T,class... Args>
    static auto process(T&& t,Args&&... args)
     -> decltype(
         Caller::call(
             t._c.getRef(),
             std::conditional<
                 !std::decay<T>::type::Tail::is_last,
                 FunctionalReverseProcessor<Caller>,
                 TailDoProcessor<Caller>
             >::type::process(
                 t._t,
                 std::forward<Args>(args)...
             )
         )
     )
    {
        typedef typename std::conditional<
            !std::decay<T>::type::Tail::is_last,
            FunctionalReverseProcessor<Caller>,
            TailDoProcessor<Caller>
        >::type Processor;

        return Caller::call(
            t._c.getRef(),
            Processor::process(
                t._t,
                std::forward<Args>(args)...
            )
        );
    }

};

template <class Caller>
struct StatefulProcessor {

    template <class T,class... Args>
    static void process(T&& t,Args&&... args)
    {
        typedef typename std::conditional<
            !std::decay<T>::type::Tail::is_last,
            StatefulProcessor<Caller>,
            TailDoStatefulProcessor<Caller>
        >::type Processor;

        Caller::call(t._c.getRef(),
            std::forward<Args>(args)...
        );

        Processor::process(
            t._t,
            std::forward<Args>(args)...
        );
    }

};

template <class Caller>
struct StatefulReverseProcessor {

    template <class T,class... Args>
    static void process(T&& t,Args&&... args)
    {
        typedef typename std::conditional<
            !std::decay<T>::type::Tail::is_last,
            StatefulReverseProcessor<Caller>,
            TailDoStatefulProcessor<Caller>
        >::type Processor;

        Processor::process(
            t._t,
            std::forward<Args>(args)...
        );

        Caller::call(
                t._c.getRef(),
                std::forward<Args>(args)...
        );
    }

};

template <
    bool statefulDefault,bool reversed,
    template <class> class StoragePolicy
>
struct ReverseTransform {
    template <class... Args>
    static auto transform(Args&&... args)
     -> decltype(
         makeChainFunctor<
             statefulDefault,!reversed,StoragePolicy
         >(
             unwrapFunctorPairReverse(
                 std::forward<Args>(args)
             )...
         )
     )
    {
        return makeChainFunctor<
            statefulDefault,!reversed,StoragePolicy
        >(
            unwrapFunctorPairReverse(
                std::forward<Args>(args)
            )...
        );
    }
};

template <
    bool statefulDefault,bool reversed,
    template <class> class StoragePolicy
>
struct UndoTransform {
    template <class... Args>
    static auto transform(Args&&... args)
     -> decltype(
         makeChainFunctor<
             statefulDefault,!reversed,StoragePolicy
         >(
             unwrapFunctorPairUndo(
                 std::forward<Args>(args)
             )...
         )
     )
    {
        return makeChainFunctor<
            statefulDefault,!reversed,StoragePolicy
        >(
            unwrapFunctorPairUndo(
                std::forward<Args>(args)
            )...
        );
    }
};

template <
    bool statefulDefault,bool reversed,
    template <class> class StoragePolicy
>
struct DoTransform {
    template <class... Args>
    static auto transform(Args&&... args)
     -> decltype(
         makeChainFunctor<
             statefulDefault,reversed,StoragePolicy
         >(
             unwrapFunctorPairDo(
                 std::forward<Args>(args)
             )...
         )
     )
    {
        return makeChainFunctor<
            statefulDefault,reversed,StoragePolicy
        >(
            unwrapFunctorPairDo(
                std::forward<Args>(args)
            )...
        );
    }
};

struct DummyTransform {
    template <class... Args>
    static bool transform(Args&&... args)
    {
        return false;
    }
};

template <
    bool statefulDefault,
    bool reversed,
    template <class> class StoragePolicy,
    class A,class... Tl
>
struct ChainFunctor<statefulDefault,reversed,StoragePolicy,A,Tl...> {
    typedef typename StoragePolicy<A>::Container Container;
    typedef ChainFunctor<
        statefulDefault,reversed,StoragePolicy,Tl...> Tail;
    typedef ChainFunctor<
        statefulDefault,reversed,StoragePolicy,A,Tl...> ThisFctor;

    static const bool hasUndo = A::undo && Tail::hasUndo;

    template <class Caller>
    friend struct FunctionalProcessor;
    template <class Caller>
    friend struct FunctionalReverseProcessor;
    template <class Caller>
    friend struct StatefulProcessor;
    template <class Caller>
    friend struct StatefulReverseProcessor;
    template <class Caller>
    friend struct TailDoProcessor;
    template <class Caller>
    friend struct TailDoStatefulProcessor;

    typedef typename std::conditional<
        !reversed,
        DoCaller,
        UndoCaller
    >::type CurrDoCaller;

    typedef typename std::conditional<
        !reversed,
        UndoCaller,
        DoCaller
    >::type CurrUndoCaller;

    typedef typename std::conditional<
        statefulDefault,
        StatefulProcessor<CurrDoCaller>,
        FunctionalProcessor<CurrDoCaller>
    >::type Proc;

    typedef typename std::conditional<
        statefulDefault,
        StatefulReverseProcessor<CurrUndoCaller>,
        FunctionalReverseProcessor<CurrUndoCaller>
    >::type RevProc;

    typedef typename std::conditional<
        !reversed,
        Proc,
        RevProc
    >::type FwdProc;

    typedef typename std::conditional<
        !reversed,
        RevProc,
        Proc
    >::type IntBwdProc;

    typedef typename std::conditional<
        hasUndo,
        IntBwdProc,
        UndoUnavailableProcessor
    >::type BwdProc;

    static const bool is_last = false;

    template <class T,class... Args>
    ChainFunctor(T&& t,Args&&... args)
     : _c(t), _t(std::forward<Args>(args)...) {}

    /**
     * Call forward action.
     * @param[in] args Arguments for function.
     *
     * Functional example:
     * ~~~~~~~ C++
     * auto mul2 = [](int i) { return 2*i; };
     * auto add3 = [](int i) { return 3+i; };
     *
     * auto chain = SF::chainFunctor(mul2,add3);
     *
     * int res = chain(1);
     * assert( res == 5 );
     * ~~~~~~~
     *
     * Stateful example:
     * ~~~~~~~ C++
     * auto mul2 = [](int& i) { i*=2; };
     * auto add3 = [](int& i) { i+=3; };
     *
     * auto chain = SF::chainFunctor<true>(mul2,add3);
     *
     * int res = 1;
     * chain(res);
     * assert( res == 5 );
     * ~~~~~~~
     */
    template <class... Args>
    auto doFwd(Args&&... args)
     -> decltype(
         FwdProc::process(
             *this,
             std::forward<Args>(args)...
         )
     )
    {
        return FwdProc::process(*this,
            std::forward<Args>(args)...);
    }

    /**
     * Call backward action.
     * @param[in] args Arguments for function.
     *
     * Functional example:
     * ~~~~~~~ C++
     * auto mul2 = [](int i) { return 2*i; };
     * auto div2 = [](int i) { return 2/i; };
     *
     * auto add3 = [](int i) { return 3+i; };
     * auto sub3 = [](int i) { return i-3; };
     *
     * auto chain = SF::chainFunctor(
     *     SF::functorPair(mul2,div2),
     *     SF::functorPair(add3,sub3)
     * );
     *
     * int res = chain(1);
     * assert( res == 5 );
     * int rev = chain.doBwd(res);
     * assert( rev == 1 );
     * ~~~~~~~
     *
     * Stateful example:
     * ~~~~~~~ C++
     * auto mul2 = [](int& i) { i*=2; };
     * auto div2 = [](int& i) { i/=2; };
     *
     * auto add3 = [](int& i) { i+=3; };
     * auto sub3 = [](int& i) { i-=3; };
     *
     * auto chain = SF::chainFunctor<true>(
     *     SF::functorPair(mul2,div2),
     *     SF::functorPair(add3,sub3)
     * );
     *
     * int res = 1;
     * chain(res);
     * assert( res == 5 );
     * chain.doBwd(res);
     * assert( res == 1 );
     * ~~~~~~~
     */
    template <class... Args>
    auto doBwd(Args&&... args)
     -> decltype(
         BwdProc::process(
             *this,
             std::forward<Args>(args)...
         )
     )
    {
        return BwdProc::process(*this,
            std::forward<Args>(args)...);
    }

    typedef typename std::conditional<
        hasUndo,
        ReverseTransform<statefulDefault,reversed,StoragePolicy>,
        DummyTransform
    >::type RevTr;

    typedef typename std::conditional<
        hasUndo,
        UndoTransform<statefulDefault,reversed,StoragePolicy>,
        DummyTransform
    >::type UndoTr;

    typedef typename std::conditional<
        hasUndo,
        DoTransform<statefulDefault,reversed,StoragePolicy>,
        DummyTransform
    >::type DoTr;

    /**
     * Reverse chain functor.
     *
     * Functional example:
     * ~~~~~~~
     * auto mul2 = [](int i) { return 2*i; };
     * auto div2 = [](int i) { return 2/i; };
     *
     * auto add3 = [](int i) { return 3+i; };
     * auto sub3 = [](int i) { return i-3; };
     *
     * auto chain = SF::chainFunctor(
     *     SF::functorPair(mul2,div2),
     *     SF::functorPair(add3,sub3)
     * );
     *
     * auto revChain = chain.reverse();
     *
     * int res = chain(1);
     * int rev = revChain(res);
     * assert( rev == 1 );
     * ~~~~~~~
     *
     * Stateful example:
     * ~~~~~~~~
     * auto mul2 = [](int& i) { i*=2; };
     * auto div2 = [](int& i) { i/=2; };
     *
     * auto add3 = [](int& i) { i+=3; };
     * auto sub3 = [](int& i) { i-=3; };
     *
     * auto chain = SF::chainFunctor<true>(
     *     SF::functorPair(mul2,div2),
     *     SF::functorPair(add3,sub3)
     * );
     *
     * auto revChain = chain.reverse();
     *
     * int res = 1;
     * chain(res);
     * revChain(res);
     * assert( res == 1 );
     * ~~~~~~~~
     */
    auto reverse() const
     -> decltype(
         std::declval<Tail>().template transform<
             RevTr
         >(
             std::declval<Container>().cgetRef()
         )
     )
    {
        return _t.template transform<
            RevTr
        >(
            _c.cgetRef()
        );
    }

    /**
     * Get duno functor
     *
     * Functional example:
     * ~~~~~~~
     * auto mul2 = [](int i) { return 2*i; };
     * auto div2 = [](int i) { return 2/i; };
     *
     * auto add3 = [](int i) { return 3+i; };
     * auto sub3 = [](int i) { return i-3; };
     *
     * auto chain = SF::chainFunctor(
     *     SF::functorPair(mul2,div2),
     *     SF::functorPair(add3,sub3)
     * );
     *
     * auto undo = chain.getUndo();
     *
     * int res = chain(1);
     * int rev = undo(res);
     * assert( rev == 1 );
     * ~~~~~~~
     *
     * Stateful example:
     * ~~~~~~~
     * auto mul2 = [](int& i) { i*=2; };
     * auto div2 = [](int& i) { i/=2; };
     *
     * auto add3 = [](int& i) { i+=3; };
     * auto sub3 = [](int& i) { i-=3; };
     *
     * auto chain = SF::chainFunctor<true>(
     *     SF::functorPair(mul2,div2),
     *     SF::functorPair(add3,sub3)
     * );
     *
     * auto undo = chain.getUndo();
     *
     * int res = 1;
     * chain(res);
     * undo(res);
     * assert( res == 1 );
     * ~~~~~~~
     */
    auto getUndo() const
     -> decltype(
         std::declval<Tail>().template transform<
            UndoTr
         >(
             std::declval<Container>().cgetRef()
         )
     )
    {
        return _t.template transform<
            UndoTr
        >(
            _c.cgetRef()
        );
    }

    /**
     * Get do action.
     * Functor returned does the same
     * as () operator of this chain functor.
     */
    auto getDo() const
     -> decltype(
         std::declval<Tail>().template transform<
            DoTr
         >(
             std::declval<Container>().cgetRef()
         )
     )
    {
        return _t.template transform<
            DoTr
        >(
            _c.cgetRef()
        );
    }

    /**
     * Same as doFwd method.
     */
    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(
         this->doFwd(
             std::forward<Args>(args)...
         )
     )
    {
        return doFwd(
            std::forward<Args>(args)...);
    }

    template <class Tr,class... Args>
    auto transform(Args&&... args) const
     -> decltype(
         std::declval<Tail>().template transform<Tr>(
             std::forward<Args>(args)...,
             std::declval<Container>().cgetRef()
         )
     )
    {
        return _t.template transform<Tr>(
                std::forward<Args>(args)...,
                _c.cgetRef());
    }

private:
    Container _c;
    Tail _t;
};

template <
    bool statefulDefault,
    bool reversed,
    template <class> class StoragePolicy,
    class A
>
struct ChainFunctor<statefulDefault,reversed,StoragePolicy,A> {
    typedef typename StoragePolicy<A>::Container Container;
    typedef ChainFunctor<
        statefulDefault,reversed,StoragePolicy,A> ThisFctor;

    static const bool hasUndo = A::undo;

    template <class Caller>
    friend struct FunctionalProcessor;
    template <class Caller>
    friend struct FunctionalReverseProcessor;
    template <class Caller>
    friend struct StatefulProcessor;
    template <class Caller>
    friend struct StatefulReverseProcessor;
    template <class Caller>
    friend struct TailDoProcessor;
    template <class Caller>
    friend struct TailDoStatefulProcessor;

    static const bool is_last = true;

    template <class T,class... Args>
    ChainFunctor(T&& t)
     : _c(t) {}

    template <class... Args>
    auto doFwd(Args&&... args)
     -> decltype(
         std::declval<Container>().getRef()(
             std::forward<Args>(args)...
         )
     )
    {
        return _c.getRef()(std::forward<Args>(args)...);
    }

    template <class... Args>
    auto doBwd(Args&&... args)
     -> decltype(
         std::declval<Container>().getRef()(
             std::forward<Args>(args)...
         )
     )
    {
        return _c.getRef()(std::forward<Args>(args)...);
    }

    typedef typename std::conditional<
        hasUndo,
        ReverseTransform<statefulDefault,reversed,StoragePolicy>,
        DummyTransform
    >::type RevTr;

    typedef typename std::conditional<
        hasUndo,
        UndoTransform<statefulDefault,reversed,StoragePolicy>,
        DummyTransform
    >::type UndoTr;

    typedef typename std::conditional<
        hasUndo,
        DoTransform<statefulDefault,reversed,StoragePolicy>,
        DummyTransform
    >::type DoTr;

    auto reverse() const
     -> decltype(
         RevTr::transform(
             std::declval<Container>().cgetRef()
         )
     )
    {
        return RevTr::transform(
            std::declval<Container>().cgetRef()
        );
    }

    auto getUndo() const
     -> decltype(
        UndoTr::transform(
            std::declval<Container>().cgetRef()
        )
     )
    {
        return UndoTr::transform(
            std::declval<Container>().cgetRef()
        );
    }

    auto getDo() const
     -> decltype(
        DoTr::transform(
            std::declval<Container>().cgetRef()
        )
     )
    {
        return DoTr::transform(
            std::declval<Container>().cgetRef()
        );
    }

    template <class... Args>
    auto operator()(Args&&... args)
     -> decltype(
         doFwd(std::forward<Args>(args)...)
     )
    {
        return doFwd(std::forward<Args>(args)...);
    }

    template <class Tr,class... Args>
    auto transform(Args&&... args) const
     -> decltype(
         Tr::transform(
             std::forward<Args>(args)...,
             std::declval<Container>().cgetRef()
         )
     )
    {
        return Tr::transform(
            std::forward<Args>(args)...,
            _c.cgetRef()
        );
    }

private:
    Container _c;
};

}
}

#endif /* end of include guard: CHAINFUNCTOR_JQWE6P11 */
