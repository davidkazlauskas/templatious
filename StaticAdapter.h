/*
 * =====================================================================================
 *
 *       Filename:  StaticAdapter.h
 *
 *    Description:  Static adapter for convenience
 *
 *        Version:  1.0
 *        Created:  08/18/2014 07:38:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef STATICADAPTER_ONGU27R7
#define STATICADAPTER_ONGU27R7

#include <assert.h>

#include <templatious/CollectionAdapter.h>
#include <templatious/virtual/Iterator.h>

namespace templatious {
namespace sa_spec {

template <class T, class U>
struct AdditionSelector {
    static const bool areAdaptable =
        templatious::adapters::CollectionAdapter<T>::is_valid
        && templatious::adapters::CollectionAdapter<U>::is_valid;
    enum {
        val = templatious::util::IntSelector<
            areAdaptable, 1, templatious::util::IntSelector<
                                 std::is_array<U>::value, 2, 0>::val>::val
    };
};

template <int var>
struct add_custom;

template <>  // add data one by one
struct add_custom<0> {
    template <class T>
    static void add(T& c,typename templatious::adapters::CollectionAdapter<T>::const_value_type& i) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::add(c, i);
    }
};

template <>  // add collections
struct add_custom<1> {
    template <class T, class U>
    static void add(T& t,const U& u) {
        typedef templatious::adapters::CollectionAdapter<T> AdT;
        typedef templatious::adapters::CollectionAdapter<const U> AdU;
        static_assert(AdT::is_valid, "Adapter not supported.");
        static_assert(AdU::is_valid, "Adapter not supported.");
        for (auto i = AdU::begin(u); i != AdU::end(u); ++i) {
            AdT::add(t, *i);
        }
    }
};

template <>  // add static arrays
struct add_custom<2> {
    template <class T, class Arr, unsigned long count>
    static void add(T& c, const Arr (&arr)[count]) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        for (int i = 0; i < count; ++i) {
            Ad::add(c, arr[i]);
        }
    }
};
}

struct StaticAdapter {
    template <class T>
    static auto begin(T&& c)
        -> decltype(adapters::CollectionAdapter<T>::begin(c)) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::begin(c);
    }

    template <class T>
    static auto end(T&& c) -> decltype(adapters::CollectionAdapter<T>::end(c)) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::end(c);
    }

    template <class T>
    static auto cbegin(const T& c)
        -> typename adapters::CollectionAdapter<T>::const_iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::cbegin(c);
    }

    template <class T>
    static auto cend(const T& c)
        -> typename adapters::CollectionAdapter<T>::const_iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::cend(c);
    }

    template <class T, class U>
    static void add(T& c, const std::initializer_list<U>& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        for (auto i = o.begin(); i != o.end(); ++i) {
            Ad::add(c,*i);
        }
    }

    template <class T, class U>
    static void add(T& c, U&& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        sa_spec::add_custom<
                sa_spec::AdditionSelector<T, U>::val
            >::add(c, std::forward<U>(o));
    }

    template <class T, class U, class... Args>
    static void add(T& c, U&& o, Args&&... args) {
        add(c, std::forward<U>(o));

        add(c,std::forward<Args>(args)...);
    }

    template <class T>
    static T instantiate() {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::instantiate();
    }

    template <class T>
    static T instantiate(int size) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::instantiate(size);
    }

    template <class T>
    static size_t getSize(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::getSize(c);
    }

    template <class T>
    static auto getByIndex(T& c, int i)
        -> typename adapters::CollectionAdapter<T>::value_type {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::getByIndex(c, i);
    }

    template <class T>
    static void erase(T& c,
                      typename adapters::CollectionAdapter<T>::iterator beg,
                      typename adapters::CollectionAdapter<T>::iterator end) 
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, beg, end);
    }

    template <class T>
    static void erase(T& c,
                      typename adapters::CollectionAdapter<T>::iterator pos) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, pos);
    }

    template <class T>
    static void eraseTillEnd(T& c,
         typename adapters::CollectionAdapter<T>::iterator pos)
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, pos, Ad::end(c));
    }

    template <class T>
    static auto iterAt(T& c,size_t i)
        -> typename adapters::CollectionAdapter<T>::iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::iter_at(c, i);
    }

    template <class T>
    static auto citerAt(T& c,size_t i)
        -> typename adapters::CollectionAdapter<T>::const_iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::citer_at(c, i);
    }

    template <class T>
    static void insert(
        T& c, typename adapters::CollectionAdapter<T>::iterator at,
        const typename adapters::CollectionAdapter<T>::value_type& val) 
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::insert_at(c,at,val);
    }

    template <class T>
    static void clear(T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::clear(c);
    }

    template <class T>
    static auto vbegin(T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::value_type >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::value_type ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        VImpl *v = new VImpl( Ad::begin(c) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto vend(T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::value_type >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::value_type ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        VImpl *v = new VImpl( Ad::end(c) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto vcbegin(const T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::const_value_type >
    {
        typedef adapters::CollectionAdapter<const T> Ad;
        typedef typename Ad::const_value_type ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        VImpl *v = new VImpl( Ad::cbegin(c) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto vcend(const T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::const_value_type >
    {
        typedef adapters::CollectionAdapter<const T> Ad;
        typedef typename Ad::const_value_type ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        VImpl *v = new VImpl( Ad::cend(c) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto viterAt(T& c,size_t s)
    -> VIterator< typename adapters::CollectionAdapter<T>::value_type >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::value_type ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        VImpl *v = new VImpl( Ad::iter_at(c,s) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto vciterAt(const T& c,size_t s)
    -> VIterator< typename adapters::CollectionAdapter<T>::const_value_type >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::const_value_type ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        VImpl *v = new VImpl( Ad::citer_at(c,s) );
        return VIterator< ValType >(v);
    }

    // will be removed in the future.
    template <bool reverse = false,class T,class Comp = typename templatious::util::Default>
    static void sortedAdd(T& c, const typename adapters::CollectionAdapter<T>::value_type& val) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::value_type ValType;

        typedef typename templatious::util::ComparatorDiff<ValType,ValType,Comp> Comparator;
        auto comp = templatious::util::rev<reverse>(Comparator());

        if (0 == Ad::getSize(c)) {
            Ad::add(c,val);
        }

        if (0 >= comp(Ad::last(c),val)) {
            Ad::add(c,val);
        }

        if (0 <= comp(Ad::first(c),val)) {
            Ad::insert_at(c,Ad::begin(c),val);
        }

        for (auto i = Ad::begin(c); i != Ad::end(c); ++i) {
            if (0 <= comp(*i,val)) {
                Ad::insert_at(c,i,val);
            }
        }

        assert(false);
    }

};

}

#endif /* end of include guard: STATICADAPTER_ONGU27R7 */
