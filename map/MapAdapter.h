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

#include <cstddef>
#include <templatious/util/Comparator.h>
#include <templatious/util/Exceptions.h>

TEMPLATIOUS_BOILERPLATE_EXCEPTION( MapAdapterNoSuchElementException,
    "Element doesn't exist in a map.");

template <class T>
struct MapAdapter {

    static const bool is_valid = false;
    static const bool is_comparator_const = true;

    typedef void* KeyType;
    typedef void* ValueType;
    typedef T ThisMap;
    typedef void* Comparator;

    static bool keyExists(const ThisMap& h,const KeyType& k);
    static bool get(const ThisMap& h,const KeyType& k,ValueType& v);
    static ValueType& get(const ThisMap& h,const KeyType& k);
    static bool put(ThisMap& h,const KeyType& k,const ValueType& v);
    static int size(const ThisMap& h);

};

}
}


#endif /* end of include guard: HASH_93Z0H2A5 */
