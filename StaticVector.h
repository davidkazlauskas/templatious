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

#include <assert.h>

#include <templatious/CollectionAdapter.h>
#include <templatious/Sugar.h>

namespace templatious {

template <class T,size_t sz>
struct StaticVector {

    typedef size_t ulong;
    typedef StaticVector<T,sz> ThisVector;

    template <bool isConst = false>
    struct SvIterator;

    typedef SvIterator<false> Iterator;
    typedef SvIterator<true> ConstIter;

    static const ulong size_const = sz;
    static_assert(size_const > 0,"Static vector cannot be of negative size.");

    StaticVector(T (&vct)[size_const]) : _vct(vct), _cnt(0) { }
    StaticVector(T (&vct)[size_const],ulong currCnt) :
        _vct(vct), _cnt(currCnt)
    {
        assert(currCnt <= size_const
            && "Initial static array size cannot be larger than a capacity.");
    }

    template <class V>
    void push(V&& e) {
        assert(!isFull() && "Trying to push to a full vector.");

        _vct[_cnt++] = std::forward<V>(e);
    }

    template <class V>
    void push_first(V&& e) {
        insert(0,std::forward<V>(e));
    }

    template <class V>
    void insert(ulong at,V&& e) {
        assert(!isFull() && "Trying to insert to a full vector.");
        assert(at <= _cnt && "Insertion point cannot be past the end of the vector.");

        ++_cnt;
        TEMPLATIOUS_FOREACH(auto i,
            templatious::SeqL<ulong>(at+1,_cnt).rev())
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

        out = _vct[--_cnt];
        return true;
    }

    T&& pop() {
        assert(_cnt > 0 && "Trying to pop an empty vector.");
        return std::move(_vct[--_cnt]);
    }

    T&& pop_first() {
        assert(_cnt > 0 && "Trying to pop an empty vector.");
        T res = std::move(_vct[0]);
        --_cnt;
        TEMPLATIOUS_FOREACH(auto i,templatious::SeqL<ulong>(_cnt)) {
            _vct[i] = std::move(_vct[i + 1]);
        }
        return std::move(res);
    }

    // xchg
    bool pop_first(T& out) {
        if (isEmpty()) {
            return false;
        }

        out = std::move(_vct[0]);
        --_cnt;
        TEMPLATIOUS_FOREACH(auto i,templatious::SeqL<ulong>(_cnt)) {
            _vct[i] = std::move(_vct[i + 1]);
        }
        return true;
    }

    Iterator iterAt(ulong pos) const {
        assert(pos <= _cnt && "Position cannot be greater than size");
        return Iterator(_vct,_cnt,pos);
    }

    ConstIter citerAt(ulong pos) const {
        assert(pos <= _cnt && "Position cannot be greater than size");
        return ConstIter(_vct,_cnt,pos);
    }

    void erase(const Iterator& beg,const Iterator& end) {
        assert(_vct == beg._vct && _cnt == beg._size
                && "Begginning iterator does not belong to this vector.");
        assert(_vct == end._vct && _cnt == end._size
                && "End iterator does not belong to this vector.");
        assert(beg._iter < end._iter
                && "Beggining is greater than end.");
        assert(beg._iter < _cnt
                && "Beginning does not belong in the vector.");
        assert(end._iter <= _cnt
                && "End goes past end of the vector.");

        Iterator j = beg;
        for (auto i = end; i != this->end(); ++i) {
            if (std::is_destructible<T>::value) {
                (*j).~T();
            }

            *j = std::move(*i);

            if (std::is_destructible<T>::value) {
                (*i).~T();
            }
            ++j;
        }

        _cnt -= (end._iter - beg._iter);
    }

    void erase(const Iterator& i) {
        erase(i,Iterator(_vct,_cnt,i._iter + 1));
    }

    T& at(ulong pos) const {
        assert(pos >= 0 && pos < _cnt && pos < size_const
                && "Requested position out of bounds.");
        return _vct[pos];
    }

    bool isFull() const {
        return _cnt >= size_const;
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

    void clear() {
        _cnt = 0;
    }

    template <bool isConst>
    struct SvIterator {
        typedef typename templatious::util::TypeSelector<isConst,const T,T>::val ValType;
        typedef SvIterator<isConst> Iterator;

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
            assert(_iter < _size && "Iterator out of bounds.");
            return _vct[_iter];
        }

        ValType* operator->() const {
            assert(_iter < _size && "Iterator out of bounds.");
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

        friend struct StaticVector<T,sz>;

    private:
        ValType* _vct;
        ulong _size;
        ulong _iter;

    };

private:
    T* _vct;
    ulong _cnt;


};

template <class T,size_t sz>
StaticVector<T,sz> makeStaticVector(T (&arr)[sz]) {
    return StaticVector<T,sz>(arr);
}

namespace adapters {

template <class T,size_t sz>
struct CollectionAdapter< StaticVector<T,sz> > {
    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef StaticVector<T,sz> ThisCol;
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

    static size_t size(const ThisCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c, Iterator pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        c.erase(beg,end);
    }

    template <class U = int>
    static ThisCol instantiate() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "StaticVector cannot be just instantiated \
                       because it uses static array memory.");
    }

    template <class U = int>
    static ThisCol instantiate(int size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "StaticVector cannot be just instantiated \
                       because it uses static array memory.");
    }

    template <class U = int>
    static ThisCol* instHeap() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "StaticVector cannot be just instantiated \
                       because it uses static array memory.");
    }

    template <class U = int>
    static ThisCol* instHeap(int size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "StaticVector cannot be just instantiated \
                       because it uses static array memory.");
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

    static const ValueType& first(ConstCol& c) {
        return c.at(0);
    }

    static ValueType& last(ThisCol& c) {
        return c.at(c.size() - 1);
    }

    static const ValueType& last(ConstCol& c) {
        return c.at(c.size() - 1);
    }

    static void clear(ThisCol& c) {
        return c.clear();
    }

    static bool canAdd(ThisCol& c) {
        return !(c.isFull());
    }
};

template <class T,size_t sz>
struct CollectionAdapter< const StaticVector<T,sz> > {
    static const bool is_valid = true;
    static const bool floating_iterator = true;

    typedef const StaticVector<T,sz> ThisCol;
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

    static size_t size(const ThisCol& c) {
        return c.size();
    }

    static void erase(ThisCol& c, Iterator pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c, Iterator beg, Iterator end) {
        c.erase(beg,end);
    }

    template <class U = int>
    static ThisCol instantiate() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "StaticVector cannot be just instantiated \
                       because it uses static array memory.");
    }

    template <class U = int>
    static ThisCol instantiate(int size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "StaticVector cannot be just instantiated \
                       because it uses static array memory.");
    }

    template <class U = int>
    static ThisCol* instHeap() {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "StaticVector cannot be just instantiated \
                       because it uses static array memory.");
    }

    template <class U = int>
    static ThisCol* instHeap(int size) {
        // suppress static assert until method is actually called
        static_assert(templatious::util::DummyResolver<U, false>::val,
                      "StaticVector cannot be just instantiated \
                       because it uses static array memory.");
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

    static bool canAdd(ThisCol& c) {
        return !(c.isFull());
    }
};
}

}


#endif /* end of include guard: STATICVECTOR_IIC8KCSO */
