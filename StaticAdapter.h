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

enum AdditionVariant { Data, Collection, StaticArrays };

template <class T, class U>
struct AdditionSelector {
    static const bool areAdaptable =
        templatious::adapters::CollectionAdapter<T>::is_valid
        && templatious::adapters::CollectionAdapter<U>::is_valid;
    enum {
        val = templatious::util::IntSelector<
            areAdaptable,
            AdditionVariant::Collection,
            templatious::util::IntSelector<
                std::is_array<U>::value,
                AdditionVariant::StaticArrays,
                AdditionVariant::Data
            >::val
        >::val
    };
};

struct ForwardFunctor {
    template <class T>
    auto operator()(T&& t)
     -> decltype(std::forward<T>(t))
    {
        return std::forward<T>(t);
    }
};

template <int var>
struct add_custom;

template <>  // add data one by one
struct add_custom< AdditionVariant::Data > {
    template <class T,class F,class U>
    static void add(T& c,F&& f,U&& i) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::add(c, f(i));
    }
};

template <>  // add collections
struct add_custom< AdditionVariant::Collection > {
    template <class T, class F, class U>
    static void add(T& t, F&& f, const U& u) {
        typedef templatious::adapters::CollectionAdapter<T> AdT;
        typedef templatious::adapters::CollectionAdapter<const U> AdU;
        static_assert(AdT::is_valid, "Adapter not supported.");
        static_assert(AdU::is_valid, "Adapter not supported.");
        // cache end because compiler doesn't
        // seem to optimize out adapter end calls
        // on rvalue case.
        auto e = AdU::end(u);
        for (auto i = AdU::begin(u); i != e; ++i) {
            AdT::add(t, f(*i));
        }
    }
};

template <>  // add static arrays
struct add_custom< AdditionVariant::StaticArrays > {
    template <class T, class F, class Arr, unsigned long count>
    static void add(T& c, F&& f, const Arr (&arr)[count]) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        for (int i = 0; i < count; ++i) {
            Ad::add(c, f(arr[i]));
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
        -> typename adapters::CollectionAdapter<T>::ConstIterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::cbegin(c);
    }

    template <class T>
    static auto cend(const T& c)
        -> typename adapters::CollectionAdapter<T>::ConstIterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::cend(c);
    }

    template <class T, class U>
    static void add(T& c, const std::initializer_list<U>& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        sa_spec::ForwardFunctor fwd;
        for (auto i = o.begin(); i != o.end(); ++i) {
            Ad::add(c,fwd(*i));
        }
    }

    template <class T, class U>
    static void add(T& c, U&& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        sa_spec::ForwardFunctor fwd;
        sa_spec::add_custom<
                sa_spec::AdditionSelector<T, U>::val
            >::add(c,fwd,std::forward<U>(o));
    }

    template <class T, class U, class... Args>
    static void add(T& c, U&& o, Args&&... args) {
        sa_spec::ForwardFunctor fwd;
        addCustom(c, fwd, std::forward<U>(o));

        addCustom(c, fwd, std::forward<Args>(args)...);
    }

    template <class T, class F, class U>
    static void addCustom(T& c, F&& f, U&& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        sa_spec::add_custom<
                sa_spec::AdditionSelector<T, U>::val
            >::add(c, std::forward<F>(f), std::forward<U>(o));
    }

    template <class T, class F, class U, class... Args>
    static void addCustom(T& c, F&& f, U&& o, Args&&... args) {
        addCustom(c, std::forward<F>(f), std::forward<U>(o));

        addCustom(c, std::forward<F>(f), std::forward<Args>(args)...);
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
    static int size(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::size(c);
    }

    template <class T>
    static auto getByIndex(T& c, int i)
        -> typename adapters::CollectionAdapter<T>::ValueType {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::getByIndex(c, i);
    }

    template <class T>
    static void erase(T& c,
                      typename adapters::CollectionAdapter<T>::Iterator beg,
                      typename adapters::CollectionAdapter<T>::Iterator end)
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, beg, end);
    }

    template <class T>
    static void erase(T& c,
                      typename adapters::CollectionAdapter<T>::Iterator pos) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, pos);
    }

    template <class T>
    static void eraseTillEnd(T& c,
         typename adapters::CollectionAdapter<T>::Iterator pos)
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::erase(c, pos, Ad::end(c));
    }

    template <class T>
    static auto iterAt(T& c,size_t i)
        -> typename adapters::CollectionAdapter<T>::Iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::iterAt(c, i);
    }

    template <class T>
    static auto citerAt(T& c,size_t i)
        -> typename adapters::CollectionAdapter<T>::ConstIterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::citerAt(c, i);
    }

    template <class T>
    static void insert(
        T& c, typename adapters::CollectionAdapter<T>::Iterator at,
        const typename adapters::CollectionAdapter<T>::ValueType& val)
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::insertAt(c,at,val);
    }

    template <class T>
    static void clear(T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::clear(c);
    }

    template <class T>
    static auto vbegin(T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::ValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        VImpl *v = new VImpl( Ad::begin(c) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto vend(T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::ValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        VImpl *v = new VImpl( Ad::end(c) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto vcbegin(const T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::ConstValueType >
    {
        typedef adapters::CollectionAdapter<const T> Ad;
        typedef typename Ad::ConstValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        VImpl *v = new VImpl( Ad::cbegin(c) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto vcend(const T& c)
    -> VIterator< typename adapters::CollectionAdapter<T>::ConstValueType >
    {
        typedef adapters::CollectionAdapter<const T> Ad;
        typedef typename Ad::ConstValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        VImpl *v = new VImpl( Ad::cend(c) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto viterAt(T& c,size_t s)
    -> VIterator< typename adapters::CollectionAdapter<T>::ValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::ValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<T> VImpl;
        VImpl *v = new VImpl( Ad::iterAt(c,s) );
        return VIterator< ValType >(v);
    }

    template <class T>
    static auto vciterAt(const T& c,size_t s)
    -> VIterator< typename adapters::CollectionAdapter<T>::ConstValueType >
    {
        typedef adapters::CollectionAdapter<T> Ad;
        typedef typename Ad::ConstValueType ValType;
        static_assert(Ad::is_valid, "Adapter not supported.");

        typedef VIteratorImpl<const T> VImpl;
        VImpl *v = new VImpl( Ad::citerAt(c,s) );
        return VIterator< ValType >(v);
    }

    // will be removed in the future.
    template <bool reverse = false,class T,class Comp = typename templatious::util::Default>
    static void sortedAdd(T& c, const typename adapters::CollectionAdapter<T>::ValueType& val) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::ValueType ValType;

        typedef typename templatious::util::ComparatorDiff<ValType,ValType,Comp> Comparator;
        auto comp = templatious::util::rev<reverse>(Comparator());

        if (0 == Ad::size(c)) {
            Ad::add(c,val);
        }

        if (0 >= comp(Ad::last(c),val)) {
            Ad::add(c,val);
        }

        if (0 <= comp(Ad::first(c),val)) {
            Ad::insertAt(c,Ad::begin(c),val);
        }

        for (auto i = Ad::begin(c); i != Ad::end(c); ++i) {
            if (0 <= comp(*i,val)) {
                Ad::insertAt(c,i,val);
            }
        }

        assert(false);
    }

};

}

#endif /* end of include guard: STATICADAPTER_ONGU27R7 */
