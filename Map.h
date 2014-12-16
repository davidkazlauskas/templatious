/*
 * =====================================================================================
 *
 *       Filename:  Map.h
 *
 *    Description:  Map implementation taking map type, key, value, allocator
 *
 *        Version:  1.0
 *        Created:  06/19/2014 04:16:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef MAP_H3UVMK27
#define MAP_H3UVMK27

#include <templatious/adapters/All.h>
#include <templatious/util/spec/DefaultHashes.h>
#include <templatious/map/MapAdapter.h>

namespace templatious {

template <class T>
struct Map {

    typedef T MapType;
    typedef templatious::adapters::MapAdapter<T> Ad;
    typedef typename Ad::KeyType KeyType;
    typedef typename Ad::ValueType ValueType;

    static_assert(Ad::is_valid,"Map is invalid.");

    MapType _m;

    Map() : _m(std::move(Ad::instantiate())) {}

    Map(MapType&& m) : _m(std::move(Ad::instantiate())) {
        std::swap(_m,m);
    }

    Map(const MapType& m) : _m(m) {}

    bool keyExists(const KeyType& k) {
        return Ad::keyExists(_m,k);
    }

    bool get(const KeyType& k,ValueType& v) {
        return Ad::get(_m,k,v);
    }

    ValueType& get(const KeyType& k) {
        return Ad::get(_m,k);
    }

    bool put(const KeyType& k,const ValueType& v) {
        return Ad::put(_m,k,v);
    }

    int size() {
        return Ad::size(_m);
    }


};

}


#endif /* end of include guard: MAP_H3UVMK27 */
