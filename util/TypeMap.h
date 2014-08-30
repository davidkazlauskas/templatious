/*
 * =====================================================================================
 *
 *       Filename:  TypeMap.h
 *
 *    Description:  Type map for convenient selection of types on number
 *
 *        Version:  1.0
 *        Created:  08/30/2014 10:39:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef TYPEMAP_OFK2FEEH
#define TYPEMAP_OFK2FEEH

#include <templatious/util/Selectors.h>

namespace templatious {
namespace util {

#define TEMPLATIOUS_PAIR_NT(keyNum,valueType) \
    templatious::util::NumTypeKeyValue<keyNum,valueType>

template <int K,class V>
struct NumTypeKeyValue {
    static const int Key = K;
    typedef V Value;
};

template <class... Values>
struct NumTypeMap;

template <int K,class V,class... Other>
struct NumTypeMap< NumTypeKeyValue<K,V>, Other... >
{
    typedef NumTypeKeyValue<K,V> Node;
    typedef NumTypeMap< Other... > Child;

    template <int i,class FalseType>
    struct getType {
        typedef typename TypeSelector< i == Node::Key,
            typename Node::Value,
            typename Child::template getType<i,FalseType>::val
        >::val val;
    };
};

template <int K,class V>
struct NumTypeMap< NumTypeKeyValue<K,V> >
{
    typedef NumTypeKeyValue<K,V> Node;

    template <int i,class FalseType>
    struct getType {
        typedef typename TypeSelector< i == Node::Key,
            typename Node::Value,
            FalseType
        >::val val;
    };
};

}
}

#endif /* end of include guard: TYPEMAP_OFK2FEEH */
