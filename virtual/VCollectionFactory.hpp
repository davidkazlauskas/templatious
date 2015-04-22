//==================================================
// Copyright (c) 2015 Deividas Kazlauskas
//
// See the file license.txt for copying permission.
//==================================================

/*
 * =====================================================================================
 *
 *       Filename:  VCollectionFactory.hpp
 *
 *    Description:  Class to assembly collections out of modules
 *
 *        Version:  1.0
 *        Created:  08/24/2014 10:30:47 AM
 *       Compiler:  gcc
 *
 *         Author:  David Kazlauskas (dk), david@templatious.org
 *
 * =====================================================================================
 */

#ifndef VCOLLECTIONFACTORY_4CCSY5Y2
#define VCOLLECTIONFACTORY_4CCSY5Y2

#include <templatious/virtual/Modular.hpp>
#include <templatious/util/TypeMap.hpp>

namespace templatious {

enum SizablePolicy {
    SP_ENABLED = 7,
    SP_FAKE,
    SP_THROW,
    SP_PREVENT
};

enum AccessPolicy {
    ACP_ENABLED = 27,
    ACP_THROW,
    ACP_PREVENT
};

enum AddablePolicy {
    AP_ENABLED = 47,
    AP_FAKE,
    AP_THROW,
    AP_PREVENT
};

enum ClearablePolicy {
    CP_ENABLED = 67,
    CP_FAKE,
    CP_THROW,
    CP_PREVENT
};

enum TraversablePolicy {
    TP_ENABLED = 87,
    TP_FAKE,
    TP_THROW,
    TP_PREVENT
};

template <
    class T,
    template <class> class StoragePolicy,
    int sizablePolicy = SP_ENABLED,
    int accessPolicy = ACP_ENABLED,
    int addablePolicy = AP_ENABLED,
    int clearablePolicy = CP_ENABLED,
    int traversablePolicy = TP_ENABLED
>
struct VCollectionFactory {
    typedef T ThisCol;
    typedef const T ConstCol;

    typedef typename StoragePolicy<ThisCol>::Container Cont;
    typedef templatious::vmodular::Root<ThisCol,Cont> Root;

    typedef templatious::util::NumTypeMap<
        TEMPLATIOUS_PAIR_NT(SP_ENABLED,templatious::vmodular::Sizable<Root>),
        TEMPLATIOUS_PAIR_NT(SP_FAKE,templatious::vmodular::SizableFake<Root>),
        TEMPLATIOUS_PAIR_NT(SP_THROW,templatious::vmodular::SizableThrow<Root>),
        TEMPLATIOUS_PAIR_NT(SP_PREVENT,templatious::vmodular::SizablePrevent<Root>)
        > SzMap;
    typedef typename SzMap::template getType<sizablePolicy,void>::val SzPol;

    typedef templatious::util::NumTypeMap<
        TEMPLATIOUS_PAIR_NT(ACP_ENABLED,templatious::vmodular::Access<SzPol>),
        TEMPLATIOUS_PAIR_NT(ACP_THROW,templatious::vmodular::AccessThrow<SzPol>),
        TEMPLATIOUS_PAIR_NT(ACP_PREVENT,templatious::vmodular::AccessPrevent<SzPol>)
        > AccMap;
    typedef typename AccMap::template getType<accessPolicy,void>::val AccPol;

    typedef templatious::util::NumTypeMap<
        TEMPLATIOUS_PAIR_NT(AP_ENABLED,templatious::vmodular::Addable<AccPol>),
        TEMPLATIOUS_PAIR_NT(AP_FAKE,templatious::vmodular::AddableFake<AccPol>),
        TEMPLATIOUS_PAIR_NT(AP_THROW,templatious::vmodular::AddableThrow<AccPol>),
        TEMPLATIOUS_PAIR_NT(AP_PREVENT,templatious::vmodular::AddablePrevent<AccPol>)
        > AddMap;
    typedef typename AddMap::template getType<addablePolicy,void>::val AddPol;

