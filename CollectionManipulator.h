#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <utility>
#include <assert.h>

#include <templatious/adapters/All.h>
#include <templatious/Utilities.h>
#include <templatious/IterMaker.h>

namespace templatious {
namespace manip {

struct StaticManipulator {
    private:
     template <class F, class ITER,bool callWithIndex = false,typename Index = int>
     struct IteratorCaller;

     template <class F,class ITER,typename Index>
     struct IteratorCaller<F, ITER, false, Index> {
         static auto call(F& f, Index idx, ITER& i) -> decltype(i.callFunction(f)) {
             return i.callFunction(f);
         }
     };

     template <class F,class ITER,typename Index>
     struct IteratorCaller<F, ITER, true, Index> {
         static auto call(F& f, Index idx, ITER& i) -> decltype(i.callFunction(f,idx)) {
             return i.callFunction(f,idx);
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
        return res;
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
        return res;
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
        return res;
    }

    public:

    // T - return col, U - functor, Args - collections
    template <class T,bool passIndex = false,class U,class ...Args> 
    static T moldToOne(U& fn,Args & ... args) {
        assert(templatious::util::SizeVerifier<Args...>(args...).areAllEqual());

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace tup = templatious::tuple;
        namespace ut = templatious::util;

        typedef typename templatious::adapters::CollectionAdapter<T> CA;
        auto it = ItMk::makeIter(args...);
        typedef IteratorCaller<U,decltype(it),passIndex,int> ICall;

        int size = SA::getSize(ut::getFirst(args...));
        auto result = CA::instantiate(size);
        for (int i = 0; i < size; ++i) {
            CA::add(result,ICall::call(fn,i,it));
            it.inc();
        }

        return result;
    }


    template <bool passIndex = false, class U, class... Args>
    static void traverse(U fn, Args&... args) {
        traverseRef<passIndex>(fn,args...);
    }

    template <bool passIndex = false, class U, class... Args>
    static void traverseRef(U& fn, Args&... args) {
        assert(templatious::util::SizeVerifier<Args...>(args...).areAllEqual());

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::StaticAdapter SA;
        namespace tup = templatious::tuple;
        namespace ut = templatious::util;

        auto it = ItMk::makeIter(args...);
        typedef IteratorCaller<U,decltype(it),passIndex,int> ICall;

        int size = SA::getSize(ut::getFirst(args...));
        for (int i = 0; i < size; ++i) {
            typedef typename ut::RetValSelector<decltype(ICall::call(fn,i,it))> Sel;
            if (!Sel::callAndEval(ICall::call,fn,i,it)) {
                return;
            }
            it.inc();
        }
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

}
}

#endif
