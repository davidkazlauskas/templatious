/*
 * =====================================================================================
 *
 *       Filename:  TupleMacros.h
 *
 *    Description:  Call tuples n stuff
 *
 *        Version:  1.0
 *        Created:  05/25/2014 07:09:45 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef TUPLEMACROS_31JFL6E7
#define TUPLEMACROS_31JFL6E7

#include <tuple>

namespace templatious {
namespace tuple {

#define CALLTUPLE_0(f, t) f()
#define CALLTUPLE_1(f, t) f(std::get<0>(t))
#define CALLTUPLE_2(f, t) f(std::get<0>(t), std::get<1>(t))
#define CALLTUPLE_3(f, t) f(std::get<0>(t), std::get<1>(t), std::get<2>(t))
#define CALLTUPLE_4(f, t) \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t))
#define CALLTUPLE_5(f, t)                                             \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), \
      std::get<4>(t))
#define CALLTUPLE_6(f, t)                                             \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), \
      std::get<4>(t), std::get<5>(t))
#define CALLTUPLE_7(f, t)                                             \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), \
      std::get<4>(t), std::get<5>(t), std::get<6>(t))
#define CALLTUPLE_8(f, t)                                             \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), \
      std::get<4>(t), std::get<5>(t), std::get<6>(t), std::get<7>(t))
#define CALLTUPLE_9(f, t)                                             \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), \
      std::get<4>(t), std::get<5>(t), std::get<6>(t), std::get<7>(t), \
      std::get<8>(t))
#define CALLTUPLE_10(f, t)                                            \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), \
      std::get<4>(t), std::get<5>(t), std::get<6>(t), std::get<7>(t), \
      std::get<8>(t), std::get<9>(t))
#define CALLTUPLE_11(f, t)                                            \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), \
      std::get<4>(t), std::get<5>(t), std::get<6>(t), std::get<7>(t), \
      std::get<8>(t), std::get<9>(t), std::get<10>(t))
#define CALLTUPLE_12(f, t)                                            \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), \
      std::get<4>(t), std::get<5>(t), std::get<6>(t), std::get<7>(t), \
      std::get<8>(t), std::get<9>(t), std::get<10>(t), std::get<11>(t))
#define CALLTUPLE_13(f, t)                                              \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t),   \
      std::get<4>(t), std::get<5>(t), std::get<6>(t), std::get<7>(t),   \
      std::get<8>(t), std::get<9>(t), std::get<10>(t), std::get<11>(t), \
      std::get<12>(t))
#define CALLTUPLE_14(f, t)                                              \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t),   \
      std::get<4>(t), std::get<5>(t), std::get<6>(t), std::get<7>(t),   \
      std::get<8>(t), std::get<9>(t), std::get<10>(t), std::get<11>(t), \
      std::get<12>(t), std::get<13>(t))
#define CALLTUPLE_15(f, t)                                              \
    f(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t),   \
      std::get<4>(t), std::get<5>(t), std::get<6>(t), std::get<7>(t),   \
      std::get<8>(t), std::get<9>(t), std::get<10>(t), std::get<11>(t), \
      std::get<12>(t), std::get<13>(t), std::get<14>(t))

}
}

#endif /* end of include guard: TUPLEMACROS_31JFL6E7 */
