/*
 * =====================================================================================
 *
 *       Filename:  Unirator.h
 *
 *    Description:  Uni iterator - class to be "iterator" but to contain one element
 *
 *        Version:  1.0
 *        Created:  09/22/2014 05:03:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef UNIRATOR_G56INC10
#define UNIRATOR_G56INC10

namespace templatious {
namespace util {

template <class T>
struct Unirator {
    typedef Unirator<T> ThisUnirator;

    Unirator(T c, bool isEnd)
     : _c(c), _end(isEnd) {}

    bool operator==(const ThisUnirator& o) const {
        return o._end == _end;
    }

    Unirator& operator++() {
        _end = true;
    }

    T operator*() {
        return _c;
    }

private:
    T _c;
    bool _end;
};

}
}

#endif /* end of include guard: UNIRATOR_G56INC10 */

