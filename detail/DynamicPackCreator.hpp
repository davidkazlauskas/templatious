//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

#ifndef DYNAMICPACKCREATOR_A8GJJ2Z6
#define DYNAMICPACKCREATOR_A8GJJ2Z6

#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <templatious/detail/VirtualPack.hpp>

namespace templatious {

struct DynVPackFactory;

namespace detail {
template <int packBitmask,class CallbackType>
struct DynamicVirtualPack;
    namespace dynvpacktraits {
        template <int packBitmask,class CallbackType>
        struct SingleCoreContainer;
        template <int packBitmask,class CallbackType>
        struct TraitedContainer;
    }
}

struct TypeNode {
    virtual long size() const = 0;
    virtual long align() const = 0;
    virtual std::type_index type() const = 0;
    virtual bool isConst() const = 0;
    virtual void sizeAndAlign(long& size,long& align) const = 0;

    // throws if object is unconstructable
    virtual void construct(void* ptr,const char* param) const = 0;
    virtual void destroy(void* ptr) const = 0;
    virtual void toString(const void* ptr,std::string& str) const = 0;

    virtual ~TypeNode() {}

    TypeNode(const TypeNode&) = delete;
    TypeNode(TypeNode&&) = delete;

protected:
    TypeNode() {}
};

typedef const TypeNode* TNodePtr;

namespace detail {

static size_t templatiousHashCString(const char* word)
{
    std::int64_t hash = 0;
    std::int32_t max = std::numeric_limits<std::int32_t>::max();
    for (int i = 0; word[i] != '\0'; i++)
    {
        hash = 31*hash + word[i];
        hash %= max;
    }
    return hash % max;
}

struct CStringHasher {
    size_t operator()(const char* const& str) const {
        return templatiousHashCString(str);
    }

    bool operator()(const char* const a,const char* const b) const {
        return 0 == std::strcmp(a,b);
    }
};

typedef std::unordered_map< const char*, TNodePtr, CStringHasher, CStringHasher > TNodeMapType;

// std::align would be used but g++ 4.8 doesn't have it...
static void* ptrAlign(size_t align,size_t size,void* ptr) {
    char* p1 = static_cast<char*>(ptr);
    char* p2 = reinterpret_cast<char*>(
        reinterpret_cast<size_t>(p1 + (align - 1)) & -align);
    return p2;
}

struct NodeFuncNull {
    // do nothing
    template <class... T>
    void operator()(T&&... t) const {}
};

template <class Construct,class Destroy,class Format>
struct NodeFuncUtil {
    typedef typename std::remove_reference<
        Construct>::type NoRefConstruct;
    typedef typename std::remove_reference<
        Destroy>::type NoRefDestroy;
    typedef typename std::remove_reference<
        Format>::type NoRefFormat;

    typedef NodeFuncUtil<NoRefConstruct,NoRefDestroy,NoRefFormat> ThisUtil;

    template <class C,class D,class F>
    static ThisUtil& inst(C&& c,D&& d,F&& f) {
        static ThisUtil util(
            std::forward<C>(c),
            std::forward<D>(d),
            std::forward<F>(f)
        );
        return util;
    }

    template <class... T>
    void construct(T&&... t) {
        _cons(std::forward<T>(t)...);
    }

    template <class... T>
    void destroy(T&&... t) {
        _dest(std::forward<T>(t)...);
    }

    template <class... T>
    void format(T&&... t) {
        _form(std::forward<T>(t)...);
    }

private:
    template <class C,class D,class F>
    NodeFuncUtil(C&& c,D&& d,F&& f) :
        _cons(std::forward<C>(c)),
        _dest(std::forward<D>(d)),
        _form(std::forward<F>(f)) {}

    NoRefConstruct _cons;
    NoRefDestroy _dest;
    NoRefFormat _form;
};

template <class T,class Func>
struct TypeNodeImpl : public TypeNode {
    typedef TypeNodeImpl<T,Func> ThisFact;
    typedef typename std::remove_reference<T>::type DecType;

    static const bool isRaw = std::is_same<DecType,T>::value;
    static_assert(isRaw,"Only raw types, without reference"
            " qualifiers can be used as template argument.");

