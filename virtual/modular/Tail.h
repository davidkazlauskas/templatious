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
namespace vmodular {

template <
    class T
>
struct Tail: public T {
    typedef typename T::Ad Ad;
    typedef typename Ad::ThisCol ThisCol;

    Tail(ThisCol& c) : T(c) {}

protected:
    ThisCol& getRef() {
        return T::getRef();
    }
};

}

namespace adapters {

template <class T>
struct CollectionAdapter< ::templatious::vmodular::Tail<T> > {
    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef typename T::Ad Inner;

    typedef typename ::templatious::vmodular::Tail<T> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename Inner::Iterator Iterator;
    typedef typename Inner::ConstIterator ConstIterator;
    typedef typename Inner::ValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.add(i);
    }

    template <class V>
    static void insertAt(ThisCol& c, Iterator at, V&& i) {
        c.insert(at,i);
    }

    static ValueType& getByIndex(ThisCol& c, size_t i) {
        return c.getByIndex(i);
    }

    static ConstValueType& getByIndex(ConstCol& c, size_t i) {
        return c.cgetByIndex(i);
    }

    static long size(ConstCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c,const Iterator& i) {
        return c.erase(i);
    }

    static void erase(ThisCol& c,const Iterator& beg,const Iterator& end) {
        return c.erase(beg,end);
    }

    static Iterator begin(ThisCol& c) {
        return c.begin();
    }

    static Iterator end(ThisCol& c) {
        return c.end();
    }

    static Iterator iterAt(ThisCol& c, size_t i) {
        return c.iterAt(i);
    }

    static ConstIterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator end(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator iterAt(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator citerAt(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static ValueType& first(ThisCol& c) {
        return c.first();
    }

    static ConstValueType& first(ConstCol& c) {
        return c.cfirst();
    }

    static ValueType& last(ThisCol& c) {
        return c.last();
    }

    static ConstValueType& last(ConstCol& c) {
        return c.clast();
    }

    static void clear(ThisCol& c) {
        return c.clear();
    }

    static bool canAdd(ConstCol& c) {
        return c.canAdd();
    }
};

template <class T>
struct CollectionAdapter< const ::templatious::vmodular::Tail<T> > {
    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef typename T::Ad Inner;

    typedef const ::templatious::vmodular::Tail<T> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename Inner::ConstIterator Iterator;
    typedef typename Inner::ConstIterator ConstIterator;
    typedef typename Inner::ConstValueType ValueType;
    typedef typename Inner::ConstValueType ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.add(i);
    }

    template <class V>
    static void insertAt(ThisCol& c, Iterator at, V&& i) {
        c.insert(at,i);
    }

    static ConstValueType& getByIndex(ConstCol& c, size_t i) {
        return c.cgetByIndex(i);
    }

    static long size(ConstCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c,const Iterator& i) {
        return c.erase(i);
    }

    static void erase(ThisCol& c,const Iterator& beg,const Iterator& end) {
        return c.erase(beg,end);
    }

    static ConstIterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator end(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator iterAt(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator citerAt(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static ConstValueType& first(ConstCol& c) {
        return c.cfirst();
    }

    static ConstValueType& last(ConstCol& c) {
        return c.clast();
    }

    static void clear(ThisCol& c) {
        return c.clear();
    }

    static bool canAdd(ConstCol& c) {
        return c.canAdd();
    }
};

}
}

#endif /* end of include guard: TAIL_8FSTJFTQ */

