/*
 * =====================================================================================
 *
 *       Filename:  Exceptions.h
 *
 *    Description:  Common exceptions in templatious
 *
 *        Version:  1.0
 *        Created:  08/23/2014 07:30:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
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

    virtual const char* what() const throw() {
        return _w;
    }
private:
    const char* _w;
};

}
}

#endif /* end of include guard: EXCEPTIONS_YKNFM1IM */
