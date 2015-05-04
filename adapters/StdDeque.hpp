//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  StdList.hpp
 *
 *    Description:  std::deque adapter
 *
 *        Version:  1.0
 *        Created:  04/11/2015 13:51:27 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef STDDEQUE_CBCZLYNK
#define STDDEQUE_CBCZLYNK

#include <utility>
#include <deque>

#include <templatious/util/Selectors.hpp>
#include <templatious/CollectionAdapter.hpp>
#include <templatious/CollectionMaker.hpp>

namespace templatious {
namespace adapters {

template <class T,template <class> class Alloc >
struct CollectionAdapter< std::deque< T,Alloc<T> > > {

    static const bool is_valid = true;
    static const bool floating_iterator = true;

	typedef typename std::deque< T,Alloc<T> > ThisCol;
	typedef typename std::deque< T,Alloc<T> > const ConstCol;
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

	static long size(ConstCol& c) {
		return c.size();
	}

	static ValueType& getByIndex(ThisCol& c,long i) {
        if (size(c) <= i) {
            throw CollectionAdapterNoSuchElementException();
        }

        return *iterAt(c,i);
    }

	static ConstValueType& getByIndex(ConstCol& c,long i) {
        if (size(c) <= i) {
            throw CollectionAdapterNoSuchElementException();
        }

        return *citerAt(c,i);
    }

    static void erase(ThisCol& c,Iterator pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c,Iterator beg,Iterator end) {
        c.erase(beg,end);
    }

    static Iterator iterAt(ThisCol& c,long i) {
        // no compiler warning
        if (static_cast<long>(size(c)) < i || i < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }
        auto iter = begin(c);
        std::advance(iter,i);
        return iter;
    }

    static ConstIterator iterAt(ConstCol& c,long i) {
        // no compiler warning
        if (static_cast<long>(size(c)) < i || i < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }
        auto iter = cbegin(c);
        std::advance(iter,i);
        return iter;
    }

    static ConstIterator citerAt(ConstCol& c,long i) {
        if (static_cast<long>(size(c)) < i || i < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        auto iter = cbegin(c);
        std::advance(iter,i);
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

    static bool canAdd(ConstCol& c) {
        return true;
    }
};


template <class T,template <class> class Alloc >
struct CollectionAdapter< const std::deque< T,Alloc<T> > > {

    static const bool is_valid = true;
    static const bool floating_iterator = false;

	typedef typename std::deque< T,Alloc<T> > const ThisCol;
	typedef typename std::deque< T,Alloc<T> > const ConstCol;
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

	static long size(ConstCol& c) {
		return c.size();
	}

    static ConstValueType& getByIndex(ThisCol& c, long i) {
        if (size(c) <= i) {
            throw CollectionAdapterNoSuchElementException();
        }

        return *iterAt(c,i);
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

    static ConstIterator iterAt(ConstCol& c,long i) {
        return citerAt(c,i);
    }

    static ConstIterator citerAt(ConstCol& c,long i) {
        if (size(c) < i || i < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        auto iter = cbegin(c);
        std::advance(iter,i);
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
struct CollectionMaker<Val,std::deque,Alloc> {
    typedef std::deque< Val,Alloc<Val> > Collection;
    typedef Collection* CollectionPtr;

    static const bool is_maker_valid = true;

    static Collection make() {
        return std::move(Collection());
    }

    static Collection make(long size) {
        Collection res;
        return std::move(res);
    }

    static Collection* makeHeap() {
        return new Collection();
    }

    static Collection* makeHeap(long size) {
        CollectionPtr res = new Collection();
        return res;
    }

};

}
}

#endif /* end of include guard: STDLIST_8E2DYM7Y */
