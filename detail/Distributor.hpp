//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Distributor.hpp
 *
 *    Description:  Distributor routines
 *
 *        Version:  1.0
 *        Created:  10/02/2014 06:16:21 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef DISTRIBUTOR_T42YQ29F
#define DISTRIBUTOR_T42YQ29F

#include <utility>
#include <templatious/util/Selectors.hpp>
#include <templatious/util/Compterator.hpp>

namespace templatious {
namespace detail {

    typedef unsigned long CountType;

    struct AssignDispatcher {
        template <class T,class U>
        static void dispatch(const T& t,U& u) {
            u = t;
        }

        template <class T,class U>
        void operator()(const T& t,U& u) {
            dispatch(t,u);
        }
    };

    struct RevAssignDispatcher {
        template <class T,class U>
        static void dispatch(T& t,const U& u) {
            t = u;
        }

        template <class T,class U>
        void operator()(T& t,const U& u) {
            dispatch(t,u);
        }
    };

    struct PackToRestDistribution {

        template <
            bool ignoreBooleanReturn = false,
            class Func,class T,class... Tail
        >
        static CountType distribute(Func&& f,T&& p,Tail&&... t) {
            namespace UD = templatious::util::detail;
            auto cTer = UD::makeCompteratorV2( std::forward<Tail>(t)... );

            return step<ignoreBooleanReturn,0>(
                    std::forward<Func>(f),
                    std::forward<T>(p),
                    cTer);
        }

        template <
            bool ignoreBooleanReturn = false,
            int i,class Func,class P,class C
        >
        static CountType step(Func&& f,P&& p,C& compterator) {
            typedef typename std::decay<decltype(p)>::type PType;
            typedef typename std::conditional<
                C::last || i == PType::size - 1,
                CatchAllStepper,
                TailStepper
            >::type TailStepper;

            typedef typename std::conditional<
                i == PType::size - 1,
                CatchAllStepper,
                ForwardStepper
            >::type FwdStepper;

            namespace U = templatious::util;
            typedef decltype(compterator._p) ValType;

            static const bool isIterPair = U::IsIteratorPair<ValType>::value;

            typedef typename std::conditional<
                isIterPair,
                IterActions,
                ValActions
            >::type Actions;

            bool res = Actions::template assign<i>(
                    std::forward<Func>(f),
                    std::forward<P>(p),
                    compterator);
            if (!ignoreBooleanReturn && !res) {
                return i;
            }

            if (!Actions::isDone(compterator)) {
                return FwdStepper::template stepSpecial<
                        ignoreBooleanReturn,i + 1
                    >(std::forward<Func>(f),
                      std::forward<P>(p),
                      compterator);
            } else {
                return TailStepper::template stepSpecial<
                        ignoreBooleanReturn,i + 1
                    >(std::forward<Func>(f),
                      std::forward<P>(p),
                      compterator);
            }
        }

        struct ForwardStepper {
            template <
                bool ignoreBooleanReturn = false,
                int i,class Func,class P,class C
            >
            static CountType stepSpecial(Func&& f,P&& p,C& compterator) {
                return step<ignoreBooleanReturn,i>(
                        std::forward<Func>(f),
                        std::forward<P>(p),
                        compterator);
            }
        };

        struct TailStepper {
            template <
                bool ignoreBooleanReturn = false,
                int i,class Func,class P,class C
            >
            static CountType stepSpecial(Func&& f,P&& p,C& compterator) {
                return step<ignoreBooleanReturn,i>(
                        std::forward<Func>(f),
                        std::forward<P>(p),
                        compterator._t);
            }
        };

        struct CatchAllStepper {
            template <
                bool ignoreBooleanReturn = false,
                int i,class Func,class P,class C
            >
            static CountType stepSpecial(Func&& f,P&& p,C& compterator) {
                return i;
            }
        };

        struct IterActions {
            // return value could be called "keepGoing"
            // if function returns true routine continues
            // if function returns false routine breaks
            // if function doesnt return boolean routine continues
            template <int i,class Func,class P,class C>
            static bool assign(Func&& f,P&& p,C& compterator) {
                typedef templatious::util::RetValSelector<
                    decltype(f(p.template get<i>(),*compterator._p._a)),
                    true> Sel;

                bool res = Sel::callAndEval(
                    std::forward<Func>(f),
                    p.template get<i>(),
                    *compterator._p._a
                );
                ++compterator._p._a;
                return res;
            }

            template <class C>
            static bool isDone(C& compterator) {
                return compterator._p._a == compterator._p._b;
            }
        };

