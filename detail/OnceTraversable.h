/*
 * =====================================================================================
 *
 *       Filename:  OnceTraversable.h
 *
 *    Description:  Once traversable collection wrapper that mutates iterators
 *
 *        Version:  1.0
 *        Created:  10/31/2014 03:52:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ONCETRAVERSABLE_7K4ACVNE
#define ONCETRAVERSABLE_7K4ACVNE

#include <templatious/CollectionAdapter.h>

#include <utility>

namespace templatious {
namespace detail {

template <class T>
struct OnceTraversable {
    typedef OnceTraversable<T> ThisTraversable;

    OnceTraversable(const T& beg,const T& end)
     : _b(beg), _e(end) {}

    struct Iterator {
        Iterator(T& t) : _t(t) {}

        auto operator*()
         -> decltype(*std::declval<T>())
        {
            return *_t;
        }

        Iterator operator++() {
            ++_t;
            return *this;
        }
    private:
        T& _t;
    };

    Iterator begin() {
        return Iterator(_b);
    }

    Iterator end() {
        return Iterator(_e);
    }
private:
    T _b;
    T _e;
};

}

namespace adapters {

template <class T>
struct CollectionAdapter<
    ::templatious::detail::OnceTraversable<T>
>{

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef ::templatious::detail::OnceTraversable<T> ThisCol;
    typedef ::templatious::detail::OnceTraversable<const T> ConstCol;
    typedef typename ThisCol::Iterator iterator;
    typedef typename ConstCol::Iterator const_iterator;
    typedef decltype(
        *(std::declval<ThisCol>().begin())
    ) value_type;
    typedef decltype(
        *(std::declval<ConstCol>().begin())
    ) const_value_type;

    template <class V,class U = int>
    static void add(ThisCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

    template <class V,class U = int>
    static void insert(ThisCol c, V&& i) {
        static_assert(templatious::util::DummyResolver<U,false>::val,
                "OnceTraversable is not full fledged collection"
                " and can be traversed with iterators only.");
    }

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

#endif /* end of include guard: ONCETRAVERSABLE_7K4ACVNE */
