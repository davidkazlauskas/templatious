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

template <class T, class U>
struct ActionTwoToOne<T, U, Type2Type<Multiply>> {
    template <class V>
    V operator()(const T& a, const T& b) {
        return a * b;
    }
};

template <class T, class U>
struct ActionTwoToOne<T, U, Type2Type<Subtract>> {
    template <class V>
    V operator()(const T& a, const U& b) {
        return a - b;
    }
};

template <class T, class U>
struct ActionTwoToOne<T, U, Type2Type<Add>> {
    template <class V>
    V operator()(const T& a, const U& b) {
        return a + b;
    }
};

template <class T, class U>
struct ActionTwoToOne<T, U, Type2Type<Divide>> {
    template <class V>
    V operator()(const T& a, const U& b) {
        return a / b;
    }
};

template <class T, class U>
struct ActionTwoToOne<T, U, Type2Type<Mod>> {
    template <class V>
    V operator()(const T& a, const U& b) {
        return a % b;
    }
};

}
}

#endif /* end of include guard: BASIC_FOO7VZOQ */
