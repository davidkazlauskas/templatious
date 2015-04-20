//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

#ifndef VIRTUALPACK_989GSW71
#define VIRTUALPACK_989GSW71

#include <templatious/util/Exceptions.hpp>
#include <templatious/detail/TypeList.hpp>

#include <cassert>
#include <utility>
#include <typeindex>
#include <type_traits>
#include <bitset>

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
     * Check how many times this pack was used
     * when expanding into function.
     */
    virtual int useCount() const = 0;

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
        if (size() != sizeof...(Args)) {
            return false;
        }

        return sigCheckInternal<Args...>();
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
        const int COUNT = sizeof...(Args);
        if (size() != COUNT) {
            throw VirtualPackBadNumberOfArgumentsException();
        }

        if (!matchesSignature<Args...>()) {
            throw VirtualPackWrongTypeSignatureException();
        }

        void* arr[COUNT];
        dumpAddresses(arr);

        typedef TypeList<Args...> TheList;
        RecursiveCaller<COUNT-1>::template
            callRecursive<TheList>(arr,std::forward<F>(f));
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
        const int COUNT = sizeof...(Args);
        if (size() != COUNT) {
            throw VirtualPackBadNumberOfArgumentsException();
        }

        typedef TypeList<Args...> TheList;
        const bool allConst = TheList::
            template ForAll< std::is_const >::value;
        static_assert( allConst, "Const version of this "
            "method has to pass all types in signature as const.");
        if (!matchesSignature<Args...>()) {
            throw VirtualPackWrongTypeSignatureException();
        }

        void* arr[COUNT];
        dumpAddresses(arr);

        RecursiveCaller<COUNT-1>::template
            callRecursive<TheList>(arr,std::forward<F>(f));
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
        if (size() != COUNT) {
            return false;
        }

        if (!matchesSignature<Args...>()) {
            return false;
        }

        void* arr[COUNT];
        dumpAddresses(arr);

        typedef TypeList<Args...> TheList;
        RecursiveCaller<COUNT-1>::template
            callRecursive<TheList>(arr,std::forward<F>(f));

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
        if (size() != COUNT) {
            return false;
        }

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

        RecursiveCaller<COUNT-1>::template
            callRecursive<TheList>(arr,std::forward<F>(f));

        return true;
    }
protected:
    virtual void dumpAddresses(void** arr) const = 0;
private:

    template <class... Args>
    bool sigCheckInternal() const {
        const bool isLast = sizeof...(Args) == 1;

        typedef typename std::conditional<
            isLast,
            bool,
            void*
        >::type Disambiguator;

        return sigCheck<0,Args...>(Disambiguator());
    }

    template <int cnt,class A,class... Tail>
    bool sigCheck(void*) const {
        const bool isLast = sizeof...(Tail) == 1;
        const bool isConst = std::is_const<A>::value;
        if (!isConst && constAt(cnt)) {
            return false;
        }

        typedef typename std::conditional<
            isLast,
            bool,
            void*
        >::type Disambiguator;

        if (typeAt(cnt) == std::type_index(typeid(A))) {
            return sigCheck<cnt+1,Tail...>(Disambiguator());
        }
        return false;
    }

    template <int cnt,class A>
    bool sigCheck(bool) const {
        const bool isConst = std::is_const<A>::value;
        if (!isConst && constAt(cnt)) {
            return false;
        }

        if (typeAt(cnt) == std::type_index(typeid(A))) {
            return true;
        }
        return false;
    }
};

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

    template <size_t n,class Curr,class... V>
    VPackContainer(std::bitset<n>& constness,
            std::type_index* ptr,int i,Curr&& c,V&&... v) :
        _c(std::forward<Curr>(c)),
        _t(constness,ptr,i+1,std::forward<V>(v)...)
    {
        constness[i] = std::is_const<Head>::value;
        new(std::addressof(ptr[i])) std::type_index(typeid(Head));
    }

    struct ThisValGetter {
        template <int i>
        static Head& get(ThisContainer& cont) {
            return cont._c;
        }
    };

    struct TailValGetter {
        template <int i>
        static auto get(ThisContainer& cont)
         -> decltype(
             std::declval<ThisContainer&>()._t
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

        static auto get(ThisContainer& c)
         -> decltype(Getter::template get<i>(c))
        {
            return Getter::template get<i>(c);
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
    static const bool stripped_same =
        std::is_same<Decay,Tail>::value;
    static_assert(stripped_same,
        "Virtual packs shall only store raw copies"
        " without references.");

    typedef typename std::remove_const<Tail>::type NConstTail;

    template <int i>
    auto fGet()
     -> Tail&
    {
        static_assert( i == 0,
            "Trying to get past end of virtual pack.");
        return _c;
    }

    template <size_t n,class Curr>
    VPackContainer(std::bitset<n>& constness,
            std::type_index* ptr,int i,Curr&& c) :
        _c(std::forward<Curr>(c))
    {
        constness[i] = std::is_const<Tail>::value;
        new(std::addressof(ptr[i])) std::type_index(typeid(Tail));
    }

    void dumpAddress(void** ptr,int i) const {
        // cast away const as we trust our
        // client
        ptr[i] = std::addressof(
            const_cast<NConstTail&>(_c));
    }

    Tail _c;
};

template <class... T>
struct VirtualPackImpl : public VirtualPack {
    static const int pack_size = sizeof...(T);
    typedef VPackContainer<T...> ContType;

    template <class... V>
    explicit VirtualPackImpl(V&&... v) :
        _useCount(0),
        _cont(_constness,
            tArr(),0,
            std::forward<V>(v)...)
    {}

    ~VirtualPackImpl() {
        std::type_index* arr = tArr();
        for (int i = 0; i < pack_size; ++i) {
            arr[i].~type_index();
        }
    }

    int size() const override {
        return pack_size;
    }

    int useCount() const override {
        return _useCount;
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
        assert( (i < pack_size || i > 0)
            && "Whoa cholo, going too far, don't ya think?" );
        return tArr()[i];
    }

    bool constAt(int i) const override {
        assert( (i < pack_size || i > 0)
            && "Whoa cholo, going too far, don't ya think?" );
        return _constness[i];
    }

    // function should only be called when using
    // the pack.
    void dumpAddresses(void** arr) const override {
        ++_useCount;
        _cont.dumpAddress(arr,0);
    }

    const std::type_index* tArr() const {
        return reinterpret_cast<const std::type_index*>(_types);
    }

    std::type_index* tArr() {
        return reinterpret_cast<std::type_index*>(_types);
    }

private:
    mutable int _useCount;
    std::bitset<pack_size> _constness;
    std::aligned_storage<
        sizeof(std::type_index),alignof(std::type_index)
    >::type _types[pack_size];

    ContType _cont;
};

}

#endif /* end of include guard: VIRTUALPACK_989GSW71 */
