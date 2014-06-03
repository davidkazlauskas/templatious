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

    static const bool is_valid = true;

	typedef typename std::vector<T> ThisCol;
	typedef typename ThisCol::iterator iterator;
	typedef typename ThisCol::const_iterator const_iterator;
	typedef T value_type;

	static bool add(ThisCol& c,const value_type& i) {
		c.push_back(i);
		return true;
	}

	static ThisCol instantiate() {
		return ThisCol();
	}

	static ThisCol instantiate(int size) {
		ThisCol r;
		r.reserve(size);
		return r;
	}

	static iterator begin(ThisCol& c) {
		return c.begin();
	}

	static iterator end(ThisCol& c) {
		return c.end();
	}

	static const_iterator begin(const ThisCol& c) {
		return c.begin();
	}

	static const_iterator end(const ThisCol& c) {
		return c.end();
	}

	static int getSize(const ThisCol& c) {
		return c.size();
	}

    static value_type& getByIndex(ThisCol& c, int i) {
        return c[i];
    }

};

template <class T>
struct CollectionAdapter< std::vector<T>* > {

	typedef typename std::vector<T>* ThisCol;
    typedef typename std::vector<T> const* ConstCol;
    typedef typename std::vector<T> ColType;
	typedef typename ColType::iterator iterator;
	typedef typename ColType::const_iterator const_iterator;
	typedef T value_type;

	static bool add(ThisCol c,const value_type& i) {
		c->push_back(i);
		return true;
	}

	static ThisCol instantiate() {
		return new ColType();
	}

	static ThisCol instantiate(int size) {
        ThisCol r = new ColType();
		r->reserve(size);
		return r;
	}

	static iterator begin(ThisCol c) {
		return c->begin();
	}

	static iterator end(ThisCol c) {
		return c->end();
	}

	static const_iterator begin(ConstCol c) {
		return c->cbegin();
	}

	static const_iterator end(ConstCol c) {
		return c->cend();
	}

	static int getSize(ConstCol c) {
		return c->size();
	}

    static value_type& getByIndex(ConstCol c, int i) {
        return (*c)[i];
    }

};
}
}

#endif /* end of include guard: STDVECTOR_H6YPGTPK */