    template <class C,class F>
    static TNodePtr sInst(C&& c,F&& f) {
        static ThisFact thisFact(
            Func::inst(
                std::forward<C>(c),
                NodeFuncNull(),
                std::forward<F>(f)
            )
        );
        return &thisFact;
    }

    template <class C,class D,class F>
    static TNodePtr sInst(C&& c,D&& d,F&& f) {
        static ThisFact thisFact(
            Func::inst(
                std::forward<C>(c),
                std::forward<D>(d),
                std::forward<F>(f)
            )
        );
        return &thisFact;
    };

    long size() const override {
        return sizeof(T);
    }

    long align() const override {
        return alignof(T);
    }

    bool isConst() const override {
        return std::is_const<T>::value;
    }

    void sizeAndAlign(long& size,long& align) const override {
        size = sizeof(T);
        align = alignof(T);
    }

    std::type_index type() const override {
        return std::type_index(typeid(T));
    }

    void construct(void* ptr,const char* param) const override {
        _funcInst.construct(ptr,param);
    }

    void destroy(void* ptr) const override {
        _funcInst.destroy(ptr);
    }

    void toString(const void* ptr,std::string& str) const override {
        _funcInst.format(ptr,str);
    }
private:
    Func& _funcInst;

    TypeNodeImpl(Func& ref) : _funcInst(ref) {}
};

struct DynamicVirtualPackCore {

    DynamicVirtualPackCore() = delete;
    DynamicVirtualPackCore(const DynamicVirtualPackCore&) = delete;
    DynamicVirtualPackCore(DynamicVirtualPackCore&&) = delete;

    ~DynamicVirtualPackCore() {
        void* buf[32];
        dumpOffsets(buf);
        TNodePtr* nodes = nodeArr();
        for (int i = _size - 1; i >= 0; --i) {
            nodes[i]->destroy(buf[i]);
        }
    }

    template <int packBitmask,class CallbackType>
    friend struct ::templatious::detail::DynamicVirtualPack;
    friend struct ::templatious::DynVPackFactory;
    template <int packBitmask,class CallbackType>
    friend struct ::templatious::detail::dynvpacktraits::SingleCoreContainer;
    template <int packBitmask,class CallbackType>
    friend struct ::templatious::detail::dynvpacktraits::TraitedContainer;

    template <class Expected>
    const Expected* get(int pos) const {
        if (pos < 0 || pos >= _size) {
            return nullptr;
        }

        static_assert(!std::is_reference<Expected>::value,
            "This function must receive plain type without"
            " reference qualifiers.");
        auto expIdx = std::type_index(typeid(Expected));
        if (expIdx != this->typeAt(pos)) {
            return nullptr;
        }

        return reinterpret_cast<const Expected*>(
            getNthPointer(pos)
        );
    }
#ifndef TEMPLATIOUS_TESTING
private:
#endif

    // if stuff throws while we construct
    // virtual pack core we need to free
    // nodes we already constructed.
    template <int size>
    struct PartialConstructionGuard {
        // default ctor for array init
        PartialConstructionGuard() :
            _used(0) {}

        ~PartialConstructionGuard() {
            for (int i = _used - 1; i >= 0; --i) {
                _node[i]->destroy(_mem[i]);
            }
        }

        void dismiss() {
            _used = 0;
        }

        void push(TNodePtr node,void* mem) {
            assert( _used < size );
            _node[_used] = node;
            _mem[_used] = mem;
            ++_used;
        }

    // kinda redundant but I'm paranoid
    private:
        int _used;
        TNodePtr _node[size];
        void* _mem[size];
    };

    int size() const {
        return _size;
    }

    const std::type_index& typeAt(int i) const {
        if (i >= _size || i < 0) {
            throw VirtualPackTypeOutOfBoundsException();
        }
        return idxArr()[i];
    }

    bool constAt(int i) const {
        if (i >= _size || i < 0) {
            throw VirtualPackTypeOutOfBoundsException();
        }
        return _set[i];
    }

    void dumpAddresses(void** arr) const {
        dumpOffsets(arr);
    }

