
#ifndef COLLECTIONADAPTER_728YVC81
#define COLLECTIONADAPTER_728YVC81

#include <type_traits>
#include <array>
#include <memory>
#include <initializer_list>

#include <templatious/util/Selectors.h>
#include <templatious/util/Comparator.h>

namespace templatious {
namespace adapters {

template <class T>
struct CollectionAdapter {
    static const bool is_valid = false;

    typedef void* ThisCol;
    typedef const void* ConstCol;
    typedef void* iterator;
    typedef const void* const_iterator;
    typedef void* value_type;
    typedef const void* const_value_type;

    static bool add(ThisCol& c, const value_type& i);
    static bool remove(ThisCol& c, const value_type& i);
    static value_type& getByIndex(ThisCol& c, int i);
    static const_value_type& getByIndex(ConstCol& c, int i);

    static size_t getSize(const ThisCol& c);

    static bool erase(ThisCol& c, iterator beg);
    static bool erase(ThisCol& c, iterator beg, iterator end);

    static ThisCol instantiate();
    static ThisCol instantiate(int size);

    static iterator begin(ThisCol& c);
    static iterator end(ThisCol& c);
    static iterator iter_at(ThisCol& c, int i);

    static const_iterator cbegin(ThisCol& c);
    static const_iterator cend(ThisCol& c);
    static const_iterator citer_at(ThisCol& c, int i);

    static value_type& first(ThisCol& c);
    static const value_type& first(ConstCol& c);
    static value_type& last(ThisCol& c);
    static const value_type& last(ConstCol& c);

    static bool insert_at(ThisCol& c, iterator at, const value_type& i);

    static void clear(ThisCol& c);
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

    template <class T>
    static auto cbegin(const T& c)
        -> typename adapters::CollectionAdapter<T>::const_iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::cbegin(c);
    }

    template <class T>
    static auto cend(const T& c)
        -> typename adapters::CollectionAdapter<T>::const_iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::cend(c);
    }

    template <class T, class U>
    static bool add(T& c, const std::initializer_list<U>& o) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        for (auto i = o.begin(); i != o.end(); ++i) {
            if (!Ad::add(c,*i)) {
                return false;
            }
        }
        return true;
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
    static size_t getSize(const T& c) {
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
    static auto iterAt(T& c, int i)
        -> typename adapters::CollectionAdapter<T>::iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::iter_at(c, i);
    }

    template <class T>
    static auto citerAt(T& c, int i)
        -> typename adapters::CollectionAdapter<T>::const_iterator {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::citer_at(c, i);
    }

    template <class T>
    static bool insert(
        T& c, typename adapters::CollectionAdapter<T>::iterator at,
        const typename adapters::CollectionAdapter<T>::value_type& val) 
    {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        return Ad::insert_at(c,at,val);
    }

    template <class T>
    static void clear(T& c) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        Ad::clear(c);
    }

    template <bool reverse = false,class T,class Comp = typename templatious::util::Default>
    static bool sortedAdd(T& c, const typename adapters::CollectionAdapter<T>::value_type& val) {
        typedef adapters::CollectionAdapter<T> Ad;
        static_assert(Ad::is_valid, "Adapter not supported.");
        typedef typename Ad::value_type ValType;

        typedef typename templatious::util::ComparatorDiff<ValType,ValType,Comp> Comparator;
        auto comp = templatious::util::rev<reverse>(Comparator());

        if (0 == Ad::getSize(c)) {
            return Ad::add(c,val);
        }

        if (0 >= comp(Ad::last(c),val)) {
            return Ad::add(c,val);
        }

        if (0 <= comp(Ad::first(c),val)) {
            return Ad::insert_at(c,Ad::begin(c),val);
        }

        for (auto i = Ad::begin(c); i != Ad::end(c); ++i) {
            if (0 <= comp(*i,val)) {
                return Ad::insert_at(c,i,val);
            }
        }

        assert(false);

    }

};
}

#endif
