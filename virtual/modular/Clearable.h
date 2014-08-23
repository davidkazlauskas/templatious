/*
 * =====================================================================================
 *
 *       Filename:  Clearable.h
 *
 *    Description:  Clearable virtual collection
 *
 *        Version:  1.0
 *        Created:  08/23/2014 05:28:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef CLEARABLE_D3H5AWSJ
#define CLEARABLE_D3H5AWSJ

namespace templatious {


template <class T>
struct Clearable: public T {
    typedef typename T::Ad Ad;

    typedef typename Ad::iterator Iter;
    typedef typename Ad::const_iterator CIter;
    typedef typename Ad::value_type ValType;
    typedef typename Ad::const_value_type CValType;
    typedef typename Ad::ThisCol ThisCol;
    typedef typename Ad::ConstCol ConstCol;

    void erase(const Iter& i) {
        Ad::erase(getRef(),i);
    }

    void erase(const Iter& beg,const Iter& end) {
        Ad::erase(getRef(),beg,end);
    }

    void clear() {
        Ad::clear(getRef());
    }

    Clearable(ThisCol& t) : T(t) {}

protected:
    ThisCol& getRef() {
        return T::getRef();
    }
};

}

#endif /* end of include guard: CLEARABLE_D3H5AWSJ */
