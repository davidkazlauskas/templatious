/*
 * =====================================================================================
 *
 *       Filename:  IteratorTagExtractor.h
 *
 *    Description:  Extract STL iterator tag if one exists
 *
 *        Version:  1.0
 *        Created:  12/10/2014 05:37:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ITERATORTAGEXTRACTOR_UW33L3EF
#define ITERATORTAGEXTRACTOR_UW33L3EF

#include <iterator>

namespace templatious {
namespace util {

template <class T>
struct VoidWrap {
    typedef void type;
};

template <class T,template <class> class ToExtract,class U = void>
struct IteratorTagSelector {
    typedef void type;
    static const bool valid = false;
};

template <class T,template <class> class ToExtract>
struct IteratorTagSelector<
    T, ToExtract,
    typename VoidWrap<
        typename ToExtract<T>::iterator_category
    >::type
> {
    typedef typename ToExtract<T>::iterator_category type;
    static const bool valid = true;
};

template <class T>
struct ExtractIteratorTag {
    typedef IteratorTagSelector<T,std::iterator_traits> Selector;
    typedef typename Selector::type type;
    static const bool valid = Selector::valid;
};

template <class T>
struct IsRandomAccessIteratorTagged {
    static const bool value = std::is_same<
        typename ExtractIteratorTag<T>::type,
        std::random_access_iterator_tag
    >::value;
};

}
}

#endif /* end of include guard: ITERATORTAGEXTRACTOR_UW33L3EF */