    void* getNthPointer(int j) const {
        void* iterator = valueStart();
        TNodePtr* nodes = nodeArr();
        for (int i = 0; i < _size; ++i) {
            if (i == j) return iterator;
            long size,align;
            nodes[i]->sizeAndAlign(size,align);
            void* newIter = ptrAlign(align,size,iterator);
            iterator = newIter;
            iterator = reinterpret_cast<char*>(iterator) + size;
        }

        return nullptr;
    }

    void dumpMetaInfo(PackMetaInfo& out) const {
        out._size = _size;
        out._constness = _set;
        out._idxPtr = idxArr();
    }

    DynamicVirtualPackCore(int size,TNodePtr* types,const char* values[])
        : _size(size)
    {
        std::type_index* idxes = idxArr();
        TNodePtr* nodes = nodeArr();
        for (int i = 0; i < _size; ++i) {
            nodes[i] = types[i];
            idxes[i] = nodes[i]->type();
            _set[i] = nodes[i]->isConst();
        }

        PartialConstructionGuard<32> guard;
        void* buf[32];
        dumpOffsets(buf);
        for (int i = 0; i < _size; ++i) {
            nodes[i]->construct(buf[i],values[i]);
            guard.push(nodes[i],buf[i]);
        }

        guard.dismiss();
    }

    static size_t calcHash(int size,TNodePtr* types) {
        size_t result = 0;
        for (int i = 0; i < size; ++i) {
            size_t currCode = types[i]->type().hash_code();
            size_t rightPart = 0;
            if (i > 0) {
                rightPart = (currCode << (sizeof(currCode)*CHAR_BIT - i));
            }
            result ^= (currCode >> i) | rightPart;
        }
        return result;
    }

    char* getMem() const {
        return reinterpret_cast<char*>(&_buff);
    }

    std::type_index* idxArr() const {
        return reinterpret_cast<std::type_index*>(getMem());
    }

    int idxOffset() const {
        return sizeof(std::type_index) * _size;
    }

    int nodeOffset() const {
        return idxOffset() + sizeof(TypeNode*) * _size;
    }

    TNodePtr* nodeArr() const {
        return reinterpret_cast<TNodePtr*>(
            getMem() + idxOffset());
    }

    char* valueStart() const {
        return reinterpret_cast<char*>(
            getMem() + nodeOffset());
    }

    void dumpOffsets(void** buf) const {
        void* iterator = valueStart();
        TNodePtr* nodes = nodeArr();
        for (int i = 0; i < _size; ++i) {
            long size,align;
            nodes[i]->sizeAndAlign(size,align);
            void* newIter = ptrAlign(align,size,iterator);
            buf[i] = reinterpret_cast<char*>(newIter);
            iterator = newIter;
            iterator = reinterpret_cast<char*>(iterator) + size;
        }
    }

    static int cummulativeSize(int size,TNodePtr* types) {
        int countedSize = 0;
        static_assert(sizeof(char) == 1,"HUH?!?");
        char* nlPtr = nullptr;
        char* countPtr = nullptr;
        void* iterator;
        for (int i = 0; i < size; ++i) {
            long size,align;
            iterator = countPtr;
            types[i]->sizeAndAlign(size,align);
            iterator = ptrAlign(align,size,iterator);
            countPtr = reinterpret_cast<char*>(iterator);
            countPtr += size;
            countedSize = (countPtr - nlPtr);
        }

        int total = 0;
        total += sizeof(std::type_index) * size;
        total += sizeof(TypeNode*) * size;
        total += countedSize;
        if (total > static_cast<int>(sizeof(_buff))) {
            total -= sizeof(_buff);
        }
        return total;
    }

    int _size;
    std::bitset<32> _set;
    // doesn't point to anything, rather, a place
    // holder for our memory
    mutable void* _buff;
};

namespace dynvpacktraits {

template <class Inherit>
struct VirtualPackCountTrait : public Inherit {
    template <class... Args>
    VirtualPackCountTrait(Args&&... args) :
        Inherit(std::forward<Args>(args)...),
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
    template <class... Args>
    VirtualPackNoCountTrait(Args&&... args) :
        Inherit(std::forward<Args>(args)...) {}

    void increment() const {}

