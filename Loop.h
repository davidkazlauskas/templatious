/*
 * =====================================================================================
 *
 *       Filename:  Loop.h
 *
 *    Description:  Loop index emulation collection
 *
 *        Version:  1.0
 *        Created:  07/02/2014 05:03:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef LOOP_U82GY31R
#define LOOP_U82GY31R

#include <cstddef>
#include <assert.h>
#include <templatious/Utilities.h>

namespace templatious {

struct LoopL {
    size_t _beg;
    size_t _end;
    size_t _step;

    LoopL(size_t end) : _beg(0), _end(end), _step(1) {}
    LoopL(size_t beg,size_t end) : _beg(beg), _end(end), _step(1) {
        assert(beg <= end);
    }

    LoopL(size_t beg,size_t end,size_t step) : _beg(beg), _end(end), _step(step) {
        assert(beg <= end);
        assert(step > 0);
    }

    struct Iter {
        size_t _count;
        size_t _step;

        Iter(size_t count,size_t step) :
            _count(count), _step(step) {}

        Iter(size_t count) :
            _count(count) {}

        Iter& operator++() {
            _count += _step;
            return *this;
        }

        Iter& operator--() {
            _count -= _step;
            return *this;
        }

        bool operator==(const Iter& rhs) const {
            return _count == rhs._count;
        }

        bool operator!=(const Iter& rhs) const {
            return _count != rhs._count;
        }

        size_t operator*() const {
            return _count;
        }
    };

    Iter begin() const {
        return Iter(_beg,_step);
    }

    Iter end() const {
        size_t res = _end - _beg;
        res = (res / _step) + ( (res % _step) == 0 ? 0 : 1 );
        return Iter(_beg + res * _step);
    }

};

typedef LoopL Loop;

}

#endif /* end of include guard: LOOP_U82GY31R */
