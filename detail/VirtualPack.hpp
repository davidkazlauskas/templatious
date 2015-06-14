//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

#ifndef VIRTUALPACK_989GSW71
#define VIRTUALPACK_989GSW71

#include <cassert>
#include <utility>
#include <typeindex>
#include <type_traits>
#include <bitset>
#include <future>

#include <templatious/util/Exceptions.hpp>
#include <templatious/util/Variadic.hpp>
#include <templatious/util/DefaultStoragePolicy.hpp>
#include <templatious/detail/TypeList.hpp>

namespace templatious {

TEMPLATIOUS_BOILERPLATE_EXCEPTION(
    VirtualPackBadNumberOfArgumentsException,
    "Virtual pack contains wrong function of arguments"
    " to call function."
);

TEMPLATIOUS_BOILERPLATE_EXCEPTION(
    VirtualPackWrongTypeSignatureException,
    "Virtual pack has different signature"
    " than expected."
);

TEMPLATIOUS_BOILERPLATE_EXCEPTION(
    VirtualPackTypeOutOfBoundsException,
    "Asked type/constness info is out of bounds."
);

template <int i>
struct RecursiveCaller {
    template <class TypeList,class F,class... Tail>
    static void callRecursive(void** ptr,F&& f,Tail&&... tail) {
        const int index = TypeList::size - i - 1;
        typedef typename TypeList::
            template ByIndex<index>::type CastTo;

        CastTo* thePointer =
            reinterpret_cast<CastTo*>(ptr[index]);
        RecursiveCaller<i-1>::template
            callRecursive<TypeList>(
                ptr,
                std::forward<F>(f),
                std::forward<Tail>(tail)...,
                *thePointer
            );
    }

    template <class TypeList,class F>
    static void callRecursive(void** ptr,F&& f) {
        const int index = TypeList::size - i - 1;
        typedef typename TypeList::
            template ByIndex<index>::type CastTo;

        CastTo* thePointer =
            reinterpret_cast<CastTo*>(ptr[index]);
        RecursiveCaller<i-1>::template
            callRecursive<TypeList>(
                ptr,
                std::forward<F>(f),
                *thePointer
            );
    }
};

template <>
struct RecursiveCaller<0> {
    template <class TypeList,class F,class... Tail>
    static void callRecursive(void** ptr,F&& f,Tail&&... tail) {
        const int index = TypeList::size - 1;
        typedef typename TypeList::
            template ByIndex<index>::type CastTo;

        CastTo* thePointer =
            reinterpret_cast<CastTo*>(ptr[index]);
        f(std::forward<Tail>(tail)...,*thePointer);
    }

    template <class TypeList,class F>
    static void callRecursive(void** ptr,F&& f) {
        const int index = TypeList::size - 1;
        typedef typename TypeList::
            template ByIndex<index>::type CastTo;

        CastTo* thePointer =
            reinterpret_cast<CastTo*>(ptr[index]);
        f(*thePointer);
    }
};

struct PackMetaInfo {
    int _size;
    std::bitset<32> _constness;
    const std::type_index* _idxPtr;
};

/**
 * The main purpose of this class is
 * to encapsulate any kind of values
 * into tuple form that can be passed
 * around at runtime and used. Uses
 * std::type_index to differentiate
 * the classes. Stores everything by copy.
 */
struct VirtualPack {
    /**
     * Return size of the pack in elements.
     */
    virtual int size() const = 0;

    virtual ~VirtualPack() {}

    /**
     * Return std::type_index of nth element
     * in the pack.
     * @param[in] i Which element to check.
     */
    virtual const std::type_index& typeAt(int i) const = 0;

    /**
     * Check if nth element is const in the pack.
     * @param[in] i Which element to check.
     */
    virtual bool constAt(int i) const = 0;

