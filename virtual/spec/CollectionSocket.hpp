//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  CollectionSocket.hpp
 *
 *    Description:  Socket for adding to collections
 *
 *        Version:  1.0
 *        Created:  08/17/2014 02:34:24 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef COLLECTIONSOCKET_18UW7ASG
#define COLLECTIONSOCKET_18UW7ASG

#include <templatious/CollectionAdapter.hpp>

#include <templatious/virtual/Socket.hpp>

namespace templatious {

template <class T>
struct CollectionSocket:
    public Socket< typename adapters::CollectionAdapter<T>::ValueType >
{
private:
    T& _c;
    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::ValueType ValueType;
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
