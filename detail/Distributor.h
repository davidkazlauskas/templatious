/*
 * =====================================================================================
 *
 *       Filename:  Distributor.h
 *
 *    Description:  Distributor routines
 *
 *        Version:  1.0
 *        Created:  10/02/2014 06:16:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef DISTRIBUTOR_T42YQ29F
#define DISTRIBUTOR_T42YQ29F

#include <utility>
#include <templatious/util/Selectors.h>
#include <templatious/util/Compterator.h>

namespace templatious {
namespace detail {

    typedef unsigned int CountType;

    struct AssignDispatcher {
        template <class T,class U>
        static void dispatch(T& t,const U& u) {
            t = u;
        }
    };

    struct RevAssignDispatcher {
        template <class T,class U>
        static void dispatch(const T& t,U& u) {
            u = t;
        }
    };

    template <class Dispatcher>
    struct PackToRestDistribution {

        template <class T,class... Tail>
        static CountType distribute(T&& p,Tail&&... t) {
            namespace UD = templatious::util::detail;
            auto cTer = UD::makeCompteratorV2( std::forward<Tail>(t)... );

            return step<0>(std::forward<T>(p),cTer);
        }

        template <int i,class P,class C>
        static CountType step(P&& p,C& compterator) {
            typedef typename templatious::util::TypeSelector<
                compterator.last || i == p.size - 1,
                CatchAllStepper,
                TailStepper
            >::val TailStepper;

            typedef typename templatious::util::TypeSelector<
                i == p.size - 1,
                CatchAllStepper,
                ForwardStepper
            >::val FwdStepper;

            namespace U = templatious::util;
            typedef decltype(compterator._p) ValType;

            static const bool isIterPair = U::IsIteratorPair<ValType>::value;

            typedef typename templatious::util::TypeSelector<
                isIterPair,
                IterActions,
                ValActions
            >::val Actions;

            Actions::template assign<i>(std::forward<P>(p),compterator);

            if (!Actions::isDone(compterator)) {
                return FwdStepper::template stepSpecial<i + 1>(std::forward<P>(p),compterator);
            } else {
                return TailStepper::template stepSpecial<i + 1>(std::forward<P>(p),compterator);
            }
        }

        struct ForwardStepper {
            template <int i,class P,class C>
            static CountType stepSpecial(P&& p,C& compterator) {
                return step<i>(std::forward<P>(p),compterator);
            }
        };

        struct TailStepper {
            template <int i,class P,class C>
            static CountType stepSpecial(P&& p,C& compterator) {
                return step<i>(std::forward<P>(p),compterator._t);
            }
        };

        struct CatchAllStepper {
            template <int i,class P,class C>
            static CountType stepSpecial(P&& p,C& compterator) {
                return i;
            }
        };

        struct IterActions {
            template <int i,class P,class C>
            static void assign(P&& p,C& compterator) {
                Dispatcher::dispatch(*compterator._p._a,p.template get<i>());
                ++compterator._p._a;
            }

            template <class C>
            static bool isDone(C& compterator) {
                return compterator._p._a == compterator._p._b;
            }
        };

        struct ValActions {
            template <int i,class P,class C>
            static void assign(P&& p,C& compterator) {
                Dispatcher::dispatch(compterator._p,p.template get<i>());
            }

            template <class C>
            static bool isDone(C& compterator) {
                return true;
            }
        };

        struct DummyActions {
            template <int i,class P,class C>
            static void assign(P&& p,C& compterator) {}

            template <class C>
            static bool isDone(C& compterator) {
                return true;
            }
        };

    };

    template <class Dispatcher>
    struct CollectionToRestDistribution {

        template <class T,class... Tail>
        static CountType distribute(T&& p,Tail&&... t) {
            typedef templatious::adapters::CollectionAdapter<T> Ad;
            namespace U = templatious::util;
            namespace UD = U::detail;
            typedef U::IteratorPair<typename Ad::iterator> Pair;

            Pair pair(Ad::begin(std::forward<T>(p)),Ad::end(std::forward<T>(p)));
            auto cTer = UD::makeCompteratorV2( std::forward<Tail>(t)... );

            return step(pair,cTer);
        }

        template <class P,class C>
        static CountType step(P&& p,C& compterator) {

            namespace U = templatious::util;
            typedef decltype(compterator._p) ValType;
            static const bool isIterPair = U::IsIteratorPair<ValType>::value;

            typedef typename std::conditional<
                isIterPair,
                IterActions,
                ValActions
            >::type Actions;

            typedef typename std::conditional<
                compterator.last,
                CatchAllStepper,
                TailStepper
            >::type Stepper;


            CountType myCnt = 0;
            do {
                Actions::assign(std::forward<P>(p),compterator);
                ++p._a;
                ++myCnt;
            } while (!Actions::isDone(compterator) && p._a != p._b);

            if (p._a == p._b) {
                return myCnt;
            }

            return myCnt + Stepper::stepSpecial(
                    std::forward<P>(p),compterator);
        }

        struct CatchAllStepper {
            template <class P,class C>
            static CountType stepSpecial(P&& p,C& compterator) {
                return 0;
            }
        };

        struct TailStepper {
            template <class P,class C>
            static CountType stepSpecial(P&& p,C& compterator) {
                return step(std::forward<P>(p),compterator._t);
            }
        };

        struct IterActions {
            template <class P,class C>
            static void assign(P&& p,C& compterator) {
                Dispatcher::dispatch(*compterator._p._a,*p._a);
                ++compterator._p._a;
            }

            template <class C>
            static bool isDone(C& compterator) {
                return compterator._p._a == compterator._p._b;
            }
        };

        struct ValActions {
            template <class P,class C>
            static void assign(P&& p,C& compterator) {
                Dispatcher::dispatch(compterator._p,*p._a);
            }

            template <class C>
            static bool isDone(C& compterator) {
                return true;
            }
        };

        struct DummyActions {
            template <class P,class C>
            static void assign(P&& p,C& compterator) {}

            template <class C>
            static bool isDone(C& compterator) {
                return true;
            }
        };
    };

    struct DummyErrorDistributor {
        template <class T,class... Tail>
        static CountType distribute(T&& p,Tail&&... t) {
            static_assert(
                templatious::util::DummyResolver<T,false>::val,
                "Item passed is undistributable."
            );
        }
    };

}
}

#endif /* end of include guard: DISTRIBUTOR_T42YQ29F */

