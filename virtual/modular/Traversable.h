/*
 * =====================================================================================
 *
 *       Filename:  Traversable.h
 *
 *    Description:  Traversable trait
 *
 *        Version:  1.0
 *        Created:  08/22/2014 04:20:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef TRAVERSABLE_9YOIEPI9
#define TRAVERSABLE_9YOIEPI9

namespace templatious {

template <class T>
struct Traversable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    Iter begin() {
        return Ad::begin(getRef());
    }

    Iter end() {
        return Ad::end(getRef());
    }

    CIter cbegin() {
        return Ad::cbegin(getRef());
    }

    CIter cend() {
        return Ad::cend(getRef());
    }

protected:
    virtual ThisCol& getRef() = 0;
};

}

#endif /* end of include guard: TRAVERSABLE_9YOIEPI9 */
