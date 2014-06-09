/*
 * =====================================================================================
 *
 *       Filename:  Hasher.h
 *
 *    Description:  Hash to collections
 *
 *        Version:  1.0
 *        Created:  06/09/2014 07:44:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef HASHER_T4GBMR7G
#define HASHER_T4GBMR7G

#include <templatious/Utilities.h>
#include <templatious/CollectionAdapter.h>

namespace templatious {

template <
    class Val,class Fun,
    class Comparator = templatious::util::Comparator<Val,Val>
>
class Hasher {
private:
    Fun _f;
public:

    Hasher(Fun f) : _f(f) {}
    
    template <class Col>
    static bool putVal(Col& c,const Val& v) {
        static_assert(CollectionAdapter<Col>::is_valid,"Invalid collection.");
        static_assert(CollectionAdapter<Col>::hash_supported,"This collection doesn't support hash.");

        typedef typename templatious::adapters::StaticAdapter SA;

        // PROBLEM ENCOUNTERED, TO DO
    }

};

}

#endif /* end of include guard: HASHER_T4GBMR7G */
