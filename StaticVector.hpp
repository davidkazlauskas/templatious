//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  StaticVector.hpp
 *
 *    Description:  Static vector to efficiently use stack space
 *
 *        Version:  1.0
 *        Created:  07/07/2014 05:40:56 PM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef STATICVECTOR_IIC8KCSO
#define STATICVECTOR_IIC8KCSO

#include <new>

#include <templatious/CollectionAdapter.hpp>
#include <templatious/Sugar.hpp>
#include <templatious/util/Exceptions.hpp>

namespace templatious {

TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticVectorSpaceException,
    "Initial static array size cannot be larger than a capacity.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticVectorFullAddException,
    "Trying to add to a full vector.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticVectorMovedOperationException,
    "Vector was moved out and can no longer be operated on.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticVectorOutOfBoundsException,
    "Operation goes out of bounds of vector.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticVectorEmptyPopException,
    "Trying to pop an empty vector.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticVectorEraseException,
    "Element erase exception.");

/**
 * StaticVector is a class whose
 * main purpose is to be like
 * std::vector, but rather use
 * static memory without dynamic
 * allocation.
 *
 * Although, this class was intended
 * to be used with stack memory, it
 * may be used with any memory given
 * a pointer, element type and size.
 *
 * To create instances of this collection
 * see templatious::StaticBuffer.
 *
 * Static vector doesn't take ownership
 * of it's memory, nor is responsible
 * for freeing it. Static vector only
 * destroys objects that reside in the
 * initialized memory which were
 * constructed by this vector. After
 * this vector is destroyed, it's
 * consumed memory is free for reuse.
 *
 * @param[in] T type of objects to hold.
 */
template <class T>
struct StaticVector {

    typedef long ulong;
    typedef StaticVector<T> ThisVector;
    typedef typename std::remove_const<T>::type ValTrue;

    template <bool isConst = false>
    struct SvIterator;

    static const bool is_const = std::is_const<T>::value;

    typedef SvIterator<is_const> Iterator;
    typedef SvIterator<true> ConstIter;


    /**
     * Constructor for static vector which
     * takes in pointer and capacity of this vector.
     * @param[in] vct Memory to use for this vector.
     * @param[in] size Capacity of this vector.
     */
    StaticVector(ValTrue* vct,ulong size) : _vct(vct), _cnt(0), _sz(size) { }

    /**
     * Constructor which constructs amount of elements
     * specified in currCnt variable. Default constructor
     * with no parameters is used.
     *
     * @param[in] vct Memory to use for this vector.
     * @param[in] size Capacity of the vector.
     * @param[in] currCnt Current count of elements.
     * Elements are instantiated with default constructor.
     */
    // don't throw template errors unless instance of this
    // constructor is called
    template <class Type = T>
    StaticVector(ValTrue* vct,ulong size,ulong currCnt) :
        _vct(vct), _cnt(currCnt), _sz(size)
    {
        if (currCnt > _sz) {
            throw StaticVectorSpaceException();
        }

        preallocate<Type>(_cnt);
    }

    StaticVector(const ThisVector& other) = delete;

    /**
     * Move constructor. Takes over memory
     * from the other StaticVector.
     * @param[in] other Static vector to take
     * over memory from.
     */
    StaticVector(ThisVector&& other) :
        _vct(other._vct), _cnt(other._cnt), _sz(other._sz)
    {
        other._vct = nullptr;
        other._cnt = 0;
        other._sz = 0;
    }

    ~StaticVector() {
        destroyAll();
    }

    /**
     * Push element to the end of the vector.
     * Throws if vector is full or moved.
     * @param[in] e Element to push. Can be
     * const T& or T&&.
     */
    template <class V>
    void push(V&& e) {
        if (isFull()) {
            if (isMoved()) {
                throw StaticVectorMovedOperationException();
            }
            throw StaticVectorFullAddException();
        }

        new(&_vct[_cnt]) T(std::forward<V>(e));
        ++_cnt;
    }

    /**
     * Push element to the beginning of the vector.
     * Throws if vector is full or moved.
     * @param[in] e Element to push. Can be
     * const T& or T&&.
     */
    template <class V>
    void pushFirst(V&& e) {
        insert(0,std::forward<V>(e));
    }

