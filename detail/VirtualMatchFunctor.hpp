//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

#ifndef VIRTUALMATCHFUNCTOR_695UA513
#define VIRTUALMATCHFUNCTOR_695UA513

#include <utility>
#include <memory>

#include <templatious/detail/TypeList.hpp>
#include <templatious/detail/VirtualPack.hpp>

namespace templatious {

TEMPLATIOUS_BOILERPLATE_EXCEPTION(
    VirtualPackMatcherNoMatchException,
    "No virtual matcher could handle the message."
);

/**
 * Main purpose of this class
 * is to match virtual packs
 * to functions by their types
 * at runtime with typesafety.
 */
struct VirtualMatchFunctor {

    /**
     * Try to match VirtualPack. Returns true
     * if matched, false otherwise.
     *
     * @param[in] p Virtual pack to match.
     *
     * Example:
     * ~~~~~~~
     * auto vmf = SF::virtualMatchFunctorPtr(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "INT MATCHED - "
     *                       << i << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "CHAR MATCHED - "
     *                       << i << std::endl;
     *         }
     *     )
     * );
     *
     * auto p1 = SF::vpack<int>(40);
     * auto p2 = SF::vpack<char>('7');
     * auto p3 = SF::vpack<long>(7);
     *
     * bool aMatch = vmf->tryMatch(p1);
     * bool bMatch = vmf->tryMatch(p2);
     * bool cMatch = vmf->tryMatch(p3);
     *
     * // prints out:
     * // INT MATCHED - 40
     * // CHAR MATCHED - 7
     *
     * assert( aMatch);
     * assert( bMatch);
     * assert(!cMatch);
     * ~~~~~~~
     */
    virtual bool tryMatch(VirtualPack& p) = 0;

    /**
     * Try to match VirtualPack. Returns true
     * if matched, false otherwise.
     *
     * @param[in] p Virtual pack to match.
     *
     * Example:
     * ~~~~~~~
     * auto vmf = SF::virtualMatchFunctorPtr(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "INT MATCHED - "
     *                       << i << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "CHAR MATCHED - "
     *                       << i << std::endl;
     *         }
     *     )
     * );
     *
     * auto p1 = SF::vpack<int>(40);
     * auto p2 = SF::vpack<char>('7');
     * auto p3 = SF::vpack<long>(7);
     *
     * bool aMatch = vmf->tryMatch(p1);
     * bool bMatch = vmf->tryMatch(p2);
     * bool cMatch = vmf->tryMatch(p3);
     *
     * // prints out:
     * // INT MATCHED - 40
     * // CHAR MATCHED - 7
     *
     * assert( aMatch);
     * assert( bMatch);
     * assert(!cMatch);
     * ~~~~~~~
     */
    virtual bool tryMatch(const VirtualPack& p) = 0;

    /**
     * Try to match VirtualPack. Returns true
     * if matched, false otherwise.
     *
     * @param[in] p Virtual pack to match.
     *
     * Example:
     * ~~~~~~~
     * auto vmf = SF::virtualMatchFunctorPtr(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "INT MATCHED - "
     *                       << i << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "CHAR MATCHED - "
     *                       << i << std::endl;
     *         }
     *     )
     * );
     *
     * auto p1 = SF::vpack<int>(40);
     * auto p2 = SF::vpack<char>('7');
     * auto p3 = SF::vpack<long>(7);
     *
     * bool aMatch = vmf->tryMatch(p1);
     * bool bMatch = vmf->tryMatch(p2);
     * bool cMatch = vmf->tryMatch(p3);
     *
     * // prints out:
     * // INT MATCHED - 40
     * // CHAR MATCHED - 7
     *
     * assert( aMatch);
     * assert( bMatch);
     * assert(!cMatch);
     * ~~~~~~~
     */
    virtual bool tryMatch(VirtualPack& p) const = 0;

    /**
     * Try to match VirtualPack. Returns true
     * if matched, false otherwise.
     *
     * @param[in] p Virtual pack to match.
     *
     * Example:
     * ~~~~~~~
     * auto vmf = SF::virtualMatchFunctorPtr(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "INT MATCHED - "
     *                       << i << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "CHAR MATCHED - "
     *                       << i << std::endl;
     *         }
     *     )
     * );
     *
     * auto p1 = SF::vpack<int>(40);
     * auto p2 = SF::vpack<char>('7');
     * auto p3 = SF::vpack<long>(7);
     *
     * bool aMatch = vmf->tryMatch(p1);
     * bool bMatch = vmf->tryMatch(p2);
     * bool cMatch = vmf->tryMatch(p3);
     *
     * // prints out:
     * // INT MATCHED - 40
     * // CHAR MATCHED - 7
     *
     * assert( aMatch);
     * assert( bMatch);
     * assert(!cMatch);
     * ~~~~~~~
     */
    virtual bool tryMatch(const VirtualPack& p) const = 0;

