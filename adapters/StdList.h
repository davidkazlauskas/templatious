/*
 * =====================================================================================
 *
 *       Filename:  StdList.h
 *
 *    Description:  std::list adapter
 *
 *        Version:  1.0
 *        Created:  05/16/2014 12:08:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef STDLIST_8E2DYM7Y
#define STDLIST_8E2DYM7Y

#include <list>

#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter< std::list<T> > {

	typedef typename std::list<T> ThisCol;
	typedef typename ThisCol::iterator iterator;
	typedef typename ThisCol::const_iterator const_iterator;
	//typedef typename ValueTypeExtractor< std::list<T> >::value value_type;
	typedef T value_type;

	bool add(ThisCol& c,const value_type& i) {
		c.push_back(i);
		return true;
	}

	ThisCol instantiate() {
		return ThisCol();
	}

	ThisCol instantiate(int size) {
		ThisCol r;
		return r;
	}

	iterator begin(ThisCol& c) {
		return c.begin();
	}

	iterator end(ThisCol& c) {
		return c.end();
	}

	const iterator begin(const ThisCol& c) {
		return c.begin();
	}

	const iterator end(const ThisCol& c) {
		return c.end();
	}

	int getSize(const ThisCol& c) {
		return c.size();
	}

};

}
}

#endif /* end of include guard: STDLIST_8E2DYM7Y */