    /**
     * Check if the specified signature matches
     * this pack.
     * @param[in] Args signature to check. Has
     * to contain raw types without references.
     * Const qualifiers allowed.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<const int,long>(1,2);
     *
     * bool aMatch = p.matchesSignature<int,long>();
     * bool bMatch = p.matchesSignature<const int,long>();
     * bool cMatch = p.matchesSignature<const int,const long>();
     * bool dMatch = p.matchesSignature<const int>();
     * bool eMatch = p.matchesSignature<char>();
     *
     * assert( !aMatch );
     * assert(  bMatch );
     * assert(  cMatch );
     * assert( !dMatch );
     * assert( !eMatch );
     * ~~~~~~~
     */
    template <class... Args>
    bool matchesSignature() const {
        PackMetaInfo inf;
        dumpMetaInfo(inf);

        if (inf._size != sizeof...(Args)) {
            return false;
        }

        return sigCheckInternal<Args...>(inf);
    }

    /**
     * Call function for this pack with
     * specified signature. If specified signature
     * doesn't match this pack throws an exception.
     * @param[in] f Function to call if signature
     * matches.
     * @param[in] Args Signature arguments. If
     * signature matches these are what will be
     * expanded into function.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<int,long>(1,2);
     *
     * // signature has to match exactly
     * // for call to succeed (with exception
     * // that const can bind to non const)
     * p.callFunction<const int,long>(
     *     [](const int& a,long b) {
     *         std::cout << "Call succeeded!" << std::endl;
     *     }
     * );
     *
     * // prints out:
     * // Call succeeded!
     * ~~~~~~~
     */
    template <class... Args,class F>
    void callFunction(F&& f) {
        matchesSignatureThrow<Args...>();
        const int COUNT = sizeof...(Args);

        void* arr[COUNT];
        dumpAddresses(arr);

        typedef TypeList<Args...> TheList;
        callFunctionInternal<COUNT-1,TheList>(
            arr,std::forward<F>(f));
    }

    /**
     * Call function for this pack with
     * specified signature. If specified signature
     * doesn't match this pack throws an exception.
     * @param[in] f Function to call if signature
     * matches.
     * @param[in] Args Signature arguments. If
     * signature matches these are what will be
     * expanded into function.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<int,long>(1,2);
     *
     * // signature has to match exactly
     * // for call to succeed (with exception
     * // that const can bind to non const)
     * p.callFunction<const int,long>(
     *     [](const int& a,long b) {
     *         std::cout << "Call succeeded!" << std::endl;
     *     }
     * );
     *
     * // prints out:
     * // Call succeeded!
     * ~~~~~~~
     */
    template <class... Args,class F>
    void callFunction(F&& f) const {
        matchesSignatureThrow<Args...>();
        const int COUNT = sizeof...(Args);
        typedef TypeList<Args...> TheList;
        const bool allConst = TheList::
            template ForAll< std::is_const >::value;
        static_assert( allConst, "Const version of this "
            "method has to pass all types in signature as const.");

        void* arr[COUNT];
        dumpAddresses(arr);

        callFunctionInternal<COUNT-1,TheList>(
            arr,std::forward<F>(f));
    }


    /**
     * Try to call function for this pack with
     * specified signature. If specified signature
     * doesn't match this pack returned value is false,
     * if the call succeeds returns true.
     * @param[in] f Function to call if signature
     * matches.
     * @param[in] Args Signature arguments. If
     * signature matches these are what will be
     * expanded into function.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<int,long>(1,2);
     *
     * // signature has to match exactly
     * // for call to succeed (with exception
     * // that const can bind to non const)
     * bool matched = p.tryCallFunction<const int,long>(
     *     [](const int& a,long b) {
     *         std::cout << "Call succeeded!" << std::endl;
     *     }
     * );
     *
     * // prints out:
     * // Call succeeded!
     *
     * assert( matched );
     * ~~~~~~~
     */
    template <class... Args,class F>
    bool tryCallFunction(F&& f) {
        const int COUNT = sizeof...(Args);
        if (!matchesSignature<Args...>()) {
            return false;
        }

        void* arr[COUNT];
        dumpAddresses(arr);

        typedef TypeList<Args...> TheList;
        callFunctionInternal<COUNT-1,TheList>(
            arr,std::forward<F>(f));

        return true;
    }

