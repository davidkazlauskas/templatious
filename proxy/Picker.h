/*
 * =====================================================================================
 *
 *       Filename:  Picker.h
 *
 *    Description:  Proxy picker
 *
 *        Version:  1.0
 *        Created:  08/01/2014 06:59:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef PICKER_RLHXNUK4
#define PICKER_RLHXNUK4

#include <utility>
#include <vector>

#include <templatious/util/Exceptions.h>
#include <templatious/CollectionAdapter.h>
#include <templatious/StaticAdapter.h>

namespace templatious {
namespace detail {

TEMPLATIOUS_BOILERPLATE_EXCEPTION( ProxyClearedUsageException,
    "Cleared proxy cannot be traversed anymore." );

template <class T>
struct IsProxy {
    static const bool val = false;

    typedef adapters::CollectionAdapter<T> Ad;
    typedef typename Ad::ConstIterator ConstIterator;
    typedef T ICollection;
    typedef Ad IAdapter;

    static const bool random_access_iterator = Ad::floating_iterator;

    template <class V>
    static V&& unwrap(V&& c) {
        return std::forward<V>(c);
    }

    template <class V>
    static V&& iter_unwrap(V&& i) {
        return std::forward<V>(i);
    }

    typedef int Dist;
    template <class U>
    static Dist get_mul(U&& u) {
        return 1;
    }

    template <class U>
    static void tag_cleared(U&& u) { }

    template <class Iter>
    static ConstIterator const_iter_cast(Iter&& i)
    {
        return ConstIterator(std::forward<Iter>(i));
    }
};

template <class T>
struct IsProxy<T&&> {
    typedef IsProxy<T> Internal;
    static const bool val = Internal::val;

    typedef typename Internal::Ad Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename Internal::IAdapter IAdapter;

    static const bool random_access_iterator
        = Internal::random_access_iterator;

    template <class C>
    static auto unwrap(C&& c)
        -> decltype( Internal::unwrap(std::forward<C>(c)) )
    {
        return Internal::unwrap(
                std::forward<C>(c));
    }

    template <class C>
    static auto iter_unwrap(C&& c)
        -> decltype( Internal::iter_unwrap(std::forward<C>(c)) )
    {
        return Internal::iter_unwrap(
                std::forward<C>(c));
    }

    typedef int Dist;
    template <class U>
    static Dist get_mul(U&& u) {
        return Internal::get_mul(
                std::forward<U>(u));
    }

    template <class U>
    static void tag_cleared(U&& u) {
        Internal::tag_cleared(std::forward<U>(u));
    }

    template <class Iter>
    static auto const_iter_cast(Iter&& i)
     -> decltype(Internal::const_iter_cast(std::forward<Iter>(i)))
    {
        return Internal::const_iter_cast(std::forward<Iter>(i));
    }
};

template <class T>
struct IsProxy<const T&&> {
    typedef IsProxy<T> Internal;
    static const bool val = Internal::val;

    typedef typename Internal::Ad Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename Internal::IAdapter IAdapter;

    static const bool random_access_iterator
        = Internal::random_access_iterator;

    template <class C>
    static auto unwrap(C&& c)
        -> decltype( Internal::unwrap(std::forward<C>(c)) )
    {
        return Internal::unwrap(
                std::forward<C>(c));
    }

    template <class C>
    static auto iter_unwrap(C&& c)
        -> decltype( Internal::iter_unwrap(std::forward<C>(c)) )
    {
        return Internal::iter_unwrap(
                std::forward<C>(c));
    }

    typedef int Dist;
    template <class U>
    static Dist get_mul(U&& u) {
        return Internal::get_mul(
                std::forward<U>(u));
    }

    template <class U>
    static void tag_cleared(U&& u) {
        Internal::tag_cleared(std::forward<U>(u));
    }

    template <class Iter>
    static auto const_iter_cast(Iter&& i)
     -> decltype(Internal::const_iter_cast(std::forward<Iter>(i)))
    {
        return Internal::const_iter_cast(std::forward<Iter>(i));
    }
};

template <class T>
struct IsProxy<T&> {
    typedef IsProxy<T> Internal;
    static const bool val = Internal::val;

    typedef typename Internal::Ad Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename Internal::IAdapter IAdapter;

    static const bool random_access_iterator
        = Internal::random_access_iterator;

    template <class C>
    static auto unwrap(C&& c)
        -> decltype( Internal::unwrap(std::forward<C>(c)) )
    {
        return Internal::unwrap(
                std::forward<C>(c));
    }

    template <class C>
    static auto iter_unwrap(C&& c)
        -> decltype( Internal::iter_unwrap(std::forward<C>(c)) )
    {
        return Internal::iter_unwrap(
                std::forward<C>(c));
    }

    typedef int Dist;
    template <class U>
    static Dist get_mul(U&& u) {
        return Internal::get_mul(
                std::forward<U>(u));
    }

    template <class U>
    static void tag_cleared(U&& u) {
        Internal::tag_cleared(std::forward<U>(u));
    }

    template <class Iter>
    static auto const_iter_cast(Iter&& i)
     -> decltype(Internal::const_iter_cast(std::forward<Iter>(i)))
    {
        return Internal::const_iter_cast(std::forward<Iter>(i));
    }
};

template <class T>
struct IsProxy<const T&> {
    typedef IsProxy<T> Internal;
    static const bool val = Internal::val;

    typedef typename Internal::Ad Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename Internal::IAdapter IAdapter;

    static const bool random_access_iterator
        = Internal::random_access_iterator;

    template <class C>
    static auto unwrap(C&& c)
        -> decltype( Internal::unwrap(std::forward<C>(c)) )&
    {
        return Internal::unwrap(
                std::forward<C>(c));
    }

    template <class C>
    static auto iter_unwrap(C&& c)
        -> decltype( Internal::iter_unwrap(std::forward<C>(c)) )&
    {
        return Internal::iter_unwrap(
                std::forward<C>(c));
    }

    typedef int Dist;
    template <class U>
    static Dist get_mul(U&& u) {
        return Internal::get_mul(
                std::forward<U>(u));
    }

    template <class U>
    static void tag_cleared(U&& u) {
        Internal::tag_cleared(std::forward<U>(u));
    }

    template <class Iter>
    static auto const_iter_cast(Iter&& i)
     -> decltype(Internal::const_iter_cast(std::forward<Iter>(i)))
    {
        return Internal::const_iter_cast(std::forward<Iter>(i));
    }
};


template <bool floating_iterator,class C>
void clearRoutine(C&& c) {
    typedef IsProxy<C> ProxUtil;
    typedef decltype(ProxUtil::unwrap(c)) ICol;
    typedef StaticAdapter SA;

    ICol& ic = ProxUtil::unwrap(c);

    if (floating_iterator) {
        auto i = SA::begin(ic);
        auto j = i;
        auto next = SA::begin(c);
        auto end = SA::end(c);
        bool endReached = false;

        while (i != SA::end(ic)) {
            if (i != j) {
                *j = *i;
            }
            if (ProxUtil::iter_unwrap(next) != i
                || endReached)
            {
                ++j;
            } else if (end != next) {
                ++next;
                if (end == next) {
                    endReached = true;
                }
            }

            ++i;
        }
        SA::erase(ic,j,SA::end(ic));
    } else {
        bool flag;
        auto beg = SA::begin(c);
        for (auto i = SA::begin(ic);
            i != SA::end(ic) && beg != SA::end(c);
            ({if (!flag) ++i;}))
        {
            flag = false;
            auto& u = ProxUtil::iter_unwrap(beg);
            if (i == u) {
                flag = true;
                auto cpy = i;
                ++i;
                ++beg;
                SA::erase(ic,cpy);
            }
        }
    }
}

template <class T>
void naiveIterAdvance(T& i,const T& end,size_t t) {
    for (size_t j = 0; j < t; ++j) {
        if (i == end) {
            return;
        }
        ++i;
    }
}

template <bool naiveAdvance>
struct AdvancePicker {

    template <class T>
    static void adv(T& i,const T& end,size_t t) {
        naiveIterAdvance(i,end,t);
    }

};

template <>
struct AdvancePicker<false> {

    template <class T>
    static void adv(T& i,const T& end,size_t t) {
        // std::advance could be used but
        // assumption is made that ONLY
        // random access iterator will be used here
        i += t;
        if (i > end) {
            i = end;
        }
        assert(i <= end && "What the flock?..");
    }

};

}
}

#endif /* end of include guard: PICKER_RLHXNUK4 */
