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

	value_type& getByIndex(ThisCol& c,int i) {
        assert(getSize(c) > i);

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return *iter;
    }

    bool erase(ThisCol& c,iterator beg) {
        c.erase(beg);
        return true;
    }

    bool erase(ThisCol& c,iterator beg,iterator end) {
        c.erase(beg,end);
        return true;
    }

    iterator iter_at(ThisCol& c,int i) {
        assert(getSize(c) > i);

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return iter;
    }
};

template <class T>
struct CollectionAdapter< std::list<T>* > {

	typedef typename std::list<T>* ThisCol;
	typedef typename std::list<T> const* ConstCol;
	typedef typename std::list<T> ColType;
	typedef typename ColType::iterator iterator;
	typedef typename ColType::const_iterator const_iterator;
	//typedef typename ValueTypeExtractor< std::list<T> >::value value_type;
	typedef T value_type;

	bool add(ThisCol c,const value_type& i) {
		c->push_back(i);
		return true;
	}

	ThisCol instantiate() {
		return new ColType();
	}

	ThisCol instantiate(int size) {
		return new ColType();
	}

	iterator begin(ThisCol c) {
		return c->begin();
	}

	iterator end(ThisCol c) {
		return c->end();
	}

	const iterator begin(ConstCol c) {
		return c->cbegin();
	}

	const iterator end(ConstCol c) {
		return c->cend();
	}

	int getSize(const ThisCol c) {
		return c->size();
	}

	value_type& getByIndex(ThisCol c,int i) {
        assert(getSize(c) > i);

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return *iter;
    }

    bool erase(ThisCol c,iterator beg) {
        c.erase(beg);
        return true;
    }

    bool erase(ThisCol c,iterator beg,iterator end) {
        c.erase(beg,end);
        return true;
    }

    iterator iter_at(ThisCol c,int i) {
        assert(getSize(c) > i);

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return iter;
    }
};

}
}

#endif /* end of include guard: STDLIST_8E2DYM7Y */
