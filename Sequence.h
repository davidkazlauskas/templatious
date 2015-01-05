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
#include <templatious/util/Exceptions.h>
#include <templatious/CollectionAdapter.h>

namespace templatious {

// ----------------------------------- FORWARD

template <class T,bool addOnIncrement = true>
struct SeqIter;
template <class T>
struct SeqBase;

template <class T = int,bool isReversed = false>
struct SeqL;

typedef SeqL<int> Loop;

// Sequence exceptions
TEMPLATIOUS_BOILERPLATE_EXCEPTION(IllogicalSequenceException,
    "Sequence is illogical.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(IncorrectBoundsException,
    "Include sequence doesn't include last element.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(IteratorPastEndException,
    "Iterator goes past end of a sequence.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(UnsignedSequenceException,
    "Unsigned loop can only move forward."
    " (if you want unsigned loop to go backwards reverse forward loop)");
// ----------------------------------- FORWARD

template <class T,bool addOnIncrement>
struct SeqIter {
    typedef T Unit;
    typedef SeqIter<Unit,addOnIncrement> ThisIter;

    SeqIter(Unit count,Unit step) :
        _count(count), _step(step) {}

    SeqIter(Unit count) :
        _count(count), _step(1) {}

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

private:
    mutable Unit _count;
    Unit _step;
};

template <class T>
struct SeqBase {
    SeqIter<T> begin();
    SeqIter<T> end();
    T size();

    static const bool is_signed = std::numeric_limits<T>::is_signed;

};

template <class T,bool isReversed>
struct SeqL : public SeqBase<T> {
    typedef T Unit;
    typedef SeqL<T> ThisLoop;
    typedef SeqBase<T> Base;
    typedef typename std::conditional<
            Base::is_signed,ThisLoop,
                typename std::conditional<
                    !isReversed,SeqL<T,true>,SeqL<T,false>
                >::type
        >::type RevType;

    typedef typename std::conditional<
        Base::is_signed,SeqIter<T>,
                typename std::conditional<
                    !isReversed,SeqIter<T>,SeqIter<T,false>
                >::type
        >::type ThisIter;

    typedef typename std::conditional<
        Base::is_signed,SeqIter<const T>,
                typename std::conditional<
                    !isReversed,SeqIter<const T>,SeqIter<const T,false>
                >::type
        >::type ConstIter;

    SeqL(Unit end) {
        _beg = 0;
        _step = 1;
        _end = getPerfectEnd(end);

        loopAssert();
    }

    SeqL(Unit beg,Unit end) {
        if (Base::is_signed && beg > end) {
            _step = -1;
        } else {
            _step = 1;
        }

        _beg = beg;
        _end = getPerfectEnd(end);

        loopAssert();
    }

    SeqL(Unit beg,Unit end,Unit step) {
        _beg = beg;
        _step = step;
        _end = getPerfectEnd(end);

        if (Base::is_signed) {
            setAppropriateStep();
        }

        loopAssert();
    }

    ThisIter begin() const {
        if (!Base::is_signed && isReversed) {
            return ThisIter(_end - _step,_step);
        }
        return ThisIter(_beg,_step);
    }

    ThisIter end() const {
        if (!Base::is_signed && isReversed) {
            return ThisIter(_beg - _step,_step);
        }

        return standardEnd();
    }

    ThisIter cbegin() const {
        if (!Base::is_signed && isReversed) {
            return ThisIter(_end - _step,_step);
        }
        return ThisIter(_beg,_step);
    }

    ThisIter cend() const {
        if (!Base::is_signed && isReversed) {
            return ThisIter(_beg - _step,_step);
        }

        return standardEnd();
    }