    int getCount() const { return -1; }
};

template <class Inherit>
struct VirtualPackWaitTrait : public Inherit {
    template <class... Args>
    VirtualPackWaitTrait(Args&&... args) :
        Inherit(std::forward<Args>(args)...),
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
    template <class... Args>
    VirtualPackNoWaitTrait(Args&&... args) :
        Inherit(std::forward<Args>(args)...) {}

    void setReady() const {}
    void wait() const {}
    void waitMs(int milliseconds) const {}
    bool isReady() const { return true; }
};

template <class Inherit>
struct VirtualPackSynchronizedTrait : public Inherit {
    template <class... Args>
    VirtualPackSynchronizedTrait(Args&&... args) :
        Inherit(std::forward<Args>(args)...) {}

    std::mutex* getMutex() const {
        return &_mtx;
    }
private:
    mutable std::mutex _mtx;
};

template <class Inherit>
struct VirtualPackUnsynchronizedTrait : public Inherit {
    template <class... Args>
    VirtualPackUnsynchronizedTrait(Args&&... args) :
        Inherit(std::forward<Args>(args)...) {}

    std::mutex* getMutex() const { return nullptr; }
};

template <
    class Inherit,class Function,
    template <class> class StoragePolicy
>
struct VirtualPackOnReadyTrait : public Inherit {
    template <class Func,class... Args>
    VirtualPackOnReadyTrait(Func&& f,Args&&... args) :
        Inherit(std::forward<Args>(args)...),
        _c(std::forward<Func>(f)) {}

    void invokeCallback(const DynamicVirtualPackCore& vpc) const {
        _c.cgetRef()(vpc);
    }

    typedef typename StoragePolicy<Function>::Container Cont;
private:
    Cont _c;
};

template <
    class Inherit,class Function,
    template <class> class StoragePolicy
>
struct VirtualPackNoOnReadyTrait : public Inherit {
    template <class... Args>
    VirtualPackNoOnReadyTrait(Args&&... args) :
        Inherit(std::forward<Args>(args)...) {}

    void invokeCallback(const DynamicVirtualPackCore& vpc) const {}
};

template <int packBitmask,class CallbackType>
struct DynamicVirtualPackTraitCreator {
    typedef templatious::vpacktraits::
        ConvertBitmaskToVPackCoreSettings<packBitmask> Settings;

    // 0 size when inherited
    struct Dummy {
        Dummy() {}

        template <class... Anything>
        Dummy(Anything&&... any) {}
    };

    typedef Dummy OrigCore;

    typedef typename std::conditional<
        Settings::is_counted,
        VirtualPackCountTrait<OrigCore>,
        VirtualPackNoCountTrait<OrigCore>
    >::type Counted;

    typedef typename std::conditional<
        Settings::is_waitable,
        VirtualPackWaitTrait<Counted>,
        VirtualPackNoWaitTrait<Counted>
    >::type Waited;

    typedef typename std::conditional<
        Settings::is_synced,
        VirtualPackSynchronizedTrait<Waited>,
        VirtualPackUnsynchronizedTrait<Waited>
    >::type Synced;

    typedef typename std::conditional<
        Settings::has_callback,
        VirtualPackOnReadyTrait<
            Synced, CallbackType,
            templatious::util::CopyOnlyStoragePolicy
        >,
        VirtualPackNoOnReadyTrait<
            Synced, CallbackType,
            templatious::util::CopyOnlyStoragePolicy
        >
    >::type Called;

    typedef Called FinalTraits;
};

template <int packBitmask,class CallbackType>
struct SingleCoreContainer {
    typedef typename dynvpacktraits::DynamicVirtualPackTraitCreator<
        packBitmask,CallbackType>::FinalTraits FinalTraits;

    template <class Func>
    SingleCoreContainer(int size,TNodePtr* types,const char* values[],Func&& f)
        : _core(size,types,values) {}

    const SingleCoreContainer& getTraits() const {
        return *this;
    }

    int getCount() const {
        return -1;
    }

    std::mutex* getMutex() const {
        return nullptr;
    }

    void increment() const {}

    void setReady() const {}

    template <class Anything>
    void invokeCallback(Anything&& a) const {}

