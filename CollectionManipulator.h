#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <assert.h>

#include <templatious/Action.h>
#include <templatious/adapters/All.h>

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
	static T twoToOne(const U& l,const V& r,X& action) {

        namespace ad = templatious::adapters;

        ad::CollectionAdapter<U> lef;
        ad::CollectionAdapter<V> rgt;

		assert(lef.getSize(l) == rgt.getSize(r));

		int size = lef.getSize(l);

		auto lIt = lef.begin(l);
		auto rIt = rgt.begin(r);

        typedef typename ad::CollectionAdapter<T> RetType;
        RetType ret;
		auto res = ret.instantiate(size);
		
		while (lIt != lef.end(l)) {
			ret.add(res,action(*lIt,*rIt));
			++lIt; ++rIt;
		}

		return res;
	}

};

}
}

#endif
