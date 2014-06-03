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
    struct IsVoid<void> {
        static const bool val = true;

        template <class U>
        void retVal(U u) { }
    };

    template <>
    struct IsVoid<void*> {
        static const bool val = true;

        template <class U>
        void retVal(U u) { }
    };

}
}

#endif /* end of include guard: SELECTORS_1ZD32CZT */
