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

#include <templatious/util/Comparator.h>

namespace templatious {
namespace util {

template <>
struct ComparatorDiff<std::string,std::string> {

    int operator()(const std::string& t,const std::string& u) {
        return t.compare(u);
    }

};

template <>
struct ComparatorDiff<char*,char*> {

    int operator()(const char* t,const char* u) {
        return strcmp(t,u);
    }

};

template <>
struct ComparatorEq<char*,char*> {

    bool operator()(const char* t,const char* u) {
        return 0 == strcmp(t,u);
    }

};

template <>
struct ComparatorEq<std::string,std::string> {

    bool operator()(const std::string& t,const std::string& u) {
        return t == u;
    }

};

template <>
struct ComparatorM<char*,char*> {

    bool operator()(const char* t,const char* u) {
        return 0 < strcmp(t,u);
    }

};

template <>
struct ComparatorM<std::string,std::string> {

    bool operator()(const std::string& t,const std::string& u) {
        return 0 < t.compare(u);
    }

};

template <>
struct ComparatorL<char*,char*> {

    bool operator()(const char* t,const char* u) {
        return 0 > strcmp(t,u);
    }

};

template <>
struct ComparatorL<std::string,std::string> {

    bool operator()(const std::string& t,const std::string& u) {
        return 0 > t.compare(u);
    }

};

}
}

#endif /* end of include guard: DEFAULTCOMPARATORS_IVHHLPT2 */
