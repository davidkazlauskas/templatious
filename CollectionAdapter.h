
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
    static const bool floating_iterator = false;

    typedef void* ThisCol;
    typedef const void* ConstCol;
    typedef void* iterator;
    typedef const void* const_iterator;
    typedef void* value_type;
    typedef const void* const_value_type;

    template <class V>
    static void add(ThisCol& c, V&& i);
    template <class V>
    static void insert_at(ThisCol& c, iterator at, V&& i);

    static value_type& getByIndex(ThisCol& c, size_t i);
    static const_value_type& getByIndex(ConstCol& c, size_t i);

    static size_t getSize(ConstCol& c);

    static void erase(ThisCol& c, iterator i);
    static void erase(ThisCol& c, iterator beg, iterator end);

    static ThisCol instantiate();
    static ThisCol instantiate(size_t size);
    static ThisCol* instHeap();
    static ThisCol* instHeap(size_t size);

    static iterator begin(ThisCol& c);
    static iterator end(ThisCol& c);
    static iterator iter_at(ThisCol& c, size_t i);

    static iterator begin(ConstCol& c);
    static iterator end(ConstCol& c);
    static iterator iter_at(ConstCol& c, size_t i);

    static const_iterator cbegin(ConstCol& c);
    static const_iterator cend(ConstCol& c);
    static const_iterator citer_at(ConstCol& c, size_t i);

    static value_type& first(ThisCol& c);
    static const_value_type& first(ConstCol& c);
    static value_type& last(ThisCol& c);
    static const_value_type& last(ConstCol& c);

    static void clear(ThisCol& c);
    static bool canAdd(ConstCol& c);
};

}
}

#endif