    /**
     * Try to call function for this pack with
     * specified signature. If specified signature
     * doesn't match this pack returned value is false,
     * if the call succeeds returns true.
     * @param[in] f Function to call if signature
     * matches.
     * @param[in] Args Signature arguments. If
     * signature matches these are what will be
     * expanded into function.
     *
     * Example:
     * ~~~~~~~
     * auto p = SF::vpack<int,long>(1,2);
     *
     * // signature has to match exactly
     * // for call to succeed (with exception
     * // that const can bind to non const)
     * bool matched = p.tryCallFunction<const int,long>(
     *     [](const int& a,long b) {
     *         std::cout << "Call succeeded!" << std::endl;
     *     }
     * );
     *
     * // prints out:
     * // Call succeeded!
     *
     * assert( matched );
     * ~~~~~~~
     */
    template <class... Args,class F>
    bool tryCallFunction(F&& f) const {
        const int COUNT = sizeof...(Args);

        typedef TypeList< Args... > TheList;
        const bool allConst = TheList::
            template ForAll< std::is_const >::value;
        static_assert( allConst, "Const version of this "
            "method has to pass all types in signature as const.");
        if (!matchesSignature< Args... >()) {
            return false;
        }

        void* arr[COUNT];
        dumpAddresses(arr);

        callFunctionInternal<COUNT-1,TheList>(
            arr,std::forward<F>(f));

        return true;
    }

    /**
     * Get hash of this pack.
     */
    size_t getHash() const {
        return _hash;
    }
protected:
    VirtualPack(size_t hash) : _hash(hash) {}
    virtual void dumpAddresses(void** arr) const = 0;
    virtual void dumpMetaInfo(PackMetaInfo& out) const = 0;

    virtual std::mutex* mutexPtr() const = 0;
    virtual void invokeCallback() const = 0;
private:
    template <int count,class TypeList,class Func>
    void callFunctionInternal(void** args,Func&& f) const {
        std::mutex* mPtr = mutexPtr();
        if (nullptr == mPtr) {
            RecursiveCaller<count>::template
                callRecursive<TypeList>(args,std::forward<Func>(f));
            invokeCallback();
            return;
        }

        std::lock_guard< std::mutex > lg(*mPtr);
        RecursiveCaller<count>::template
            callRecursive<TypeList>(args,std::forward<Func>(f));
        invokeCallback();
    }

    template <class... Args>
    void matchesSignatureThrow() const {
        PackMetaInfo inf;
        dumpMetaInfo(inf);

        const int COUNT = sizeof...(Args);
        if (inf._size != COUNT) {
            throw VirtualPackBadNumberOfArgumentsException();
        }

        if (!sigCheckInternal<Args...>(inf)) {
            throw VirtualPackWrongTypeSignatureException();
        }
    }

    template <class... Args>
    bool sigCheckInternal(const PackMetaInfo& inf) const {
        const bool isLast = sizeof...(Args) == 1;

        typedef typename std::conditional<
            isLast,
            bool,
            void*
        >::type Disambiguator;

        return sigCheck<0,Args...>(Disambiguator(),inf);
    }

    template <int cnt,class A,class... Tail>
    bool sigCheck(void*,const PackMetaInfo& inf) const {
        const bool isLast = sizeof...(Tail) == 1;
        const bool isConst = std::is_const<A>::value;

        if (!isConst && inf._constness[cnt]) {
            return false;
        }

        typedef typename std::conditional<
            isLast,
            bool,
            void*
        >::type Disambiguator;

        if (inf._idxPtr[cnt] == std::type_index(typeid(A))) {
            return sigCheck<cnt+1,Tail...>(Disambiguator(),inf);
        }
        return false;
    }

    template <int cnt,class A>
    bool sigCheck(bool,const PackMetaInfo& inf) const {
        const bool isConst = std::is_const<A>::value;
        if (!isConst && inf._constness[cnt]) {
            return false;
        }

        if (inf._idxPtr[cnt] == std::type_index(typeid(A))) {
            return true;
        }
        return false;
    }

