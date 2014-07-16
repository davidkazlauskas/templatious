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

#include <utility>
#include <list>

#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace adapters {

template <class T,template <class> class Alloc >
struct CollectionAdapter< std::list<T,Alloc<T> > > {

    static const bool is_valid = true;

	typedef typename std::list<T,Alloc<T> > ThisCol;
	typedef typename std::list<T,Alloc<T> > const ConstCol;
	typedef typename ThisCol::iterator iterator;
	typedef typename ThisCol::const_iterator const_iterator;
	typedef T value_type;
	typedef const T const_value_type;

	static bool add(ThisCol& c,const value_type& i) {
		c.push_back(i);
		return true;
	}

	static bool add(ThisCol& c,value_type&& i) {
		c.push_back(i);
		return true;
	}

	static ThisCol instantiate() {
		return std::move(ThisCol()); }

	static ThisCol instantiate(int size) {
		ThisCol r;
		return std::move(r);
	}

	static iterator begin(ThisCol& c) {
		return c.begin();
	}

	static iterator end(ThisCol& c) {
		return c.end();
	}

	static const_iterator begin(ConstCol& c) {
		return c.cbegin();
	}

	static const_iterator end(ConstCol& c) {
		return c.cend();
	}

	static const_iterator cbegin(ConstCol& c) {
		return c.cbegin();
	}

	static const_iterator cend(ConstCol& c) {
		return c.cend();
	}

	static size_t getSize(ConstCol& c) {
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

    static bool insert_at(ThisCol& c, iterator at, const value_type& v) {
        c.insert(at,v);
        return true;
    }

    static value_type& first(ThisCol& c) {
        return c.front();
    }

    static const_value_type& first(ConstCol& c) {
        return c.front();
    }

    static value_type& last(ThisCol& c) {
        return c.back();
    }

    static const_value_type& last(ConstCol& c) {
        return c.back();
    }

    static void clear(ThisCol& c) {
        c.clear();
    }
};

template <class T,template <class> class Alloc >
struct CollectionAdapter< std::list<T,Alloc<T> >* > {

    static const bool is_valid = true;

	typedef typename std::list<T, Alloc<T> >* ThisCol;
	typedef typename std::list<T, Alloc<T> > const* ConstCol;
	typedef typename std::list<T, Alloc<T> > ColType;
	typedef typename ColType::iterator iterator;
	typedef typename ColType::const_iterator const_iterator;
	typedef T value_type;
	typedef const T const_value_type;

	static bool add(ThisCol c,const value_type& i) {
		c->push_back(i);
		return true;
	}

	static bool add(ThisCol c,value_type&& i) {
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

	static const iterator cbegin(ConstCol c) {
		return c->cbegin();
	}

	static const iterator cend(ConstCol c) {
		return c->cend();
	}

	static size_t getSize(ConstCol c) {
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
        c->erase(beg);
        return true;
    }

    static bool erase(ThisCol c,iterator beg,iterator end) {
        c->erase(beg,end);
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

    static const_iterator citer_at(ConstCol c,int i) {
        assert(getSize(c) > i);

        int count = 0;
        auto iter = cbegin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return iter;
    }

    static bool insert_at(ThisCol c, iterator at, const value_type& v) {
        c->insert(at,v);
        return true;
    }

    static value_type& first(ThisCol c) {
        return c->front();
    }

    static const_value_type& first(ConstCol c) {
        return c->front();
    }

    static value_type& last(ThisCol c) {
        return c->back();
    }

    static const_value_type& last(ConstCol c) {
        return c->back();
    }

    static void clear(ThisCol c) {
        c->clear();
    }
};

template <
    class Val,
    template <class> class Alloc
>
struct CollectionMaker<Val,std::list,Alloc> {
    typedef std::list<Val,Alloc<Val> > Collection;
    typedef Collection* CollectionPtr;

    static const bool is_maker_valid = true;

    static Collection make() {
        return std::move(Collection());
    }

    static Collection make(size_t size) {
        Collection res;
        return std::move(res);
    }

    static Collection* makeHeap() {
        return new Collection();
    }

    static Collection* makeHeap(size_t size) {
        CollectionPtr res = new Collection();
        return res;
    }

};

}
}

#endif /* end of include guard: STDLIST_8E2DYM7Y */
