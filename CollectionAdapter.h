
#ifndef COL_ADAPTER_SADWA
#define COL_ADAPTER_SADWA

#include <array>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter {

	typedef T ThisCol;
	typedef void* iterator;
	typedef const void* const_iterator;
    typedef typename T::value_type value_type;

	CollectionAdapter();

	bool add(ThisCol& c,const value_type& i);
	bool remove(ThisCol& c,const value_type& i);
	value_type& getByIndex(ThisCol& c,int i);
	int getSize(const ThisCol& c);

    bool erase(ThisCol& c,iterator beg);
    bool erase(ThisCol& c,iterator beg,iterator end);

	ThisCol instantiate();
	ThisCol instantiate(int size);

    iterator begin(ThisCol& c);
    iterator end(ThisCol& c);
    iterator iter_at(ThisCol& c,int i);
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
	static auto begin(const T& c) -> typename CollectionAdapter<T>::const_iterator {
        CollectionAdapter<T> a;
        return a.begin(c);
    }

    template <class T>
	static auto end(const T& c) -> typename CollectionAdapter<T>::const_iterator {
        CollectionAdapter<T> a;
        return a.end(c);
    }

    template <class T>
	static bool add(T& c,const typename CollectionAdapter<T>::value_type& i) {
        CollectionAdapter<T> a;
        return a.add(c,i);
    }

    template <class T,int count>
    static bool add(T& c,const std::array<typename CollectionAdapter<T>::value_type,count>& arr) {
        CollectionAdapter<T> a;
        for (auto& s: arr) {
            if (!a.add(c,s)) {
                return false;
            }
        }

        return true;
    }

    template <class T>
	static bool remove(T& c,const typename CollectionAdapter<T>::value_type& i) {
        CollectionAdapter<T> a;
        return a.remove(c,i);
    }

    template <class T>
    static T instantiate() {
        CollectionAdapter<T> a;
        return a.instantiate();
    }

    template <class T>
    static T instantiate(int size) {
        CollectionAdapter<T> a;
        return a.instantiate(size);
    }

    template <class T>
    static int getSize(const T& c) {
        CollectionAdapter<T> a;
        return a.getSize(c);
    }

    template <class T>
    static auto getByIndex(T& c, int i) -> typename CollectionAdapter<T>::value_type {
        CollectionAdapter<T> a;
        return a.getByIndex(c, i);
    }

    template <class T>
    static bool erase(T& c,typename CollectionAdapter<T>::iterator beg,typename CollectionAdapter<T>::iterator end) {
        CollectionAdapter<T> a;
        return a.erase(c,beg,end);
    }

    template <class T>
    static bool erase(T& c,typename CollectionAdapter<T>::iterator beg) {
        CollectionAdapter<T> a;
        return a.erase(c,beg);
    }

    template <class T>
    static auto iter_at(T& c, int i) -> typename CollectionAdapter<T>::iterator {
        CollectionAdapter<T> a;
        return a.iter_at(c, i);
    }
};

}
}

#endif
