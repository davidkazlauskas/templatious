/*
 * =====================================================================================
 *
 *       Filename:  StdMap.h
 *
 *    Description:  Std map implementation of MapMaker and MapAdapter
 *
 *        Version:  1.0
 *        Created:  06/23/2014 07:58:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef STDMAP_6NXOL9XH
#define STDMAP_6NXOL9XH

#include <map>

#include <templatious/util/Hasher.h>
#include <templatious/map/MapAdapter.h>
#include <templatious/map/MapMaker.h>

namespace templatious {
namespace adapters {

template <class Key,class Value,class Comp,template <class> class Alloc>
struct MapMaker<Key,Value,std::map,Comp,Alloc> {

    static const bool is_maker_valid = true;
    typedef typename templatious::util::HashKit<Comp> Kit;
    typedef std::map <
        Key, Value, typename Kit::HL,
        Alloc<std::pair<const Key, Value> >
    > ProductType;

    Kit _k;

    MapMaker(const Comp& c) : _k(c) {}
    MapMaker() : _k(Comp()) {}

    static ProductType make(size_t size) {
        return ProductType();
    }

    static ProductType make() {
        return ProductType();
    }

};

template <
    class Key,
    class Value,
    class Comp,
    template <class> class Alloc
>
struct MapAdapter<
    std::map<
        Key,Value,Comp,
        Alloc< std::pair<const Key,Value> >
    > 
>
{
    static const bool is_valid = true;

    typedef Key KeyType;
    typedef Value ValueType;
    typedef std::map<
                Key,Value,Comp,
                Alloc< std::pair<const Key,Value> >
            > ThisMap;
    typedef Comp Comparator;

    static bool keyExists(const ThisMap& h,const KeyType& k) {
        return h.end() != h.find(k);
    }

    static bool get(const ThisMap& h,const KeyType& k,ValueType& v) {
        auto it = h.find(k);
        if (h.end() == it) {
            return false;
        }
        v = it->second;
        return true;
    }

    static ValueType& get(ThisMap& h,const KeyType& k) {
        if (!keyExists(h,k)) {
            throw MapAdapterNoSuchElementException();
        }

        return h[k];
    }

    static bool put(ThisMap& h,const KeyType& k,const ValueType& v) {
        h[k] = v;
        return true;
    }

    static long size(const ThisMap& h) {
        return h.size();
    }

};



}
}

#endif /* end of include guard: STDMAP_6NXOL9XH */
