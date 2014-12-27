#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <utility>
#include <type_traits>
#include <assert.h>

#include <templatious/adapters/All.h>
#include <templatious/Utilities.h>
#include <templatious/IterMaker.h>
#include <templatious/Pack.h>
#include <templatious/detail/Distributor.h>
#include <templatious/detail/UserUtil.h>

namespace templatious {
namespace detail{ 

    enum Variant { Item, Pack, Collection };

    template <
        int variant,
        template <class> class Decider
    >
    struct CallHandler;


    // Difference between callEach and forEach:
    // callEach treats packs as composite
    // forEach treats collections as composite
    //
    // Why not all in one?
    // Because of static arrays
    // Static arrays are collections for templatious
    // But literal strings like "hello world"
    // are desired to be considered like one item
    // in callEach, not a collection of chars.
    //
    // Maybe there will be a better solution
    // in the future to treat all three uniformily?
    //
    // We'll see.
    template <class T>
    struct DeciderCallEach {
        static const int TheVar = templatious::util::IntSelector<
            IsPack<T>::val,
            Variant::Pack,
            Variant::Item
        >::val;
    };

    template <class T>
    struct DeciderForEach {
        static const int TheVar = templatious::util::IntSelector<
            templatious::adapters::CollectionAdapter<T>::is_valid,
            Variant::Collection,
            Variant::Item
        >::val;
    };

    template <class T>
    struct DeciderAllUniform {
        static const int TheVar = templatious::util::IntSelector<
            templatious::adapters::CollectionAdapter<T>::is_valid,
            Variant::Collection,
            templatious::util::IntSelector<
                IsPack<T>::val,
                Variant::Pack,
                Variant::Item
            >::val
        >::val;
    };

