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
#include <templatious/Pack.h>

namespace templatious {
namespace sa_spec {

enum AdditionVariant { Data, Collection, Pack, Invalid };

template <class T, class U, class Function>
struct AdditionSelector {
    typedef templatious::adapters::CollectionAdapter<T> AdT;
    typedef templatious::adapters::CollectionAdapter<U> AdU;

    typedef templatious::util::IsCallableWith< Function, U > ICval;
    typedef typename ICval::type FwdType;

    static const bool areAdaptable =
        AdT::is_valid && AdU::is_valid;

    static const bool isConvertable =
        std::is_convertible<
            FwdType, typename AdT::ValueType
        >::value;

    template <bool validAdapter,class ValType>
    struct IsInnerConvertable {
        typedef templatious::adapters::CollectionAdapter<ValType> Ad;
        typedef typename Ad::ValueType VType;
        typedef templatious::util::IsCallableWith<
            Function, VType > ICInner;
        typedef typename ICInner::type FwdType;
    };

    template <class ValType>
    struct IsInnerConvertable<false,ValType> {
        typedef templatious::util::InvalidType FwdType;
    };

    typedef IsInnerConvertable<
        AdU::is_valid,U> InnerConv;

    static const bool innerConv =
        std::is_convertible<
            typename InnerConv::FwdType, typename AdT::ValueType
        >::value;

    enum {
        val = templatious::util::IntSelector<
            isConvertable,
            AdditionVariant::Data,
            templatious::util::IntSelector<
                innerConv,
                AdditionVariant::Collection,
                templatious::util::IntSelector<
                    templatious::detail::IsPack<U>::val,
                    AdditionVariant::Pack,
                    AdditionVariant::Invalid
                >::val
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
    static void add(T& t, F&& f, U&& u) {
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

template <>  // add invalid overload for friendly user message
struct add_custom< AdditionVariant::Invalid > {
    template <class T, class F, class... Any>
    static void add(T& c, F&& f, Any&&... anything) {
        static_assert(templatious::util::DummyResolver<T,false>::val,
            "Type could not be added to collection.");
    }
};

template <>  // add static arrays
struct add_custom< AdditionVariant::Pack > {
    typedef add_custom< AdditionVariant::Pack > ThisAddition;

    template <
         int which = 0,
         class T, class F,class P
    >
    static void add(
        T& c, F&& f,
        P&& p)
    {
        typedef typename std::conditional<
            which < std::decay<P>::type::size,
            AddNextVar,
            EmptyVar
        >::type Var;

        Var::template addInternal<
            which
        >(
            c,
            std::forward<F>(f),
            std::forward<P>(p)
        );
    }

private:
    struct AddNextVar {
        template <
            int which,
            class T, class F,
            class P
        >
        static void addInternal(
            T& c,F&& f,
            P&& p)
        {
            typedef templatious::adapters::CollectionAdapter<T> Ad;
            Ad::add(c,f(p.template get<which>()));
            ThisAddition::add< which + 1 >(
                c,
                std::forward<F>(f),
                std::forward<P>(p)
            );
        }
    };

    struct EmptyVar {
        // do nothing
        template <int which,class... Args>
        static void addInternal(Args&&... args) { }
    };
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
        auto end = o.end();
        for (auto i = o.begin(); i != end; ++i) {
            Ad::add(c,fwd(*i));
        }
    }

    template <class T, class U>
    static void add(T& c, U&& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef sa_spec::ForwardFunctor Fwd;
        Fwd fwd;
        typedef decltype(std::forward<U>(o)) UTrue;
        sa_spec::add_custom<
            sa_spec::AdditionSelector<T, UTrue, Fwd>::val
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
        typedef decltype(std::forward<F>(f)) Fwd;
        typedef decltype(std::forward<U>(o)) UTrue;
        sa_spec::add_custom<
            sa_spec::AdditionSelector<T, UTrue, Fwd>::val
        >::add(c, std::forward<F>(f), std::forward<U>(o));
    }

    template <class T, class F, class U, class... Args>
    static void addCustom(T& c, F&& f, U&& o, Args&&... args) {
        addCustom(c, std::forward<F>(f), std::forward<U>(o));

        addCustom(c, std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <class T>
    static int size(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::size(c);
    }

    template <class T>
    static int trueSize(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        int initial = size(c);
        if (initial != -1) {
            return initial;
        }

        int count = 0;
        auto e = cend(c);
        for (auto i = cbegin(c); i != e; ++i) {
            ++count;
        }

        return count;
    }

    template <class T>
    static bool canAdd(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::canAdd(c);
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

    template <class T,class... Tail>
    static void clear(T& c,Tail&... t) {
        clear(c);
        clear(t...);
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

};

}

#endif /* end of include guard: STATICADAPTER_ONGU27R7 */
