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
    static const bool floating_iterator = false;

	typedef typename std::list<T,Alloc<T> > ThisCol;
	typedef typename std::list<T,Alloc<T> > const ConstCol;
	typedef typename ThisCol::iterator Iterator;
	typedef typename ThisCol::const_iterator ConstIterator;
	typedef T ValueType;
	typedef const T ConstValueType;

    template <class V>
	static void add(ThisCol& c,V&& i) {
		c.push_back(std::forward<V>(i));
	}

    template <class V>
    static void insertAt(ThisCol& c,Iterator at,V&& v) {
        c.insert(at,std::forward<V>(v));
    }

	static ThisCol instantiate() {
		return std::move(ThisCol()); }

	static ThisCol instantiate(int size) {
		ThisCol r;
		return std::move(r);
	}

	static Iterator begin(ThisCol& c) {
		return c.begin();
	}

	static Iterator end(ThisCol& c) {
		return c.end();
	}

	static ConstIterator begin(ConstCol& c) {
		return c.cbegin();
	}

	static ConstIterator end(ConstCol& c) {
		return c.cend();
	}

	static ConstIterator cbegin(ConstCol& c) {
		return c.cbegin();
	}

	static ConstIterator cend(ConstCol& c) {
		return c.cend();
	}

	static size_t size(ConstCol& c) {
		return c.size();
	}

	static ValueType& getByIndex(ThisCol& c,int i) {
        if (size(c) <= i) {
            throw CollectionAdapterNoSuchElementException();
        }

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return *iter;
    }

	static ConstValueType& getByIndex(ConstCol& c,int i) {
        if (size(c) <= i) {
            throw CollectionAdapterNoSuchElementException();
        }

        int count = 0;
        auto iter = cbegin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return *iter;
    }

    static void erase(ThisCol& c,Iterator pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c,Iterator beg,Iterator end) {
        c.erase(beg,end);
    }

    static Iterator iterAt(ThisCol& c,size_t i) {
        if (size(c) < i) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return iter;
    }

    static ConstIterator iterAt(ConstCol& c,size_t i) {
        if (size(c) < i) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        int count = 0;
        auto iter = cbegin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return iter;
    }

    static Iterator iterAt(ThisCol&& c,size_t i) {
        if (size(c) < i) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return iter;
    }

    static ConstIterator citerAt(ConstCol& c,size_t i) {
        if (size(c) < i) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        int count = 0;
        auto iter = cbegin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return iter;
    }

    static ValueType& first(ThisCol& c) {
        return c.front();
    }

    static ConstValueType& first(ConstCol& c) {
        return c.front();
    }

    static ValueType& last(ThisCol& c) {
        return c.back();
    }

    static ConstValueType& last(ConstCol& c) {
        return c.back();
    }

    static void clear(ThisCol& c) {
        c.clear();
    }

    static bool canAdd(ThisCol& c) {
        return true;
    }
};


template <class T,template <class> class Alloc >
struct CollectionAdapter< const std::list<T,Alloc<T> > > {

    static const bool is_valid = true;
    static const bool floating_iterator = false;

	typedef typename std::list<T, Alloc<T> > const ThisCol;
	typedef typename std::list<T, Alloc<T> > const ConstCol;
	typedef typename ThisCol::const_iterator Iterator;
	typedef typename ThisCol::const_iterator ConstIterator;
	typedef const T ValueType;
	typedef const T ConstValueType;

    template <class V,class U = int>
	static void add(ThisCol& c,V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
	}

    template <class V,class U = int>
    static void insertAt(ThisCol& c, Iterator at, V&& v) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }


	static ThisCol instantiate() {
		return std::move(ThisCol());
	}

	static ThisCol instantiate(size_t size) {
		ThisCol r;
		return std::move(r);
	}

	static ThisCol* instHeap() {
		return new ThisCol();
	}

	static ThisCol* instHeap(size_t size) {
		ThisCol* r = new ThisCol();
		return r;
	}

	static ConstIterator begin(ConstCol& c) {
		return c.cbegin();
	}

	static ConstIterator end(ConstCol& c) {
		return c.cend();
	}

	static ConstIterator cbegin(ConstCol& c) {
		return c.cbegin();
	}

	static ConstIterator cend(ConstCol& c) {
		return c.cend();
	}

	static size_t size(ConstCol& c) {
		return c.size();
	}

    static ConstValueType& getByIndex(ThisCol& c, size_t i) {
        if (size(c) < i) {
            throw CollectionAdapterNoSuchElementException();
        }

        int count = 0;
        auto iter = begin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return *iter;
    }

    template <class U>
    static void erase(ThisCol& c,Iterator pos) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U>
    static void erase(ThisCol& c,Iterator beg,Iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static ConstValueType& first(ConstCol& c) {
        return c.front();
    }

    static ConstValueType& last(ConstCol& c) {
        return c.back();
    }

    static ConstIterator iterAt(ConstCol& c,size_t i) {
        return citerAt(c,i);
    }

    static ConstIterator citerAt(ConstCol& c,size_t i) {
        if (size(c) < i) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        int count = 0;
        auto iter = cbegin(c);
        while (count < i) {
            ++iter;
            ++count;
        }

        return iter;
    }

    template <class U>
    static void clear(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static bool canAdd(ThisCol& c) {
        return false;
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
