#ifndef COL_MANIPULATOR_SDJKQE
#define COL_MANIPULATOR_SDJKQE

#include <assert.h>

#include "TypeTraits.h"

// T - left side, U - right side, V - return type, Type2Type - additional anchor to differentiate
template <class T,class U,class V,class X = Type2Type<Default>>
struct Action {
	V operator()(T& a,U& b);
};

struct Multiply;
struct Subtract;
struct Add;

template<class T,class U,class V>
struct Action<T,U,V,Type2Type<Multiply>> {
	V operator()(T& a,T& b) {
		return a * b;
	}
};

template<class T,class U,class V>
struct Action<T,U,V,Type2Type<Subtract>> {
	V operator()(int& a,int& b) {
		return a - b;
	}
};

template<class T,class U,class V>
struct Action<T,U,V,Type2Type<Add>> {
	V operator()(int& a,int& b) {
		return a + b;
	}
};

//typedef Action<int,int,int,Type2Type<Multiply>> Multiplication;
//typedef Action<double,double,double,Type2Type<Multiply>> Multiplication;
//typedef Action<int,int,int,Type2Type<Subtract>> Subtraction;
//typedef Action<int,int,int,Type2Type<Add>> Addition;

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

#endif