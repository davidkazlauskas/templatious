/*
 * =====================================================================================
 *
 *       Filename:  QuadroIter.h
 *
 *    Description:  Iterator for quadro function
 *
 *        Version:  1.0
 *        Created:  07/27/2014 07:31:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef QUADROITER_NY9I9DCO
#define QUADROITER_NY9I9DCO

#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace recursive {

template <class... Args>
struct QuadroIterator;

template <class A>
struct QuadroIterator<A> {

};

template <class A, class... Tail>
struct QuadroIterator<A, Tail...> {

};

}
}

#endif /* end of include guard: QUADROITER_NY9I9DCO */
