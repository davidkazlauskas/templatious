/*
 * =====================================================================================
 *
 *       Filename:  Comparator.h
 *
 *    Description:  Item comparator
 *
 *        Version:  1.0
 *        Created:  06/08/2014 11:01:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef COMPARATOR_CIFG4Q23
#define COMPARATOR_CIFG4Q23

namespace templatious {
namespace util {

struct Default;

template <class T,class U,class Var = Default>
struct ComparatorEq;
template <class T,class U,class Var = Default>
struct ComparatorL;
template <class T,class U,class Var = Default>
struct ComparatorM;
template <class T,class U,class Var = Default>
struct ComparatorDiff;


template <class T,class U>
struct ComparatorEq<T,U,Default> {

    // default equality test
    bool operator()(const T& t,const U& u) {
        return t == u;
    }

};

template <class T,class U>
struct ComparatorL<T,U,Default> {

    // default comparator is Less
    bool operator()(const T& t,const U& u) {
        return t < u;
    }

};

template <class T,class U>
struct ComparatorM<T,U,Default> {

    // default comparator is More
    bool operator()(const T& t,const U& u) {
        return t > u;
    }

};

template <class T,class U>
struct ComparatorDiff<T,U,Default> {

    // default comparator by difference
    auto operator()(const T& t,const U& u) -> decltype(t - u) {
        return t - u;
    }

};

template <class Comp,bool isReversed = true>
struct ReverseComparator;

template <class Comp>
struct ReverseComparator<Comp,true> {
    Comp _c;

    ReverseComparator(const Comp& c) : _c(c) {}

    template <class T,class U>
    auto operator()(const T& t,const U& u) -> decltype(_c(u,t)) {
        return _c(u,t);
    }
};

template <class Comp>
struct ReverseComparator<Comp,false> {
    Comp _c;

    ReverseComparator(const Comp& c) : _c(c) {}

    template <class T,class U>
    auto operator()(const T& t,const U& u) -> decltype(_c(t,u)) {
        return _c(t,u);
    }
};

template <bool isReversed = true,class Comp>
ReverseComparator<Comp,isReversed> rev(const Comp& c) {
    return ReverseComparator<Comp,isReversed>(c);
}

template <bool isReversed = false>
struct StaticComparator {

    template <class Var = Default,class T,class U>
    static bool isEqual(const T& t,const U& u) {
        auto c = rev<isReversed>(ComparatorEq<T,U,Var>());
        return c(t,u);
    }

    template <class Var = Default,class T,class U>
    static bool isLess(const T& t,const U& u) {
        auto c = rev<isReversed>(ComparatorL<T,U,Var>());
        return c(t,u);
    }

    template <class Var = Default,class T,class U>
    static bool isMore(const T& t,const U& u) {
        auto c = rev<isReversed>(ComparatorM<T,U,Var>());
        return c(t,u);
    }

    template <class Var = Default, class T, class U>
    static auto diff(const T& t, const U& u)
        -> decltype((rev<isReversed>(ComparatorDiff<T, U, Var>()))(t, u)) 
    {
        auto c = rev<isReversed>(ComparatorDiff<T, U, Var>());
        return c(t, u);
    }
};

}
}

#endif /* end of include guard: COMPARATOR_CIFG4Q23 */
