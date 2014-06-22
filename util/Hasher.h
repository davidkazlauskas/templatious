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

struct Default;

template <class T,class U = Default>
struct Hasher {

    static const bool is_hash_valid = false;

    typedef T ValueType;
    typedef U Variant;

    size_t operator()(const T& t);
};

template <class T>
struct HashLess {

    static_assert(T::is_hash_valid,"Hash function is not valid.");
    typedef typename T::ValueType ValueType;

    T _h;
    HashLess(const T& h) : _h(h) {}

    bool operator()(const T& t1,const T& t2) {
        return _h(t1) < _h(t2);
    }
};

template <class T>
struct HashMore {

    static_assert(T::is_hash_valid,"Hash function is not valid.");
    typedef typename T::ValueType ValueType;

    T _h;
    HashMore(const T& h) : _h(h) {}

    bool operator()(const T& t1,const T& t2) {
        return _h(t1) > _h(t2);
    }
};

template <class T>
struct HashEqual {

    static_assert(T::is_hash_valid,"Hash function is not valid.");
    typedef typename T::ValueType ValueType;

    T _h;
    HashEqual(const T& h) : _h(h) {}

    bool operator()(const T& t1,const T& t2) {
        return _h(t1) == _h(t2);
    }
};

template <class T>
struct HashKit {

    T _h;
    HashLess<T> _hl;
    HashMore<T> _hm;
    HashEqual<T> _he;
    typedef typename T::ValueType ValueType;

    static_assert(T::is_hash_valid,"Hash function is not valid.");

    HashComparator(const ValueType& h) : _h(h), _hl(h), _hm(h), _he(h) {}

    size_t hash(const ValueType& t) {
        return _h(t);
    }

    bool isLess(const ValueType& t1,const ValueType& t2) {
        return _hl(t1,t2);
    }

    bool isMore(const ValueType& t1,const ValueType& t2) {
        return _hm(t1,t2);
    }

    bool isEqual(const ValueType& t1,const ValueType& t2) {
        return _he(t1,t2);
    }

};

}
}


#endif /* end of include guard: HASHER_W4HP5KR9 */

