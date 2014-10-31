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
}

#endif /* end of include guard: ONCETRAVERSABLE_7K4ACVNE */
