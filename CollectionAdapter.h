
#ifndef COL_ADAPTER_SADWA
#define COL_ADAPTER_SADWA

#include <vector>

namespace templatious {
    namespace adapters {

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

}
}

#endif
