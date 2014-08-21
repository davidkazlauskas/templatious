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

    static const ulong size = sz;
    static_assert(size > 0,"Static vector cannot be of negative size.");

    StaticVector(T (&vct)[size]) : _vct(vct), _cnt(0) { }
    StaticVector(T (&vct)[size],ulong currCnt) :
        _vct(vct), _cnt(currCnt)
    {
        assert(currCnt <= size
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
            templatious::LoopL<ulong>(at+1,_cnt).rev())
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
        TEMPLATIOUS_FOREACH(auto i,templatious::LoopL<ulong>(_cnt)) {
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
        TEMPLATIOUS_FOREACH(auto i,templatious::LoopL<ulong>(_cnt)) {
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
        assert(pos >= 0 && pos < _cnt && pos < size
                && "Requested position out of bounds.");
        return _vct[pos];
    }

    bool isFull() const {
        return _cnt >= size;
    }

    bool isEmpty() const {
        return _cnt == 0;
    }

    ulong getSize() const {
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

        friend class StaticVector<T,sz>;

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
    typedef typename ThisCol::Iterator iterator;
    typedef typename ThisCol::ConstIter const_iterator;
    typedef T value_type;
    typedef const T const_value_type;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.push(std::forward<V>(i));
    }

    template <class V>
    static void insert_at(ThisCol& c, iterator at,V&& i) {
        c.insert(at,std::forward<V>(i));
    }

    static value_type& getByIndex(ThisCol& c, int i) {
        return c.at(i);
    }

    static const_value_type& getByIndex(ConstCol& c, int i) {
        return c.at(i);
    }

    static size_t getSize(const ThisCol& c) {
        return c.getSize();
    }

    static void erase(ThisCol& c, iterator pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c, iterator beg, iterator end) {
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

    static iterator begin(ThisCol& c) {
        return c.begin();
    }

    static iterator end(ThisCol& c) {
        return c.end();
    }

    static iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static iterator end(ConstCol& c) {
        return c.cend();
    }

    static iterator iter_at(ThisCol& c,size_t i) {
        return c.iterAt(i);
    }

    static iterator iter_at(ConstCol& c,size_t i) {
        return c.citerAt(i);
    }

    static const_iterator citer_at(ConstCol& c,size_t i) {
        return c.citerAt(i);
    }

    static const_iterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator cend(ConstCol& c) {
        return c.cend();
    }

    static const_iterator citer_at(ThisCol& c,size_t i) {
        return c.citerAt(i);
    }

    static value_type& first(ThisCol& c) {
        return c.at(0);
    }

    static const value_type& first(ConstCol& c) {
        return c.at(0);
    }

    static value_type& last(ThisCol& c) {
        return c.at(c.getSize() - 1);
    }

    static const value_type& last(ConstCol& c) {
        return c.at(c.getSize() - 1);
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
    typedef typename ThisCol::ConstIter iterator;
    typedef typename ThisCol::ConstIter const_iterator;
    typedef const T value_type;
    typedef const T const_value_type;

    template <class V>
    static void add(ThisCol& c, V&& i) {
        c.push(std::forward<V>(i));
    }

    template <class V>
    static void insert_at(ThisCol& c, iterator at,V&& i) {
        c.insert(at,std::forward<V>(i));
    }

    static value_type& getByIndex(ThisCol& c, int i) {
        return c.at(i);
    }

    static size_t getSize(const ThisCol& c) {
        return c.getSize();
    }

    static void erase(ThisCol& c, iterator pos) {
        c.erase(pos);
    }

    static void erase(ThisCol& c, iterator beg, iterator end) {
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

    static iterator begin(ConstCol& c) {
        return c.cbegin();
    }

    static iterator end(ConstCol& c) {
        return c.cend();
    }

    static const_iterator iter_at(ConstCol& c,size_t i) {
        return c.citerAt(i);
    }

    static const_iterator citer_at(ConstCol& c,size_t i) {
        return c.citerAt(i);
    }

    static const_iterator cbegin(ConstCol& c) {
        return c.cbegin();
    }

    static const_iterator cend(ConstCol& c) {
        return c.cend();
    }

    static const_value_type& first(ThisCol& c) {
        return c.at(0);
    }

    static const_value_type& last(ConstCol& c) {
        return c.at(c.getSize() - 1);
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
