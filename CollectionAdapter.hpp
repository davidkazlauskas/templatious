//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  StaticAdapter.hpp
 *
 *    Description:  Generic collection adapter for collections and alike.
 *
 *        Version:  1.0
 *        Created:  08/18/2014 07:38:12 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef COLLECTIONADAPTER_728YVC81
#define COLLECTIONADAPTER_728YVC81

#include <type_traits>
#include <array>
#include <memory>
#include <initializer_list>

#include <templatious/util/Exceptions.hpp>

namespace templatious {
namespace adapters {

TEMPLATIOUS_BOILERPLATE_EXCEPTION( CollectionAdapterNoSuchElementException,
    "Trying to get non-existing element in a collection.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION( CollectionAdapterNoSuchIteratorException,
    "Trying to get non-existing iterator in a collection.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION( CollectionAdapterIteratorOutOfBoundsException,
    "Iterator passed doesn't belong to collection.");

template <class T>
struct CollectionAdapter {
    static const bool is_valid = false;
    static const bool floating_iterator = false;

    typedef void* ThisCol;
    typedef const void* ConstCol;
    typedef void* Iterator;
    typedef const void* ConstIterator;
    typedef void* ValueType;
    typedef const void* ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i);
    template <class V>
    static void insertAt(ThisCol& c, Iterator at, V&& i);

    static ValueType& getByIndex(ThisCol& c, long i);
    static ConstValueType& getByIndex(ConstCol& c, long i);

    static long size(ConstCol& c);

    static void erase(ThisCol& c, Iterator i);
    static void erase(ThisCol& c, Iterator beg, Iterator end);

    static Iterator begin(ThisCol& c);
    static Iterator end(ThisCol& c);
    static Iterator iterAt(ThisCol& c, long i);

    static ConstIterator begin(ConstCol& c);
    static ConstIterator end(ConstCol& c);
    static ConstIterator iterAt(ConstCol& c, long i);

    static ConstIterator cbegin(ConstCol& c);
    static ConstIterator cend(ConstCol& c);
    static ConstIterator citerAt(ConstCol& c, long i);

    static ValueType& first(ThisCol& c);
    static ConstValueType& first(ConstCol& c);
    static ValueType& last(ThisCol& c);
    static ConstValueType& last(ConstCol& c);

    static void clear(ThisCol& c);
    static bool canAdd(ConstCol& c);
};

}
}

#endif
