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
#include <templatious/Utilities.h>

namespace templatious {

struct LoopL {
    size_t _beg;
    size_t _end;
    size_t _step;

    LoopL(size_t end) : _beg(0), _end(end), _step(1) {}

    struct Iter {
        size_t _beg;
        size_t _end;
        size_t _step;
        size_t _count;

        Iter(size_t beg,size_t end,size_t step,size_t count) :
            _beg(beg), _end(end), _step(step), _count(count) {}

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
            //return _beg == rhs._beg;
            return _count == rhs._count;
        }

        bool operator!=(const Iter& rhs) const {
            //return _beg != rhs._beg;
            return _count != rhs._count;
        }

        size_t operator*() const {
            return _count;
        }
    };

    Iter begin() const {
        return Iter(_beg,_end,_step,_beg);
    }

    Iter end() const {
        return Iter(_beg + 1,_end,_step,_end);
    }

};

typedef LoopL Loop;

}

#endif /* end of include guard: LOOP_U82GY31R */