    struct SetImplCollection;
    struct SetImplVariable;
    struct SetImplPack;

}

struct StaticManipulator {
private:
    template <
        int var,
        template <class> class Decider
    >
    friend struct detail::CallHandler;

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
            typename templatious::adapters::CollectionAdapter<T>::Iterator
        >
    >
    static Out findIterInternal(
            T& col,
            const typename templatious::adapters::CollectionAdapter<T>::ValueType& v
            )
    {
        namespace ut = templatious::util;
        namespace ad = templatious::adapters;
        typedef typename templatious::StaticAdapter SA;
        typedef typename ad::CollectionAdapter<T> AD;
        typedef typename AD::ValueType ValType;
        ut::ComparatorEq<ValType,ValType> comp;

        auto res =
            stopAtFirst ? SA::instantiate<Out>(1) : SA::instantiate<Out>();

        auto end = SA::end(col);
        for (auto i = SA::begin(col); i != end; ++i) {
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
            const typename templatious::adapters::CollectionAdapter<T>::ValueType& v
            )
    {
        namespace ut = templatious::util;
        namespace ad = templatious::adapters;
        typedef typename templatious::StaticAdapter SA;
        typedef typename ad::CollectionAdapter<T> AD;
        typedef typename AD::ValueType ValType;
        ut::ComparatorEq<ValType,ValType> comp;

        auto res =
            stopAtFirst ? SA::instantiate<Out>(1) : SA::instantiate<Out>();
        int idx = 0;
        auto end = SA::end(col);
        for (auto i = SA::begin(col); i != end; ++i) {
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
                typename templatious::adapters::CollectionAdapter<T>::Iterator
            >
        >
    >
    static Out findIdxIterInternal(
            T& col,
            const typename templatious::adapters::CollectionAdapter<T>::ValueType& v
            )
    {
        namespace ut = templatious::util;
        namespace ad = templatious::adapters;
        typedef typename templatious::StaticAdapter SA;
        typedef typename ad::CollectionAdapter<T> AD;
        typedef typename AD::ValueType ValType;
        ut::ComparatorEq<ValType,ValType> comp;

        auto res =
            stopAtFirst ? SA::instantiate<Out>(1) : SA::instantiate<Out>();
        int idx = 0;
        auto end = SA::end(col);
        for (auto i = SA::begin(col); i != end; ++i) {
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

    template <
        bool ignoreBooleanReturn = false,
        class Func,class T,class... Args
    >
    static size_t distributeInternal(Func&& f,T&& t,Args&&... args) {
        typedef templatious::adapters::CollectionAdapter<T> Ad;
        typedef templatious::detail::IsPack<T> IP;

        static const bool isCollection = Ad::is_valid;
        static const bool isPack = IP::val;

        namespace TD = templatious::detail;

        typedef typename std::conditional<
                isPack,
                TD::PackToRestDistribution,
                typename std::conditional<
                    isCollection,
                    TD::CollectionToRestDistribution,
                    TD::DummyErrorDistributor
                >::type
            >::type Distrubutor;

        return Distrubutor::template distribute<ignoreBooleanReturn>(
                std::forward<Func>(f),
                std::forward<T>(t),
                std::forward<Args>(args)...);
    }

    template <
        template <class> class DeciderAlg,
        bool ignoreBooleanReturn = false,
        class T,class U,class... V>
    static bool genericCallAll(T&& f,U&& arg,V&&... args) {
        bool res = detail::CallHandler< DeciderAlg<U>::TheVar, DeciderAlg >
            ::template call<ignoreBooleanReturn>(
                    std::forward<T>(f),std::forward<U>(arg));
        if (!ignoreBooleanReturn && !res) {
            return false;
        }

        return genericCallAll<DeciderAlg,ignoreBooleanReturn>(
                std::forward<T>(f),std::forward<V>(args)...);
    }

    template <
        template <class> class DeciderAlg,
        bool ignoreBooleanReturn = false,
        class T,class U>
    static bool genericCallAll(T&& f,U&& arg) {
        return detail::CallHandler< DeciderAlg<U>::TheVar, DeciderAlg >
            ::template call<ignoreBooleanReturn>(
                    std::forward<T>(f),
                    std::forward<U>(arg));
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

        int size = SA::size(ut::getFirst(std::forward<Args>(args)...));
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
    static size_t quadro(U&& fn, Args&&... args) {
        typedef typename templatious::recursive::IteratorMaker ItMk;
        namespace ut = templatious::util;

        auto it = ItMk::makeQuadro(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,size_t> ICall;

        size_t idx = 0;

        do {
            typedef typename ut::RetValSelector<
                decltype(
                    ICall::call(std::forward<U>(fn),idx,std::forward<Iter>(it))
                ) > Sel;
            if (!Sel::callAndEval(
                ICall::call,std::forward<U>(fn),idx,std::forward<Iter>(it)))
            { return ++idx; }

            ++idx;
        } while (!it.inc());

        return idx;
    }

    template <class T,class U,class Comp = templatious::util::ComparatorEq<U,U,templatious::util::Default> >
    static bool valExists(const T& col,const U& v) {
        typedef typename templatious::StaticAdapter SA;
        Comp c;

        auto end = SA::end(col);
        for (auto i = SA::begin(col); i != end; ++i) {
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
        typedef typename templatious::adapters::CollectionAdapter<V> Ad;
        typedef typename std::conditional< Ad::is_valid,
                detail::SetImplCollection,
                typename std::conditional< detail::IsPack<V>::val,
                detail::SetImplPack,
                detail::SetImplVariable
            >::type
        >::type Impl;

        Impl::impl(std::forward<T>(t),col);
    }

    template <
        bool ignoreBooleanReturn = false,
        class T,class... V
    >
    static size_t callEach(T&& f,V&&... args) {
        templatious::util::CallCountFunctor<T>
            func(std::forward<T>(f));

        genericCallAll< detail::DeciderCallEach, ignoreBooleanReturn >(
            func,std::forward<V>(args)...
        );

        return func.getCount();
    }

    template <
        bool ignoreBooleanReturn = false,
        class T,class... V
    >
    static size_t forEach(T&& f,V&&... args) {
        templatious::util::CallCountFunctor<T>
            func(std::forward<T>(f));

        genericCallAll< detail::DeciderForEach, ignoreBooleanReturn >(
            func,std::forward<V>(args)...
        );

        return func.getCount();
    }

    template <class RetVal = double,class... V>
    static RetVal sum(V&&... args) {
        RetVal r(0);
        templatious::detail::SumFunctor<RetVal> func(r);

        // since we make an assumption that
        // we're dealing with numeric values
        // we can treat values, packs and collections.
        genericCallAll< detail::DeciderAllUniform, true >(
            func,std::forward<V>(args)...
        );

        return r;
    }

    template <class RetVal = double,class... V>
    static RetVal avg(V&&... args) {
        RetVal r(0);
        templatious::detail::SumFunctor<RetVal,true> func(r);
        func._cnt = 0;

        // again, assumption about numeric values
        genericCallAll< detail::DeciderAllUniform, true >(
            func,std::forward<V>(args)...
        );

        return r / func._cnt;
    }

    template <
        bool ignoreBooleanReturn = false,
        class T,class... Args
    >
    static size_t distribute(T&& t,Args&&... args) {
        return distributeInternal<ignoreBooleanReturn>(
            templatious::detail::AssignDispatcher(),
            std::forward<T>(t),
            std::forward<Args>(args)...);
    }

    template <
        bool ignoreBooleanReturn = false,
        class T,class... Args
    >
    static size_t distributeR(T&& t,Args&&... args) {
        return distributeInternal<ignoreBooleanReturn>(
            templatious::detail::RevAssignDispatcher(),
            std::forward<T>(t),
            std::forward<Args>(args)...);
    }

    template <
        bool ignoreBooleanReturn = false,
        class Func,class T,class... Args
    >
    static size_t distributeSpecial(Func&& f,T&& t,Args&&... args) {
        return distributeInternal<ignoreBooleanReturn>(
            std::forward<Func>(f),
            std::forward<T>(t),
            std::forward<Args>(args)...);
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
    template <
        template <class> class Decider
    >
    struct CallHandler< Variant::Item, Decider > {
        template <
            bool ignoreBooleanReturn = false,
            class F,class T
        >
        static bool call(F&& f,T&& t) {
            typedef templatious::util::RetValSelector<
                decltype(f(std::forward<T>(t))),
                true> Sel;

            bool res = Sel::callAndEval(
                std::forward<F>(f),
                std::forward<T>(t)
            );

            return ignoreBooleanReturn || res;
        }
    };

    template <
        template <class> class Decider
    >
    struct CallHandler< Variant::Collection, Decider > {
        template <
            bool ignoreBooleanReturn = false,
            class F,class T
        >
        static bool call(F&& f,T&& t) {
            typedef templatious::adapters::CollectionAdapter<T> Ad;
            typedef decltype(Ad::begin(std::forward<T>(t))) It;
            typedef templatious::util::RetValSelector<
                decltype(f(*std::declval<It>())),
                true> Sel;

            auto end = Ad::end(std::forward<T>(t));
            for (auto i = Ad::begin(std::forward<T>(t));
                    i != end;
                    ++i)
            {
                bool res = Sel::callAndEval(std::forward<F>(f),*i);
                if (!ignoreBooleanReturn && !res)
                {
                    return false;
                }
            }

            return true;
        }
    };

    template <
        template <class> class Decider
    >
    struct CallHandler< Variant::Pack, Decider > {

        template <bool ignoreBooleanReturn, class Func>
        struct PackCaller {
            PackCaller(Func f) : _r(f) {}

            template <class... T>
            bool operator()(T&&... t) {
                return templatious::StaticManipulator::
                    genericCallAll< Decider, ignoreBooleanReturn >(
                        _r.getRef(),std::forward<T>(t)...);
            }

        private:
            typedef typename templatious::util::
                DefaultStoragePolicy<Func>::Container Cont;

            Cont _r;
        };

        template <
            bool ignoreBooleanReturn = false,
            class F,class T
        >
        static bool call(F&& f,T t) {
            PackCaller<ignoreBooleanReturn,F> p(std::forward<F>(f));
            bool res = t.template call<ignoreBooleanReturn>(p);
            return ignoreBooleanReturn || res;
        }
    };

    struct SetImplCollection {

        template <class T,class V>
        static void impl(T&& t,V& c) {
            typedef typename templatious::adapters::CollectionAdapter<V> Ad;

            auto end = Ad::end(c);
            for (auto i = Ad::begin(c);
                      i != end;
                      ++i)
            {
                *i = t;
            }
        }

    };

    struct SetImplVariable {

        template <class T,class V>
        static void impl(T&& t,V& c) {
            c = t;
        }

    };

    template <class T>
    struct SetCalleachFctor {
        const T& _c; // set variable storage
        SetCalleachFctor(const T& t) : _c(t) {}

        template <class V>
        void operator()(V& v) {
            v = _c;
        }
    };


    struct SetImplPack {

        template <class T,class V>
        static void impl(T&& t,V& c) {
            SetCalleachFctor<const T&> f(std::forward<T>(t));
            templatious::StaticManipulator::callEach(f,c);
        }

    };



}
}

#endif
