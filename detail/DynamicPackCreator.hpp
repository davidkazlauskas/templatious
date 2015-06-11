//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

#ifndef DYNAMICPACKCREATOR_A8GJJ2Z6
#define DYNAMICPACKCREATOR_A8GJJ2Z6

#include <string>
#include <unordered_map>
#include <templatious/detail/VirtualPack.hpp>

namespace templatious {
namespace detail {

// std::align would be used but g++ 4.8 doesn't have it...
void* ptrAlign(size_t align,size_t size,void* ptr) {
    char* p1 = static_cast<char*>(ptr);
    char* p2 = reinterpret_cast<char*>(
        reinterpret_cast<size_t>(p1 + (align - 1)) & -align);
    return p2;
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
};

typedef const TypeNode* TNodePtr;

struct NodeFuncNull {
    // do nothing
    template <class... T>
    void operator()(T&&... t) const {}
};

template <class Construct,class Destroy,class Format>
struct NodeFuncUtil {
    static const bool consEmpty = std::is_empty<Construct>::value;
    static const bool destEmpty = std::is_empty<Destroy>::value;
    static const bool formEmpty = std::is_empty<Format>::value;

    static_assert(consEmpty,"Construction function has to have no members.");
    static_assert(destEmpty,"Destruction function has to have no members.");
    static_assert(formEmpty,"Format function has to have no members.");

    static const bool cons = !std::is_same<NodeFuncNull,Construct>::value;
    static const bool dest = !std::is_same<NodeFuncNull,Destroy>::value;
    static const bool form = !std::is_same<NodeFuncNull,Format>::value;

    typedef typename std::conditional<
        cons, Construct, NodeFuncNull >::type ConstructRes;
    typedef typename std::conditional<
        dest, Destroy, NodeFuncNull >::type DestroyRes;
    typedef typename std::conditional<
        form, Format, NodeFuncNull >::type FormatRes;

    template <class C,class D,class F>
    static void inst(C&& c,D&& d,F&& f) {
        static volatile bool isSet = false;
        static std::mutex mtx;
        typedef std::lock_guard<std::mutex> Guard;

        if (isSet) return;
        Guard g(mtx);
        if (isSet) return;

        new (_consBuf) C(std::forward<C>(c));
        new (_destBuf) D(std::forward<D>(d));
        new (_formBuf) F(std::forward<F>(f));

        _cons = reinterpret_cast<ConstructRes*>(_consBuf);
        _dest = reinterpret_cast<DestroyRes*>(_destBuf);
        _form = reinterpret_cast<FormatRes*>(_formBuf);

        isSet = true;
    }

    template <class... T>
    static void construct(T&&... t) {
        _cons->operator()(std::forward<T>(t)...);
    }

    template <class... T>
    static void destroy(T&&... t) {
        _dest->operator()(std::forward<T>(t)...);
    }

    template <class... T>
    static void format(T&&... t) {
        _form->operator()(std::forward<T>(t)...);
    }

private:
    typedef typename std::aligned_storage<
        sizeof(ConstructRes),alignof(ConstructRes)>::type StorCons;
    typedef typename std::aligned_storage<
        sizeof(DestroyRes),alignof(DestroyRes)>::type StorDest;
    typedef typename std::aligned_storage<
        sizeof(FormatRes),alignof(FormatRes)>::type StorForm;
    static StorCons _consBuf[1];
    static StorDest _destBuf[1];
    static StorForm _formBuf[1];

    static const ConstructRes* _cons;
    static const DestroyRes* _dest;
    static const FormatRes* _form;
};

// .. so, all this work just so llvm
// undefined behaviour sanitizer wouldn't
// complain when you call () operator on
// null pointer empty lambda?
template <class Construct,class Destroy,class Format>
const typename NodeFuncUtil<Construct,Destroy,Format>::ConstructRes*
    NodeFuncUtil<Construct,Destroy,Format>::_cons = nullptr;
template <class Construct,class Destroy,class Format>
const typename NodeFuncUtil<Construct,Destroy,Format>::DestroyRes*
    NodeFuncUtil<Construct,Destroy,Format>::_dest = nullptr;
template <class Construct,class Destroy,class Format>
const typename NodeFuncUtil<Construct,Destroy,Format>::FormatRes*
    NodeFuncUtil<Construct,Destroy,Format>::_form = nullptr;

template <class Construct,class Destroy,class Format>
typename NodeFuncUtil<Construct,Destroy,Format>::StorCons
    NodeFuncUtil<Construct,Destroy,Format>::_consBuf[1] = {};

template <class Construct,class Destroy,class Format>
typename NodeFuncUtil<Construct,Destroy,Format>::StorDest
    NodeFuncUtil<Construct,Destroy,Format>::_destBuf[1] = {};

template <class Construct,class Destroy,class Format>
typename NodeFuncUtil<Construct,Destroy,Format>::StorForm
    NodeFuncUtil<Construct,Destroy,Format>::_formBuf[1] = {};

template <class T,class Func>
struct PodType : public TypeNode {
    typedef PodType<T,Func> ThisFact;
    typedef typename std::remove_reference<T>::type DecType;

