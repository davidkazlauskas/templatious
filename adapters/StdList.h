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

    static const bool is_valid = true;

	typedef typename std::list<T> ThisCol;
	typedef typename ThisCol::iterator iterator;
	typedef typename ThisCol::const_iterator const_iterator;
	//typedef typename ValueTypeExtractor< std::list<T> >::value value_type;
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
		return r;
	}

	static iterator begin(ThisCol& c) {
		return c.begin();
	}

	static iterator end(ThisCol& c) {
		return c.end();
	}

	static const_iterator begin(const ThisCol& c) {
		return c.cbegin();
	}

	static const_iterator end(const ThisCol& c) {
		return c.cend();
	}

	static int getSize(const ThisCol& c) {
		return c.size();
	}

	static value_type& getByIndex(ThisCol& c,int i) {
        assert(getSize(c) > i);

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return *iter;
    }

    static bool erase(ThisCol& c,iterator beg) {
        c.erase(beg);
        return true;
    }

    static bool erase(ThisCol& c,iterator beg,iterator end) {
        c.erase(beg,end);
        return true;
    }

    static iterator iter_at(ThisCol& c,int i) {
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

	static bool add(ThisCol c,const value_type& i) {
		c->push_back(i);
		return true;
	}

	static ThisCol instantiate() {
		return new ColType();
	}

	static ThisCol instantiate(int size) {
		return new ColType();
	}

	static iterator begin(ThisCol c) {
		return c->begin();
	}

	static iterator end(ThisCol c) {
		return c->end();
	}

	static const iterator begin(ConstCol c) {
		return c->cbegin();
	}

	static const iterator end(ConstCol c) {
		return c->cend();
	}

	static int getSize(const ThisCol c) {
		return c->size();
	}

	static value_type& getByIndex(ThisCol c,int i) {
        assert(getSize(c) > i);

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return *iter;
    }

    static bool erase(ThisCol c,iterator beg) {
        c.erase(beg);
        return true;
    }

    static bool erase(ThisCol c,iterator beg,iterator end) {
        c.erase(beg,end);
        return true;
    }

    static iterator iter_at(ThisCol c,int i) {
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