    /**
     * Insert element at the position
     * of the vector. Throws if vector
     * is full or moved.
     * @param[in] at Position to push at.
     * @param[in] e Element to push. Can be
     * const T& or T&&.
     */
    template <class V>
    void insert(ulong at,V&& e) {
        if (isFull()) {
            if (isMoved()) {
                throw StaticVectorMovedOperationException();
            }
            throw StaticVectorFullAddException();
        }
        if (at > _cnt) {
            throw StaticVectorOutOfBoundsException();
        }

        if (_cnt > 0) {
            new (&_vct[_cnt]) T(std::move(_vct[_cnt-1]));
            for (long i = _cnt - 1; i >= at + 1; --i)
            {
                _vct[i] = std::move(_vct[i - 1]);
            }
            ++_cnt;
            _vct[at] = std::forward<V>(e);
        } else {
            ++_cnt;
            new (&_vct[at]) T(std::forward<V>(e));
        }
    }

    /**
     * Insert element at the iterator
     * of the vector. Throws if vector
     * is full or moved.
     * @param[in] at Iterator to push at.
     * @param[in] e Element to push. Can be
     * const T& or T&&.
     */
    template <class V>
    void insert(Iterator at,V&& e) {
        insert(at._iter,std::forward<V>(e));
    }

    /**
     * Pop element out of the vector
     * into out variable.
     * @param[out] out Variable to pop into.
     * Returns if true if pop succeeds
     * (vector was not empty), false
     * otherwise.
     */
    bool pop(T& out) {
        if (isEmpty()) {
            return false;
        }

        out = std::move(_vct[_cnt-1]);
        _vct[_cnt-1].~T();
        --_cnt;
        return true;
    }

    /**
     * Pop element directly to return value.
     * Throws if vector is empty.
     */
    T pop() {
        if (_cnt <= 0) {
            throw StaticVectorEmptyPopException();
        }
        auto v = std::move(_vct[_cnt-1]);
        _vct[_cnt-1].~T();
        --_cnt;
        return std::move(v);
    }

    /**
     * Pop first element from the vector
     * directly to return value.
     * Throws if vector is empty.
     * Moves subsequent elements back.
     */
    T popFirst() {
        if (_cnt <= 0) {
            throw StaticVectorEmptyPopException();
        }
        T res = std::move(_vct[0]);
        _vct[0].~T();
        --_cnt;
        for (long i = 0; i < _cnt; ++i) {
            _vct[i] = std::move(_vct[i + 1]);
        }
        return std::move(res);
    }

    /**
     * Pop first element of the vector
     * to out variable.
     * Returns false if vector is empty.
     * Returns true if pop succeeded.
     * @param[out] out Value to pop to.
     */
    bool popFirst(T& out) {
        if (isEmpty()) {
            return false;
        }

        out = std::move(_vct[0]);
        --_cnt;
        for (long i = 0; i < _cnt; ++i) {
            _vct[i] = std::move(_vct[i + 1]);
        }
        return true;
    }

    /**
     * Return iterator at position pos
     * of this vector.
     * @param[in] pos Position of iterator.
     */
    Iterator iterAt(ulong pos) const {
        if (pos > _cnt || pos < 0) {
            throw StaticVectorOutOfBoundsException();
        }
        return Iterator(_vct,_cnt,pos);
    }

    /**
     * Return constant iterator at position pos
     * of this vector.
     * @param[in] pos Position of iterator.
     */
    ConstIter citerAt(ulong pos) const {
        if (pos > _cnt || pos < 0) {
            throw StaticVectorOutOfBoundsException();
        }
        return ConstIter(_vct,_cnt,pos);
    }

    /**
     * Erase elements from this vector from beg to end.
     * @param[in] beg Beginning iterator for erase.
     * @param[in] end End iterator for erase.
     */
    void erase(const Iterator& beg,const Iterator& end) {
        eraseAssertions(beg,end);

        Iterator j = beg;
        for (auto i = end; i != this->end(); ++i) {
            *j = std::move(*i);
            (*i).~T();
            ++j;
        }

        _cnt -= (end._iter - beg._iter);
    }

    /**
     * Erase single element pointed by iterator i
     * from this vector.
     * @param[in] i Iterator pointing to element
     * for erase.
     */
    void erase(const Iterator& i) {
        erase(i,Iterator(_vct,_cnt,i._iter + 1));
    }

