/*
 * =====================================================================================
 *
 *       Filename:  RefContainer.h
 *
 *    Description:  Structs to contain references/copies
 *
 *        Version:  1.0
 *        Created:  08/22/2014 03:57:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef REFCONTAINER_AGSDB7NC
#define REFCONTAINER_AGSDB7NC

#include <type_traits>
#include <utility>

namespace templatious {
namespace util {

// Didnt add extended functionality
// to the RefContainer because it
// has promise of immutability
template <class T>
struct MutRefContainer {
    typedef typename std::
        remove_reference<T>::type SaveType;

    // default ctor so we could assign
    MutRefContainer() : _r(nullptr) {}

    MutRefContainer(SaveType& t) : _r(std::addressof(t)) {}

    void assign(SaveType& t) {
        _r = std::addressof(t);
    }

    SaveType& getRef() {
        return *_r;
    }

    const SaveType& getRef() const {
        return *_r;
    }

    const SaveType& cgetRef() const {
        return *_r;
    }

    const SaveType& cpy() const {
        return *_r;
    }

private:
    SaveType* _r;
};

// Didnt add extended functionality
// to the CopyContainer because it
// has promise of immutability
template <class T>
struct MutCopyContainer {
    typedef typename std::
        remove_reference<T>::type SaveType;

    // default ctor so we could assign
    // value must be default constructable
    MutCopyContainer() {}

    MutCopyContainer(const SaveType& t) : _r(t) {}

    template <class V>
    void assign(V&& t) {
        _r = std::forward<V>(t);
    }

    SaveType& getRef() {
        return _r;
    }

    const SaveType& getRef() const {
        return _r;
    }

    const SaveType& cgetRef() const {
        return _r;
    }

    const SaveType& cpy() const {
        return _r;
    }

private:
    SaveType _r;
};

template <class T>
struct RefContainer {

    RefContainer(T& t) : _r(t) {}

    T& getRef() {
        return _r;
    }

    const T& getRef() const {
        return _r;
    }

    const T& cgetRef() const {
        return _r;
    }

    const T& cpy() const {
        return _r;
    }

    const T& constCpy() const {
        return _r;
    }

private:
    T& _r;
};

template <class T>
struct RefContainer<const T> {

    RefContainer(const T& t) : _r(t) {}

    const T& getRef() const {
        return _r;
    }

    const T& cgetRef() const {
        return _r;
    }

    const T& cpy() const {
        return _r;
    }

    const T& constCpy() const {
        return _r;
    }

private:
    const T& _r;
};

template <class T>
struct RefContainer<const T*> {

    RefContainer(const T* const& t) : _r(t) {}

    const T* const& getRef() const {
        return _r;
    }

    const T* const& cgetRef() const {
        return _r;
    }

    const T* cpy() const {
        return _r;
    }

    const T* constCpy() const {
        return _r;
    }

private:
    const T* const& _r;
};

struct CopyContainerAccess {
    template <class Cont,class Val>
    static void mutate(Cont& c,Val&& v) {
        c.mutate(std::forward<Val>(v));
    }

    template <class Cont>
    static auto move(Cont& c)
     -> decltype(std::move(c.move()))
    {
        return std::move(c.move());
    }
};

template <class T>
struct CopyContainer {
    CopyContainer(const T& t) : _r(t) {}

    T& getRef() {
        return _r;
    }

    const T& cgetRef() const {
        return _r;
    }

    const T cpy() const {
        return T(_r);
    }

    const T constCpy() const {
        return T(_r);
    }

    friend struct CopyContainerAccess;
private:
    void mutate(const T& t) {
        _r = t;
    }

    void mutate(T&& t) {
        _r = std::move(t);
    }

    T&& move() {
        return std::move(_r);
    }

    T _r;
};

template <class T>
struct RvalueCopyContainer {
    RvalueCopyContainer(T&& t)
        : _r(std::move(t)) {}

    T& getRef() {
        return _r;
    }

    const T& cgetRef() const {
        return _r;
    }

    T&& cpy() {
        return std::move(_r);
    }

    T constCpy() const {
        return T(_r);
    }

private:
    T _r;
};

template <class T>
struct StaticPointerContainer {
    StaticPointerContainer(const T& t) : _r(&t) {}

    const T& getRef() const {
        return *_r;
    }

    const T& cgetRef() const {
        return *_r;
    }

    const T* cpy() const {
        return _r;
    }

    const T* constCpy() const {
        return _r;
    }

private:
    const T* _r;
};

}
}


#endif /* end of include guard: REFCONTAINER_AGSDB7NC */
