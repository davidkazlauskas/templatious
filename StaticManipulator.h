#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <utility>
#include <type_traits>
#include <assert.h>

#include <templatious/adapters/All.h>
#include <templatious/Utilities.h>
#include <templatious/IterMaker.h>
#include <templatious/Pack.h>

namespace templatious {
namespace detail {

    template <bool isPack>
    struct PackHandler;

}

struct StaticManipulator {
private:

    template <class F, class ITER,bool callWithIndex = false,typename Index = size_t>
    struct IteratorCaller;

    template <class F,class ITER,class Index>
    struct IteratorCaller<F, ITER, false, Index> {
        static auto call(F&& f, Index idx, ITER&& i)
            -> decltype(i.callFunction(std::forward<F>(f)))
        {
            return i.callFunction(std::forward<F>(f));
        }
    };

    template <class F,class ITER,class Index>
    struct IteratorCaller<F, ITER, true, Index> {
        static auto call(F&& f, Index idx, ITER&& i)
            -> decltype(i.callFunction(std::forward<F>(f),
                        idx))
        {
            return i.callFunction(std::forward<F>(f),idx);
        }
    };

    template 
    <
        bool stopAtFirst,
        class T,
        class Out = std::vector<
            typename templatious::adapters::CollectionAdapter<T>::iterator
        >
    >
    static Out findIterInternal(
            T& col,
            const typename templatious::adapters::CollectionAdapter<T>::value_type& v
            )
    {
        namespace ut = templatious::util;
        namespace ad = templatious::adapters;
        typedef typename templatious::StaticAdapter SA;
        typedef typename ad::CollectionAdapter<T> AD;
        typedef typename AD::value_type ValType;
        ut::ComparatorEq<ValType,ValType> comp;

        auto res =
            stopAtFirst ? SA::instantiate<Out>(1) : SA::instantiate<Out>();
        for (auto i = SA::begin(col); i != SA::end(col); ++i) {
            if (comp(*i,v)) {
                SA::add(res,i);
                if (stopAtFirst) {
                    break;
                }
            }
        }
        return std::move(res);
    }

    template
    <
        bool stopAtFirst,
        class T,
        class Out = std::vector<int>
    >
    static Out findIdxInternal(
            T& col,
            const typename templatious::adapters::CollectionAdapter<T>::value_type& v
            )
    {
        namespace ut = templatious::util;
        namespace ad = templatious::adapters;
        typedef typename templatious::StaticAdapter SA;
        typedef typename ad::CollectionAdapter<T> AD;
        typedef typename AD::value_type ValType;
        ut::ComparatorEq<ValType,ValType> comp;

        auto res =
            stopAtFirst ? SA::instantiate<Out>(1) : SA::instantiate<Out>();
        int idx = 0;
        for (auto i = SA::begin(col); i != SA::end(col); ++i) {
            if (comp(*i,v)) {
                SA::add(res,idx);
                if (stopAtFirst) {
                    break;
                }
            }
            ++idx;
        }
        return std::move(res);
    }

    template
    <
        bool stopAtFirst,
        class T,
        class Out = std::vector<
            std::pair<
                int,
                typename templatious::adapters::CollectionAdapter<T>::iterator
            >
        >
    >
    static Out findIdxIterInternal(
            T& col,
            const typename templatious::adapters::CollectionAdapter<T>::value_type& v
            )
    {
        namespace ut = templatious::util;
        namespace ad = templatious::adapters;
        typedef typename templatious::StaticAdapter SA;
        typedef typename ad::CollectionAdapter<T> AD;
        typedef typename AD::value_type ValType;
        ut::ComparatorEq<ValType,ValType> comp;

        auto res =
            stopAtFirst ? SA::instantiate<Out>(1) : SA::instantiate<Out>();
        int idx = 0;
        for (auto i = SA::begin(col); i != SA::end(col); ++i) {
            if (comp(*i,v)) {
                SA::add(res,std::make_pair(idx,i));
                if (stopAtFirst) {
                    break;
                }
            }
            ++idx;
        }
        return std::move(res);
    }

    public:

    // T - return col, U - functor, Args - collections
    template <class T,bool passIndex = false,class U,class... Args>
    static T moldToOne(U&& fn,Args&&... args) {
        assert(templatious::util::SizeVerifier<Args...>(std::forward<Args>(args)...).areAllEqual());

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace tup = templatious::tuple;
        namespace ut = templatious::util;

        typedef typename templatious::adapters::CollectionAdapter<T> CA;
        auto it = ItMk::makeIter(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,size_t> ICall;

        size_t size = SA::getSize(ut::getFirst(std::forward<Args>(args)...));
        auto e = SA::end(ut::getFirst(std::forward<Args>(args)...));
        auto result = CA::instantiate(size);

        size_t idx;
        if (passIndex) {
            idx = 0;
        }

        for (; it._a != e; it.inc()) {
            CA::add(result,
                    ICall::call(std::forward<U>(fn),
                    idx,std::forward<Iter>(it)));

            if (passIndex) {
                ++idx;
            }
        }

        return std::move(result);
    }

    template <bool passIndex = false, class U, class... Args>
    static void traverse(U&& fn, Args&&... args) {
        assert(templatious::util::SizeVerifier<Args...>(args...).areAllEqual());

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace ut = templatious::util;

        auto it = ItMk::makeIter(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,size_t> ICall;

        auto e = SA::end(ut::getFirst(std::forward<Args>(args)...));
        size_t idx;
        if (passIndex) {
            idx = 0;
        }

        for (; it._a != e; it.inc()) {
            typedef typename ut::RetValSelector<
                decltype(
                    ICall::call(std::forward<U>(fn),idx,std::forward<Iter>(it))
                ) > Sel;

            if (!Sel::callAndEval(
                ICall::call,std::forward<U>(fn),idx,std::forward<Iter>(it)))
            { return; }

            if (passIndex) {
                ++idx;
            }
        }
    }

    template <bool passIndex = false, class U, class... Args>
    static void quadro(U&& fn, Args&&... args) {
        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace ut = templatious::util;

        auto it = ItMk::makeQuadro(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,size_t> ICall;

        size_t idx;
        if (passIndex) {
            idx = 0;
        }

        do {
            typedef typename ut::RetValSelector<
                decltype(
                    ICall::call(std::forward<U>(fn),idx,std::forward<Iter>(it))
                ) > Sel;
            if (!Sel::callAndEval(
                ICall::call,std::forward<U>(fn),idx,std::forward<Iter>(it)))
            { return; }

            if (passIndex) {
                ++idx;
            }
        } while (!it.inc());
    }

    template <class T,class U,class Comp = templatious::util::ComparatorEq<U,U,templatious::util::Default> >
    static bool valExists(const T& col,const U& v) {
        typedef typename templatious::StaticAdapter SA;
        Comp c;

        for (auto i = SA::begin(col); i != SA::end(col); ++i) {
            if (c(v,*i)) {
                return true;
            }
        }

        return false;
    }

    template <class T,class V,class... Col>
    static void set(T&& t,V& v,Col&... args) {
        set(std::forward<T>(t),v);
        set(std::forward<T>(t),args...);
    }

    template <class T,class V>
    static void set(T&& t,V& col) {
        typedef typename templatious::StaticAdapter SA;

        for (auto i = SA::begin(col);
                  i != SA::end(col);
                  ++i)
        {
            *i = t;
        }
    }

    template <class T,class U,class... V>
    static void callEach(T&& f,U&& arg,V&&... args) {
        detail::PackHandler< IsPack<U>::val >::call(
                std::forward<T>(f),std::forward<U>(arg));
        callEach(std::forward<T>(f),std::forward<V>(args)...);
    }

    template <class T,class U>
    static void callEach(T&& f,U&& arg) {
        detail::PackHandler< IsPack<U>::val >::call(
                std::forward<T>(f),std::forward<U>(arg));
    }

    template <class T,class U>
    static auto findFirstIter(T& col,const U& v)
        -> decltype(findIterInternal<true>(col,v))
    {
        return findIterInternal<true>(col,v);
    }

    template <class T,class U>
    static auto findIter(T& col,const U& v)
        -> decltype(findIterInternal<false>(col,v))
    {
        return findIterInternal<false>(col,v);
    }

    template <class T,class U>
    static auto findFirstIdx(T& col,const U& v)
        -> decltype(findIdxInternal<true>(col,v))
    {
        return findIdxInternal<true>(col,v);
    }

    template <class T,class U>
    static auto findIdx(T& col,const U& v)
        -> decltype(findIdxInternal<false>(col,v))
    {
        return findIdxInternal<false>(col,v);
    }

    template <class T,class U>
    static auto findIdxIter(T& col,const U& v)
        -> decltype(findIdxIterInternal<false>(col,v))
    {
        return findIdxIterInternal<false>(col,v);
    }

    template <class T,class U>
    static auto findFirstIdxIter(T& col,const U& v)
        -> decltype(findIdxIterInternal<true>(col,v))
    {
        return findIdxIterInternal<true>(col,v);
    }


};

namespace detail {
    template <>
    struct PackHandler<false> {
        template <class F,class T>
        static void call(F&& f,T&& t) {
            f(std::forward<T>(t));
        }
    };

    template <>
    struct PackHandler<true> {

        template <class Func>
        struct PackCaller {
            PackCaller(Func f) : _r(f) {}

            template <class... T>
            void operator()(T&&... t) {
                templatious::StaticManipulator::callEach(_r.getRef(),std::forward<T>(t)...);
            }

        private:
            typedef typename templatious::util::TypeSelector<
                    std::is_lvalue_reference<Func>::value,
                    templatious::util::RefContainer<Func>,
                    templatious::util::CopyContainer<Func>
                >::val Cont;

            Cont _r;
        };

        template <class F,class T>
        static void call(F&& f,T&& t) {
            PackCaller<F> p(std::forward<F>(f));
            t.call(p);
        }
    };
}
}

#endif
