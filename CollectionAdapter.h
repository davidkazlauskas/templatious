
#ifndef COL_ADAPTER_SADWA
#define COL_ADAPTER_SADWA

#include <type_traits>
#include <array>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter {

    static const bool is_valid = false;

	typedef T ThisCol;
	typedef void* iterator;
	typedef const void* const_iterator;
    typedef void* value_type;

	CollectionAdapter() {}

	static bool add(ThisCol& c,const value_type& i);
	static bool remove(ThisCol& c,const value_type& i);
	static value_type& getByIndex(ThisCol& c,int i);
	static int getSize(const ThisCol& c);

    static bool erase(ThisCol& c,iterator beg);
    static bool erase(ThisCol& c,iterator beg,iterator end);

	static ThisCol instantiate();
	static ThisCol instantiate(int size);

    static iterator begin(ThisCol& c);
    static iterator end(ThisCol& c);
    static iterator iter_at(ThisCol& c,int i);
};

struct StaticAdapter {

    template <class T>
	static auto begin(T& c) -> typename CollectionAdapter<T>::iterator {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::begin(c);
    }

    template <class T>
	static auto end(T& c) -> typename CollectionAdapter<T>::iterator {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::end(c);
    }

    template <class T>
	static auto begin(const T& c) -> typename CollectionAdapter<T>::const_iterator {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::begin(c);
    }

    template <class T>
	static auto end(const T& c) -> typename CollectionAdapter<T>::const_iterator {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::end(c);
    }

    //template <class T,class U>
    //static bool add_experimental(T& c,const U& u) {
        //CollectionAdapter<T> a;
        //static_assert(CanAdd<decltype(a),U>::value, "Can't add for jack!");
        //return true;
    //}

    template <class T>
	static bool add(T& c,const typename CollectionAdapter<T>::value_type& i) {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::add(c,i);
    }

    template <class T,int count>
    static bool add(T& c,const std::array<typename CollectionAdapter<T>::value_type,count>& arr) {
        typedef CollectionAdapter<T> Ad;
        for (auto& s: arr) {
            if (!Ad::add(c,s)) {
                return false;
            }
        }

        return true;
    }

    template <class T>
	static bool remove(T& c,const typename CollectionAdapter<T>::value_type& i) {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::remove(c,i);
    }

    template <class T>
    static T instantiate() {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::instantiate();
    }

    template <class T>
    static T instantiate(int size) {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::instantiate(size);
    }

    template <class T>
    static int getSize(const T& c) {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::getSize(c);
    }

    template <class T>
    static auto getByIndex(T& c, int i) -> typename CollectionAdapter<T>::value_type {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::getByIndex(c, i);
    }

    template <class T>
    static bool erase(T& c,typename CollectionAdapter<T>::iterator beg,typename CollectionAdapter<T>::iterator end) {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::erase(c,beg,end);
    }

    template <class T>
    static bool erase(T& c,typename CollectionAdapter<T>::iterator beg) {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::erase(c,beg);
    }

    template <class T>
    static auto iter_at(T& c, int i) -> typename CollectionAdapter<T>::iterator {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid,"Adapter not supported.");
        return Ad::iter_at(c, i);
    }
};

}
}

#endif
