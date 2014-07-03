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

#include <limits>
#include <assert.h>
#include <templatious/Utilities.h>
#include <templatious/CollectionAdapter.h>

namespace templatious {

// ----------------------------------- FORWARD

template <class T>
struct Iter;
template <class T>
struct LoopBase;

template <class T>
struct LoopL;
template <class T>
struct LoopME;

typedef LoopL<int> Loop;

// ----------------------------------- FORWARD

template <class T>
struct Iter {
    typedef T Unit;
    typedef Iter<Unit> ThisIter;
    Unit _count;
    Unit _step;

    Iter(Unit count,Unit step) :
        _count(count), _step(step) {}

    Iter(Unit count) :
        _count(count) {}

    ThisIter& operator++() {
        _count += _step;
        return *this;
    }

    ThisIter& operator--() {
        _count -= _step;
        return *this;
    }

    bool operator==(const ThisIter& rhs) const {
        return _count == rhs._count;
    }

    bool operator!=(const ThisIter& rhs) const {
        return _count != rhs._count;
    }

    Unit operator*() const {
        return _count;
    }
};

template <class T>
struct LoopBase {
    Iter<T> begin();
    Iter<T> end();

    static_assert(std::numeric_limits<T>::is_signed,"Loop type must be signed.");

};

template <class T>
struct LoopL : public LoopBase<T> {
    typedef T Unit;
    typedef Iter<T> ThisIter;

    Unit _beg;
    Unit _end;
    Unit _step;

    LoopL(Unit end) : _beg(0), _end(end), _step(1) {}
    LoopL(Unit beg,Unit end) : _beg(beg), _end(end), _step(1) {
        assert(beg <= end && "Beginning of loop is less than end. (LoopL)");
    }

    LoopL(Unit beg,Unit end,Unit step) : _beg(beg), _end(end),
        _step(templatious::util::makePos(step))
    {
        assert(beg <= end && "Beginning of loop is less than end. (LoopL)");
        assert(_step > 0 && "Step must be positive (LoopL)");
    }

    ThisIter begin() const {
        return ThisIter(_beg,_step);
    }

    ThisIter end() const {
        Unit res = _end - _beg;
        res = (res / _step) + ( (res % _step) == 0 ? 0 : 1 );
        return ThisIter(_beg + res * _step);
    }

    LoopME<Unit> rev() {
        return LoopME<Unit>(_end - 1,_beg,-_step);
    }

};

template <class T>
struct LoopME : public LoopBase<T> {
    typedef T Unit;
    typedef Iter<T> ThisIter;

    Unit _beg;
    Unit _end;
    Unit _step;

    LoopME(Unit end) : _beg(end), _end(0), _step(-1) {}
    LoopME(Unit beg,Unit end) : _beg(beg), _end(end), _step(-1) {
        assert(end <= beg && "End of loop is less than end. (LoopME)");
    }

    LoopME(Unit beg,Unit end,Unit step) : _beg(beg), _end(end),
        _step(templatious::util::makeNeg(step))
    {
        assert(end <= beg && "End of loop is less than end. (LoopME)");
        assert(_step < 0 && "Step must be positive (LoopME)");
    }

    ThisIter begin() const {
        return ThisIter(_beg,_step);
    }

    ThisIter end() const {
        Unit res = _beg - _end;
        res = (res / (-_step)) + ( (res % (-_step)) == 0 ? 0 : 1 ) + 1;
        return ThisIter(_beg + res * _step);
    }

    LoopL<Unit> rev() {
        return LoopL<Unit>(_end,_beg + 1,-_step);
    }

};

namespace adapters {

}

}

#endif /* end of include guard: LOOP_U82GY31R */
