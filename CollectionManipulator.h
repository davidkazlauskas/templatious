#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <assert.h>

#include <templatious/Action.h>
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


    public:

    // T - return col, U - functor, Args - collections
    template <class T,bool passIndex = false,class U,class ...Args> 
    static T moldToOne(U& fn,Args & ... args) {
        assert(templatious::util::SizeVerifier<Args...>(args...).areAllEqual());

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::adapters::StaticAdapter SA;
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
        typedef typename templatious::adapters::StaticAdapter SA;
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

    template 
    <
        class T,
        class Out = std::vector<
            typename templatious::adapters::CollectionAdapter<T>::iterator
        >
    >
    static Out findFirst(
            T& col,
            const typename templatious::adapters::CollectionAdapter<T>::value_type& v
            ) 
    {
        namespace ut = templatious::util;
        namespace ad = templatious::adapters;
        typedef typename ad::StaticAdapter SA;
        typedef typename ad::CollectionAdapter<T> AD;
        typedef typename AD::value_type ValType;
        typedef typename ut::ComparatorEq<ValType,ValType> Comp;

        auto res = SA::instantiate<Out>(1);
        for (auto i = SA::begin(col); i != SA::end(col); ++i) {
            if (Comp::eq(*i,v)) {
                SA::add(res,i);
                break;
            }
        }
        return res;
    }

};

}
}

#endif
