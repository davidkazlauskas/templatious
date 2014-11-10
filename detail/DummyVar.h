/*
 * =====================================================================================
 *
 *       Filename:  DummyVar.h
 *
 *    Description:  Dummy variable which can be assigned anything
 *
 *        Version:  1.0
 *        Created:  11/10/2014 05:10:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef DUMMYVAR_MTQR7T2M
#define DUMMYVAR_MTQR7T2M

#include <utility>

namespace templatious {
namespace detail {

struct DummyVar {

    template <class T>
    auto operator=(T&& rhs) const
     -> decltype(std::forward<T>(rhs))
    {
        return std::forward<T>(rhs);
    }
};

static auto dummyVar = DummyVar();

// no unused variable warning for dummyVar
DummyVar getDummy() { return dummyVar; }

}
}

#endif /* end of include guard: DUMMYVAR_MTQR7T2M */
