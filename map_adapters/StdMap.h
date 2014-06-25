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
#include <templatious/MapAdapter.h>
#include <templatious/MapMaker.h>

namespace templatious {
namespace adapters {

template <class Key,class Value,class Comp>
struct MapMaker<Key,Value,std::map,Comp> {

    static const bool is_maker_valid = true;
    typedef typename templatious::util::HashKit<Comp> Kit;
    Kit _k;

    MapMaker(const Comp& c) : _k(c) {}
    MapMaker() : _k(Comp()) {}

};


}
}

#endif /* end of include guard: STDMAP_6NXOL9XH */
