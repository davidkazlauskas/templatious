/*
 * =====================================================================================
 *
 *       Filename:  LogicPrimitives.h
 *
 *    Description:  primitives for logic
 *
 *        Version:  1.0
 *        Created:  07/29/2014 08:52:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef LOGICPRIMITIVES_5XC0LXS
#define LOGICPRIMITIVES_5XC0LXS

#include <type_traits>

namespace templatious {

template <class... T>
struct Or;

template <class T>
struct Or<T> {
    T&& _f;

    template <class V>
    Or(V&& f) : _f(std::forward<V>(f)) { }

    template <class... V>
    bool operator()(V&&... v) {
        static_assert(std::is_same<
                bool,
                decltype(_f(std::forward<V>(v)...))
        >::value, "Return value of a function is not boolean");

        return _f(std::forward<V>(v)...);
    }
};

template <class T,class... Tail>
struct Or<T,Tail...> {
    T&& _f;
    Or<Tail...> _t;

    template <class V,class... TailC>
    Or(V&& f,TailC&&... t) :
        _f(std::forward<V>(f)),
        _t(std::forward<TailC>(t)...)
    { }

    template <class... V>
    bool operator()(V&&... v) {
        static_assert(std::is_same<
                bool,
                decltype(_f(std::forward<V>(v)...))
        >::value, "Return value of a function is not boolean");

        if (_f(std::forward<V>(v)...)) {
            return true;
        }

        return _t(std::forward<V>(v)...);
    }
};

template <class... T>
auto mOr(T&&... t) -> Or<T...> {
    return Or<T...>(std::forward<T>(t)...);
}

template <class... T>
struct And;

template <class T>
struct And<T> {
    T&& _f;

    template <class V>
    And(V&& f) : _f(std::forward<V>(f)) { }

    template <class... V>
    bool operator()(V&&... v) {
        static_assert(std::is_same<
                bool,
                decltype(_f(std::forward<V>(v)...))
        >::value, "Return value of a function is not boolean");

        return _f(std::forward<V>(v)...);
    }
};

template <class T,class... Tail>
struct And<T,Tail...> {
    T&& _f;
    And<Tail...> _t;

    template <class V,class... TailC>
    And(V&& f,TailC&&... t) :
        _f(std::forward<V>(f)),
        _t(std::forward<TailC>(t)...)
    { }

    template <class... V>
    bool operator()(V&&... v) {
        static_assert(std::is_same<
                bool,
                decltype(_f(std::forward<V>(v)...))
        >::value, "Return value of a function is not boolean");

        if (!_f(std::forward<V>(v)...)) {
            return false;
        }

        return _t(std::forward<V>(v)...);
    }
};

template <class... T>
auto mAnd(T&&... t) -> And<T...> {
    return And<T...>(std::forward<T>(t)...);
}


}

#endif /* end of include guard: LOGICPRIMITIVES_5XC0LXS */
