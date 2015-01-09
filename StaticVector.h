/*
 * =====================================================================================
 *
 *       Filename:  StaticVector.h
 *
 *    Description:  Static vector to efficiently use stack space
 *
 *        Version:  1.0
 *        Created:  07/07/2014 05:40:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef STATICVECTOR_IIC8KCSO
#define STATICVECTOR_IIC8KCSO

#include <new>

#include <templatious/CollectionAdapter.h>
#include <templatious/Sugar.h>
#include <templatious/util/Exceptions.h>

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

template <class T>
struct StaticVector {

    typedef size_t ulong;
    typedef StaticVector<T> ThisVector;
    typedef typename std::remove_const<T>::type ValTrue;

    template <bool isConst = false>
    struct SvIterator;

    static const bool is_const = std::is_const<T>::value;

    typedef SvIterator<is_const> Iterator;
    typedef SvIterator<true> ConstIter;

    StaticVector(ValTrue* vct,ulong size) : _vct(vct), _cnt(0), _sz(size) { }
    StaticVector(ValTrue* vct,ulong size,ulong currCnt) :
        _vct(vct), _cnt(currCnt), _sz(size)
    {
        if (currCnt > _sz) {
            throw StaticVectorSpaceException();
        }
    }

    StaticVector(const ThisVector& other) = delete;
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

    template <class V>
    void pushFirst(V&& e) {
        insert(0,std::forward<V>(e));
    }

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

        new (&_vct[_cnt]) T();
        ++_cnt;
        for (size_t i = _cnt - 1; i >= at + 1; --i)
        {
            _vct[i] = std::move(_vct[i - 1]);
        }
        _vct[at] = std::forward<V>(e);
    }

    template <class V>
    void insert(Iterator at,V&& e) {
        insert(at._iter,std::forward<V>(e));
    }

    bool pop(T& out) {
        if (isEmpty()) {
            return false;
        }

        out = std::move(_vct[_cnt-1]);
        _vct[_cnt-1].~T();
        --_cnt;
        return true;
    }

    T pop() {
        if (_cnt <= 0) {
            throw StaticVectorEmptyPopException();
        }
        auto v = std::move(_vct[_cnt-1]);
        _vct[_cnt-1].~T();
        --_cnt;
        return std::move(v);
    }

    T popFirst() {
        if (_cnt <= 0) {
            throw StaticVectorEmptyPopException();
        }
        T res = std::move(_vct[0]);
        _vct[0].~T();
        --_cnt;
        for (size_t i = 0; i < _cnt; ++i) {
            _vct[i] = std::move(_vct[i + 1]);
        }
        return std::move(res);
    }

    // xchg
    bool popFirst(T& out) {
        if (isEmpty()) {
            return false;
        }

        out = std::move(_vct[0]);
        --_cnt;
        for (size_t i = 0; i < _cnt; ++i) {
            _vct[i] = std::move(_vct[i + 1]);
        }
        return true;
    }

    Iterator iterAt(ulong pos) const {
        if (pos > _cnt) {
            throw StaticVectorOutOfBoundsException();
        }
        return Iterator(_vct,_cnt,pos);
    }

    ConstIter citerAt(ulong pos) const {
        if (pos > _cnt) {
            throw StaticVectorOutOfBoundsException();
        }
        return ConstIter(_vct,_cnt,pos);
    }

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

    void erase(const Iterator& i) {
        erase(i,Iterator(_vct,_cnt,i._iter + 1));
    }

    T& at(ulong pos) const {
        if (!(pos >= 0 && pos < _cnt && pos < _sz)) {
            StaticVectorOutOfBoundsException();
        }
        return _vct[pos];
    }

    bool isFull() const {
        return _cnt >= _sz;
    }

    bool isMoved() const {
        return _vct == nullptr;
    }

    bool isEmpty() const {
        return _cnt == 0;
    }

    ulong size() const {
        return _cnt;
    }

    Iterator begin() const {
        return Iterator(_vct,_cnt);
    }

    Iterator end() const {
        return Iterator(_vct,_cnt,_cnt);
    }

    ConstIter cbegin() const {
        return ConstIter(_vct,_cnt);
    }

    ConstIter cend() const {
        return ConstIter(_vct,_cnt,_cnt);
    }

    ValTrue* rawBegin() {
        return _vct;
    }

    ValTrue* rawEnd() {
        return _vct + _cnt;
    }

    const ValTrue* rawCBegin() const {
        return _vct;
    }

    const ValTrue* rawCEnd() const {
        return _vct + _cnt;
    }

    void clear() {
        destroyAll();
        _cnt = 0;
    }

    // added purely for exception
    // correctness if you're the 1 in the million
    // whose class might throw on copy
    T& top() {
        if (isEmpty()) {
            throw StaticVectorOutOfBoundsException();
        }
        return _vct[_cnt - 1];
    }

    template <class V = T>
    typename std::enable_if< !std::is_const<V>::value, const T& >::type
    top() const {
        if (isEmpty()) {
            throw StaticVectorOutOfBoundsException();
        }
        return _vct[_cnt - 1];
    }

    void popState() {
        if (isEmpty()) {
            throw StaticVectorEmptyPopException();
        }
        _vct[_cnt - 1].~T();
        --_cnt;
    }

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
        for (size_t i = 0; i < _cnt; ++i) {
            _vct[i].~T();
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

template <class T,size_t sz>
StaticVector<T> makeStaticVector(char (&arr)[sz]) {
    static_assert(sz % sizeof(T) == 0,
        "Array size in bytes must be properly aligned.");
    return StaticVector<T>(reinterpret_cast<T*>(arr),sz / sizeof(T));
}

TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticBufferExceedException,
    "Remaining capacity of this buffer is too low for required vector.");
TEMPLATIOUS_BOILERPLATE_EXCEPTION(StaticBufferWrongSize,
    "StaticVector has to contain at least one element.");

template <class T,size_t sz>
struct StaticBuffer {

    static const int total_size = sz;

    StaticBuffer() : _currSize(0) {}

    // int in capacity because user is more likely to pass
    // a negative value than a very large number.
    // this is not meant for huge collections anyway
    // and shall be contained on the stack
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
    size_t remainingSize() const {
        return total_size - _currSize;
    }

    ValTrue* nextPtr(size_t bump) {
        ValTrue* res = basePtr() + _currSize;
        _currSize += bump;
        return res;
    }

    ValTrue* basePtr() {
        return reinterpret_cast<ValTrue*>(_buf);
    }

    size_t _currSize;
    char _buf[total_size * sizeof(T)];
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

    static ValueType& getByIndex(ThisCol& c, int i) {
        return c.at(i);
    }

    static ConstValueType& getByIndex(ConstCol& c, int i) {
        return c.at(i);
    }

    static int size(const ThisCol& c) {
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

    static Iterator iterAt(ThisCol& c,size_t i) {
        return c.iterAt(i);
    }

    static Iterator iterAt(ConstCol& c,size_t i) {
        return c.citerAt(i);
    }

    static ConstIterator citerAt(ConstCol& c,size_t i) {
        return c.citerAt(i);
    }

    static ConstIterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static ConstIterator cend(ConstCol& c) {
        return c.cend();
    }

    static ConstIterator citerAt(ThisCol& c,size_t i) {
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

    static ValueType& getByIndex(ThisCol& c, int i) {
        return c.at(i);
    }

    static int size(const ThisCol& c) {
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

    static ConstIterator iterAt(ConstCol& c,size_t i) {
        return c.citerAt(i);
    }

    static ConstIterator citerAt(ConstCol& c,size_t i) {
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