    /**
     * Match VirtualPack. Throws an exception
     * if pack couldn't be matched.
     *
     * @param[in] p Virtual pack to match.
     *
     * Example:
     * ~~~~~~~
     * auto vmf = SF::virtualMatchFunctorPtr(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "INT MATCHED - "
     *                       << i << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "CHAR MATCHED - "
     *                       << i << std::endl;
     *         }
     *     )
     * );
     *
     * auto p1 = SF::vpack<int>(40);
     * auto p2 = SF::vpack<char>('7');
     * auto p3 = SF::vpack<long>(7);
     *
     * (*vmf)(p1);
     * (*vmf)(p2);
     * (*vmf)(p3);
     *
     * // prints out:
     * // INT MATCHED - 40
     * // CHAR MATCHED - 7
     * // throws...
     * ~~~~~~~
     */
    virtual void operator()(VirtualPack& p) {
        if (!this->tryMatch(p)) {
            throw VirtualPackMatcherNoMatchException();
        }
    }

    /**
     * Match VirtualPack. Throws an exception
     * if pack couldn't be matched.
     *
     * @param[in] p Virtual pack to match.
     *
     * Example:
     * ~~~~~~~
     * auto vmf = SF::virtualMatchFunctorPtr(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "INT MATCHED - "
     *                       << i << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "CHAR MATCHED - "
     *                       << i << std::endl;
     *         }
     *     )
     * );
     *
     * auto p1 = SF::vpack<int>(40);
     * auto p2 = SF::vpack<char>('7');
     * auto p3 = SF::vpack<long>(7);
     *
     * (*vmf)(p1);
     * (*vmf)(p2);
     * (*vmf)(p3);
     *
     * // prints out:
     * // INT MATCHED - 40
     * // CHAR MATCHED - 7
     * // throws...
     * ~~~~~~~
     */
    virtual void operator()(const VirtualPack& p) {
        if (!this->tryMatch(p)) {
            throw VirtualPackMatcherNoMatchException();
        }
    }

    /**
     * Match VirtualPack. Throws an exception
     * if pack couldn't be matched.
     *
     * @param[in] p Virtual pack to match.
     *
     * Example:
     * ~~~~~~~
     * auto vmf = SF::virtualMatchFunctorPtr(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "INT MATCHED - "
     *                       << i << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "CHAR MATCHED - "
     *                       << i << std::endl;
     *         }
     *     )
     * );
     *
     * auto p1 = SF::vpack<int>(40);
     * auto p2 = SF::vpack<char>('7');
     * auto p3 = SF::vpack<long>(7);
     *
     * (*vmf)(p1);
     * (*vmf)(p2);
     * (*vmf)(p3);
     *
     * // prints out:
     * // INT MATCHED - 40
     * // CHAR MATCHED - 7
     * // throws...
     * ~~~~~~~
     */
    virtual void operator()(VirtualPack& p) const {
        if (!this->tryMatch(p)) {
            throw VirtualPackMatcherNoMatchException();
        }
    }

    /**
     * Match VirtualPack. Throws an exception
     * if pack couldn't be matched.
     *
     * @param[in] p Virtual pack to match.
     *
     * Example:
     * ~~~~~~~
     * auto vmf = SF::virtualMatchFunctorPtr(
     *     SF::virtualMatch<int>(
     *         [](int i) {
     *             std::cout << "INT MATCHED - "
     *                       << i << std::endl;
     *         }
     *     ),
     *     SF::virtualMatch<char>(
     *         [](char i) {
     *             std::cout << "CHAR MATCHED - "
     *                       << i << std::endl;
     *         }
     *     )
     * );
     *
     * auto p1 = SF::vpack<int>(40);
     * auto p2 = SF::vpack<char>('7');
     * auto p3 = SF::vpack<long>(7);
     *
     * (*vmf)(p1);
     * (*vmf)(p2);
     * (*vmf)(p3);
     *
     * // prints out:
     * // INT MATCHED - 40
     * // CHAR MATCHED - 7
     * // throws...
     * ~~~~~~~
     */
    virtual void operator()(const VirtualPack& p) const {
        if (!this->tryMatch(p)) {
            throw VirtualPackMatcherNoMatchException();
        }
    }
};

template <
    class F,
    bool transparent,
    template <class> class StoragePolicy,
    class... Args
>
struct VirtualMatch {
    typedef typename StoragePolicy<F>::Container Cont;
    typedef VirtualMatch<
        F,
        transparent,
        StoragePolicy,
        Args...
    > ThisMatch;