    static const bool isPod = std::is_pod<T>::value;
    static const bool isRaw = std::is_same<DecType,T>::value;
    static_assert(isPod,"Only plain old datatypes (POD's) can"
            " be template parameters for this factory.");
    static_assert(isRaw,"Only raw types, without reference"
            " qualifiers can be used as template argument.");

    template <class C,class F>
    static TNodePtr sInst(C&& c,F&& f) {
        Func::inst(
            std::forward<C>(c),
            NodeFuncNull(),
            std::forward<F>(f)
        );
        return &_inst;
    };

    template <class C,class D,class F>
    static TNodePtr sInst(C&& c,D&& d,F&& f) {
        Func::inst(
            std::forward<C>(c),
            std::forward<D>(d),
            std::forward<F>(f)
        );
        return &_inst;
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

    std::type_index type() const {
        return std::type_index(typeid(T));
    }

    void construct(void* ptr,const char* param) const override {
        Func::construct(ptr,param);
    }

    void destroy(void* ptr) const override {
        Func::destroy(ptr);
    }

    void toString(const void* ptr,std::string& str) const override {
        Func::format(ptr,str);
    }
private:
    static const ThisFact _inst;
    // function uniquely identified and inlined by type
    // lambda should not capture anything
};

template <class T,class Func>
struct PlainObject : public TypeNode {
    typedef PlainObject<T,Func> ThisFact;
    typedef typename std::remove_reference<T>::type DecType;

    static const bool isPolymorphic = std::is_polymorphic<T>::value;
    static const bool isRaw = std::is_same<DecType,T>::value;
    static_assert(!isPolymorphic,"Class used with this factory"
            " cannot be polymorphic.");
    static_assert(isRaw,"Only raw types, without reference"
            " qualifiers can be used as template argument.");

    template <class C,class D,class F>
    static TNodePtr sInst(C&& c,D&& d,F&& f) {
        Func::inst(
            std::forward<C>(c),
            std::forward<D>(d),
            std::forward<F>(f)
        );
        return &_inst;
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

    std::type_index type() const {
        return std::type_index(typeid(T));
    }

    void construct(void* ptr,const char* param) const override {
        Func::construct(ptr,param);
    }

    void destroy(void* ptr) const override {
        Func::destroy(ptr);
    }

    void toString(const void* ptr,std::string& str) const override {
        Func::format(ptr,str);
    }
private:
    static const ThisFact _inst;
    // function uniquely identified and inlined by type
    // lambda should not capture anything
};

template <class T,class Func>
const PodType<T,Func> PodType<T,Func>::_inst = ThisFact();

template <class T,class Func>
const PlainObject<T,Func> PlainObject<T,Func>::_inst = ThisFact();

// Dynamic virtual pack traits
struct DynPackCountTrait : public VirtualPack {

    void incCount() const {
        ++_count;
    }
private:
    mutable int _count;
};

struct DynamicVirtualPackCore {

