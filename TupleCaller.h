/*
 * =====================================================================================
 *
 *       Filename:  TupleCaller.h
 *
 *    Description:  Call tuples automatically
 *
 *        Version:  1.0
 *        Created:  05/25/2014 07:45:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef TUPLECALLER_UNZX3QCI
#define TUPLECALLER_UNZX3QCI

#include <templatious/tuple/TupleMacros.h>

namespace templatious {
namespace tuple {

template <class RetType,class ...Args>
RetType CallTuple(std::function<RetType(Args...)> f,std::tuple<Args...>& t) {
	tuple_caller<sizeof...(Args),RetType,Args...> tc;
	return tc.call(f,t);
}

}
}

#endif /* end of include guard: TUPLECALLER_UNZX3QCI */