    typedef TypeList< Args... > TheList;
    static const bool allConst = TheList::
        template ForAll< std::is_const >::value;

    static const bool isTransparent = transparent;

    template <class Function>
    VirtualMatch(Function&& f) :
        _hash(templatious::util::hashTypes<Args...>()),
        _cf(std::forward<Function>(f)) {}

    VirtualMatch(ThisMatch&& other) :
        _hash(other._hash),
        _cf(other._cf.cpy()) {}

    VirtualMatch(const ThisMatch& other) :
        _hash(other._hash),
        _cf(other._cf.constCpy()) {}

    template <bool consts = allConst>
    typename std::enable_if< consts, bool >::type
    operator()(const VirtualPack& vp) {
        return vp.tryCallFunction<Args...>(_cf.getRef())
            && !isTransparent;
    }

    template <bool consts = allConst>
    typename std::enable_if< consts, bool >::type
    operator()(const VirtualPack& vp) const {
        return vp.tryCallFunction<Args...>(_cf.cgetRef())
            && !isTransparent;
    }

    // if we receive const but we know
    // we're not asking const just return false
    template <bool consts = allConst>
    typename std::enable_if< !consts, bool >::type
    operator()(const VirtualPack& vp) {
        return false;
    }

    // if we receive const but we know
    // we're not asking const just return false
    template <bool consts = allConst>
    typename std::enable_if< !consts, bool >::type
    operator()(const VirtualPack& vp) const {
        return false;
    }

    template <bool consts = allConst>
    typename std::enable_if< !consts, bool >::type
    operator()(VirtualPack& vp) {
        return vp.tryCallFunction<Args...>(_cf.getRef())
            && !isTransparent;
    }

    template <bool consts = allConst>
    typename std::enable_if< !consts, bool >::type
    operator()(VirtualPack& vp) const {
        return vp.tryCallFunction<Args...>(_cf.cgetRef())
            && !isTransparent;
    }

    size_t getHash() const {
        return _hash;
    }

private:
    size_t _hash;
    Cont _cf;
};

template <class T>
struct IsVirtualMatch {
    static const bool value = false;
};

template <
    class F,
    bool transparent,
    template <class> class StoragePolicy,
    class... Args
>
struct IsVirtualMatch<
    VirtualMatch<
        F, transparent, StoragePolicy, Args...
    >
>
{
    static const bool value = true;
};

template <>
struct IsVirtualMatch<
    std::unique_ptr< VirtualMatchFunctor >
>
{
    static const bool value = true;
};

template <
    template <class> class StoragePolicy,
    class... Args
>
struct VirtualMatchFunctorImpl;

template <
    template <class> class StoragePolicy,
    class... Args
>
struct IsVirtualMatch<
    VirtualMatchFunctorImpl<
        StoragePolicy,
        Args...
    >
> {
    static const bool value = true;
};

struct SingleMatchCalls {
    template <class M,class V>
    static bool tryMatch(M&& m,V&& v) {
        if (v.getHash() != m.getHash()) {
            return false;
        }

        return m(std::forward<V>(v));
    }
};

struct VirtualMatchFunctorCalls {
    template <class M,class V>
    static bool tryMatch(M&& m,V&& v) {
        return m.tryMatch(std::forward<V>(v));
    }
};

struct VirtualMatchFunctorPtrCalls {
    template <class V>
    static bool tryMatch(std::unique_ptr< VirtualMatchFunctor >& m,V&& v) {
        return m->tryMatch(std::forward<V>(v));
    }
};

template <class T>
struct VMatchFunctorCallHndl {
    typedef void CallHandler;
};

template <
    class F,
    bool transparent,
    template <class> class StoragePolicy,
    class... Args
>
struct VMatchFunctorCallHndl<
    VirtualMatch<
        F, transparent, StoragePolicy, Args...
    >
>
{
    typedef SingleMatchCalls CallHandler;
};

template <
    template <class> class StoragePolicy,
    class... Args
>
struct VMatchFunctorCallHndl<
    VirtualMatchFunctorImpl<
        StoragePolicy,
        Args...
    >
> {
    typedef VirtualMatchFunctorCalls CallHandler;
};

template <>
struct VMatchFunctorCallHndl<
    std::unique_ptr<
        VirtualMatchFunctor
    >
> {
    typedef VirtualMatchFunctorPtrCalls CallHandler;
};

template <
    template <class> class StoragePolicy,
    class... Args
>
struct VirtualMatchFunctorVImpl : public VirtualMatchFunctor {
    typedef VirtualMatchFunctorImpl<
        StoragePolicy, Args... > Impl;

