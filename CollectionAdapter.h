
#ifndef COL_ADAPTER_SADWA
#define COL_ADAPTER_SADWA

#include <vector>

template <class T>
struct CollectionAdapter {

	typedef T ThisCol;
	typedef void* iterator;
	typedef typename T::value_type value_type;

	CollectionAdapter();

	bool add(ThisCol& c,const value_type& i);
	bool remove(ThisCol& c,const value_type& i);
	value_type& getByIndex(int i);
	int getSize(const ThisCol& c);
	ThisCol instantiate();
	ThisCol instantiate(int size);
	iterator begin(ThisCol& c);
	iterator end(ThisCol& c);

};

template <class T>
struct CollectionAdapter<std::vector<T>> {

	typedef typename std::vector<T> ThisCol;
	typedef typename ThisCol::iterator iterator;
	typedef typename ThisCol::value_type value_type;

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

	int getSize(const ThisCol& c) {
		return c.size();
	}

};

#endif
