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
struct ComparatorEq {

    // default equality test
    bool operator()(const T& t,const U& u) {
        return t == u;
    }

};

template <class T,class U,class Var = Default>
struct ComparatorL {

    // default comparator is Less
    bool operator()(const T& t,const U& u) {
        return t < u;
    }

};

template <class T,class U,class Var = Default>
struct ComparatorM {

    // default comparator is More
    bool operator()(const T& t,const U& u) {
        return t > u;
    }

};

template <class T,class U,class Var = Default>
struct ComparatorDiff {

    // default comparator is More
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

}
}

#endif /* end of include guard: COMPARATOR_CIFG4Q23 */
