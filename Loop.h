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
    mutable Unit _count;
    Unit _step;

    LoopIter(Unit count,Unit step) :
        _count(count), _step(step) {}

    LoopIter(Unit count) :
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

    typedef typename templatious::util::TypeSelector<
        Base::is_signed,LoopIter<const T>,
                typename templatious::util::TypeSelector<
                    !isReversed,LoopIter<const T>,LoopIter<const T,false>
                >::val
        >::val ConstIter;

    LoopL(Unit end) {
        _beg = 0;
        _step = 1;
        _end = getPerfectEnd(end);

        loopAssert();
    }

    LoopL(Unit beg,Unit end) {
        if (Base::is_signed && beg > end) {
            _step = -1;
        } else {
            _step = 1;
        }

        _beg = beg;
        _end = getPerfectEnd(end);

        loopAssert();
    }

    LoopL(Unit beg,Unit end,Unit step) {
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

    ConstIter cbegin() const {
        if (!Base::is_signed && isReversed) {
            return ConstIter(_end - _step,_step);
        }
        return ConstIter(_beg,_step);
    }

    ConstIter cend() const {
        if (!Base::is_signed && isReversed) {
            return ConstIter(_beg - _step,_step);
        }

        return cstandardEnd();
    }

    template <class U = int>
    RevType rev() const {
        //static_assert(templatious::util::DummyResolver<U, Base::is_signed >::val,
                      //"Unsigned loop cannot be reversed.");
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
        assert(res <= end() && "Iterator goes past end of loop.");
    }

    ConstIter citerAt(Unit i) const {
        ConstIter res(_beg + i * _step,_step);
        assert(res <= end() && "Iterator goes past end of loop.");
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
            assert(_beg <= _end
                && "Unsigned loop can only move forward."
                " (if you want unsigned loop to go backwards reverse forward loop)");
        } else {
            assert( _beg <= _end && _step > 0
                 || _beg >= _end && _step < 0
                 && "Loop is illogical.");
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
struct CollectionAdapter< templatious::LoopL<T,isReversed> > {

    static const bool is_valid = true;

    typedef templatious::LoopL<T,isReversed> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::ThisIter iterator;
    typedef typename ThisCol::ConstIter const_iterator;
    typedef T value_type;
    typedef const T const_value_type;

    static iterator begin(ThisCol& c) {
        return c.begin();
    }

    static iterator end(ThisCol& c) {
        return c.end();
    }

    static const_iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator end(ConstCol& c) {
        return c.cend();
    }

    static const_iterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator cend(ConstCol& c) {
        return c.cend();
    }

    template <class U = int>
    static iterator iter_at(ThisCol& c,size_t i) {
        return c.iterAt(c,i);
    }

    template <class U = int>
    static const_iterator citer_at(ThisCol& c,size_t i) {
        return c.citerAt(c,i);
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
    static ThisCol instantiate(size_t size) {
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

template <class T,bool isReversed>
struct CollectionAdapter< const templatious::LoopL<T,isReversed> > {

    static const bool is_valid = true;

    typedef const templatious::LoopL<T,isReversed> ThisCol;
    typedef ThisCol ConstCol;
    typedef typename ThisCol::ThisIter iterator;
    typedef typename ThisCol::ConstIter const_iterator;
    typedef const T value_type;
    typedef const T const_value_type;

    template <class U = int>
    static const_iterator begin(const ThisCol& c) {
        return c.cbegin();
    }

    template <class U = int>
    static const_iterator end(ThisCol& c) {
        return c.cend();
    }

    template <class U = int>
    static const_iterator cbegin(ThisCol& c) {
        return c.cbegin();
    }

    template <class U = int>
    static const_iterator cend(ThisCol& c) {
        return c.cend();
    }

    template <class U = int>
    static iterator iter_at(ThisCol& c,size_t i) {
        return c.iterAt(c,i);
    }

    template <class U = int>
    static const_iterator citer_at(ThisCol& c,size_t i) {
        return c.citerAt(c,i);
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
    static ThisCol instantiate(size_t size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static const_value_type& first(ConstCol& c) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "Loop class is not meant to be a full fledged \
                collection, therefore, doesn't support this method.");
    }

    template <class U = int>
    static const_value_type& last(ConstCol& c) {
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
