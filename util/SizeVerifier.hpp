/*
 * =====================================================================================
 *
 *       Filename:  SizeVerifier.h
 *
 *    Description:  Verify that all sizes are equal
 *
 *        Version:  1.0
 *        Created:  05/25/2014 02:50:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef SIZEVERIFIER_S81TOGPX
#define SIZEVERIFIER_S81TOGPX

#include <templatious/StaticAdapter.hpp>

namespace templatious {
namespace util {

template <class ...Args>
struct SizeVerifier;

template <class A,class ...Args>
struct SizeVerifier<A,Args...> {
    typedef templatious::StaticAdapter SA;
    long _size;
    SizeVerifier<Args...> _next;

    bool areAllEqual() {
        // if size is -1 adapter is saying that
        // size of collection is not known currently
        if (_size != -1 && _size == _next._size) {
            return _next.areAllEqual();
        }

        return false;
    }

    SizeVerifier(A&& a, Args&&... args) :
        _size(SA::size(std::forward<A>(a))),
        _next(std::forward<Args>(args)...) {}
};

template <class A>
struct SizeVerifier<A> {
    typedef templatious::StaticAdapter SA;

    long _size;

    SizeVerifier(A& a) : _size(SA::size(a)) {}

    bool areAllEqual() {
        return true;
    }
};

}
}

#endif /* end of include guard: SIZEVERIFIER_S81TOGPX */