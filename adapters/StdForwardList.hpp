//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  StdForwardList.hpp
 *
 *    Description:  std::forward_list adapter
 *
 *        Version:  1.0
 *        Created:  04/11/2015 08:29:30 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef STDFORWARDLIST_XN0SHWHB
#define STDFORWARDLIST_XN0SHWHB

#include <utility>
#include <forward_list>

#include <templatious/util/Selectors.hpp>
#include <templatious/CollectionMaker.hpp>
#include <templatious/CollectionAdapter.hpp>

namespace templatious {
namespace adapters {

template <class T,template <class> class Alloc >
struct CollectionAdapter< std::forward_list<T,Alloc<T> > > {

    static const bool is_valid = true;
    static const bool floating_iterator = false;

	typedef typename std::forward_list<T, Alloc<T> > ThisCol;
	typedef typename std::forward_list<T, Alloc<T> > const ConstCol;
	typedef typename ThisCol::iterator Iterator;
	typedef typename ThisCol::const_iterator ConstIterator;
	typedef T ValueType;
	typedef const T ConstValueType;

    template <class V>
	static void add(ThisCol& c,V&& i) {
        c.insert_after(lastIterator(c),std::forward<V>(i));
	}

    template <class V>
    static void insertAt(ThisCol& c, Iterator at, V&& v) {
        auto bef = iteratorBefore(c,at);
        if (bef == c.end()) {
            throw CollectionAdapterIteratorOutOfBoundsException();
        }

        c.insert_after(bef,std::forward<V>(v));
    }

	static Iterator begin(ThisCol& c) {
		return c.begin();
	}

	static Iterator end(ThisCol& c) {
		return c.end();
	}

