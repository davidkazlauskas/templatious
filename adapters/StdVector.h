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

#include <utility>
#include <vector>
#include <templatious/CollectionMaker.h>
#include <templatious/CollectionAdapter.h>

namespace templatious {
namespace adapters {

template <class T,template <class> class Alloc >
struct CollectionAdapter< std::vector<T,Alloc<T> > > {

    static const bool is_valid = true;

	typedef typename std::vector<T, Alloc<T> > ThisCol;
	typedef typename std::vector<T, Alloc<T> > const ConstCol;
	typedef typename ThisCol::iterator iterator;
	typedef typename ThisCol::const_iterator const_iterator;
	typedef T value_type;
	typedef const T const_value_type;

    template <class V>
	static bool add(ThisCol& c,V&& i) {
		c.push_back(std::forward<V>(i));
		return true;
	}

    template <class V>
    static bool insert_at(ThisCol& c, iterator at, V&& v) {
        assert(at >= begin(c) && at < end(c));

        c.insert(at,std::forward<V>(v));
        return true;
    }


	static ThisCol instantiate() {
		return std::move(ThisCol());
	}

	static ThisCol instantiate(int size) {
		ThisCol r;
		r.reserve(size);
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

    static value_type& getByIndex(ThisCol& c, int i) {
        return c[i];
    }

    static bool erase(ThisCol& c,iterator beg) {
        c.erase(beg);
        return true;
    }

    static bool erase(ThisCol& c,iterator beg,iterator end) {
        c.erase(beg,end);
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

template <class T,template <class> class Alloc>
struct CollectionAdapter< std::vector<T,Alloc<T> >* > {

    static const bool is_valid = true;

	typedef typename std::vector<T, Alloc<T> >* ThisCol;
    typedef typename std::vector<T, Alloc<T> > const* ConstCol;
    typedef typename std::vector<T, Alloc<T> > ColType;
	typedef typename ColType::iterator iterator;
	typedef typename ColType::const_iterator const_iterator;
	typedef T value_type;
	typedef const T const_value_type;

    template <class V>
	static bool add(ThisCol c,V&& i) {
		c->push_back(std::forward<V>(i));
		return true;
	}

    template <class V>
    static bool insert_at(ThisCol c, iterator at,V&& v) {
        assert(at >= begin(c) && at < end(c));

        c->insert(at,std::forward<V>(v));
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

	static const_iterator cbegin(ConstCol c) {
		return c->cbegin();
	}

	static const_iterator cend(ConstCol c) {
		return c->cend();
	}

	static size_t getSize(ConstCol c) {
		return c->size();
	}

    static value_type& getByIndex(ConstCol c, int i) {
        return (*c)[i];
    }

    static bool erase(ThisCol c,iterator beg) {
        c->erase(beg);
        return true;
    }

    static bool erase(ThisCol c,iterator beg,iterator end) {
        c->erase(beg,end);
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
struct CollectionMaker<Val,std::vector,Alloc> {
    typedef std::vector<Val,Alloc<Val> > Collection;
    typedef Collection* CollectionPtr;

    static const bool is_maker_valid = true;

    static Collection make() {
        return std::move(Collection());
    }

    static Collection make(size_t size) {
        Collection res;
        res.reserve(size);
        return std::move(res);
    }

    static Collection* makeHeap() {
        return new Collection();
    }

    static Collection* makeHeap(size_t size) {
        CollectionPtr res = new Collection();
        res->reserve(size);
        return res;
    }

};

}
}

#endif /* end of include guard: STDVECTOR_H6YPGTPK */
