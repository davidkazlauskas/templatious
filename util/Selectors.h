/*
 * =====================================================================================
 *
 *       Filename:  Selectors.h
 *
 *    Description:  Small utils
 *
 *        Version:  1.0
 *        Created:  05/26/2014 06:30:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
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

}
}

#endif /* end of include guard: SELECTORS_1ZD32CZT */