    // store this here rather down the
    // virtual inheritance chain to allow
    // aggressive inlining by the compiler
    size_t _hash;
};

struct ExpVpackConInvoke {};

template <class... T>
struct VPackContainer;

template <class Head,class... Tail>
struct VPackContainer<Head,Tail...> {
    typedef VPackContainer<Head,Tail...> ThisContainer;
    typedef VPackContainer<Tail...> TailPack;
    typedef typename std::remove_reference<Head>::type Decay;
    static const bool stripped_same =
        std::is_same<Decay,Head>::value;
    static_assert(stripped_same,
        "Virtual packs shall only store raw copies"
        " without references.");

    typedef typename std::remove_const<Head>::type NConstHead;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisContainer >::value;

    template <size_t n,class Curr,class... V>
    VPackContainer(ExpVpackConInvoke e,std::bitset<n>& constness,
            std::type_index* ptr,int i,Curr&& c,V&&... v) :
        _c(std::forward<Curr>(c)),
        _t(e,constness,ptr,i+1,std::forward<V>(v)...)
    {
        constness[i] = std::is_const<Head>::value;
        new(std::addressof(ptr[i])) std::type_index(typeid(Head));
    }

    template <bool canCopy = copy_constructable>
    VPackContainer(typename std::enable_if<
        canCopy,const ThisContainer&>::type other)
        : _c(other._c), _t(other._t) {}

    VPackContainer(ThisContainer&& other)
        : _c(std::move(other._c)), _t(std::move(other._t)) {}

    struct ThisValGetter {
        template <int i>
        static Head& get(ThisContainer& cont) {
            return cont._c;
        }

        template <int i>
        static const Head& get(const ThisContainer& cont) {
            return cont._c;
        }
    };

    struct TailValGetter {
        template <int i,class Val>
        static auto get(Val&& cont)
         -> decltype(
             cont._t
             .template fGet<i-1>()
         )
        {
            return cont._t.template fGet<i-1>();
        }
    };

    template <int i>
    struct GetVal {
        typedef typename std::conditional<
            i == 0,
            ThisValGetter,
            TailValGetter
        >::type Getter;

        template <class Val>
        static auto get(Val&& c)
         -> decltype(Getter::template get<i>(
                     std::forward<Val>(c)))
        {
            return Getter::template get<i>(
                    std::forward<Val>(c));
        }
    };

    template <int i>
    auto fGet()
     -> decltype(GetVal<i>::get(
         std::declval<ThisContainer&>()))
    {
        static_assert( i >= 0,
            "fGet index has to be non-negative." );
        return GetVal<i>::get(*this);
    }

    template <int i>
    auto fGet() const
     -> decltype(GetVal<i>::get(
         std::declval<const ThisContainer&>()))
    {
        static_assert( i >= 0,
            "fGet index has to be non-negative." );
        return GetVal<i>::get(*this);
    }

    void dumpAddress(void** ptr,int i) const {
        // cast away const as we trust our
        // client
        ptr[i] = std::addressof(
            const_cast<NConstHead&>(_c));
        _t.dumpAddress(ptr,i+1);
    }

    Head _c;
    TailPack _t;
};

template <class Tail>
struct VPackContainer<Tail> {
    typedef typename std::remove_reference<Tail>::type Decay;
    typedef VPackContainer< Tail > ThisContainer;
    static const bool stripped_same =
        std::is_same<Decay,Tail>::value;
    static_assert(stripped_same,
        "Virtual packs shall only store raw copies"
        " without references.");

    typedef typename std::remove_const<Tail>::type NConstTail;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisContainer >::value;

    template <int i>
    auto fGet()
     -> Tail&
    {
        static_assert( i == 0,
            "Trying to get past end of virtual pack.");
        return _c;
    }

    template <int i>
    auto fGet() const
     -> const Tail&
    {
        static_assert( i == 0,
            "Trying to get past end of virtual pack.");
        return _c;
    }

