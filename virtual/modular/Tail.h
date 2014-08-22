/*
 * =====================================================================================
 *
 *       Filename:  Tail.h
 *
 *    Description:  Tail of every single module based virtual collection
 *
 *        Version:  1.0
 *        Created:  08/22/2014 04:16:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef TAIL_8FSTJFTQ
#define TAIL_8FSTJFTQ

#include <templatious/util/Container.h>
#include <templatious/virtual/Modular.h>
#include <templatious/virtual/VirtualCollection.h>

namespace templatious {

template <
    class T,
    template <class> class Container
>
struct Tail: public T {
    typedef typename T::Ad Ad;
    typedef typename Ad::ThisCol ThisCol;
    typedef Container<typename Ad::ThisCol> Cont;

    Tail(ThisCol& c) : _c(c) {}

protected:
    virtual ThisCol& getRef() {
        return _c.getRef();
    }
private:
    Cont _c;
};

}


#endif /* end of include guard: TAIL_8FSTJFTQ */

