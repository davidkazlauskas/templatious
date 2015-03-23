//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Selectors.hpp
 *
 *    Description:  Small utils
 *
 *        Version:  1.0
 *        Created:  05/26/2014 06:30:57 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef SELECTORS_1ZD32CZT
#define SELECTORS_1ZD32CZT

#include <utility>

namespace templatious {
namespace util {

    template <
        bool isTrue,
        int trueVal,
        int falseVal
    >
    struct IntSelector;

    template <
        int trueVal,
        int falseVal
    >
    struct IntSelector<true,trueVal,falseVal> {
        enum { val = trueVal };
    };

    template <
        int trueVal,
        int falseVal
    >
    struct IntSelector<false,trueVal,falseVal> {
        enum { val = falseVal };
    };

    // IS VOID
    template <class T>
    struct IsVoid {
        static const bool val = false;

        T retVal(T& t) {
            return t;
        }
    };

    template <>
    struct IsVoid<void> { static const bool val = true;

        template <class U>
        void retVal(U u) { }
    };

    template <>
    struct IsVoid<void*> {
        static const bool val = true;

        template <class U>
        void retVal(U u) { }
    };


    // T - class to evaluate. Has to be specialized
    // ret - default return value
    template <class T,bool ret = true>
    struct RetValSelector {
        template <class F,class... Args>
        static bool callAndEval(F&& f,Args&&... args) {
            f(std::forward<Args>(args)...);
            return ret;
        }
    };

    template <bool ret>
    struct RetValSelector<bool,ret> {

        template <class F,class... Args>
        static bool callAndEval(F&& f,Args&&... args) {
            return f(std::forward<Args>(args)...);
        }
    };

    // a dummy class that's main use is
    // mainly to suppress static asserts
    // by taking template parameter
    template <class T,bool expected>
    struct DummyResolver {
        static const bool val = expected;
    };

    // copied from stack overflow
    // http://stackoverflow.com/questions/22882170/
    // c-compile-time-predicate-to-test-if-a-callable-object-of-type-f-can-be-called
    // tests if we can call function with a specific type
    struct IsCallableWithHelper
    {
        template<typename F, typename... A>
        static decltype(std::declval<F>()(std::declval<A>()...), std::true_type())
        f(int);
    
        template<typename F, typename... A>
        static std::false_type
        f(...);
    };

    struct InvalidType;

    template<typename F, typename... A>
    struct IsCallableWith {

        typedef decltype(IsCallableWithHelper::f<F, A...>(0)) Helper;

        // true overload
        template <bool isValid,class Func,class... Args>
        struct Decider {
            typedef decltype(
                std::declval<Func>()(std::declval<Args>()...)
            ) type;
        };

        template <class Func,class... Args>
        struct Decider<false,Func,Args...> {
            typedef InvalidType type;
        };

        typedef typename Decider< Helper::value, F, A... >::type type;
        static const bool value = Helper::value;
    };
}
}

#endif /* end of include guard: SELECTORS_1ZD32CZT */