    template <size_t n,class Curr>
    VPackContainer(ExpVpackConInvoke e,std::bitset<n>& constness,
            std::type_index* ptr,int i,Curr&& c) :
        _c(std::forward<Curr>(c))
    {
        constness[i] = std::is_const<Tail>::value;
        new(std::addressof(ptr[i])) std::type_index(typeid(Tail));
    }

    template <bool canCopy = copy_constructable>
    VPackContainer(typename std::enable_if<
        canCopy, const ThisContainer&>::type other)
        : _c(other._c) {}

    VPackContainer(ThisContainer&& other)
        : _c(std::move(other._c)) {}

    void dumpAddress(void** ptr,int i) const {
        // cast away const as we trust our
        // client
        ptr[i] = std::addressof(
            const_cast<NConstTail&>(_c));
    }

    Tail _c;
};

template <class... T>
struct VirtualPackCore {
    static const int pack_size = sizeof...(T);
    typedef VPackContainer<T...> ContType;
    typedef VirtualPackCore<T...> ThisCore;
    typedef std::bitset<32> ConstBitset;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisCore >::value;

    template <class... V>
    explicit VirtualPackCore(ExpVpackConInvoke e,V&&... v) :
        _cont(e,_constness,
            tArr(),0,
            std::forward<V>(v)...)
    {}

    template <bool canCopy = copy_constructable>
    VirtualPackCore(typename std::enable_if<
        canCopy,const ThisCore&>::type other) :
        _constness(other._constness),
        _cont(other._cont)
    {
        initCopyTypes(pack_size,other.tArr());
    }

    VirtualPackCore(ThisCore&& other) :
        _constness(other._constness),
        _cont(std::move(other._cont))
    {
        initCopyTypes(pack_size,other.tArr());
    }

    /**
     * Fast get function for the creators of this object.
     * Get nth element of this pack.
     * Call is resolved at compile time, so, it's as
     * fast as it can get.
     * @param[in] i Which element to get, starting at 0.
     */
    template <int i>
    auto fGet()
     -> decltype(
         std::declval<ContType>().template fGet<i>()
     )
    {
        return _cont.template fGet<i>();
    }

    /**
     * Fast get function for the creators of this object.
     * Get nth element of this pack.
     * Call is resolved at compile time, so, it's as
     * fast as it can get.
     * @param[in] i Which element to get, starting at 0.
     */
    template <int i>
    auto fGet() const
     -> decltype(
         std::declval<const ContType>().template fGet<i>()
     )
    {
        return _cont.template fGet<i>();
    }

    const ThisCore& getCore() const {
        return *this;
    }

    void dumpMetaInfo(PackMetaInfo& out) const {
        out._size = pack_size;
        out._constness = _constness;
        out._idxPtr = tArr();
    }

    bool constness(int i) const {
        return _constness[i];
    }

    void dumpAddresses(void** arr) const {
        _cont.dumpAddress(arr,0);
    }

    const std::type_index* tArr() const {
        return reinterpret_cast<const std::type_index*>(_types);
    }

    std::type_index* tArr() {
        return reinterpret_cast<std::type_index*>(_types);
    }

    ConstBitset& constBitSet() {
        return _constness;
    }

    static size_t calcHash() {
        return templatious::util::hashTypes<T...>();
    }
private:
    void initCopyTypes(int size,const std::type_index* arr) {
        std::type_index* thisArr = tArr();
        for (int i = 0; i < size; ++i) {
            new (&thisArr[i]) std::type_index(arr[i]);
        }
    }

    ConstBitset _constness;
    std::aligned_storage<
        sizeof(std::type_index),alignof(std::type_index)
    >::type _types[pack_size];

    ContType _cont;
};

static const int VPACK_COUNT = 1;
static const int VPACK_WAIT = 2;
static const int VPACK_SYNCED = 4;
static const int VPACK_WCALLBACK = 8;

static const int VPACK_DEF_MASK = VPACK_COUNT;

namespace vpacktraits {

template <class Inherit>
struct VirtualPackCountTrait : public Inherit {
    typedef VirtualPackCountTrait< Inherit > ThisTrait;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisTrait >::value;

