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
    struct Iterator;

    static const ulong size = sz;
    static_assert(size > 0,"Static vector cannot be of negative size.");

    StaticVector(T (&vct)[size]) : _vct(vct), _cnt(0) { }
    StaticVector(T (&vct)[size],ulong currCnt) :
        _vct(vct), _cnt(currCnt)
    {
        assert(currCnt <= size
            && "Initial static array size cannot be larger than a capacity.");
    }

    bool push(const T& e) {
        if (isFull()) {
            return false;
        }

        _vct[_cnt++] = e;
        return true;
    }

    bool push(T&& e) {
        if (isFull()) {
            return false;
        }

        _vct[_cnt++] = e;
        return true;
    }

    bool push_first(const T& e) {
        return insert(0,e);
    }

    bool push_first(T&& e) {
        return insert(0,e);
    }

    bool insert(ulong at,const T& e) {
        if (isFull()) {
            return false;
        }

        assert(at <= _cnt && "Insertion point cannot be past the end of the vector.");

        ++_cnt;
        TEMPLATIOUS_FOREACH(auto i,templatious::LoopL<ulong>(at+1,_cnt).rev()) {
            _vct[i] = std::move(_vct[i - 1]);
        }
        _vct[at] = e;
        return true;
    }

    bool insert(ulong at,T&& e) {
        if (isFull()) {
            return false;
        }

        assert(at <= _cnt && "Insertion point cannot be past the end of the vector.");

        ++_cnt;
        TEMPLATIOUS_FOREACH(auto i,templatious::LoopL<ulong>(at+1,_cnt).rev()) {
            _vct[i] = std::move(_vct[i - 1]);
        }
        _vct[at] = e;
        return true;
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

    Iterator iterAt(size_t pos) const {
        assert(pos < _cnt && "Position cannot be greater than size");
        return Iterator(_vct,_cnt,pos);
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
            *j = *i;
            ++j;
        }

        _cnt -= (end._iter - beg._iter);
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

    Iterator begin() const {
        return Iterator(_vct,_cnt);
    }

    Iterator end() const {
        return Iterator(_vct,_cnt,_cnt);
    }

    const Iterator cbegin() const {
        return Iterator(_vct,_cnt);
    }

    const Iterator cend() const {
        return Iterator(_vct,_cnt,_cnt);
    }

    struct Iterator {

        Iterator(T* vct,ulong size) :
            _vct(vct), _size(size), _iter(0) {}

        Iterator(T* vct,ulong size,ulong pos) :
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

        Iterator& operator+=(size_t s) {
            _iter += s;
            return *this;
        }

        Iterator& operator-=(size_t s) {
            _iter -= s;
            return *this;
        }

        bool operator==(const Iterator& rhs) const {
            return _iter == rhs._iter;
        }

        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

        T& operator*() const {
            assert(_iter < _size && "Iterator out of bounds.");
            return _vct[_iter];
        }

        T* operator->() const {
            assert(_iter < _size && "Iterator out of bounds.");
            return &_vct[_iter];
        }

        friend Iterator operator+(const Iterator& i,size_t s) {
            Iterator res = i;
            res._iter += s;
            return res;
        }

        friend Iterator operator-(const Iterator& i,size_t s) {
            Iterator res = i;
            res._iter -= s;
            return res;
        }

        friend class StaticVector<T,sz>;

    private:
        T* _vct;
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

    typedef StaticVector<T,sz> ThisCol;
    typedef const ThisCol ConstCol;
    typedef void* iterator;
    typedef const void* const_iterator;
    typedef void* value_type;
    typedef const void* const_value_type;

    static bool add(ThisCol& c, const value_type& i) {
        return c.push(i);
    }

    static bool remove(ThisCol& c, const value_type& i);
    static value_type& getByIndex(ThisCol& c, int i);
    static const_value_type& getByIndex(ConstCol& c, int i);

    static size_t getSize(const ThisCol& c);

    static bool erase(ThisCol& c, iterator beg);
    static bool erase(ThisCol& c, iterator beg, iterator end);

    static ThisCol instantiate();
    static ThisCol instantiate(int size);

    static iterator begin(ThisCol& c);
    static iterator end(ThisCol& c);
    static iterator iter_at(ThisCol& c, int i);

    static const_iterator cbegin(ThisCol& c);
    static const_iterator cend(ThisCol& c);
    static const_iterator citer_at(ThisCol& c, int i);

    static value_type& first(ThisCol& c);
    static const value_type& first(ConstCol& c);
    static value_type& last(ThisCol& c);
    static const value_type& last(ConstCol& c);

    static bool insert_at(ThisCol& c, iterator at, const value_type& i);

    static void clear(ThisCol& c);
};
}

}


#endif /* end of include guard: STATICVECTOR_IIC8KCSO */
