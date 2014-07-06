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

template <class T = int>
struct LoopL : public LoopBase<T> {
    typedef T Unit;
    typedef Iter<T> ThisIter;
    typedef LoopL<T> ThisLoop;

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

    Unit size() {
        Unit res = _end - _beg;
        res = res / _step + (res % _step != 0 ? 1 : 0);
        if (res < 0) {
            res = -res;
        }
        return res;
    }

private:
    Unit _beg;
    Unit _end;
    Unit _step;

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

template <class T>
struct CollectionAdapter< templatious::LoopL<T> > {
    
    static const bool is_valid = true;

    typedef templatious::LoopL<T> ThisCol;
    typedef const templatious::LoopL<T> ConstCol;
    typedef decltype(ThisCol(0).begin()) iterator;
    typedef const decltype(ConstCol(0).begin()) const_iterator;
    typedef T value_type;
    typedef const T const_value_type;

    template <class U = int>
    static iterator begin(ThisCol& c) {
        return c.begin();
    }

    template <class U = int>
    static iterator end(ThisCol& c) {
        return c.end();
    }

    template <class U = int>
    static const_iterator cbegin(ThisCol& c) {
        return c.begin();
    }

    template <class U = int>
    static const_iterator cend(ThisCol& c) {
        return c.end();
    }

    // Invalid adapter method for loop class follow:
    template <class U = int>
    static bool add(ThisCol& c, const value_type& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static bool remove(ThisCol& c, const value_type& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static value_type& getByIndex(ThisCol& c, int i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static const_value_type& getByIndex(ConstCol& c, int i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static int getSize(const ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static bool erase(ThisCol& c, iterator beg) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static bool erase(ThisCol& c, iterator beg, iterator end) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static ThisCol instantiate() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static ThisCol instantiate(int size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }


    template <class U = int>
    static iterator iter_at(ThisCol& c, int i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static const_iterator citer_at(ThisCol& c, int i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static value_type& first(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static const value_type& first(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static value_type& last(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static const value_type& last(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static bool insert_at(ThisCol& c, iterator at, const value_type& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static void clear(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }
};

}

}

#endif /* end of include guard: LOOP_U82GY31R */