    template <bool canCopy = copy_constructable>
    VirtualPackCountTrait(typename std::enable_if<
        canCopy,const ThisTrait&>::type other)
        : Inherit(other), _useCount(other._useCount) {}

    VirtualPackCountTrait(ThisTrait&& other)
        : Inherit(std::move(other)), _useCount(other._useCount) {}

    template <class... V>
    VirtualPackCountTrait(ExpVpackConInvoke e,V&&... v) :
        Inherit(e,std::forward<V>(v)...),
        _useCount(0) {}

    void increment() const {
        ++_useCount;
    }

    int getCount() const {
        return _useCount;
    }
private:
    mutable int _useCount;
};

template <class Inherit>
struct VirtualPackNoCountTrait : public Inherit {
    typedef VirtualPackNoCountTrait< Inherit > ThisTrait;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisTrait >::value;

    template <bool canCopy = copy_constructable>
    VirtualPackNoCountTrait(typename std::enable_if<
        canCopy,const ThisTrait&>::type other)
        : Inherit(other) {}

    VirtualPackNoCountTrait(ThisTrait&& other)
        : Inherit(std::move(other)) {}

    template <class... V>
    VirtualPackNoCountTrait(ExpVpackConInvoke e,V&&... v) :
        Inherit(e,std::forward<V>(v)...) {}

    void increment() const { }

    int getCount() const { return -1; }
};

template <class Inherit>
struct VirtualPackWaitTrait : public Inherit {
    typedef VirtualPackWaitTrait< Inherit > ThisTrait;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisTrait >::value;

    template <bool canCopy = copy_constructable>
    VirtualPackWaitTrait(typename std::enable_if<
        canCopy,const ThisTrait&>::type other)
        : Inherit(other), _p(other._p), _f(other._f) {}

    VirtualPackWaitTrait(ThisTrait&& other)
        : Inherit(std::move(other)),
        _p(std::move(other._p)),
        _f(std::move(other._f)) {}

    template <class... V>
    VirtualPackWaitTrait(ExpVpackConInvoke e,V&&... v) :
        Inherit(e,std::forward<V>(v)...),
        _f(_p.get_future()) {}

    void setReady() const {
        if (_f.wait_for(std::chrono::seconds(0)) !=
                std::future_status::ready)
        {
            _p.set_value();
        }
    }

    void wait() const {
        _f.wait();
    }

    void waitMs(int milliseconds) const {
        _f.wait_for(
            std::chrono::milliseconds(milliseconds)
        );
    }

    bool isReady() const {
        return _f.valid();
    }

private:
    mutable std::promise<void> _p;
    std::future<void> _f;
};

template <class Inherit>
struct VirtualPackNoWaitTrait : public Inherit {
    typedef VirtualPackNoWaitTrait< Inherit > ThisTrait;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisTrait >::value;

    template <bool canCopy = copy_constructable>
    VirtualPackNoWaitTrait(typename std::enable_if<
        canCopy,const ThisTrait&>::type other)
        : Inherit(other) {}

    VirtualPackNoWaitTrait(ThisTrait&& other)
        : Inherit(std::move(other)) {}

    template <class... V>
    VirtualPackNoWaitTrait(ExpVpackConInvoke e,V&&... v) :
        Inherit(e,std::forward<V>(v)...) {}

    void setReady() const {}

    void wait() const {}

    void waitMs(int milliseconds) const {}

    bool isReady() const { return true; }
};

template <class Inherit>
struct VirtualPackSynchronizedTrait : public Inherit {
    typedef VirtualPackSynchronizedTrait< Inherit > ThisTrait;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisTrait >::value;

    template <bool canCopy = copy_constructable>
    VirtualPackSynchronizedTrait(typename std::enable_if<
        canCopy,const ThisTrait&>::type other)
        : Inherit(other), _mtx(other._mtx) {}

    VirtualPackSynchronizedTrait(ThisTrait&& other)
        : Inherit(std::move(other)), _mtx(std::move(other._mtx)) {}

