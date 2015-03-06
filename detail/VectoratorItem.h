/*
 * =====================================================================================
 *
 *       Filename:  VectoratorItem.h
 *
 *    Description:  Vectorator item
 *
 *        Version:  1.0
 *        Created:  2015.02.18 17:02:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef VECTORATORITEM_ILR9BGBD
#define VECTORATORITEM_ILR9BGBD

#include <templatious/util/Container.h>
#include <templatious/util/Selectors.h>
#include <templatious/CollectionAdapter.h>
#include <templatious/CollectionMaker.h>

namespace templatious {
namespace detail {

template <class T,bool saveIndex = false>
struct VectoratorItem {
    typedef VectoratorItem<T,saveIndex> ThisVectorator;

    struct IndexNode {
        IndexNode(const T& t,const long& idx) :
            _t(t), _idx(idx) {}

        T _t;
        long _idx;
    };

    typedef typename std::conditional<
        !saveIndex,
        T,
        IndexNode
    >::type SavedItem;

    typedef typename templatious::util::
        CopyContainer<SavedItem> Cont;

    ThisVectorator& operator=(const ThisVectorator& i) {
        return this->assign(i);
    }

    // disambiguity sometimes matters
    ThisVectorator& assign(const ThisVectorator& i) {
        templatious::util::CopyContainerAccess::
            mutate(_c,i.copyRef());
        return *this;
    }

    VectoratorItem& operator=(ThisVectorator&& i) {
        using namespace templatious::util;
        CopyContainerAccess::mutate(
            _c,std::move(
                CopyContainerAccess::move(i._c)));
        return *this;
    }

    template <class V>
    VectoratorItem& operator=(V&& v) {
        *iRef() = std::forward<V>(v);
        return *this;
    }

    bool operator==(const ThisVectorator& rhs) const {
        return iCRef() == rhs.iCRef();
    }

    bool operator!=(const ThisVectorator& rhs) const {
        return !(*this == rhs);
    }

    struct RawRef {
        template <class Node>
        static auto ref(Node&& t)
         -> decltype(t.getRef())
        {
            return t.getRef();
        }

        template <class Node>
        static auto cref(const Node& t)
         -> decltype(t.cgetRef())
        {
            return t.cgetRef();
        }

        template <class Node>
        static long idx(const Node& t)
        {
            return -1;
        }

        template <class... Fwd>
        static T fwdCtor(Fwd&&... args) {
            return T(std::forward<Fwd>(args)...);
        }
    };

    struct CompRef {
        template <class Node>
        static auto ref(Node&& t)
         -> decltype(t.getRef()._t)&
        {
            return t.getRef()._t;
        }

        template <class Node>
        static auto cref(const Node& t)
         -> const decltype(t.cgetRef()._t)&
        {
            return t.cgetRef()._t;
        }

        template <class Node>
        static long idx(const Node& t)
        {
            return t.cgetRef()._idx;
        }

        template <class... Fwd>
        static IndexNode fwdCtor(Fwd&&... args) {
            return IndexNode(std::forward<Fwd>(args)...);
        }
    };

    typedef typename std::conditional<
        saveIndex, CompRef, RawRef
    >::type RStrat;

    VectoratorItem() = delete;

    template <class Dummy = void>
    VectoratorItem(const T& i) : _c(RStrat::fwdCtor(i)) {
        static_assert(
            templatious::util::DummyResolver<Dummy,!saveIndex>::val,
            "This constructor should be used only if VectoratorItem"
            " does not save index."
        );
    }

    template <class Dummy = void>
    VectoratorItem(const T& i,long idx) :
        _c(RStrat::fwdCtor(i,idx))
    {
        static_assert(
            templatious::util::DummyResolver<Dummy,saveIndex>::val,
            "This constructor should be used only if VectoratorItem"
            " does saves index."
        );
    }

    VectoratorItem(const ThisVectorator& tv)
        : _c(tv.copyRef()) {}

    auto iRef()
     -> decltype(RStrat::ref(
         std::declval<Cont>()
     ))
    {
        return RStrat::ref(_c);
    }

    auto iCRef() const
     -> decltype(RStrat::cref(
         std::declval<Cont>()
     ))
    {
        return RStrat::cref(_c);
    }

    const SavedItem& copyRef() const {
        return _c.cgetRef();
    }

    template <class Dummy = void>
    long index() const
    {
        static_assert(templatious::util
            ::DummyResolver<Dummy,saveIndex>::val,
            "To use this method VectoratorItem has"
            " to contain index.");
        return RStrat::idx(_c);
    }

    typedef decltype(*RStrat::ref(std::declval<Cont>())) Copy;
    typedef decltype(*RStrat::cref(std::declval<Cont>())) ConstCopy;
    // implicit conversion to value type
    operator Copy() { return *iRef(); }
    operator ConstCopy() const { return *iCRef(); }

    auto operator*()
     -> decltype(*(std::declval<ThisVectorator>().iRef()))
    {
        return *iRef();
    }

    auto operator*() const
     -> decltype(*std::declval<ThisVectorator>().iCRef())
    {
        return *iCRef();
    }

    auto iter()
     -> decltype(std::declval<ThisVectorator>().iRef())
    {
        return iRef();
    }

    auto citer() const
     -> decltype(std::declval<ThisVectorator>().iCRef())
    {
        return iCRef();
    }

    auto operator->()
     -> decltype(
         std::addressof(std::declval<ThisVectorator>().iRef())
     )
    {
        return std::addressof(iRef());
    }

    auto operator->() const
     -> decltype(
         std::addressof(std::declval<ThisVectorator>().iCRef())
     )
    {
        return std::addressof(iCRef());
    }

    ThisVectorator& operator++()
    {
        ++iRef();
        return *this;
    }

    ThisVectorator& operator--()
    {
        --iRef();
        return *this;
    }

    ThisVectorator& operator+=(std::ptrdiff_t n) {
        iRef() += n;
        return *this;
    }

    ThisVectorator& operator-=(std::ptrdiff_t n) {
        iRef() -= n;
        return *this;
    }

    ThisVectorator operator+(std::ptrdiff_t n) const {
        return iCRef() + n;
    }

    ThisVectorator operator-(std::ptrdiff_t n) const {
        return iCRef() - n;
    }

    bool operator<(const ThisVectorator& rhs) const
    {
        return *iCRef() < *rhs.iCRef();
    }

    bool operator>=(const ThisVectorator& rhs) const
    {
        return !(*this < rhs);
    }

    bool operator>(const ThisVectorator& rhs) const
    {
        return *iCRef() > *rhs.iCRef();
    }

    bool operator<=(const ThisVectorator& rhs) const
    {
        return !(*this > rhs);
    }

private:
    Cont _c;
};

template <
    class T,
    bool saveIdx,
    template <class...> class OutCol,
    template <class> class Alloc
>
struct CollectionIterDumper {
    typedef templatious::adapters::
        CollectionAdapter<T> Ad;
    typedef typename Ad::Iterator IteratorType;
    typedef VectoratorItem< IteratorType, saveIdx > ValueType;
    typedef templatious::adapters::CollectionMaker<
        ValueType,OutCol,Alloc
    > Maker;
    typedef typename Maker::Collection Collection;
    typedef typename templatious::adapters::CollectionAdapter<
        Collection
    > OutAd;

    struct WIndexStrat {
        template <class Fwd>
        static ValueType makeIter(Fwd&& t,long idx) {
            return ValueType(std::forward<Fwd>(t),idx);
        }
    };

    struct WoIndexStrat {
        template <class Fwd>
        static ValueType makeIter(Fwd&& t,long idx) {
            return ValueType(std::forward<Fwd>(t));
        }
    };

    typedef typename std::conditional<
        saveIdx, WIndexStrat, WoIndexStrat
    >::type AddStrat;

    template <class C>
    static Collection dumpIter(C&& c) {
        long sz = Ad::size(std::forward<C>(c));
        long idx = 0;
        if (sz > 0) {
            auto res = Maker::make(sz);
            auto beg = Ad::begin(std::forward<C>(c));
            auto end = Ad::end(std::forward<C>(c));
            while (beg != end) {
                OutAd::add(res,
                    AddStrat::makeIter(beg,idx)
                );
                ++beg;
                ++idx;
            }
            return std::move(res);
        } else {
            auto res = Maker::make();
            auto beg = Ad::begin(std::forward<C>(c));
            auto end = Ad::end(std::forward<C>(c));
            while (beg != end) {
                OutAd::add(res,
                    AddStrat::makeIter(beg,idx)
                );
                ++beg;
                ++idx;
            }
            return std::move(res);
        }
    }
};

struct VectoratorAlgs {
    template <class T,class U>
    static void vectoratorSort(T& t,U& u) {
        typedef templatious::adapters::CollectionAdapter<T> AdT;
        typedef templatious::adapters::CollectionAdapter<U> AdU;

        auto b = AdT::begin(t);
        auto i = b;
        auto e = AdT::end(t);
        auto wrappedEnd = templatious::detail::
            VectoratorItem<decltype(e)>(e);

        long sz = AdT::size(t);
        long consistent = 0;
        while (i != e) {
            // should be O(1) complexity because
            // u is assumed to be random access
            //auto bit = AdU::iterAt(u,cnt);
            auto bit = AdU::iterAt(u,std::distance(b,i));
            if ((*bit).iter() != i && (*bit).iter() != e) {
                auto saved = std::move(*i);
                *i = *(*bit);
                ++consistent;
                for (;;) {
                    auto dist = std::distance(b,(*bit).iter());
                    auto oldBit = bit;
                    bit = AdU::iterAt(u,dist);
                    if ((*bit).iter() != i) {
                        *(*oldBit) = std::move(*(*bit));
                        ++consistent;
                    } else {
                        *(*oldBit) = saved;
                        (*bit).assign(wrappedEnd);
                        (*oldBit).assign(wrappedEnd);
                        ++consistent;
                        break;
                    }
                    (*oldBit).assign(wrappedEnd);
                }
            } else if ((*bit).iter() == i) {
                ++consistent;
            }
            // if there's one element left unsorted
            // it must be in the right place
            if (consistent >= sz - 1) return;
            ++i;
        }
    }

    template <class T,class U,class EndIter>
    static void vectoratorSortExp(T& t,U& u,EndIter&& ei) {
        typedef templatious::adapters::CollectionAdapter<T> AdT;
        typedef templatious::adapters::CollectionAdapter<U> AdU;

        auto b = AdT::begin(t);
        auto i = b;
        auto e = AdT::end(t);
        auto wrappedEnd = templatious::detail::
            VectoratorItem<typename AdT::ValueType,true>(
                std::forward<EndIter>(ei),0);

        long sz = AdT::size(t);
        long consistent = 0;
        long cnt = 0;
        while (i != e) {
            // should be O(1) complexity because
            // u is assumed to be random access
            //auto bit = AdU::iterAt(u,cnt);
            auto bit = AdU::iterAt(u,std::distance(b,i));
            if ((*bit).iter() != *i && (*bit).iter() != ei) {
                auto saved = std::move(**i);
                **i = *(*bit);
                ++consistent;
                for (;;) {
                    //auto dist = std::distance(b,(*bit).iter());
                    auto dist = (*bit).index();
                    auto oldBit = bit;
                    bit = AdU::iterAt(u,dist);
                    if ((*bit).iter() != *i) {
                        *(*oldBit) = std::move(*(*bit));
                        ++consistent;
                    } else {
                        *(*oldBit) = saved;
                        (*bit).assign(wrappedEnd);
                        (*oldBit).assign(wrappedEnd);
                        ++consistent;
                        break;
                    }
                    (*oldBit).assign(wrappedEnd);
                }
            } else if ((*bit).iter() == *i) {
                ++consistent;
            }
            // if there's one element left unsorted
            // it must be in the right place
            if (consistent >= sz - 1) return;
            ++i;
            ++cnt;
        }
    }
};

}
}

#endif /* end of include guard: VECTORATORITEM_ILR9BGBD */
