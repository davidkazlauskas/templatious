/*
 * =====================================================================================
 *
 *       Filename:  Pack.h
 *
 *    Description:  Pack for packing references to variables
 *
 *        Version:  1.0
 *        Created:  09/13/2014 08:42:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef PACK_APF8NI0N
#define PACK_APF8NI0N

#include <utility>
#include <templatious/util/Container.h>

namespace templatious {

template <class... Args>
struct Pack;

template <class A,class... Tail>
struct Pack<A,Tail...> {

    typedef Pack<A,Tail...> ThisPack;
    typedef Pack<Tail...> TailPack;

    template <class Ins,class... Args>
    struct InsertType {
        //typedef Pack<Ins,A,typename TailPack::template InsertType<Ins>::value > value;
        //typedef Pack<Args...,Ins,A> value;
        typedef typename TailPack::template InsertType<Ins,Args...,Ins,A>::value value;
    };

    template <class Ins>
    struct FirstInsertType {
        //typedef Pack<A,typename TailPack::template InsertType<Ins>::value > value;
        typedef typename TailPack::template InsertType<Ins,A>::value value;
    };

    template <class ARef,class... TailRef>
    explicit Pack(ARef&& r,TailRef&&... t)
    : _r(std::forward<ARef>(r)),
      _t(std::forward<TailRef>(t)...) {}

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        _t.call(std::forward<F>(f),std::forward<Args>(args)...,_r.getRef());
    }

    template <class F>
    void call(F&& f) {
        _t.call(std::forward<F>(f),_r.getRef());
    }

    //template <class U,class T>
    template <class T>
    auto insert(T&& t)
     //-> decltype(this->_t.insert(std::forward<T>(t),this->_r.getRef()))
     //-> decltype(this->_t.insert(std::forward<T>(t),this->_r.getRef()))
     -> typename FirstInsertType<T>::value
    const {
         return _t.insert(std::forward<T>(t),_r.getRef());
    }

    //template <class U,class T,class... Args>
    template <class T,class... Args>
    //U insert(T&& t,Args&&... args)
    auto insert(T&& t,Args&&... args)
     -> typename InsertType<T,Args...>::value
     //-> decltype(this->_t.insert(
             //std::forward<T>(t),
             //std::forward<Args>(args)...,
             //std::forward<T>(t),
             //this->_r.getRef())
         //)
    const {
         return _t.insert(
                 std::forward<T>(t),
                 std::forward<Args>(args)...,
                 std::forward<T>(t),
                 _r.getRef());
         //return _t.template insert<U>(
                 //std::forward<T>(t),
                 //std::forward<Args>(args)...,
                 //std::forward<T>(t),
                 //_r.getRef());
    }

    //templatious::util::RefContainer<A> _r;
    //Pack<Tail...> _t;
private:
    templatious::util::RefContainer<A> _r;
    Pack<Tail...> _t;
};

template <class A>
struct Pack<A> {

    typedef Pack<A> ThisPack;

    //template <class Ins>
    template <class Ins,class... Args>
    struct InsertType {
        typedef Pack<Args...,Ins,A> value;
    };

    template <class Ins>
    struct FirstInsertType {
        typedef ThisPack value;
    };


    template <class ARef>
    explicit Pack(ARef&& r)
    : _r(std::forward<ARef>(r)) {}

    template <class F,class... Args>
    void call(F&& f,Args&&... args) {
        f(std::forward<Args>(args)...,_r.getRef());
    }

    template <class T>
    ThisPack insert(T&& t) {
        return ThisPack(_r.getRef());
    }

    //template <class U,class T,class... Args>
    template <class T,class... Args>
    auto insert(T&& t,Args&&... args)
     //-> Pack<Args...,decltype(t),decltype(this->_r.getRef())>
     -> typename InsertType<T,Args...>::value
    const {
         //return Pack<
             //Args...,
             //decltype(t),
             ////decltype(this->_r.getRef())
             //A
             //>(
                    //std::forward<Args>(args)...,
                    //std::forward<T>(t),
                    //_r.getRef()
                 //);
        typedef typename InsertType<T,Args...>::value RetType;
        return RetType(std::forward<Args>(args)...,
                std::forward<T>(t),_r.getRef());
    }

    //templatious::util::RefContainer<A> _r;
private:
    templatious::util::RefContainer<A> _r;
};

template <class T>
struct IsPack {
    static const bool val = false;
};

template <class... T>
struct IsPack< Pack<T...> > {
    static const bool val = true;
};

template <class... T>
struct IsPack< Pack<T...>& > {
    static const bool val = true;
};

template <class... T>
struct IsPack< const Pack<T...> > {
    static const bool val = true;
};

template <class... T>
struct IsPack< const Pack<T...>& > {
    static const bool val = true;
};

}

#endif /* end of include guard: PACK_APF8NI0N */
