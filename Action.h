/*
 * =====================================================================================
 *
 *       Filename:  Action.h
 *
 *    Description:  Actions for manipulation
 *
 *        Version:  1.0
 *        Created:  05/16/2014 10:20:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ACTION_BW2H9814
#define ACTION_BW2H9814

#include <templatious/TypeTraits.h>

namespace templatious {
namespace action {

// T - left side, U - right side, V - return type, Type2Type - additional anchor to differentiate
template < class T,class U,class X = Type2Type<Default> >
struct ActionTwoToOne {

    template <class V>
    V operator()(const T& a,const U& b);

};

}
}

#endif /* end of include guard: ACTION_BW2H9814 */
