
#ifndef COLLECTIONADAPTER_728YVC81
#define COLLECTIONADAPTER_728YVC81

#include <type_traits>
#include <array>

#include <templatious/util/Selectors.h>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter {
    static const bool is_valid = false;
    static const bool hash_supported = false;

    typedef void* ThisCol;
    typedef void* iterator;
    typedef const void* const_iterator;
    typedef void* value_type;

    CollectionAdapter() {}

    static bool add(ThisCol& c, const value_type& i);
    static bool remove(ThisCol& c, const value_type& i);
    static value_type& getByIndex(ThisCol& c, int i);
    static int getSize(const ThisCol& c);

    static bool erase(ThisCol& c, iterator beg);
    static bool erase(ThisCol& c, iterator beg, iterator end);

    static ThisCol instantiate();
    static ThisCol instantiate(int size);

    static iterator begin(ThisCol& c);
    static iterator end(ThisCol& c);
    static iterator iter_at(ThisCol& c, int i);
};

template <class T, class U>
struct AdditionSelector {
    static const bool areAdaptable =
        CollectionAdapter<T>::is_valid && CollectionAdapter<U>::is_valid;
    enum {
        val = templatious::util::IntSelector<
            areAdaptable, 1, templatious::util::IntSelector<
                                 std::is_array<U>::value, 2, 0>::val>::val
    };
};

template <int var>
struct add_custom;

template <>  // add data one by one
struct add_custom<0> {
    template <class T>
    static bool add(T& c, const typename CollectionAdapter<T>::value_type& i) {
        typedef CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::add(c, i);
    }
};

template <>  // add collections
struct add_custom<1> {
    template <class T, class U>
    static bool add(T& t, const U& u) {
        typedef CollectionAdapter<T> AdT;
        typedef CollectionAdapter<U> AdU;
        static_assert(AdT::is_valid, "Adapter not supported.");
        static_assert(AdU::is_valid, "Adapter not supported.");
        for (auto i = AdU::begin(u); i != AdU::end(u); ++i) {
            if (!AdT::add(t, *i)) {
                return false;
            }
        }

        return true;
    }
};

template <>  // add static arrays
struct add_custom<2> {
    template <class T, class Arr, unsigned long count>
    static bool add(T& c, const Arr (&arr)[count]) {
        typedef CollectionAdapter<T> Ad;
        for (int i = 0; i < count; ++i) {
            if (!Ad::add(c, arr[i])) {
                return false;
            }
        }

        return true;
    }
};
}

struct StaticAdapter {
    template <class T>
    static auto begin(T& c)
        -> typename adapters::CollectionAdapter<T>::iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::begin(c);
    }

    template <class T>
    static auto end(T& c) -> typename adapters::CollectionAdapter<T>::iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::end(c);
    }

    template <class T>
    static auto begin(const T& c)
        -> typename adapters::CollectionAdapter<T>::const_iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::begin(c);
    }

    template <class T>
    static auto end(const T& c)
        -> typename adapters::CollectionAdapter<T>::const_iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::end(c);
    }

    template <class T, class U>
    static bool add(T& c, const U& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return adapters::add_custom<
                adapters::AdditionSelector<T, U>::val
            >::add(c, o);
    }

    template <class T, class U, class... Args>
    static bool add(T& c, const U& o, const Args&... args) {
        if (!add(c, o)) {
            return false;
        }

        return add(c, args...);
    }

    template <class T>
    static bool remove(
        T& c, const typename adapters::CollectionAdapter<T>::value_type& i) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::remove(c, i);
    }

    template <class T>
    static T instantiate() {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::instantiate();
    }

    template <class T>
    static T instantiate(int size) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::instantiate(size);
    }

    template <class T>
    static int getSize(const T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::getSize(c);
    }

    template <class T>
    static auto getByIndex(T& c, int i)
        -> typename adapters::CollectionAdapter<T>::value_type {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::getByIndex(c, i);
    }

    template <class T>
    static bool erase(T& c,
                      typename adapters::CollectionAdapter<T>::iterator beg,
                      typename adapters::CollectionAdapter<T>::iterator end) 
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::erase(c, beg, end);
    }

    template <class T>
    static bool erase(T& c,
                      typename adapters::CollectionAdapter<T>::iterator beg) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::erase(c, beg);
    }

    template <class T>
    static auto iter_at(T& c, int i)
        -> typename adapters::CollectionAdapter<T>::iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::iter_at(c, i);
    }
};
}

#endif
