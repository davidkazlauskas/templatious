
#ifndef GENERIC_COMPARATOR_SADWE
#define GENERIC_COMPARATOR_SADWE

#include "TypeTraits.h"

template <class T,class U = Type2Type<Default>>
struct GenericIsEqualComparator {
	bool operator()(const T& a,const T& b);
	// is equal method, should be called as a functor
};

template <class T,class U = Type2Type<Default>>
struct GenericMoreLessComparator {
	int compare(const T& a,const T& b);

	bool isLess(const T& a,const T& b);
	bool isMore(const T& a,const T& b);
};




#endif