#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <assert.h>

#include <templatious/Action.h>
#include <templatious/adapters/All.h>
#include <templatious/Utilities.h>
#include <templatious/IterMaker.h>

namespace templatious {
namespace manip {

template <class T,class U,class V>
struct CollectionManipulator {
	T _l;
	U _r;
	V _ret;

	typedef typename T::ThisCol LeftCol;
	typedef typename U::ThisCol RightCol;
	typedef typename V::ThisCol ReturnCol;

	typedef typename T::value_type LeftVal;
	typedef typename U::value_type RightVal;
	typedef typename V::value_type ReturnVal;

	typedef typename T::iterator LeftIter;
	typedef typename U::iterator RightIter;
	typedef typename V::iterator ReturnIter;

	CollectionManipulator(T l,U r,V ret) : _l(l),_r(r),_ret(ret) {};

	template <class X>
	ReturnCol doAction(LeftCol l,RightCol r,X& action) {
		assert(_l.getSize(l) == _r.getSize(r));

		int size = _l.getSize(l);

		auto lIt = _l.begin(l);
		auto rIt = _r.begin(r);

		ReturnCol retCol = _ret.instantiate(size);
		
		while (lIt != _l.end(l) && rIt != _r.end(r)) {
			_ret.add(retCol,action(*lIt,*rIt));
			++lIt; ++rIt;
		}

		return retCol;
	}

};

struct StaticManipulator {

	template <class T,class U, class V, class X>
	static T twoToOne(const U& l,const V& r,X action) {

        namespace ad = templatious::adapters;

        ad::CollectionAdapter<U> lef;
        ad::CollectionAdapter<V> rgt;

		assert(lef.getSize(l) == rgt.getSize(r));

		int size = lef.getSize(l);

		auto lIt = lef.begin(l);
		auto rIt = rgt.begin(r);

        typedef typename ad::CollectionAdapter<T> RetType;
        typedef typename RetType::value_type RetValType;
        RetType ret;
		auto res = ret.instantiate(size);
		
		while (lIt != lef.end(l)) {
            RetValType r = action.template operator()<RetValType>(*lIt,*rIt);
			ret.add(res, r);
			++lIt; ++rIt;
		}

		return res;
	}

    template <class T,bool passIndex = false,class U,class ...Args> // T - return col, U - functor, Args - collections
    static T moldToOne(U& fn,Args & ... args) {
        assert(templatious::util::SizeVerifier<Args...>(args...).areAllEqual());

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::adapters::StaticAdapter SA;
        namespace tup = templatious::tuple;
        namespace ut = templatious::util;

        templatious::adapters::CollectionAdapter<T> ca;
        auto it = ItMk::makeIter(args...);
        auto tpl = ut::ExtractionSelector<passIndex,int,Args...>().getTuple(args...);

        util::DoIf< passIndex > setIndex;

        int size = SA::getSize(ut::getFirst(args...));
        auto result = ca.instantiate(size);
        for (int i = 0; i < size; ++i) {
            it.template setTuple< util::IntSelector<passIndex,1,0>::val >(tpl);
            setIndex.doIt( [&]() { std::get<0>(tpl) = i; } );
            ca.add(result,tup::callTuple(fn,tpl));
            it.inc();
        }

        return result;
    }

    template <bool passIndex = false, class U, class... Args>
    static void interact(U& fn, Args&... args) {
        assert(templatious::util::SizeVerifier<Args...>(args...).areAllEqual());

        typedef typename templatious::recursive::IteratorMaker ItMk;
        typedef typename templatious::adapters::StaticAdapter SA;
        namespace tup = templatious::tuple;
        namespace ut = templatious::util;

        auto it = ItMk::makeIter(args...);
        auto tpl =
            ut::ExtractionSelector<passIndex, int, Args...>().getTuple(args...);

        util::DoIf<passIndex> setIndex;

        int size = SA::getSize(ut::getFirst(args...));
        for (int i = 0; i < size; ++i) {
            it.template setTuple<util::IntSelector<passIndex, 1, 0>::val>(tpl);
            setIndex.doIt([&]() { std::get<0>(tpl) = i; });
            tup::callTuple(fn,tpl);
            it.inc();
        }
    }

    template <class T,class U>
    static void edit(U& fn,T& col) {
        typedef typename templatious::adapters::StaticAdapter SA;

        auto end = SA::end(col);
        for (auto it = SA::begin(col); it != end; ++it) {
            *it = fn(*it);
        }
    }


};

}
}

#endif