    /**
     * Get reference to element at pos.
     * Throws if out of bounds.
     * @param[in] pos Position to get.
     */
    T& at(ulong pos) const {
        if (!(pos >= 0 && pos < _cnt && pos < _sz)) {
            StaticVectorOutOfBoundsException();
        }
        return _vct[pos];
    }

    /**
     * Returns if vector is full.
     */
    bool isFull() const {
        return _cnt >= _sz;
    }

    /**
     * Returns if vector was moved.
     */
    bool isMoved() const {
        return _vct == nullptr;
    }

    /**
     * Returns if vector is empty.
     */
    bool isEmpty() const {
        return _cnt == 0;
    }

    /**
     * Returns size of the vector.
     */
    ulong size() const {
        return _cnt;
    }

    /**
     * Returns beginning iterator
     * of the vector.
     */
    Iterator begin() const {
        return Iterator(_vct,_cnt);
    }

    /**
     * Returns end iterator
     * of the vector.
     */
    Iterator end() const {
        return Iterator(_vct,_cnt,_cnt);
    }

    /**
     * Returns constant beginning
     * iterator of the vector.
     */
    ConstIter cbegin() const {
        return ConstIter(_vct,_cnt);
    }

    /**
     * Returns constant end
     * iterator of the vector.
     */
    ConstIter cend() const {
        return ConstIter(_vct,_cnt,_cnt);
    }

    /**
     * Returns raw pointer of beginning
     * of the vector.
     */
    ValTrue* rawBegin() {
        return _vct;
    }

    /**
     * Returns raw pointer to the end
     * of the vector (if size is 1) then
     * result is &v[2]
     */
    ValTrue* rawEnd() {
        return _vct + _cnt;
    }

    /**
     * Returns constant raw pointer
     * of beginning of the vector.
     */
    const ValTrue* rawCBegin() const {
        return _vct;
    }

    /**
     * Returns constant raw pointer
     * of end of the vector (if
     * size is 1) then result is &v[2]
     */
    const ValTrue* rawCEnd() const {
        return _vct + _cnt;
    }

    /**
     * Clear elements from the vector
     * calling destructors on them and
     * resetting size to 0.
     */
    void clear() {
        destroyAll();
        _cnt = 0;
    }

    // added purely for exception
    // correctness if you're the 1 in the million
    // whose class might throw on copy
    /**
     * Get reference to the top element of
     * the vector.
     */
    T& top() {
        if (isEmpty()) {
            throw StaticVectorOutOfBoundsException();
        }
        return _vct[_cnt - 1];
    }

    /**
     * Get const reference to the top
     * element of the vector.
     */
    const T& top() const {
        if (isEmpty()) {
            throw StaticVectorOutOfBoundsException();
        }
        return _vct[_cnt - 1];
    }

    /**
     * Stateful pop of the top
     * element of the vector. Top
     * element is destroyed in the
     * process.
     *
     * Throws if vector is empty.
     */
    void popState() {
        if (isEmpty()) {
            throw StaticVectorEmptyPopException();
        }
        _vct[_cnt - 1].~T();
        --_cnt;
    }

    /**
     * Emplace-construct element at the end
     * of the vector. Throws if vector
     * is full.
     * @param[in] args Arguments to use for
     * construction.
     */
    template <class... Args>
    void emplaceBack(Args&&... args) {
        if (isFull()) {
            if (isMoved()) {
                throw StaticVectorMovedOperationException();
            }
            throw StaticVectorFullAddException();
        }

        new(&_vct[_cnt]) T(std::forward<Args>(args)...);
        ++_cnt;
    }

    template <bool isConst>
    struct SvIterator {
        typedef typename std::conditional<isConst,const T,T>::type ValType;
        typedef SvIterator<isConst> Iterator;
        typedef SvIterator<false> NConstIter;

        // SFINAE used first time in this library. Right here.
        template <
            bool defaultBool = isConst,
            class Dummy = typename std::enable_if<defaultBool,void>::type
        >
        SvIterator(const NConstIter& it):
            _vct(it._vct), _size(it._size), _iter(it._iter)
        { }

        SvIterator(ValType* vct,ulong size) :
            _vct(vct), _size(size), _iter(0) {}

        SvIterator(ValType* vct,ulong size,ulong pos) :
            _vct(vct), _size(size), _iter(pos)
        {
            assert(_iter <= _size && "Iterator position cannot be greater than size.");
        }

        Iterator& operator++() {
            ++_iter;
            return *this;
        }

        Iterator& operator--() {
            --_iter;
            return *this;
        }