    DynamicVirtualPackCore() = delete;
    DynamicVirtualPackCore(const DynamicVirtualPackCore&) = delete;
    DynamicVirtualPackCore(DynamicVirtualPackCore&&) = delete;

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
            for (int i = 0; i < _used; ++i) {
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

    template <int packBitmask,class CallbackType>
    friend struct DynamicVirtualPack;

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

    ~DynamicVirtualPackCore() {
        void* buf[32];
        dumpOffsets(buf);
        TNodePtr* nodes = nodeArr();
        for (int i = 0; i < _size; ++i) {
            nodes[i]->destroy(buf[i]);
        }
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
        size_t space = 1024;
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

}

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

    int useCount() const override {
        return getTraits().getCount();
    }

    void wait() const {
        getTraits().wait();
    }

    void waitMs(int ms) const {
        getTraits().waitMs(ms);
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

    struct SingleCoreContainer {
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

    struct TraitedContainer {
        template <class Func>
        TraitedContainer(int size,TNodePtr* types,const char* values[],Func&& f)
            : _traits(std::forward<Func>(f)), _core(size,types,values) {}

        const FinalTraits& getTraits() const {
            return _traits;
        }

        FinalTraits _traits;
        DynamicVirtualPackCore _core;
    };

    static const bool isEmpty = std::is_empty<FinalTraits>::value;
    typedef typename std::conditional<
        isEmpty,
        SingleCoreContainer,
        TraitedContainer
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

#ifndef TEMPLATIOUS_TESTING
private:
#endif
    static size_t getHash(int size,TNodePtr* types) {
        return DynamicVirtualPackCore::calcHash(size,types);
    }

    // the true type signature expected is
    // [possible callback],int size,TNodePtr* types,const char* values
    template <class Func>
    DynamicVirtualPack(int size,TNodePtr* types,const char* values[],Func&& f)
        : VirtualPack(getHash(size,types)),
          _cont(size,types,values,std::forward<Func>(f)) {}

    Container _cont;
};

struct TypeNodeFactory {
    template <class T,class Construct,class Format>
    static TNodePtr makePodNode(Construct&& c,Format&& f) {
        typedef typename std::decay<Construct>::type CDec;
        typedef typename std::decay<Format>::type FDec;
        typedef NodeFuncUtil<CDec,NodeFuncNull,FDec> FUtil;
        return PodType<T,FUtil>::sInst(
            std::forward<Construct>(c),
            std::forward<Format>(f));
    }

    template <class T,class Construct,class Destroy,class Format>
    static TNodePtr makeFullNode(Construct&& c,Destroy&& d,Format&& f) {
        typedef typename std::decay<Construct>::type CDec;
        typedef typename std::decay<Destroy>::type DDec;
        typedef typename std::decay<Format>::type FDec;
        typedef NodeFuncUtil<CDec,DDec,FDec> FUtil;
        return PlainObject<T,FUtil>::sInst(
            std::forward<Construct>(c),
            std::forward<Destroy>(d),
            std::forward<Format>(f));
    }
};

TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVPackFactoryNoTypeException,
    "Type is not indexed in the dictionary." );
TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVPackFactoryTypeLimitException,
    "Maximum type limit is 32." );
TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVPackFactoryIncorrectSizeException,
    "Minimum size of a pack is 1." );

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

        typedef DynamicVirtualPack<FLAGS,Fwd> Signature;

        template <class CustCallback>
        static auto make(
            const DynVPackFactory& fact,
            int size,const char** keys,const char** values,
            CustCallback&& c
        )
         -> std::shared_ptr<
                DynamicVirtualPack<
                    FLAGS,
                    Fwd
                >
            >
        const {
            TNodePtr arr[TYPE_LIMIT];

            if (size <= 0) {
                throw DynVPackFactoryTypeLimitException();
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

    DynVPackFactory(std::unordered_map<const char*,TNodePtr>&& map)
        : _map(std::move(map)) {}

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
    auto makePackWCallback(int size,const char** keys,const char** values,Callback&& c)
     -> decltype(
            DynPackMakerGeneric<
                templatious::VPACK_COUNT,
                decltype(std::forward<Callback>(c))
            >::make(*this,size,keys,values,std::forward<Callback>(c))
        )
    const {
        return DynPackMakerGeneric<
            templatious::VPACK_COUNT,
            decltype(std::forward<Callback>(c))
        >::make(*this,size,keys,values,std::forward<Callback>(c));
    }

    template <int flags,class Callback>
    auto makePackCustomWCallback(int size,const char** keys,const char** values,Callback&& c)
     -> decltype(
            DynPackMakerGeneric<
                flags,
                decltype(std::forward<Callback>(c))
            >::make(*this,size,keys,values,std::forward<Callback>(c))
        )
    const {
        return DynPackMakerGeneric<
            flags,
            decltype(std::forward<Callback>(c))
        >::make(*this,size,keys,values,std::forward<Callback>(c));
    }

private:
    // this should be immutable and set in stone.
    std::unordered_map<const char*,TNodePtr> _map;
};

TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVpackFactoryBuilderKeyExistsException,
    "Key already taken in the map." );
TEMPLATIOUS_BOILERPLATE_EXCEPTION( DynVpackFactoryBuilderUsedException,
    "Builder was already used." );

struct DynVPackFactoryBuilder {
    DynVPackFactoryBuilder() : _isUsed(false) {}

    void attachNode(const char* key,TNodePtr value) {
        Guard g(_mtx);

        auto search = _map.find(key);
        if (search != _map.end()) {
            throw DynVpackFactoryBuilderKeyExistsException();
        }

        _map.insert(std::make_pair(key,value));
    }

    DynVPackFactory getFactory() {
        return DynVPackFactory(std::move(_map));
    }
private:
    bool _isUsed;
    std::mutex _mtx;
    std::unordered_map<const char*,TNodePtr> _map;

    typedef std::lock_guard<std::mutex> Guard;

    void assertUnused() const {
        if (_isUsed) {
            throw DynVpackFactoryBuilderUsedException();
        }
    }
};

}
}

#endif /* end of include guard: DYNAMICPACKCREATOR_A8GJJ2Z6 */
