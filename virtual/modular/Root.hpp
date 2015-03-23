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

#include <templatious/CollectionAdapter.hpp>

namespace templatious {
namespace vmodular {

template <
    class T,
    class Container
>
struct Root {
    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::ThisCol ThisCol;
    typedef Container Cont;

    template <class V>
    Root(V&& t) : _c(std::forward<V>(t)) {}
protected:
    T& getRef() {
        return _c.getRef();
    }

    const T& cgetRef() const {
        return _c.cgetRef();
    }
private:
    Cont _c;
};

}
}

#endif /* end of include guard: ROOT_7NZ548VK */

