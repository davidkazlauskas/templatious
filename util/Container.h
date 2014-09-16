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

namespace templatious {
namespace util {



template <class T>
struct RefContainer {

    RefContainer(T& t) : _r(t) {}

    T& getRef() {
        return _r;
    }

    const T& cgetRef() const {
        return _r;
    }

    const T cpy() const {
        return T(_r);
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

    const T cpy() const {
        return T(_r);
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

    const T cpy() const {
        return T(_r);
    }

private:
    const T* const& _r;
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

private:
    T _r;
};

}
}


#endif /* end of include guard: REFCONTAINER_AGSDB7NC */
