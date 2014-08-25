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
    typedef typename Inner::iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.add(i);
    }

    template <class V>
    static void insert_at(ThisCol& c, iterator at, V&& i) {
        c.insert(at,i);
    }

    static value_type& getByIndex(ThisCol& c, size_t i) {
        return c.getByIndex(i);
    }

    static const_value_type& getByIndex(ConstCol& c, size_t i) {
        return c.cgetByIndex(i);
    }

    static size_t getSize(ConstCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c,const iterator& i) {
        return c.erase(i);
    }

    static void erase(ThisCol& c,const iterator& beg,const iterator& end) {
        return c.erase(beg,end);
    }

    template <class U = int>
    static ThisCol instantiate() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Virtual collection shall be wrapped around actual collection, "
                "not instantiated on its own.");
    }

    template <class U = int>
    static ThisCol instantiate(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Virtual collection shall be wrapped around actual collection, "
                "not instantiated on its own.");
    }

    template <class U = int>
    static ThisCol* instHeap() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Virtual collection shall be wrapped around actual collection, "
                "not instantiated on its own.");
    }

    template <class U = int>
    static ThisCol* instHeap(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Virtual collection shall be wrapped around actual collection, "
                "not instantiated on its own.");
    }

    static iterator begin(ThisCol& c) {
        return c.begin();
    }

    static iterator end(ThisCol& c) {
        return c.end();
    }

    static iterator iter_at(ThisCol& c, size_t i) {
        return c.iterAt(i);
    }

    static const_iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator end(ConstCol& c) {
        return c.cend();
    }

    static const_iterator iter_at(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static const_iterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator cend(ConstCol& c) {
        return c.cend();
    }

    static const_iterator citer_at(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static value_type& first(ThisCol& c) {
        return c.first();
    }

    static const_value_type& first(ConstCol& c) {
        return c.cfirst();
    }

    static value_type& last(ThisCol& c) {
        return c.last();
    }

    static const_value_type& last(ConstCol& c) {
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
    typedef typename Inner::const_iterator iterator;
    typedef typename Inner::const_iterator const_iterator;
    typedef typename Inner::const_value_type value_type;
    typedef typename Inner::const_value_type const_value_type;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.add(i);
    }

    template <class V>
    static void insert_at(ThisCol& c, iterator at, V&& i) {
        c.insert(at,i);
    }

    static const_value_type& getByIndex(ConstCol& c, size_t i) {
        return c.cgetByIndex(i);
    }

    static size_t getSize(ConstCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c,const iterator& i) {
        return c.erase(i);
    }

    static void erase(ThisCol& c,const iterator& beg,const iterator& end) {
        return c.erase(beg,end);
    }

    template <class U = int>
    static ThisCol instantiate() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Virtual collection shall be wrapped around actual collection, "
                "not instantiated on its own.");
    }

    template <class U = int>
    static ThisCol instantiate(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Virtual collection shall be wrapped around actual collection, "
                "not instantiated on its own.");
    }

    template <class U = int>
    static ThisCol* instHeap() {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Virtual collection shall be wrapped around actual collection, "
                "not instantiated on its own.");
    }

    template <class U = int>
    static ThisCol* instHeap(size_t size) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Virtual collection shall be wrapped around actual collection, "
                "not instantiated on its own.");
    }

    static const_iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator end(ConstCol& c) {
        return c.cend();
    }

    static const_iterator iter_at(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static const_iterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator cend(ConstCol& c) {
        return c.cend();
    }

    static const_iterator citer_at(ConstCol& c, size_t i) {
        return c.citerAt(i);
    }

    static const_value_type& first(ConstCol& c) {
        return c.cfirst();
    }

    static const_value_type& last(ConstCol& c) {
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

