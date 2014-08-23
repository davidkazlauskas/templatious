/*
 * =====================================================================================
 *
 *       Filename:  Addable.h
 *
 *    Description:  Addable collection trait
 *
 *        Version:  1.0
 *        Created:  08/23/2014 01:19:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ADDABLE_T69BV8TV
#define ADDABLE_T69BV8TV

namespace templatious {

template <class T>
struct Addable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::value_type ValType;
    typedef typename Ad::const_value_type CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    Addable(ThisCol& t) : T(t) {}

    void add(const ValType& i) {
        Ad::add(getRef(),i);
    }

    void insert(const Iter& at,const ValType& i) {
        Ad::insert_at(getRef(),at,i);
    }

protected:
    ThisCol& getRef() {
        return T::getRef();
    }
};

}

#endif /* end of include guard: ADDABLE_T69BV8TV */