    template <class... V>
    VirtualPackSynchronizedTrait(ExpVpackConInvoke e,V&&... v) :
        Inherit(e,std::forward<V>(v)...) {}

    std::mutex* getMutex() const {
        return &_mtx;
    }
private:
    mutable std::mutex _mtx;
};

template <class Inherit>
struct VirtualPackUnsynchronizedTrait : public Inherit {
    typedef VirtualPackUnsynchronizedTrait< Inherit > ThisTrait;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisTrait >::value;

    template <bool canCopy = copy_constructable>
    VirtualPackUnsynchronizedTrait(typename std::enable_if<
        canCopy,const ThisTrait&>::type other)
        : Inherit(other) {}

    VirtualPackUnsynchronizedTrait(ThisTrait&& other)
        : Inherit(std::move(other)) {}

    template <class... V>
    VirtualPackUnsynchronizedTrait(ExpVpackConInvoke e,V&&... v) :
        Inherit(e,std::forward<V>(v)...) {}

    std::mutex* getMutex() const {
        return nullptr;
    }
};

template <
    class Inherit,class Function,
    template <class> class StoragePolicy
>
struct VirtualPackOnReadyTrait : public Inherit {
    typedef VirtualPackOnReadyTrait<
        Inherit, Function, StoragePolicy > ThisTrait;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisTrait >::value;

    template <bool canCopy = copy_constructable>
    VirtualPackOnReadyTrait(typename std::enable_if<
        canCopy,const ThisTrait&>::type other)
        : Inherit(other), _c(other._c) {}

    VirtualPackOnReadyTrait(ThisTrait&& other)
        : Inherit(std::move(other)), _c(std::move(other._c)) {}

    typedef typename StoragePolicy<Function>::Container Cont;

    template <class Func,class... V>
    VirtualPackOnReadyTrait(ExpVpackConInvoke e,Func&& f,V&&... v) :
        Inherit(e,std::forward<V>(v)...),
        _c(std::forward<Func>(f)) {}

    void invokeCallback() const {
        _c.cgetRef()(Inherit::getCore());
    }

private:
    Cont _c;
};

template <
    class Inherit,class Function,
    template <class> class StoragePolicy
>
struct VirtualPackNoOnReadyTrait : public Inherit {
    typedef VirtualPackNoOnReadyTrait<
        Inherit, Function, StoragePolicy > ThisTrait;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisTrait >::value;

    template <bool canCopy = copy_constructable>
    VirtualPackNoOnReadyTrait(typename std::enable_if<
        canCopy,const ThisTrait&>::type other)
        : Inherit(other) {}

    VirtualPackNoOnReadyTrait(ThisTrait&& other)
        : Inherit(std::move(other)) {}

    template <class... V>
    VirtualPackNoOnReadyTrait(ExpVpackConInvoke e,V&&... v) :
        Inherit(e,std::forward<V>(v)...) {}

    void invokeCallback() const {}
};

template <int mask>
struct ConvertBitmaskToVPackCoreSettings {
    static const bool is_counted = (mask & VPACK_COUNT) != 0;
    static const bool is_waitable = (mask & VPACK_WAIT) != 0;
    static const bool is_synced = (mask & VPACK_SYNCED) != 0;
    static const bool has_callback = (mask & VPACK_WCALLBACK) != 0;
};

template <
    class CoreSettings,
    class... Args
>
struct VirtualPackCoreCreator {

    struct CutFirst {
        template <class A,class... Tail>
        struct Func {
            typedef VirtualPackCore<Tail...> type;
        };
    };

    struct NoCutFirst {
        template <class A,class... Tail>
        struct Func {
            typedef VirtualPackCore<A,Tail...> type;
        };
    };

    typedef typename std::conditional<
        CoreSettings::has_callback,
        CutFirst,
        NoCutFirst
    >::type CoreCreator;

    typedef typename CoreCreator::template Func<Args...>::type Core;

    // for callback
    typedef typename templatious::util::
        GetFrist<Args...>::type First;

    typedef typename std::conditional<
        CoreSettings::is_counted,
        VirtualPackCountTrait< Core >,
        VirtualPackNoCountTrait< Core >
    >::type Counted;

