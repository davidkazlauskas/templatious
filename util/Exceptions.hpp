//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  Exceptions.hpp
 *
 *    Description:  Common exceptions in templatious
 *
 *        Version:  1.0
 *        Created:  08/23/2014 07:30:03 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef EXCEPTIONS_YKNFM1IM
#define EXCEPTIONS_YKNFM1IM

#include <exception>

namespace templatious {
namespace util {

struct FeatureDisabled: public std::exception {
    FeatureDisabled() : _w("Feature disabled.") {}
    FeatureDisabled(const char* msg) : _w(msg) {}
    virtual const char* what() const noexcept override {
        return _w;
    }
private:
    const char* _w;
};


#define TEMPLATIOUS_BOILERPLATE_EXCEPTION(name,message) \
struct name : public std::exception {                   \
    name() : _w(message) {}                             \
    name(const char* customMsg) : _w(customMsg) {}      \
    virtual ~name() {}                                  \
    const char* what() const noexcept override {        \
        return _w;                                      \
    }                                                   \
private:                                                \
    const char* _w;                                     \
};

}
}

#endif /* end of include guard: EXCEPTIONS_YKNFM1IM */
