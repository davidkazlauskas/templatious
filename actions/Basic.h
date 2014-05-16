/*
 * =====================================================================================
 *
 *       Filename:  Basic.h
 *
 *    Description:  Basic actions
 *
 *        Version:  1.0
 *        Created:  05/16/2014 10:23:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef BASIC_FOO7VZOQ
#define BASIC_FOO7VZOQ

#include <templatious/Action.h>

namespace templatious {
namespace action {

struct Multiply;
struct Subtract;
struct Add;
struct Divide;
struct Mod;

template <class V, class T, class U>
struct ActionTwoToOne<V, T, U, Type2Type<Multiply>> {
        V operator()(const T& a,const T& b) {
		return a * b;
	}
};

template <class V, class T, class U>
struct ActionTwoToOne<V, T, U, Type2Type<Subtract>> {
        V operator()(const T& a,const U& b) {
		return a - b;
	}
};

template <class V, class T, class U>
struct ActionTwoToOne<V, T, U, Type2Type<Add>> {
        V operator()(const T& a,const U& b) {
		return a + b;
	}
};

template <class V, class T, class U>
struct ActionTwoToOne<V, T, U, Type2Type<Divide>> {
        V operator()(const T& a,const U& b) {
		return a / b;
	}
};

template <class V, class T, class U>
struct ActionTwoToOne<V, T, U, Type2Type<Mod>> {
        V operator()(const T& a,const U& b) {
		return a % b;
	}
};

}
}

#endif /* end of include guard: BASIC_FOO7VZOQ */
