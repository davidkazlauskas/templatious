/*
 * =====================================================================================
 *
 *       Filename:  DefaultHashes.h
 *
 *    Description:  Default hash implementations
 *
 *        Version:  1.0
 *        Created:  06/22/2014 08:36:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef DEFAULTHASHES_WDTKD5HS
#define DEFAULTHASHES_WDTKD5HS

#include <templatious/util/spec/DefaultComparators.h>
#include <templatious/util/Hasher.h>

namespace templatious {
namespace util {

#define GEN_CAST_HASH(x,cast) \
    template <>\
    struct Hasher<x,Default> {\
        typedef x ValueType;\
        typedef Default Variant;\
        \
        size_t operator()(const ValueType& t) {\
            return cast<size_t>(t)\
        }\
    };

#define GEN_STAT_HASH(x) GEN_CAST_HASH(x,static_cast)
#define GEN_REIN_HASH(x) GEN_CAST_HASH(x,reinterpret_cast)


#define GEN_COMP_HASH(x,str,func) \
    template <>\
    struct str< Hasher<x> > {\
        typedef typename Hasher<x>::ValueType ValueType;\
        static_assert(Hasher<x>::is_hash_valid,"Hash function is not valid.");\
        Hasher<x> _h;\
        str(const Hasher<x>& h) : _h(h) {}\
        \
        bool operator()(const ValueType& t1,const ValueType& t2) {\
            typedef templatious::util::StaticComparator SC;\
            return SC::func(t1,t2);\
        }\
        \
    };

#define GEN_COMP_LESS_HASH(x) GEN_COMP_HASH(x,HashLess,isLess)
#define GEN_COMP_MORE_HASH(x) GEN_COMP_HASH(x,HashMore,isMore)
#define GEN_COMP_EQUAL_HASH(x) GEN_COMP_HASH(x,HashEqual,isEqual)

#define GEN_DEFAULT_COMP_SUITE(x) \
    GEN_COMP_LESS_HASH(x);\
    GEN_COMP_MORE_HASH(x);\
    GEN_COMP_EQUAL_HASH(x);

// INTEGRAL --------------------------------------
GEN_STAT_HASH(int);
GEN_STAT_HASH(long);
GEN_STAT_HASH(short);
GEN_STAT_HASH(char);

GEN_STAT_HASH(unsigned int);
GEN_STAT_HASH(unsigned long);
GEN_STAT_HASH(unsigned short);
GEN_STAT_HASH(unsigned char);

GEN_REIN_HASH(float);
GEN_REIN_HASH(double);

// STRING COMPARISON -----------------------------
GEN_DEFAULT_COMP_SUTE(std::string);
GEN_DEFAULT_COMP_SUTE(char*);

}
}


#endif /* end of include guard: DEFAULTHASHES_WDTKD5HS */
