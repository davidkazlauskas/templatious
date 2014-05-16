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

template <class T,class U,class V>
struct ActionTwoToOne<T,U,V,Type2Type<Multiply>> {
	V operator()(T& a,T& b) {
		return a * b;
	}
};

template <class T,class U,class V>
struct ActionTwoToOne<T,U,V,Type2Type<Subtract>> {
	V operator()(int& a,int& b) {
		return a - b;
	}
};

template <class T, class U, class V>
struct ActionTwoToOne<T, U, V, Type2Type<Add>> {
        V operator()(int& a,int& b) {
		return a + b;
	}
};

template <class T, class U, class V>
struct ActionTwoToOne<T, U, V, Type2Type<Divide>> {
        V operator()(int& a,int& b) {
		return a / b;
	}
};

template <class T, class U, class V>
struct ActionTwoToOne<T, U, V, Type2Type<Mod>> {
        V operator()(int& a,int& b) {
		return a % b;
	}
};

}
}

#endif /* end of include guard: BASIC_FOO7VZOQ */
