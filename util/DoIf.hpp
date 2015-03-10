/*
 * =====================================================================================
 *
 *       Filename:  DoIf.h
 *
 *    Description:  Do If static assertion is true
 *
 *        Version:  1.0
 *        Created:  05/26/2014 06:56:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef DOIF_DJSF7S1H
#define DOIF_DJSF7S1H

namespace templatious {
namespace util {

template <bool val>
struct DoIf;

template <>
struct DoIf<false> {

    template <class F>
    void doIt(F f) {}

};

template <>
struct DoIf<true> {

    template <class F>
    auto doIt(F f) -> decltype(f()) { return f(); }
};

}
}

#endif /* end of include guard: DOIF_DJSF7S1H */