        Iterator& operator+=(ulong s) {
            _iter += s;
            return *this;
        }

        Iterator& operator-=(ulong s) {
            _iter -= s;
            return *this;
        }

        bool operator<(const Iterator& rhs) const {
            return _iter < rhs._iter;
        }

        bool operator>(const Iterator& rhs) const {
            return !(*this < rhs);
        }

        bool operator<=(const Iterator& rhs) const {
            return _iter <= rhs._iter;
        }

        bool operator>=(const Iterator& rhs) const {
            return _iter >= rhs._iter;
        }

        bool operator==(const Iterator& rhs) const {
            return _iter == rhs._iter;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

        ValType& operator*() const {
            if (_iter >= _size) {
                StaticVectorOutOfBoundsException();
            }
            return _vct[_iter];
        }

        ValType* operator->() const {
            if (_iter >= _size) {
                StaticVectorOutOfBoundsException();
            }
            return &_vct[_iter];
        }

        friend Iterator operator+(const Iterator& i,ulong s) {
            Iterator res = i;
            res._iter += s;
            return res;
        }

        friend Iterator operator-(const Iterator& i,ulong s) {
            Iterator res = i;
            res._iter -= s;
            return res;
        }

        friend struct StaticVector<T>;

    private:
        ValType* _vct;
        ulong _size;
        ulong _iter;
    };

private:
    ValTrue* _vct;
    ulong _cnt;
    ulong _sz;

    void destroyAll() {
        for (long i = 0; i < _cnt; ++i) {
            _vct[i].~T();
        }
    }

    // suppress this method unless it is used
    template <class Type>
    void preallocate(long amount) {
        for (long i = 0; i < _cnt; ++i) {
            new(&_vct[i]) Type();
        }
    }

    void eraseAssertions(const Iterator& beg,const Iterator& end) {
        if (!(_vct == beg._vct && _cnt == beg._size)) {
            throw StaticVectorEraseException(
            "Begginning iterator does not belong to this vector.");
        }

        if (!(_vct == end._vct && _cnt == end._size)) {
            throw StaticVectorEraseException(
            "End iterator does not belong to this vector.");
        }

        if (beg._iter >= end._iter) {
            throw StaticVectorEraseException(
            "Beggining is greater than end.");
        }

        if (beg._iter >= _cnt) {
            throw StaticVectorEraseException(
            "Beginning does not belong in the vector.");
        }

        if (end._iter > _cnt) {
            throw StaticVectorEraseException(
            "End goes past end of the vector.");
        }
    }

};

template <class T,long sz>
StaticVector<T> makeStaticVector(char (&arr)[sz]) {
    static_assert(sz % sizeof(T) == 0,
        "Array size in bytes must be properly aligned.");
    return StaticVector<T>(reinterpret_cast<T*>(arr),sz / sizeof(T));
}

TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticBufferExceedException,
    "Remaining capacity of this buffer is too low for required vector.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticBufferWrongSize,
    "StaticVector has to contain at least one element.");

/**
 * Class that generates static vectors
 * whose memory resides on the stack.
 * Vectors generated by this class use
 * aligned memory (std::aligned_storage)
 *
 * Example:
 * ~~~~~~ C++
 * // buffer for 16 ints.
 * templatious::StaticBuffer<int,16> buffer;
 *
 * auto v1 = buffer.getStaticVector(8);
 * auto v2 = buffer.getStaticVector();
 *
 * // v1 and v2 can each hold 8 integers.
 * SA::add(v1,0,1,2,3,4,5,6,7);
 *
 * SA::add(v2,v1);
 *
 * // adding more to v1 will throw
 * // SA::add(v1,8);
 *
 * // No need to care about releasing
 * // memory or heap allocation -
 * // objects are destroyed automatically
 * ~~~~~~
 *
 * @param[in] T Type of elements to allocate
 * memory for.
 * @param[in] sz Amount of T elements this
 * buffer can contain.
 */
template <class T,long sz>
struct StaticBuffer {

    static const int total_size = sz;

    StaticBuffer() : _currSize(0) {}

