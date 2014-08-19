/*
 * =====================================================================================
 *
 *       Filename:  CollectionSocket.h
 *
 *    Description:  Socket for adding to collections
 *
 *        Version:  1.0
 *        Created:  08/17/2014 02:34:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef COLLECTIONSOCKET_18UW7ASG
#define COLLECTIONSOCKET_18UW7ASG

#include <templatious/CollectionAdapter.h>

#include <templatious/virtual/Socket.h>

namespace templatious {

template <class T>
struct CollectionSocket:
    public Socket< typename adapters::CollectionAdapter<T>::value_type >
{
private:
    T& _c;
    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::value_type ValueType;
    static_assert(Ad::is_valid,
            "Collection passed to CollectionSocket has invalid adapter.");

public:

    CollectionSocket(T& c) : _c(c) {}

    virtual void add(ValueType& a) {
        Ad::add(_c,a);
    }

    virtual void add(const ValueType& a) {
        Ad::add(_c,a);
    }

};

}

#endif /* end of include guard: COLLECTIONSOCKET_18UW7ASG */
