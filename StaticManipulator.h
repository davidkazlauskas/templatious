#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <utility>
#include <type_traits>
#include <vector>

#include <templatious/CollectionAdapter.h>
#include <templatious/CollectionMaker.h>

#include <templatious/util/SizeVerifier.h>
#include <templatious/util/CallCountFunctor.h>
#include <templatious/util/Exceptions.h>

#include <templatious/IterMaker.h>
#include <templatious/Pack.h>
#include <templatious/detail/Distributor.h>
#include <templatious/detail/UserUtil.h>

namespace templatious {
namespace detail {

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

    template <class T,class F>
    struct FoldFunctor {
        template <class Func>
        FoldFunctor(T& t,Func&& f)
            : _c(t), _f(std::forward<Func>(f)) {}

        template <class V>
        void operator()(V&& arg) {
            _c = _f.getRef()(_c,std::forward<V>(arg));
        }

        typedef typename templatious::util::
            DefaultStoragePolicy<F>::Container Container;
    private:
        T& _c;
        Container _f;
    };
}

TEMPLATIOUS_BOILERPLATE_EXCEPTION( MapFunctionNotEqualException,
    "Collections passed to map are not all equal in size.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION( TraverseFunctionNotEqualException,
    "Collections passed to traverse are not all equal in size.");

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

