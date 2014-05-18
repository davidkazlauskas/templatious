/*
 * =====================================================================================
 *
 *       Filename:  StdVector.h
 *
 *    Description:  std::vector adapter
 *
 *        Version:  1.0
 *        Created:  05/16/2014 10:07:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef STDVECTOR_H6YPGTPK
#define STDVECTOR_H6YPGTPK

#include <vector>
#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter< std::vector<T> > {

	typedef typename std::vector<T> ThisCol;
	typedef typename ThisCol::iterator iterator;
	typedef typename ThisCol::const_iterator const_iterator;
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
		r.reserve(size);
		return r;
	}

	iterator begin(ThisCol& c) {
		return c.begin();
	}

	iterator end(ThisCol& c) {
		return c.end();
	}

	const_iterator begin(const ThisCol& c) {
		return c.begin();
	}

	const_iterator end(const ThisCol& c) {
		return c.end();
	}

	int getSize(const ThisCol& c) {
		return c.size();
	}

};

}
}

#endif /* end of include guard: STDVECTOR_H6YPGTPK */