        struct ValActions {
            template <int i,class Func,class P,class C>
            static bool assign(Func&& f,P&& p,C& compterator) {
                typedef templatious::util::RetValSelector<
                    decltype(f(p.template get<i>(),compterator._p)),
                    true> Sel;

                return Sel::callAndEval(
                    std::forward<Func>(f),
                    p.template get<i>(),
                    compterator._p
                );
            }

            template <class C>
            static bool isDone(C& compterator) {
                return true;
            }
        };

        struct DummyActions {
            template <int i,class Func,class P,class C>
            static bool assign(Func&& f,P&& p,C& compterator) {
                return true;
            }

            template <class C>
            static bool isDone(C& compterator) {
                return true;
            }
        };

    };

    struct CollectionToRestDistribution {

        template <
            bool ignoreBooleanReturn = false,
            class Func,class T,class... Tail
        >
        static CountType distribute(Func&& f,T&& p,Tail&&... t) {
            typedef templatious::adapters::CollectionAdapter<T> Ad;
            namespace U = templatious::util;
            namespace UD = U::detail;
            typedef U::IteratorPair<typename Ad::Iterator> Pair;

            Pair pair(Ad::begin(std::forward<T>(p)),Ad::end(std::forward<T>(p)));
            auto cTer = UD::makeCompteratorV2( std::forward<Tail>(t)... );

            return step<ignoreBooleanReturn>(std::forward<Func>(f),pair,cTer);
        }

        template <
            bool ignoreBooleanReturn = false,
            class Func,class P,class C
        >
        static CountType step(Func&& f,P&& p,C& compterator) {

            namespace U = templatious::util;
            typedef decltype(compterator._p) ValType;
            static const bool isIterPair = U::IsIteratorPair<ValType>::value;

            typedef typename std::conditional<
                isIterPair,
                IterActions,
                ValActions
            >::type Actions;

            typedef typename std::conditional<
                C::last,
                CatchAllStepper,
                TailStepper
            >::type Stepper;

            CountType myCnt = 0;
            do {
                bool res = Actions::assign(
                    std::forward<Func>(f),
                    std::forward<P>(p),
                    compterator);

                ++p._a;
                ++myCnt;

                if (!ignoreBooleanReturn && !res) return myCnt;
            } while (!Actions::isDone(compterator) && p._a != p._b);

            if (p._a == p._b) {
                return myCnt;
            }

            return myCnt + Stepper::template stepSpecial<
                    ignoreBooleanReturn
                >(std::forward<Func>(f),
                  std::forward<P>(p),
                  compterator);
        }

        struct CatchAllStepper {
            template <
                bool ignoreBooleanReturn = false,
                class Func,class P,class C
            >
            static CountType stepSpecial(Func&& f,P&& p,C& compterator) {
                return 0;
            }
        };

        struct TailStepper {
            template <
                bool ignoreBooleanReturn = false,
                class Func,class P,class C
            >
            static CountType stepSpecial(Func&& f,P&& p,C& compterator) {
                return step<ignoreBooleanReturn>(
                        std::forward<Func>(f),
                        std::forward<P>(p),
                        compterator._t);
            }
        };

        struct IterActions {
            template <class Func,class P,class C>
            static bool assign(Func&& f,P&& p,C& compterator) {
                typedef typename templatious::util::RetValSelector<
                    decltype(f(*p._a,*compterator._p._a)),
                    true> Sel;

                bool res = Sel::callAndEval(
                    std::forward<Func>(f),
                    *p._a,
                    *compterator._p._a);
                ++compterator._p._a;
                return res;
            }

            template <class C>
            static bool isDone(C& compterator) {
                return compterator._p._a == compterator._p._b;
            }
        };

        struct ValActions {
            template <class Func,class P,class C>
            static bool assign(Func&& f,P&& p,C& compterator) {
                typedef typename templatious::util::RetValSelector<
                    decltype(f(*p._a,compterator._p)),
                    true> Sel;

                return Sel::callAndEval(
                    std::forward<Func>(f),
                    *p._a,
                    compterator._p);
            }

            template <class C>
            static bool isDone(C& compterator) {
                return true;
            }
        };

        struct DummyActions {
            template <class Func,class P,class C>
            static bool assign(Func&& f,P&& p,C& compterator) {
                return true;
            }

            template <class C>
            static bool isDone(C& compterator) {
                return true;
            }
        };
    };

    struct DummyErrorDistributor {
        template <
            bool ignoreBooleanReturn = false,
            class Func,class T,class... Tail
        >
        static CountType distribute(Func&& f,T&& p,Tail&&... t) {
            static_assert(
                templatious::util::DummyResolver<T,false>::val,
                "Item passed is undistributable."
            );
            return std::declval<CountType>(); // no compiler warning
        }
    };

}
}

#endif /* end of include guard: DISTRIBUTOR_T42YQ29F */

