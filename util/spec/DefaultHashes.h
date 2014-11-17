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

#define GEN_CAST_HASH(x,cast,type) \
    template <>\
    struct Hasher<x,Default> {\
        static const bool is_hash_valid = true;\
        typedef x ValueType;\
        typedef Default Variant;\
        \
        size_t operator()(const ValueType& t) const {\
            return size_t(cast<type>(t));\
        }\
    };

#define GEN_STAT_HASH(x) GEN_CAST_HASH(x,static_cast,size_t)
#define GEN_REIN_HASH(x) GEN_CAST_HASH(x,reinterpret_cast,const size_t&)


#define GEN_COMP_HASH(x,str,func) \
    \
    template <>\
    struct str< Hasher<x> > {\
        typedef typename Hasher<x>::ValueType ValueType;\
        static_assert(Hasher<x>::is_hash_valid,"Hash function is not valid.");\
        Hasher<x> _h;\
        str(const Hasher<x>& h) : _h(h) {}\
        \
        bool operator()(const ValueType& t1,const ValueType& t2) const {\
            typedef templatious::util::StaticComparator<false> SC;\
            return SC::func(t1,t2);\
        }\
        \
    };


#define GEN_COMP_LESS_HASH(x) GEN_COMP_HASH(x,HashLess,isLess)
#define GEN_COMP_MORE_HASH(x) GEN_COMP_HASH(x,HashMore,isMore)
#define GEN_COMP_EQUAL_HASH(x) GEN_COMP_HASH(x,HashEqual,isEqual)

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

// STRING DEFAULT HASHES -------------------------

template <>
struct Hasher<std::string> {

    static const bool is_hash_valid = true;

    typedef std::string ValueType;
    typedef Default Variant;

    size_t operator()(const std::string& t) {
        // DUMMY, BAD FUNCTION
        size_t res = 0;
        for (size_t i = 0; i < t.size(); ++i) {
            res += t[i];
        }
        return res;
    }
};

template <>
struct Hasher<char*> {

    static const bool is_hash_valid = true;

    typedef char* ValueType;
    typedef Default Variant;

    size_t operator()(const char* t) {
        // DUMMY, BAD FUNCTION
        size_t res = 0;
        const char* i = t;
        while ('\0' != *i) {
            res += *i;
        }
        return res;
    }
};

// STRING COMPARISON -----------------------------

GEN_COMP_LESS_HASH(std::string);
GEN_COMP_MORE_HASH(std::string);
GEN_COMP_EQUAL_HASH(std::string);

GEN_COMP_LESS_HASH(char*);
GEN_COMP_MORE_HASH(char*);
GEN_COMP_EQUAL_HASH(char*);

}
}


#endif /* end of include guard: DEFAULTHASHES_WDTKD5HS */