    DynamicVirtualPackCore _core;
};

template <int packBitmask,class CallbackType>
struct TraitedContainer {
    typedef typename dynvpacktraits::DynamicVirtualPackTraitCreator<
        packBitmask,CallbackType>::FinalTraits FinalTraits;

    template <class Func>
    TraitedContainer(int size,TNodePtr* types,const char* values[],Func&& f)
        : _traits(std::forward<Func>(f)), _core(size,types,values) {}

    const FinalTraits& getTraits() const {
        return _traits;
    }

    FinalTraits _traits;
    DynamicVirtualPackCore _core;
};

}

TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynamicVirtualPackArrayTooSmall,
    "String array size is too low to fit formatted values.");

template <int packBitmask,class CallbackType>
struct DynamicVirtualPack : public VirtualPack {

    DynamicVirtualPack() = delete;
    DynamicVirtualPack(const DynamicVirtualPack&) = delete;
    DynamicVirtualPack(DynamicVirtualPack&&) = delete;

    int size() const override {
        return getCore().size();
    }

    const std::type_index& typeAt(int i) const override {
        return getCore().typeAt(i);
    }

    bool constAt(int i) const override {
        return getCore().constAt(i);
    }

    int useCount() const {
        return getTraits().getCount();
    }

    void wait() const {
        getTraits().wait();
    }

    void waitMs(int ms) const {
        getTraits().waitMs(ms);
    }

    int formatAll(int size,std::string* arr) const {
        int thisSize = getCore().size();
        if (size < thisSize) {
            throw DynamicVirtualPackArrayTooSmall();
        }

        void* addr[32];
        getCore().dumpAddresses(addr);
        TNodePtr* nodeArr = getCore().nodeArr();
        for (int i = 0; i < thisSize; ++i) {
            nodeArr[i]->toString(addr[i],arr[i]);
        }

        return thisSize;
    }

    static int requiredSize(int size,TNodePtr* types) {
        int coreSize = DynamicVirtualPackCore
            ::cummulativeSize(size,types);
        return coreSize + sizeof(Container);
    }

#ifndef TEMPLATIOUS_TESTING
protected:
#endif
    typedef typename dynvpacktraits::DynamicVirtualPackTraitCreator<
        packBitmask,CallbackType>::FinalTraits FinalTraits;

    static const bool isEmpty = std::is_empty<FinalTraits>::value;
    typedef typename std::conditional<
        isEmpty,
        dynvpacktraits::SingleCoreContainer<packBitmask,CallbackType>,
        dynvpacktraits::TraitedContainer<packBitmask,CallbackType>
    >::type Container;

    const DynamicVirtualPackCore& getCore() const {
        return _cont._core;
    }

    auto getTraits() const
        -> decltype(std::declval<Container>().getTraits())
    {
        return _cont.getTraits();
    }

    void dumpAddresses(void** arr) const override {
        getCore().dumpAddresses(arr);
    }

    void dumpMetaInfo(PackMetaInfo& out) const override {
        getCore().dumpMetaInfo(out);
    }

    std::mutex* mutexPtr() const override {
        return getTraits().getMutex();
    }

    void invokeCallback() const override {
        getTraits().increment();
        getTraits().setReady();
        getTraits().invokeCallback(getCore());
    }

    static size_t getHash(int size,TNodePtr* types) {
        return DynamicVirtualPackCore::calcHash(size,types);
    }

    friend struct ::templatious::DynVPackFactory;

#ifndef TEMPLATIOUS_TESTING
private:
#endif
    // the true type signature expected is
    // [possible callback],int size,TNodePtr* types,const char* values
    template <class Func>
    DynamicVirtualPack(int size,TNodePtr* types,const char* values[],Func&& f)
        : VirtualPack(getHash(size,types)),
          _cont(size,types,values,std::forward<Func>(f)) {}

    Container _cont;
};

}

struct TypeNodeFactory {
    template <class T,class Construct,class Format>
    static TNodePtr makePodNode(Construct&& c,Format&& f) {
        typedef typename std::decay<Construct>::type CDec;
        typedef typename std::decay<Format>::type FDec;
        typedef detail::NodeFuncUtil<CDec,detail::NodeFuncNull,FDec> FUtil;
        const bool isPod = std::is_pod<T>::value;
        static_assert(isPod,
            "Destructor may not be provided only"
            " for POD (Plain-old-data) types.");
        return detail::TypeNodeImpl<T,FUtil>::sInst(
            std::forward<Construct>(c),
            std::forward<Format>(f));
    }

