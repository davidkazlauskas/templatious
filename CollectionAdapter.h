
#ifndef COL_ADAPTER_SADWA
#define COL_ADAPTER_SADWA

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter {

	typedef T ThisCol;
	typedef void* iterator;
    typedef typename T::value_type value_type;
	//typedef void value_type;

	CollectionAdapter();

	bool add(ThisCol& c,const value_type& i);
	bool remove(ThisCol& c,const value_type& i);
	value_type& getByIndex(int i);
	int getSize(const ThisCol& c);
	ThisCol instantiate();
	ThisCol instantiate(int size);

    static iterator begin(ThisCol& c);
    static iterator end(ThisCol& c);
};

struct StaticAdapter {

    template <class T>
	static auto begin(T& c) -> typename CollectionAdapter<T>::iterator {
        CollectionAdapter<T> a;
        return a.begin(c);
    }

    template <class T>
	static auto end(T& c) -> typename CollectionAdapter<T>::iterator {
        CollectionAdapter<T> a;
        return a.end(c);
    }

    template <class T>
	static bool add(T& c,const typename CollectionAdapter<T>::value_type& i) {
        CollectionAdapter<T> a;
        return a.add(c,i);
    }

    template <class T>
	static bool remove(T& c,const typename CollectionAdapter<T>::value_type& i) {
        CollectionAdapter<T> a;
        return a.remove(c,i);
    }

};

}
}

#endif