    template <class... V>
    explicit VirtualMatchFunctorVImpl(V&&... v)
        : _impl(std::forward<V>(v)...) {}

    bool tryMatch(VirtualPack& p) override {
        return _impl.tryMatch(p);
    }

    bool tryMatch(const VirtualPack& p) override {
        return _impl.tryMatch(p);
    }

    bool tryMatch(VirtualPack& p) const override {
        return _impl.tryMatch(p);
    }

    bool tryMatch(const VirtualPack& p) const override {
        return _impl.tryMatch(p);
    }

    Impl _impl;
};

template <
    template <class> class StoragePolicy,
    class Head,class... Tail
>
struct VirtualMatchFunctorImpl< StoragePolicy, Head, Tail... > {
    typedef typename std::decay< Head >::type DecHead;
    typedef typename StoragePolicy<Head>::Container Container;
    typedef VirtualMatchFunctorImpl< StoragePolicy, Tail... > TailFunct;

    static const bool doesMatch = IsVirtualMatch< DecHead >::value;
    static_assert(doesMatch,
        "Class does not belong in a VirtualMatchFunctorImpl.");

    typedef typename VMatchFunctorCallHndl<
        DecHead >::CallHandler Caller;

    template <class A,class... V>
    explicit VirtualMatchFunctorImpl(A&& a,V&&... v) :
        _c(std::forward<A>(a)),
        _t(std::forward<V>(v)...) {}

    bool tryMatch(VirtualPack& p) {
        if (Caller::tryMatch(_c.getRef(),p)) {
            return true;
        }

        return _t.tryMatch(p);
    }

    bool tryMatch(const VirtualPack& p) {
        if (Caller::tryMatch(_c.getRef(),p)) {
            return true;
        }

        return _t.tryMatch(p);
    }

    bool tryMatch(VirtualPack& p) const {
        if (Caller::tryMatch(_c.cgetRef(),p)) {
            return true;
        }

        return _t.tryMatch(p);
    }

    bool tryMatch(const VirtualPack& p) const {
        if (Caller::tryMatch(_c.cgetRef(),p)) {
            return true;
        }

        return _t.tryMatch(p);
    }

private:
    Container _c;
    TailFunct _t;
};

template <
    template <class> class StoragePolicy,
    class Head
>
struct VirtualMatchFunctorImpl< StoragePolicy, Head > {
    typedef typename std::decay< Head >::type DecHead;
    typedef typename StoragePolicy<Head>::Container Container;

    static const bool doesMatch = IsVirtualMatch< DecHead >::value;
    static_assert(doesMatch,
        "Class does not belong in a VirtualMatchFunctorImpl.");

    typedef typename VMatchFunctorCallHndl<
        DecHead >::CallHandler Caller;

    template <class A>
    explicit VirtualMatchFunctorImpl(A&& a) :
        _c(std::forward<A>(a)) {}

    bool tryMatch(VirtualPack& p) {
        return Caller::tryMatch(_c.getRef(),p);
    }

    bool tryMatch(const VirtualPack& p) {
        return Caller::tryMatch(_c.getRef(),p);
    }

    bool tryMatch(VirtualPack& p) const {
        return Caller::tryMatch(_c.cgetRef(),p);
    }

    bool tryMatch(const VirtualPack& p) const {
        return Caller::tryMatch(_c.cgetRef(),p);
    }

private:
    Container _c;
};

}

#endif /* end of include guard: VIRTUALMATCHFUNCTOR_695UA513 */
