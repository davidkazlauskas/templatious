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

#include <templatious/CollectionAdapter.h>

namespace templatious {

template <class T>
struct IsProxy {
    static const bool val = false;

    typedef void Ad;
    typedef T ICollection;
    typedef adapters::CollectionAdapter<T> IAdapter;

    template <class V>
    static V&& unwrap(V&& c) {
        return c;
    }

    template <class V>
    static V&& iter_unwrap(V&& i) {
        return i;
    }
};

template <bool floating_iterator,class C>
void clearRoutine(C&& c) {
    typedef IsProxy<C> ProxUtil;
    typedef decltype(ProxUtil::unwrap(c)) ICol;
    typedef adapters::CollectionAdapter<C> Ad;
    typedef adapters::CollectionAdapter<ICol> IAd;
    typedef adapters::CollectionAdapter<
        std::vector<typename IAd::iterator> > CAd;
    typedef StaticAdapter SA;

    ICol ic = ProxUtil::unwrap(c);

    if (floating_iterator) {
        auto vi = CAd::instantiate(SA::getSize(ic));

        for (auto i = SA::begin(c);
             i != SA::end(c);
             ++i)
        {
            SA::add(vi,ProxUtil::iter_unwrap(i));
        }

        auto res = IAd::instantiate(
            SA::getSize(ic) - SA::getSize(vi));

        auto beg = SA::begin(vi);
        for (auto i = SA::begin(ic);
            i != SA::end(ic);
            ++i)
        {
            if ((*beg) != i) {
                SA::add(res,std::move(*i));
            } else {
                ++beg;
            }
        }

        ic = res;
    } else {
        bool flag;
        auto beg = SA::begin(c);
        for (auto i = SA::begin(ic);
            i != SA::end(ic);
            ({if (!flag) ++i;}))
        {
            flag = false;
            auto u = ProxUtil::unwrap_iter(beg);
            if (i == u) {
                flag = true;
                SA::erase(ic,i);
                ++beg;
            }
        }
    }
}

template <class T>
struct IsProxy<T&&> {
    typedef IsProxy<T> Internal;
    static const bool val = Internal::val;

    typedef typename Internal::Ad Ad;
    typedef typename Ad::ThisCol ICollection;
    typedef typename Internal::IAdapter IAdapter;

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
};

}

#endif /* end of include guard: PICKER_RLHXNUK4 */
