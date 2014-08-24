/*
 * =====================================================================================
 *
 *       Filename:  VCollectionFactory.h
 *
 *    Description:  Class to assembly collections out of modules
 *
 *        Version:  1.0
 *        Created:  08/24/2014 10:30:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef VCOLLECTIONFACTORY_4CCSY5Y2
#define VCOLLECTIONFACTORY_4CCSY5Y2

#include <templatious/virtual/Modular.h>
#include <templatious/util/Selectors.h>

namespace templatious {

enum SizablePolicy {
    SP_ENABLED = 7,
    SP_FAKE,
    SP_THROW
};

enum AccessPolicy {
    ACP_ENABLED = 27,
    ACP_THROW
};

enum AddablePolicy {
    AP_ENABLED = 47,
    AP_FAKE,
    AP_THROW
};

enum ClearablePolicy {
    CP_ENABLED = 67,
    CP_FAKE,
    CP_THROW
};

enum TraversablePolicy {
    TP_ENABLED = 87,
    TP_FAKE,
    TP_THROW
};

template <
    class T,
    bool copy = false,
    int sizablePolicy = SP_ENABLED,
    int accessPolicy = ACP_ENABLED,
    int addablePolicy = AP_ENABLED,
    int clearablePolicy = CP_ENABLED,
    int traversablePolicy = TP_ENABLED
>
struct VCollectionFactory {
    typedef T ThisCol;
    typedef const T ConstCol;

    typedef typename templatious::util::TypeSelector<
        copy,
        templatious::vmodular::Root<
            ThisCol,templatious::util::CopyContainer>,
        templatious::vmodular::Root<
            ThisCol,templatious::util::RefContainer>
    >::val Root;

    typedef typename templatious::util::TypeSelector<
        sizablePolicy == SP_ENABLED,
        templatious::vmodular::Sizable<Root>,
        typename templatious::util::TypeSelector<sizablePolicy == SP_FAKE,
            templatious::vmodular::SizableFake<Root>,
            typename templatious::util::TypeSelector<sizablePolicy == SP_THROW,
                typename templatious::vmodular::SizableThrow<Root>,
                void
            >::val
        >::val
    >::val SzPol;

    typedef typename templatious::util::TypeSelector<
        accessPolicy == ACP_ENABLED,
        templatious::vmodular::Access<SzPol>,
        typename templatious::util::TypeSelector<accessPolicy == ACP_THROW,
            templatious::vmodular::AccessThrow<SzPol>,
            void
        >::val
    >::val AccPol;

    typedef typename templatious::util::TypeSelector<
        addablePolicy == AP_ENABLED,
        templatious::vmodular::Addable<AccPol>,
        typename templatious::util::TypeSelector<addablePolicy == AP_FAKE,
            templatious::vmodular::AddableFake<AccPol>,
            typename templatious::util::TypeSelector<addablePolicy == AP_THROW,
                typename templatious::vmodular::AddableThrow<AccPol>,
                void
            >::val
        >::val
    >::val AddPol;

    typedef typename templatious::util::TypeSelector<
        clearablePolicy == CP_ENABLED,
        templatious::vmodular::Clearable<AddPol>,
        typename templatious::util::TypeSelector<clearablePolicy == CP_FAKE,
            templatious::vmodular::ClearableFake<AddPol>,
            typename templatious::util::TypeSelector<clearablePolicy == CP_THROW,
                typename templatious::vmodular::ClearableThrow<AddPol>,
                void
            >::val
        >::val
    >::val ClPol;

    typedef typename templatious::util::TypeSelector<
        traversablePolicy == TP_ENABLED,
        templatious::vmodular::Traversable<ClPol>,
        typename templatious::util::TypeSelector<traversablePolicy == TP_FAKE,
            templatious::vmodular::TraversableFake<ClPol>,
            typename templatious::util::TypeSelector<traversablePolicy == TP_THROW,
                typename templatious::vmodular::TraversableThrow<ClPol>,
                void
            >::val
        >::val
    >::val TrPol;

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
            "Could not deduce type from selected addablePolicy policy. "
            "Make sure to select policy from corresponsing enums.");

    static_assert(!std::is_same<void,TrPol>::value,
            "Could not deduce type from selected addablePolicy policy. "
            "Make sure to select policy from corresponsing enums.");

    typedef templatious::vmodular::Tail<TrPol> Type;

    static Type make(ThisCol& c) {
        return Type(c);
    }
};


}

#endif /* end of include guard: VCOLLECTIONFACTORY_4CCSY5Y2 */