    template <class T,class Construct,class Destroy,class Format>
    static TNodePtr makeFullNode(Construct&& c,Destroy&& d,Format&& f) {
        typedef typename std::decay<Construct>::type CDec;
        typedef typename std::decay<Destroy>::type DDec;
        typedef typename std::decay<Format>::type FDec;
        typedef detail::NodeFuncUtil<CDec,DDec,FDec> FUtil;
        return detail::TypeNodeImpl<T,FUtil>::sInst(
            std::forward<Construct>(c),
            std::forward<Destroy>(d),
            std::forward<Format>(f));
    }

    template <class T>
    static TNodePtr makeDummyNode() {
        static auto fmt =
            [](const void*,std::string& str) {
                str = typeid(T).name();
            };
        static auto con =
            [](void* mem,const char*) {
                new (mem) T();
            };
        typedef decltype(con) Con;
        typedef decltype(fmt) Fmt;
        typedef detail::NodeFuncUtil<
            Con,
            detail::NodeFuncNull,
            Fmt
        > FUtil;
        return detail::TypeNodeImpl<T,FUtil>::sInst(con,fmt);
    }

    template <class T>
    static TNodePtr makeDummyNode(const char* name) {
        static std::string sName(name);
        static auto fmt =
            [&](const void*,std::string& str) {
                str = sName;
            };
        static auto con =
            [](void* mem,const char*) {
                new (mem) T();
            };
        typedef decltype(con) Con;
        typedef decltype(fmt) Fmt;
        typedef detail::NodeFuncUtil<
            Con,
            detail::NodeFuncNull,
            Fmt
        > FUtil;
        return detail::TypeNodeImpl<T,FUtil>::sInst(con,fmt);
    }
};

TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVPackFactoryNoTypeException,
    "Type is not indexed in the dictionary." );
TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVPackFactoryTypeLimitException,
    "Maximum type limit is 32." );
TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVPackFactoryIncorrectSizeException,
    "Minimum size of a pack is 1." );
TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVPackFactorySerializeArrayTooSmallException,
    "String array for serialization is too small." );
TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVPackFactorySerializeUnknownTypeException,
    "Type to be serialized is not present in the dictionary." );

struct DynVPackFactory {

    template <int flags,class Callback>
    struct DynPackMakerGeneric {
        struct CallbackExists {
            static const int flag = templatious::VPACK_WCALLBACK;
        };

        struct NoCallback {
            static const int flag = 0;
        };

        typedef typename std::decay<Callback>::type DecCallback;
        static const bool isVoidPtr = std::is_same<void*,DecCallback>::value;
        typedef typename std::conditional<
            isVoidPtr,NoCallback,CallbackExists>::type CBValue;
        static const int FLAGS = flags | CBValue::flag;

        typedef typename std::conditional<
            !isVoidPtr,Callback,void
        >::type Fwd;

        typedef detail::DynamicVirtualPack<FLAGS,Fwd> Signature;

        template <class CustCallback>
        static auto make(
            const DynVPackFactory& fact,
            int size,const char** keys,const char** values,
            CustCallback&& c
        )
         -> std::shared_ptr<
                detail::DynamicVirtualPack<
                    FLAGS,
                    Fwd
                >
            >
        const {
            TNodePtr arr[TYPE_LIMIT];

            if (size <= 0) {
                throw DynVPackFactoryIncorrectSizeException();
            }

            if (size > TYPE_LIMIT) {
                throw DynVPackFactoryTypeLimitException();
            }

            for (int i = 0; i < size; ++i) {
                auto f = fact._map.find(keys[i]);
                if (f == fact._map.end()) {
                    throw DynVPackFactoryNoTypeException();
                }
                arr[i] = f->second;
            }

            int theSize = Signature::requiredSize(size,arr);
            // if we might throw...
            std::unique_ptr<char[]> uptr(new char[sizeof(Signature) + theSize]);
            Signature* obj = new (uptr.get()) Signature(size,arr,values,
                std::forward<CustCallback>(c));

            // created... take back
            uptr.release();

            return std::shared_ptr<Signature>(
                obj,
                [](Signature* obj) {
                    obj->~DynamicVirtualPack();
                    char* buff = reinterpret_cast<char*>(obj);
                    delete[] buff;
                }
            );
        }
    };

