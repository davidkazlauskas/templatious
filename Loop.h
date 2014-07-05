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
    T size();

    static_assert(std::numeric_limits<T>::is_signed,"Loop type must be signed.");

};

template <class T>
struct LoopL : public LoopBase<T> {
    typedef T Unit;
    typedef Iter<T> ThisIter;
    typedef LoopL<T> ThisLoop;

    Unit _beg;
    Unit _end;
    Unit _step;

    LoopL(Unit end) : _beg(0), _end(end), _step(1) {}
    LoopL(Unit beg,Unit end) : _beg(beg), _end(end), _step(1) {
        loopAssert();
    }

    LoopL(Unit beg,Unit end,Unit step) : _beg(beg), _end(end),
        _step(step)
    {
        loopAssert();
    }

    ThisIter begin() const {
        return ThisIter(_beg,_step);
    }

    ThisIter end() {
        Unit res = _end - _beg;
        res = (res / _step) + ( (res % _step) == 0 ? 0 : 1 );
        return ThisIter(_beg + res * _step);
    }

    ThisLoop rev() {
        return ThisLoop(_end - getModulus(),_beg - getModulus(),-_step);
    }

private:
    Unit getModulus() {
        Unit diff = (_end - _beg) % _step;
        if (0 == diff) {
            diff += _step;
        }
        return diff;
    }

    void loopAssert() {
        assert( _beg <= _end && _step > 0
             || _beg >= _end && _step < 0
             && "Loop is illogical.");
    }

};

namespace adapters {

}

}

#endif /* end of include guard: LOOP_U82GY31R */