    template <class U = int>
    RevType rev() const {
        if (!Base::is_signed) {
            return RevType(_beg,_end,_step);
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

    ThisIter iterAt(Unit i) const {
        ThisIter res(_beg + i * _step,_step);
        if (res > end()) {
            throw IteratorPastEndException();
        }
    }

    ConstIter citerAt(Unit i) const {
        ConstIter res(_beg + i * _step,_step);
        if (res > end()) {
            throw IteratorPastEndException();
        }
    }

private:
    Unit _beg;
    Unit _end;
    Unit _step;

    ThisIter standardEnd() const {
        Unit res = _end - _beg;
        res = (res / _step) + ( (res % _step) == 0 ? 0 : 1 );
        return ThisIter(_beg + res * _step);
    }

    ConstIter cstandardEnd() const {
        Unit res = _end - _beg;
        res = (res / _step) + ( (res % _step) == 0 ? 0 : 1 );
        return ConstIter(_beg + res * _step);
    }

    Unit getModulus() const {
        Unit diff = (_end - _beg) % _step;
        if (0 == diff) {
            diff += _step;
        }
        return diff;
    }

    void loopAssert() const {
        if (!Base::is_signed) {
            if (_beg > _end) {
                throw UnsignedSequenceException();
            }
        } else {
            bool cond = (_beg <= _end && _step > 0)
                 || (_beg >= _end && _step < 0);
            if (!cond) {
                throw IllogicalSequenceException();
            }
        }

    }

    T getPerfectEnd(const T& end) const {
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

    void setAppropriateStep() {
        if (   (_step < 0 && _beg <= _end)
            || (_step > 0 && _beg >  _end) )
        {
            _step = -_step;
        }
    }


};

namespace adapters {

template <class T,bool isReversed>
struct CollectionAdapter< templatious::SeqL<T,isReversed> > {

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef templatious::SeqL<T,isReversed> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::ThisIter Iterator;
    typedef typename ThisCol::ThisIter ConstIterator;
    typedef T ValueType;
    typedef const T ConstValueType;

    static Iterator begin(ThisCol& c) {
        return c.begin();
    }

    static Iterator end(ThisCol& c) {
        return c.end();
    }

    static ConstIterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator end(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    template <class U = int>
    static Iterator iterAt(ThisCol& c,size_t i) {
        return c.iterAt(c,i);
    }

    template <class U = int>
    static ConstIterator citerAt(ThisCol& c,size_t i) {
        return c.citerAt(c,i);
    }

    // Invalid adapter method for loop class follow:
    template <class U = int>
    static bool add(ThisCol& c, const ValueType& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
        return false;
    }

    template <class U = int>
    static ValueType& getByIndex(ThisCol& c, int i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static ConstValueType& getByIndex(ConstCol& c, int i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    static int size(const ThisCol& c) {
        return c.size();
    }

    template <class U = int>
    static bool erase(ThisCol& c, Iterator beg) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
        return false;
    }

    template <class U = int>
    static bool erase(ThisCol& c, Iterator beg, Iterator end) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
        return false;
    }

    template <class U = int>
    static ValueType& first(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static ConstValueType& first(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static ValueType& last(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static ConstValueType& last(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static bool insertAt(ThisCol& c, Iterator at, const ValueType& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
        return false;
    }

    template <class U = int>
    static void clear(ThisCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }
};

template <class T,bool isReversed>
struct CollectionAdapter< const templatious::SeqL<T,isReversed> > {

    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef const templatious::SeqL<T,isReversed> ThisCol;
    typedef ThisCol ConstCol;
    typedef typename ThisCol::ThisIter Iterator;
    typedef typename ThisCol::ThisIter ConstIterator;
    typedef const T ValueType;
    typedef const T ConstValueType;

    template <class U = int>
    static ConstIterator begin(const ThisCol& c) {
        return c.cbegin();
    }

    template <class U = int>
    static ConstIterator end(ThisCol& c) {
        return c.cend();
    }

    template <class U = int>
    static ConstIterator cbegin(ThisCol& c) {
        return c.cbegin();
    }

    template <class U = int>
    static ConstIterator cend(ThisCol& c) {
        return c.cend();
    }

    template <class U = int>
    static Iterator iterAt(ThisCol& c,size_t i) {
        return c.iterAt(c,i);
    }

    template <class U = int>
    static ConstIterator citerAt(ThisCol& c,size_t i) {
        return c.citerAt(c,i);
    }

    // Invalid adapter method for loop class follow:
    template <class U = int>
    static bool add(ThisCol& c, const ValueType& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
        return false;
    }

    template <class U = int>
    static ConstValueType& getByIndex(ConstCol& c, int i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    static int size(const ThisCol& c) {
        return c.size();
    }

    template <class U = int>
    static bool erase(ThisCol& c, Iterator beg) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
        return false;
    }

    template <class U = int>
    static bool erase(ThisCol& c, Iterator beg, Iterator end) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
        return false;
    }

    template <class U = int>
    static ConstValueType& first(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static ConstValueType& last(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static bool insertAt(ThisCol& c, Iterator at, const ValueType& i) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
        return false;
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
