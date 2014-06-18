/*
 * =====================================================================================
 *
 *       Filename:  Hash.h
 *
 *    Description:  Uniform hash interface
 *
 *        Version:  1.0
 *        Created:  06/15/2014 07:35:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef HASH_93Z0H2A5
#define HASH_93Z0H2A5

namespace templatious {
namespace adapters {

#include <templatious/util/Comparator.h>

template <class T>
struct MapAdapter {

    static const bool is_valid = false;

    typedef void* KeyType;
    typedef void* ValueType;
    typedef T ThisMap;
    typedef templatious::util::Default DefComp;

    template <class Comp = DefComp>
    static bool keyExists(const ThisMap& h,const KeyType& k);

    template <class Comp = DefComp>
    static bool get(const ThisMap& h,const KeyType& k,ValueType& v);

    template <class Comp = DefComp>
    static ValueType& get(const ThisMap& h,const KeyType& k);

    template <class Comp = DefComp>
    static bool put(ThisMap& h,const KeyType& k,const ValueType& v);

};

}
}


#endif /* end of include guard: HASH_93Z0H2A5 */