    // int in capacity because user is more likely to pass
    // a negative value than a very large number.
    // this is not meant for huge collections anyway
    // and shall be contained on the stack
    /**
     * Generate static vector with a given
     * capacity. If this exceeds the
     * remaining memory in this buffer
     * exception is thrown.
     * @param[in] capacity Capacity of static
     * vector.
     */
    auto getStaticVector(int capacity)
     -> StaticVector<T>
    {
        if (capacity <= 0) {
            throw StaticBufferWrongSize();
        }

        if (remainingSize() < capacity) {
            throw StaticBufferExceedException();
        }

        // being explicit never hurts
        return std::move(
            StaticVector<T>(
                nextPtr(capacity),capacity)
        );
    }

    /**
     * Generate static vector with a remaining
     * memory.
     */
    auto getStaticVector()
     -> StaticVector<T>
    {
        // being explicit never hurts
        return std::move(
            getStaticVector(remainingSize())
        );
    }

private:
    typedef typename std::remove_const<T>::type ValTrue;
    int remainingSize() const {
        return total_size - _currSize;
    }

    ValTrue* nextPtr(long bump) {
        ValTrue* res = basePtr() + _currSize;
        _currSize += bump;
        return res;
    }

    ValTrue* basePtr() {
        return reinterpret_cast<ValTrue*>(_buf);
    }

    typedef typename std::aligned_storage<
        sizeof(T),alignof(T)>::type AlStor;

    long _currSize;
    AlStor _buf[total_size];
};

namespace adapters {

template <class T>
struct CollectionAdapter< StaticVector<T> > {
    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef StaticVector<T> ThisCol;
    typedef const ThisCol ConstCol;
    typedef typename ThisCol::Iterator Iterator;
    typedef typename ThisCol::ConstIter ConstIterator;
    typedef T ValueType;
    typedef const T ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.push(std::forward<V>(i));
    }

    template <class V>
    static void insertAt(ThisCol& c, Iterator at,V&& i) {
        c.insert(at,std::forward<V>(i));
    }

    static ValueType& getByIndex(ThisCol& c, long i) {
        return c.at(i);
    }

    static ConstValueType& getByIndex(ConstCol& c, long i) {
        return c.at(i);
    }

    static long size(const ThisCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c, Iterator pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        c.erase(beg,end);
    }

    static Iterator begin(ThisCol& c) {
        return c.begin();
    }

    static Iterator end(ThisCol& c) {
        return c.end();
    }

    static Iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static Iterator end(ConstCol& c) {
        return c.cend();
    }

    static Iterator iterAt(ThisCol& c,long i) {
        return c.iterAt(i);
    }

    static ConstIterator iterAt(ConstCol& c,long i) {
        return c.citerAt(i);
    }

    static ConstIterator citerAt(ConstCol& c,long i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator citerAt(ThisCol& c,long i) {
        return c.citerAt(i);
    }

    static ValueType& first(ThisCol& c) {
        return c.at(0);
    }

    static ConstValueType& first(ConstCol& c) {
        return c.at(0);
    }

    static ValueType& last(ThisCol& c) {
        return c.at(c.size() - 1);
    }

    static ConstValueType& last(ConstCol& c) {
        return c.at(c.size() - 1);
    }

    static void clear(ThisCol& c) {
        return c.clear();
    }

    static bool canAdd(ConstCol& c) {
        return !(c.isFull());
    }
};

template <class T>
struct CollectionAdapter< const StaticVector<T> > {
    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef const StaticVector<T> ThisCol;
    typedef ThisCol ConstCol;
    typedef typename ThisCol::ConstIter Iterator;
    typedef typename ThisCol::ConstIter ConstIterator;
    typedef const T ValueType;
    typedef const T ConstValueType;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.push(std::forward<V>(i));
    }

    template <class V>
    static void insertAt(ThisCol& c, Iterator at,V&& i) {
        c.insert(at,std::forward<V>(i));
    }

    static ValueType& getByIndex(ThisCol& c, long i) {
        return c.at(i);
    }

    static long size(const ThisCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c, Iterator pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        c.erase(beg,end);
    }

    static Iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static Iterator end(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator iterAt(ConstCol& c,long i) {
        return c.citerAt(i);
    }

    static ConstIterator citerAt(ConstCol& c,long i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstValueType& first(ThisCol& c) {
        return c.at(0);
    }

    static ConstValueType& last(ConstCol& c) {
        return c.at(c.size() - 1);
    }

    static void clear(ThisCol& c) {
        return c.clear();
    }

    static bool canAdd(ConstCol& c) {
        return !(c.isFull());
    }
};
}

}


#endif /* end of include guard: STATICVECTOR_IIC8KCSO */
