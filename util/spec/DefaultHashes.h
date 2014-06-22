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

#include <templatious/util/Hasher.h>

namespace templatious {
namespace util {

#define GEN_CAST_HASH(x,cast) \
    template <>\
    struct Hasher<x,Default> {\
        typedef x Val;\
        \
        size_t operator()(const Val& t) {\
            return cast<size_t>(t)\
        }\
    };

#define GEN_STAT_HASH(x) GEN_CAST_HASH(x,static_cast)
#define GEN_REIN_HASH(x) GEN_CAST_HASH(x,reinterpret_cast)

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

// STRING  ---------------------------------------

}
}


#endif /* end of include guard: DEFAULTHASHES_WDTKD5HS */
