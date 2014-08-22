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

template <
    class T,
    template <class> class Container
>
struct Root {
    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::ThisCol ThisCol;
    typedef Container<typename Ad::ThisCol> Cont;

    Root(T& t) : _c(t) {}
protected:
    T& getRef() {
        return _c.getRef();
    }
private:
    Cont _c;
};

}

#endif /* end of include guard: ROOT_7NZ548VK */