    typedef templatious::util::NumTypeMap<
        TEMPLATIOUS_PAIR_NT(CP_ENABLED,templatious::vmodular::Clearable<AddPol>),
        TEMPLATIOUS_PAIR_NT(CP_FAKE,templatious::vmodular::ClearableFake<AddPol>),
        TEMPLATIOUS_PAIR_NT(CP_THROW,templatious::vmodular::ClearableThrow<AddPol>),
        TEMPLATIOUS_PAIR_NT(CP_PREVENT,templatious::vmodular::ClearablePrevent<AddPol>)
        > ClMap;
    typedef typename ClMap::template getType<clearablePolicy,void>::val ClPol;

    typedef templatious::util::NumTypeMap<
        TEMPLATIOUS_PAIR_NT(TP_ENABLED,templatious::vmodular::Traversable<ClPol>),
        TEMPLATIOUS_PAIR_NT(TP_FAKE,templatious::vmodular::TraversableFake<ClPol>),
        TEMPLATIOUS_PAIR_NT(TP_THROW,templatious::vmodular::TraversableThrow<ClPol>),
        TEMPLATIOUS_PAIR_NT(TP_PREVENT,templatious::vmodular::TraversablePrevent<ClPol>)
        > TrMap;
    typedef typename TrMap::template getType<traversablePolicy,void>::val TrPol;

    static_assert(!std::is_same<void,SzPol>::value,
            "Could not deduce type from selected sizablePolicy policy. "
            "Make sure to select policy from corresponsing enums.");

    static_assert(!std::is_same<void,AccPol>::value,
            "Could not deduce type from selected accessPolicy policy. "
            "Make sure to select policy from corresponsing enums.");

    static_assert(!std::is_same<void,AddPol>::value,
            "Could not deduce type from selected addablePolicy policy. "
            "Make sure to select policy from corresponsing enums.");

    static_assert(!std::is_same<void,ClPol>::value,
            "Could not deduce type from selected clearablePolicy policy. "
            "Make sure to select policy from corresponsing enums.");

    static_assert(!std::is_same<void,TrPol>::value,
            "Could not deduce type from selected traversablePolicy policy. "
            "Make sure to select policy from corresponsing enums.");

    typedef templatious::vmodular::Tail<TrPol> Type;

    template <class V>
    static Type make(V&& c) {
        return Type(std::forward<V>(c));
    }
};

static const int VCOL_ADD = 1;
static const int VCOL_CLEAR = 2;
static const int VCOL_TRAVERSE = 4;
static const int VCOL_ACCESS = 8;
static const int VCOL_SIZE = 16;

template <
    int bitmask,
    bool allow,
    class T,
    template <class> class StoragePolicy
>
struct BitmaskVCollectionFactory {
    static const bool add = ((bitmask & VCOL_ADD) != 0) != allow;
    static const bool clear = ((bitmask & VCOL_CLEAR) != 0) != allow;
    static const bool traverse = ((bitmask & VCOL_TRAVERSE) != 0) != allow;
    static const bool access = ((bitmask & VCOL_ACCESS) != 0) != allow;
    static const bool size = ((bitmask & VCOL_SIZE) != 0) != allow;

    typedef typename templatious::VCollectionFactory<
        T,StoragePolicy,
        templatious::util::IntSelector<size,SP_PREVENT,SP_ENABLED>::val,
        templatious::util::IntSelector<access,ACP_PREVENT,ACP_ENABLED>::val,
        templatious::util::IntSelector<add,AP_PREVENT,AP_ENABLED>::val,
        templatious::util::IntSelector<clear,CP_PREVENT,CP_ENABLED>::val,
        templatious::util::IntSelector<traverse,TP_PREVENT,TP_ENABLED>::val
    > Maker;

    typedef typename Maker::Type Type;
};

}

#endif /* end of include guard: VCOLLECTIONFACTORY_4CCSY5Y2 */