    typedef typename std::conditional<
        CoreSettings::is_waitable,
        VirtualPackWaitTrait< Counted >,
        VirtualPackNoWaitTrait< Counted >
    >::type Waited;

    typedef typename std::conditional<
        CoreSettings::is_synced,
        VirtualPackSynchronizedTrait< Waited >,
        VirtualPackUnsynchronizedTrait< Waited >
    >::type Synced;

    // CopyOnlyStoragePolicy for
    // keeping everyone's sanity
    typedef typename std::conditional<
        CoreSettings::has_callback,
        VirtualPackOnReadyTrait<
            Synced, First,
            templatious::util::CopyOnlyStoragePolicy
        >,
        VirtualPackNoOnReadyTrait<
            Synced, First,
            templatious::util::CopyOnlyStoragePolicy
        >
    >::type Called;

    typedef Called FinalCore;
};

}

template <int coreBitmask,class... T>
struct VirtualPackImpl : public VirtualPack {
    static const int pack_size = sizeof...(T);
    typedef vpacktraits::ConvertBitmaskToVPackCoreSettings<
        coreBitmask
    > VPackSettings;
    typedef VirtualPackImpl<coreBitmask,T...> ThisImpl;
    typedef typename vpacktraits::VirtualPackCoreCreator<
        VPackSettings,T...>::FinalCore ContType;

    static const bool copy_constructable =
        std::is_copy_constructible< ThisImpl >::value;

    template <class... V>
    explicit VirtualPackImpl(ExpVpackConInvoke e,V&&... v) :
        VirtualPack(ContType::calcHash()),
        _cont(e,std::forward<V>(v)...)
    {}

    template <bool canCopy = copy_constructable>
    VirtualPackImpl(typename std::enable_if<
        canCopy,const ThisImpl&>::type impl) :
        VirtualPack(ContType::calcHash()),
        _cont(impl._cont) {}

    VirtualPackImpl(ThisImpl&& impl) :
        VirtualPack(ContType::calcHash()),
        _cont(std::move(impl._cont)) {}

    ~VirtualPackImpl() {
        std::type_index* arr = tArr();
        for (int i = 0; i < pack_size; ++i) {
            arr[i].~type_index();
        }
    }

    int size() const override {
        return pack_size;
    }

    int useCount() const {
        return _cont.getCount();
    }

    /**
     * Fast get function for the creators of this object.
     * Get nth element of this pack.
     * Call is resolved at compile time, so, it's as
     * fast as it can get.
     * @param[in] i Which element to get, starting at 0.
     */
    template <int i>
    auto fGet()
     -> decltype(
         std::declval<ContType>().template fGet<i>()
     )
    {
        return _cont.template fGet<i>();
    }

    const std::type_index& typeAt(int i) const override {
        if (i >= pack_size || i < 0) {
            throw VirtualPackTypeOutOfBoundsException();
        }
        return tArr()[i];
    }

    bool constAt(int i) const override {
        if (i >= pack_size || i < 0) {
            throw VirtualPackTypeOutOfBoundsException();
        }
        return _cont.constness(i);
    }

    // function should only be called when using
    // the pack.
    void dumpAddresses(void** arr) const override {
        _cont.dumpAddresses(arr);
    }

    void dumpMetaInfo(PackMetaInfo& out) const override {
        _cont.dumpMetaInfo(out);
    }

    const std::type_index* tArr() const {
        return _cont.tArr();
    }

    std::type_index* tArr() {
        return _cont.tArr();
    }

    void wait() const {
        _cont.wait();
    }

    void waitMs(int milliseconds) const {
        _cont.waitMs(milliseconds);
    }

    std::mutex* mutexPtr() const override {
        return _cont.getMutex();
    }

    void invokeCallback() const override {
        _cont.increment();
        _cont.setReady();
        _cont.invokeCallback();
    }

private:
    ContType _cont;
};

}

#endif /* end of include guard: VIRTUALPACK_989GSW71 */