    DynVPackFactory() = delete;
    DynVPackFactory(const DynVPackFactory&) = delete;
    DynVPackFactory(DynVPackFactory&& other)
        : _map(std::move(other._map)) {}

    static const int TYPE_LIMIT = 32;

    typedef std::pair<bool,std::type_index> ReverseMapKey;
    struct ReverseMapKeyHasher {
        size_t operator()(const ReverseMapKey& key) const {
            return key.first + key.second.hash_code();
        }

        bool operator()(
                const ReverseMapKey& lhs,
                const ReverseMapKey& rhs) const
        {
            return lhs.first == rhs.first && lhs.second == rhs.second;
        }
    };

    struct TNodePtrHasher {
        size_t operator()(TNodePtr ptr) const {
            return ptr->isConst() + ptr->type().hash_code();
        }

        bool operator()(TNodePtr lhs,TNodePtr rhs) const {
            return lhs == rhs;
        }
    };

    typedef std::unordered_map<ReverseMapKey,TNodePtr,
        ReverseMapKeyHasher,ReverseMapKeyHasher> ReverseIndexMapType;
    typedef std::unordered_map<TNodePtr,const char*,
        TNodePtrHasher,TNodePtrHasher> ReverseStringMapType;

    DynVPackFactory(detail::TNodeMapType&& map,
                    ReverseIndexMapType&& rmap,
                    ReverseStringMapType&& rstringMap)
        : _map(std::move(map)), _reverseIndexMap(std::move(rmap)),
          _reverseNameMap(std::move(rstringMap)) {}

    auto makePack(int size,const char** keys,const char** values) const
     -> decltype(
            DynPackMakerGeneric<
                templatious::VPACK_COUNT,
                void*
            >::make(*this,size,keys,values,nullptr)
        )
    {
        return DynPackMakerGeneric<
            templatious::VPACK_COUNT,
            void*
        >::make(*this,size,keys,values,nullptr);
    }

    template <int flags>
    auto makePackCustom(int size,const char** keys,const char** values) const
     -> decltype(
            DynPackMakerGeneric<
                flags,
                void*
            >::make(*this,size,keys,values,nullptr)
        )
    {
        return DynPackMakerGeneric<
            flags,
            void*
        >::make(*this,size,keys,values,nullptr);
    }

    template <class Callback>
    auto makePackWCallback(int size,const char** keys,const char** values,Callback&& c) const
     -> decltype(
            DynPackMakerGeneric<
                templatious::VPACK_COUNT,
                decltype(std::forward<Callback>(c))
            >::make(*this,size,keys,values,std::forward<Callback>(c))
        )
    {
        return DynPackMakerGeneric<
            templatious::VPACK_COUNT,
            decltype(std::forward<Callback>(c))
        >::make(*this,size,keys,values,std::forward<Callback>(c));
    }

    template <int flags,class Callback>
    auto makePackCustomWCallback(int size,const char** keys,const char** values,Callback&& c) const
     -> decltype(
            DynPackMakerGeneric<
                flags,
                decltype(std::forward<Callback>(c))
            >::make(*this,size,keys,values,std::forward<Callback>(c))
        )
    {
        return DynPackMakerGeneric<
            flags,
            decltype(std::forward<Callback>(c))
        >::make(*this,size,keys,values,std::forward<Callback>(c));
    }

    int serializePack(const VirtualPack& p,int arrSize,
        std::string* arr,
        TNodePtr* outTypes = nullptr) const
    {
        std::mutex* mPtr = p.mutexPtr();
        if (nullptr != mPtr) {
            std::lock_guard< std::mutex > guard(*mPtr);
            return serializeGeneric(p,arrSize,arr,outTypes);
        }
        return serializeGeneric(p,arrSize,arr,outTypes);
    }

