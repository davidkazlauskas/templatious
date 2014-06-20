/*
 * =====================================================================================
 *
 *       Filename:  Hasher.h
 *
 *    Description:  Hash classes and compare
 *
 *        Version:  1.0
 *        Created:  06/20/2014 03:59:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef HASHER_W4HP5KR9
#define HASHER_W4HP5KR9

namespace templatious {
namespace util {

template <class T>
struct Hasher {

    static const bool is_valid = false;

    size_t operator()(const T& t);
};

template <class T>
struct HashComparator {

    T _h;

    static_assert(T::is_valid,"Hash function is not valid.");

    HashComparator(const T& h) : _h(h) {}

    bool isLess(const T& t1,const T& t2) {
        return _h(t1) < _h(t2);
    }

    bool isMore(const T& t1,const T& t2) {
        return _h(t1) > _h(t2);
    }

    bool isEqual(const T& t1,const T& t2) {
        return _h(t1) == _h(t2);
    }

};

}
}


#endif /* end of include guard: HASHER_W4HP5KR9 */

