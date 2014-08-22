/*
 * =====================================================================================
 *
 *       Filename:  Root.h
 *
 *    Description:  Root of module based virtual collection
 *
 *        Version:  1.0
 *        Created:  08/22/2014 04:24:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ROOT_7NZ548VK
#define ROOT_7NZ548VK

#include <templatious/CollectionAdapter.h>

namespace templatious {

template <class T>
struct Root {
    typedef adapters::CollectionAdapter<T> Ad;

protected:
    virtual T& getRef() = 0;
};

}

#endif /* end of include guard: ROOT_7NZ548VK */