    std::vector< std::string > serializePack(
        const VirtualPack& p,
        TNodePtr* outTypes = nullptr) const
    {
        int size = p.size();
        std::vector< std::string > result(size);
        std::mutex* mPtr = p.mutexPtr();
        if (nullptr != mPtr) {
            std::lock_guard< std::mutex > guard(*mPtr);
            serializeGeneric(p,size,result.data(),outTypes);
            return result;
        }
        serializeGeneric(p,size,result.data(),outTypes);
        return result;
    }

    int serializeDynamicCore(
        const detail::DynamicVirtualPackCore& core,
        int arrSize,std::string* arr,
        TNodePtr* outTypes = nullptr) const
    {
        return serializeGeneric(core,arrSize,arr,outTypes);
    }

    std::vector< std::string > serializeDynamicCore(
        const detail::DynamicVirtualPackCore& core,
        TNodePtr* outTypes = nullptr) const
    {
        int size = core.size();
        std::vector< std::string > result(size);
        serializeGeneric(core,size,result.data(),outTypes);
        return result;
    }

    const char* associatedName(TNodePtr ptr) const {
        auto fnd = _reverseNameMap.find(ptr);
        if (_reverseNameMap.end() == fnd) {
            return nullptr;
        };
        return fnd->second;
    }
private:
    template <class Pack>
    int serializeGeneric(
            const Pack& arg,int arrSize,
            std::string* arr, TNodePtr* outTypes) const
    {
        PackMetaInfo inf;
        arg.dumpMetaInfo(inf);
        if (inf._size > arrSize) {
            throw DynVPackFactorySerializeArrayTooSmallException();
        }

        TNodePtr arrNode[32];
        for (int i = 0; i < inf._size; ++i) {
            auto key = ReverseMapKey(inf._constness[i],inf._idxPtr[i]);
            auto fnd = _reverseIndexMap.find(key);
            if (_reverseIndexMap.end() != fnd) {
                arrNode[i] = fnd->second;
                if (nullptr != outTypes) {
                    outTypes[i] = fnd->second;
                }
            } else {
                throw DynVPackFactorySerializeUnknownTypeException();
            }
        }

        void* addr[32];
        arg.dumpAddresses(addr);
        for (int i = 0; i < inf._size; ++i) {
            arrNode[i]->toString(addr[i],arr[i]);
        }

        return inf._size;
    }

    // this should be immutable and set in stone.
    detail::TNodeMapType _map;
    ReverseIndexMapType _reverseIndexMap;
    ReverseStringMapType _reverseNameMap;
};

TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVpackFactoryBuilderKeyExistsException,
    "Key already taken in the map." );
TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVpackFactoryBuilderUsedException,
    "Builder was already used." );

struct DynVPackFactoryBuilder {
    DynVPackFactoryBuilder() : _isUsed(false) {}

    void attachNode(const char* key,TNodePtr value) {
        Guard g(_mtx);
        assertUnused();

        auto search = _map.find(key);
        if (search != _map.end()) {
            throw DynVpackFactoryBuilderKeyExistsException();
        }

        _map.insert(std::make_pair(key,value));
    }

    DynVPackFactory getFactory() {
        Guard g(_mtx);
        assertUnused();
        _isUsed = true;
        DynVPackFactory::ReverseIndexMapType revIdxMap;
        revIdxMap.reserve(_map.size());
        DynVPackFactory::ReverseStringMapType revStringMap;
        revStringMap.reserve(_map.size());
        for (auto& i : _map) {
            auto key = std::pair<bool,std::type_index>(
                i.second->isConst(),i.second->type());
            revIdxMap.insert(std::make_pair(std::move(key),i.second));
            auto strKey = std::pair<TNodePtr,const char*>(
                i.second,i.first);
            revStringMap.insert(strKey);
        }
        return DynVPackFactory(
            std::move(_map),std::move(revIdxMap),
            std::move(revStringMap)
        );
    }
private:
    bool _isUsed;
    std::mutex _mtx;
    detail::TNodeMapType _map;

    typedef std::lock_guard<std::mutex> Guard;

    void assertUnused() const {
        if (_isUsed) {
            throw DynVpackFactoryBuilderUsedException();
        }
    }
};

}

#endif /* end of include guard: DYNAMICPACKCREATOR_A8GJJ2Z6 */