    static Iterator iterAt(ThisCol& c,long pos) {
        if (pos < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        auto res = c.begin();
        auto end = c.end();
        long cnt = 0;
        while (cnt < pos) {
            if (end == res) {
                throw CollectionAdapterNoSuchIteratorException();
            }
            ++res;
            ++cnt;
        }
        return res;
    }

    static ConstIterator iterAt(ConstCol& c,long pos) {
        if (pos < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        auto res = c.cbegin();
        auto end = c.cend();
        long cnt = 0;
        while (cnt < pos) {
            if (end == res) {
                throw CollectionAdapterNoSuchIteratorException();
            }
            ++res;
            ++cnt;
        }
        return res;
    }

    static ConstIterator citerAt(ConstCol& c,long pos) {
        if (pos < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        auto res = c.cbegin();
        auto end = c.cend();
        long cnt = 0;
        while (cnt < pos) {
            if (end == res) {
                throw CollectionAdapterNoSuchIteratorException();
            }
            ++res;
            ++cnt;
        }
        return res;
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

    // So, you're greedy enough not
    // to save size due to "performance"?
	static long size(ConstCol& c) {
        long sum = 0;
        for (auto& i: c) {
            ++sum;
        }
		return sum;
	}

    static ValueType& getByIndex(ThisCol& c, long i) {
        return *iterAt(c,i);
    }

    static ConstValueType& getByIndex(ConstCol& c, long i) {
        return *citerAt(c,i);
    }

    // this is probably
    // the biggest embarassment
    // of this adapter
    static void erase(ThisCol& c,Iterator pos) {
        auto bef = iteratorBefore(c,pos);
        if (bef == c.cend()) {
            throw CollectionAdapterIteratorOutOfBoundsException();
        }
        c.erase_after(bef);
    }

    static void erase(ThisCol& c,Iterator beg,Iterator end) {
        auto bef = iteratorBefore(c,beg);
        if (bef == c.cend()) {
            throw CollectionAdapterIteratorOutOfBoundsException();
        }
        c.erase_after(bef,end);
    }

    static ValueType& first(ThisCol& c) {
        return c.front();
    }

    static ConstValueType& first(ConstCol& c) {
        return c.front();
    }

    static ValueType& last(ThisCol& c) {
        return *lastIterator(c);
    }

    static ConstValueType& last(ConstCol& c) {
        return *clastIterator(c);
    }

    static void clear(ThisCol& c) {
        c.clear();
    }

    static bool canAdd(ConstCol& c) {
        return true;
    }

private:
    // The missing end iter...
    static Iterator lastIterator(ThisCol& c) {
        auto beforeEnd = c.before_begin();
        for (auto& i: c) {
            ++beforeEnd;
        }
        return beforeEnd;
    }

    static ConstIterator clastIterator(ConstCol& c) {
        auto beforeEnd = c.cbefore_begin();
        for (auto& i: c) {
            ++beforeEnd;
        }
        return beforeEnd;
    }

    static Iterator iteratorBefore(ThisCol& c,const Iterator& i) {
        auto bbegin = c.before_begin();
        auto begin = c.begin();
        auto end = c.end();
        while (begin != i && begin != end) {
            bbegin = begin;
            ++begin;
        }

        if (begin == i) {
            return bbegin;
        }
        return c.end();
    }

    static ConstIterator citeratorBefore(ConstCol& c,const ConstIterator& i) {
        auto bbegin = c.cbefore_begin();
        auto begin = c.cbegin();
        auto end = c.cend();
        while (begin != i && begin != end) {
            bbegin = begin;
            ++begin;
        }

        if (begin == i) {
            return bbegin;
        }
        return c.cend();
    }
};

template <class T,template <class> class Alloc >
struct CollectionAdapter< const std::forward_list<T,Alloc<T> > > {

    static const bool is_valid = true;
    static const bool floating_iterator = true;

	typedef typename std::forward_list<T, Alloc<T> > const ThisCol;
	typedef typename std::forward_list<T, Alloc<T> > const ConstCol;
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
        long sum = 0;
        for (auto& i: c) {
            ++sum;
        }
        return sum;
	}

    static ConstValueType& getByIndex(ConstCol& c, long i) {
        return *citerAt(c,i);
    }

    static Iterator iterAt(ConstCol& c,long pos) {
        if (pos < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        auto res = c.cbegin();
        auto end = c.cend();
        long cnt = 0;
        while (cnt < pos) {
            if (end == res) {
                throw CollectionAdapterNoSuchIteratorException();
            }
            ++res;
            ++cnt;
        }
        return res;
    }

    static Iterator citerAt(ConstCol& c,long pos) {
        if (pos < 0) {
            throw CollectionAdapterNoSuchIteratorException();
        }

        auto res = c.cbegin();
        auto end = c.cend();
        long cnt = 0;
        while (cnt < pos) {
            if (end == res) {
                throw CollectionAdapterNoSuchIteratorException();
            }
            ++res;
            ++cnt;
        }
        return res;
    }

    template <class U = int>
    static void erase(ThisCol& c,Iterator pos) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    template <class U = int>
    static void erase(ThisCol& c,Iterator beg,Iterator end) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static ConstValueType& first(ConstCol& c) {
        return c.front();
    }

    static ConstValueType& last(ConstCol& c) {
        return *clastIterator(c);
    }

    template <class U = int>
    static void clear(ThisCol& c) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "Const version of a collection doesn't support this method");
    }

    static bool canAdd(ThisCol& c) {
        return false;
    }

private:
    static ConstIterator clastIterator(ConstCol& c) {
        auto beforeEnd = c.cbefore_begin();
        for (auto& i: c) {
            ++beforeEnd;
        }
        return beforeEnd;
    }

    static ConstIterator citeratorBefore(ConstCol& c,const ConstIterator& i) {
        auto bbegin = c.cbefore_begin();
        auto begin = c.cbegin();
        auto end = c.cend();
        while (begin != i && begin != end) {
            bbegin = begin;
            ++begin;
        }

        if (begin == i) {
            return bbegin;
        }
        return c.cend();
    }
};


template <
    class Val,
    template <class> class Alloc
>
struct CollectionMaker<Val,std::forward_list,Alloc> {
    typedef std::forward_list<Val,Alloc<Val> > Collection;
    typedef Collection* CollectionPtr;

    static const bool is_maker_valid = true;

    static Collection make() {
        return std::move(Collection());
    }

    static Collection make(long size) {
        return std::move(Collection());
    }

    static Collection* makeHeap() {
        return new Collection();
    }

    static Collection* makeHeap(long size) {
        return new Collection();
    }

};

}
}

#endif /* end of include guard: STDVECTOR_H6YPGTPK */
