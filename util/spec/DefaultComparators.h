/*
 * =====================================================================================
 *
 *       Filename:  DefaultComparators.h
 *
 *    Description:  Default comparator specializations
 *
 *        Version:  1.0
 *        Created:  06/09/2014 07:31:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef DEFAULTCOMPARATORS_IVHHLPT2
#define DEFAULTCOMPARATORS_IVHHLPT2

#include <cstring>
#include <string>

namespace templatious {
namespace util {

template <>
struct ComparatorDiff<std::string,std::string> {

    // default comparator is More
    int operator()(const std::string& t,const std::string& u) {
        return t.compare(u);
    }

};

template <>
struct ComparatorDiff<char*,char*> {

    // default comparator is More
    int operator()(const char* t,const char* u) {
        return strcmp(t,u);
    }

};

}
}

#endif /* end of include guard: DEFAULTCOMPARATORS_IVHHLPT2 */