        template <class FInt,class IterInt>
        auto operator()(FInt&& f, Index idx, IterInt&& i)
            -> decltype(i.callFunction(std::forward<FInt>(f)))
        {
            return i.callFunction(std::forward<FInt>(f));
        }
    };

    template <class F,class ITER,class Index>
    struct IteratorCaller<F, ITER, true, Index> {

        template <class FInt,class IterInt>
        auto operator()(FInt&& f, Index idx, IterInt&& i)
            -> decltype(i.callFunction(std::forward<FInt>(f),
                        idx))
        {
            return i.callFunction(std::forward<FInt>(f),idx);
        }
    };

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

    /**
     * Map function. Accepts function which takes n amount of
     * args from n collections which are traversed linearly and
     * new collection is made containing elements which are
     * results of specified function.
     * @param[in] fn Function to use when mapping
     * @param[in] args Arguments, collections to traverse
     * in linear way. Assertion is made that they all
     * contain same amount of elements.
     * @param[out] T the collection element type to be returned.
     * Inferred by default
     * @param[in] passIndex Flag wether to pass current iteration
     * index while traversing. Index is passed first to function.
     * Defaults to false.
     * @param[out] ResCollection Collection type to return from
     * this function. Defaults to std::vector. Needed amount of
     * memory for collection is preallocated if applicable.
     * @param[out] Allocator Allocator to use for new collection
     * if applicable. Is ignored if collection is allocator
     * indifferent.
     */
    template <
        class T,bool passIndex = false,
        template <class...> class ResCollection = std::vector,
        template <class> class Allocator = std::allocator,
        class U,class... Args>
    static auto map(U&& fn,Args&&... args)
     -> decltype(
         templatious::adapters::CollectionMaker<
             T,ResCollection,Allocator>::
             make(0)
     )
    {
        if (!templatious::util::SizeVerifier<Args...>(
            std::forward<Args>(args)...).areAllEqual())
        {
            throw MapFunctionNotEqualException();
        }

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace ut = templatious::util;

        auto it = ItMk::makeIter(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,size_t> ICall;
        ICall call;

        int size = SA::size(ut::getFirst(std::forward<Args>(args)...));
        auto e = SA::end(ut::getFirst(std::forward<Args>(args)...));
        typedef templatious::adapters::CollectionMaker<
            T,ResCollection,Allocator> Mk;

        auto result = Mk::make(size);
        size_t idx = 0;

        // hopefully, static_assert is more readable
        typedef decltype(call(
            std::forward<U>(fn),
            idx,it)) RetType;
        static const bool is_void =
            std::is_same< void, RetType >::value;
        static_assert( !is_void,
            "Function passed to map has to return non-void value." );

        for (; it._a != e; it.inc()) {
            SA::add(result,
                call(std::forward<U>(fn),
                idx,it));

            if (passIndex) {
                ++idx;
            }
        }

        return std::move(result);
    }

    /**
     * Linear traversal function
     * @param[in] fn Function to be used on traversal.
     * Should take arguments from args collection value types.
     * @param[in] args Collection arguments to be traversed
     * linearly. Must all have the same count of elements.
     * @param[in] passIndex Boolean value wether to pass index
     * of current traversal (from 0) to a function. If passed,
     * index is passed first. Defaults to false.
     */
    template <bool passIndex = false, class U, class... Args>
    static void traverse(U&& fn, Args&&... args) {
        if (!templatious::util::SizeVerifier<Args...>(
            args...).areAllEqual())
        {
            throw TraverseFunctionNotEqualException();
        }

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace ut = templatious::util;

        auto it = ItMk::makeIter(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,size_t> ICall;
        ICall call;

        auto e = SA::end(ut::getFirst(std::forward<Args>(args)...));
        size_t idx;
        if (passIndex) {
            idx = 0;
        }

        for (; it._a != e; it.inc()) {
            typedef typename ut::RetValSelector<
                decltype(
                    call(std::forward<U>(fn),
                    idx,it)
                ) > Sel;

            if (!Sel::callAndEval(
                call,
                std::forward<U>(fn),
                idx,it))
            { return; }

            if (passIndex) {
                ++idx;
            }
        }
    }

    /**
     * Quadratic traversal function.
     * @param[in] fn Function to be used on each iteration.
     * Should take in args collection value types.
     * @param[in] args Collection arguments to be traversed
     * in quadratic manner.
     * @param[in] passIndex Sets whether to pass traversal
     * index (starting with 0) to function. If passed
     * it is passed first. Defaults to false.
     */
    template <bool passIndex = false, class U, class... Args>
    static size_t quadro(U&& fn, Args&&... args) {
        typedef typename templatious::recursive::IteratorMaker ItMk;
        namespace ut = templatious::util;

        auto it = ItMk::makeQuadro(std::forward<Args>(args)...);
        typedef decltype(it) Iter;
        typedef IteratorCaller<U,Iter,passIndex,size_t> ICall;
        ICall call;

        size_t idx = 0;

        do {
            typedef typename ut::RetValSelector<
                decltype(
                    call(std::forward<U>(fn),idx,it)
                ) > Sel;
            if (!Sel::callAndEval(
                call,
                std::forward<U>(fn),
                idx,it))
            { return ++idx; }

            ++idx;
        } while (!it.inc());

        return idx;
    }

    /**
     * probably will be removed
     */
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

    /**
     * Variadic set function overload.
     * @param[in] t Value used for setting.
     * @param[in,out] v One entity to set.
     * Can be collection, pack or a single
     * variable.
     * @param[in,out] args Rest of the entities
     * to use set function on.
     */
    template <class T,class V,class... Col>
    static void set(T&& t,V& v,Col&... args) {
        set(std::forward<T>(t),v);
        set(std::forward<T>(t),args...);
    }

    /**
     * Value set function.
     * @param[in] t Value used for setting
     * @param[in,out] v Entity to set. Can be collection,
     * pack or a single variable.
     */
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

    /**
     * Generic call each function for multiple elements
     * (packs are considered composite)
     * @param[in] f Function to call on each argument.
     * Function may return false to stop traversal
     * (will work if ignoreBooleanReturn is set to false).
     * @param[in,out] args Arguments to call function on.
     * Can be variables or packs. Packs are processed
     * recursively calling function on each element
     * of the pack.
     * @param[in] ignoreBooleanReturn Value which determines
     * whether to interpret function return value (if it
     * happens to be boolean) as a signal to stop traversal.
     * If set to true boolean return value is ignored and
     * traversal always iterates through every element.
     */
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

    /**
     * Generic foreach function for multiple elements
     * (collections are considered composite)
     * @param[in] f Function to call on each argument.
     * Function may return false to stop traversal
     * (will work if ignoreBooleanReturn is set to false).
     * @param[in,out] args Arguments to call function on.
     * Can be variables or collections. Collections are
     * processed element by element.
     * @param[in] ignoreBooleanReturn Value which determines
     * whether to interpret function return value (if it
     * happens to be boolean) as a signal to stop traversal.
     * If set to true boolean return value is ignored and
     * traversal always iterates through every element.
     */
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

    /**
     * Sum utility function. Returns numeric sum of
     * all elements.
     * @param[in] args Elements to sum. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double.
     */
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

    /**
     * Sum using special function. Returns numeric sum of
     * all elements.
     * @param[in] f Function to be used on every element
     * before summing an element to the result.
     * @param[in] args Elements to sum. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double.
     */
    template <class RetVal = double,class Fn,class... V>
    static RetVal sumS(Fn&& f,V&&... args) {
        RetVal r(0);
        templatious::detail::SumFunctorCustom<
            RetVal,decltype(std::forward<Fn>(f))>
            func(r,std::forward<Fn>(f));
        func._cnt = 0;

        // again, assumption about numeric values
        genericCallAll< detail::DeciderAllUniform >(
            func,std::forward<V>(args)...
        );

        return r;
    }

    /**
     * Average of values. Returns numeric average
     * of elements passed.
     * @param[in] args Elements to sum. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double.
     */
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

    /**
     * Average of values using special function.
     * Returns numeric average of elements passed.
     * @param[in] f Function to be used on every element
     * before summing an element to the result.
     * @param[in] args Elements to sum. Can process
     * packs, collections and single values.
     * @param[in] RetVal Return type. Defaults
     * to double.
     */
    template <class RetVal = double,class Fn,class... V>
    static RetVal avgS(Fn&& f,V&&... args) {
        RetVal r(0);
        templatious::detail::SumFunctorCustom<
            RetVal,decltype(std::forward<Fn>(f)),true>
            func(r,std::forward<Fn>(f));
        func._cnt = 0;

        // again, assumption about numeric values
        genericCallAll< detail::DeciderAllUniform, true >(
            func,std::forward<V>(args)...
        );

        return r / func._cnt;
    }

    /**
     * Fold function. Folds multiple values to single.
     * @param[in] start Starting value. Should be copy
     * constructable.
     * @param[in] f Function to apply in every iteration.
     * Should take in two values as arguments,
     * the accumulator value and the value processed
     * through every iteration of a sequence.
     * Should return newly calculated value.
     * @param[in] args Arguments to traverse. Can process
     * packs, collections and single variables.
     */
    template <class T,class F,class... Args>
    static T fold(T&& start,F&& f,Args&&... args) {
        auto var = start;
        detail::FoldFunctor<
            decltype(std::forward<T>(start)),
            decltype(std::forward<F>(f))
        > ff(var,std::forward<F>(f));

        genericCallAll< detail::DeciderAllUniform, true >(
                ff,std::forward<Args>(args)...
        );

        return std::move(var);
    }

    /**
     * Distribute function. Distributes from pack
     * or collection over other collections/packs/variables
     * in a linear fashion. Returns how many
     * distributions were made.
     * @param[in] t Entity to distribute from.
     * Can be pack or collection.
     * @param[out] args Arguments to distribute over.
     * Can be packs, collections or single variables.
     */
    template <
        class T,class... Args
    >
    static size_t distribute(T&& t,Args&&... args) {
        return distributeInternal<false>(
            templatious::detail::AssignDispatcher(),
            std::forward<T>(t),
            std::forward<Args>(args)...);
    }

    /**
     * Reverse distribute function. Distributes
     * over pack or collection from other
     * collections/packs/variables in a linear fashion.
     * Returns how many distributions were made.
     * @param[out] t Entity to distribute to.
     * Can be pack or collection.
     * @param[in] args Arguments to distribute from.
     * Can be packs, collections or single variables.
     */
    template <
        class T,class... Args
    >
    static size_t distributeR(T&& t,Args&&... args) {
        return distributeInternal<false>(
            templatious::detail::RevAssignDispatcher(),
            std::forward<T>(t),
            std::forward<Args>(args)...);
    }

    /**
     * Distribute using special function. Returns how
     * many distributions were made.
     * @param[in] f Function for use in distribution.
     * should take two arguments. May return boolean
     * value false to stop distribution at any time
     * (ignoreBooleanReturn has to be set to false).
     * @param[in,out] t First part distribution entity,
     * first argument for the function f.
     * Can be pack or collection.
     * @param[in,out] args Second part of distribution
     * entity. Second argument for the function f.
     * Can be packs, collections or single variables.
     * @param[in] ignoreBooleanReturn Value to determine
     * whether function return value (if it happens to
     * be boolean) should stop traversal if it returns
     * false. Defaults to false.
     */
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
