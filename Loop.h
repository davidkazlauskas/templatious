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

template <class T,bool addOnIncrement = true>
struct LoopIter;
template <class T>
struct LoopBase;

template <class T = int,bool isReversed = false>
struct LoopL;

typedef LoopL<int> Loop;
// ----------------------------------- FORWARD

template <class T,bool addOnIncrement>
struct LoopIter {
    typedef T Unit;
    typedef LoopIter<Unit,addOnIncrement> ThisIter;
    Unit _count;
    Unit _step;

    LoopIter(Unit count,Unit step) :
        _count(count), _step(step) {}

    LoopIter(Unit count) :
        _count(count) {}

    ThisIter& operator++() {
        if (addOnIncrement) {
            _count += _step;
        } else {
            _count -= _step;
        }
        return *this;
    }

    ThisIter& operator--() {
        if (addOnIncrement) {
            _count -= _step;
        } else {
            _count += _step;
        }
        return *this;
    }

    bool operator==(const ThisIter& rhs) const {
        return _count == rhs._count;
    }

    bool operator!=(const ThisIter& rhs) const {
        return !(*this == rhs);
    }

    Unit operator*() const {
        return _count;
    }
};

template <class T>
struct LoopBase {
    LoopIter<T> begin();
    LoopIter<T> end();
    T size();

    static const bool is_signed = std::numeric_limits<T>::is_signed;

};

template <class T,bool isReversed>
struct LoopL : public LoopBase<T> {
    typedef T Unit;
    typedef LoopL<T> ThisLoop;
    typedef LoopBase<T> Base;
    typedef typename templatious::util::TypeSelector<
            Base::is_signed,ThisLoop,
                typename templatious::util::TypeSelector<
                    !isReversed,LoopL<T,true>,LoopL<T,false>
                >::val
        >::val RevType;
    typedef typename templatious::util::TypeSelector<
        Base::is_signed,LoopIter<T>,
                typename templatious::util::TypeSelector<
                    !isReversed,LoopIter<T>,LoopIter<T,false>
                >::val
        >::val ThisIter;

    LoopL(Unit end) {
        _beg = 0;
        _step = 1;
        _end = getPerfectEnd(end);

        loopAssert();
    }

    LoopL(Unit beg,Unit end) {
        _beg = beg;
        _step = 1;
        _end = getPerfectEnd(end);

        loopAssert();
    }

    LoopL(Unit beg,Unit end,Unit step) {
        _beg = beg;
        _step = step;
        _end = getPerfectEnd(end);

        loopAssert();
    }

    T getPerfectEnd(const T& end) {
        T diff = end - _beg;
        T total = diff / _step;
        if (diff % _step != 0) {
            if (diff > 0) {
                ++total;
            } else {
                --total;
            }
        }

        return _beg + total * _step;
    }

    ThisIter begin() const {
        return ThisIter(_beg,_step);
    }

    ThisIter end() const {
        Unit res = _end - _beg;
        res = (res / _step) + ( (res % _step) == 0 ? 0 : 1 );
        return ThisIter(_beg + res * _step);
    }

    template <class U = int>
    RevType rev() const {
        //static_assert(templatious::util::DummyResolver<U, Base::is_signed >::val,
                      //"Unsigned loop cannot be reversed.");
        if (!Base::is_signed && !isReversed) {
            return RevType(_end - _step,_beg - _step,_step);
        } else if (!Base::is_signed && isReversed) {
            return RevType(_end + _step,_beg + _step,_step);
        }
        return RevType(_end - getModulus(),_beg - getModulus(),-_step);
    }

    Unit size() const {
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

    Unit getModulus() const {
        Unit diff = (_end - _beg) % _step;
        if (0 == diff) {
            diff += _step;
        }
        return diff;
    }

    void loopAssert() const {
        //if (!Base::is_signed) {
            //assert(_beg <= _end 
                //&& "Unsigned loop can only move forward.");
        //}

        //assert( Base::is_signed && _beg <= _end && _step > 0
             //|| Base::is_signed && _beg >= _end && _step < 0
             //&& "Loop is illogical.");
    }

};

namespace adapters {

template <class T,bool isReversed>
struct CollectionAdapter< templatious::LoopL<T,isReversed> > {
    
    static const bool is_valid = true;

    typedef templatious::LoopL<T,isReversed> ThisCol;
    typedef const ThisCol ConstCol;
    typedef decltype(ThisCol(0).begin()) iterator;
    typedef const decltype(ConstCol(0).begin()) const_iterator;
    typedef T value_type;
    typedef const T const_value_type;

    template <class U = int>
    static iterator begin(ThisCol& c) {
        return c.begin();
    }

    template <class U = int>
    static iterator end(const ThisCol& c) {
        return c.end();
    }

    template <class U = int>
    static const_iterator begin(const ThisCol& c) {
        return c.begin();
    }

    template <class U = int>
    static const_iterator end(ThisCol& c) {
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
