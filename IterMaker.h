/*
 * =====================================================================================
 *
 *       Filename:  IterMaker.h
 *
 *    Description:  Iterator maker
 *
 *        Version:  1.0
 *        Created:  05/25/2014 10:12:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef ITERMAKER_W1WSUFYD
#define ITERMAKER_W1WSUFYD

#include <templatious/util/Pointerizer.h>
#include <templatious/tuple/RecursiveIter.h>
#include <templatious/TupleCaller.h>
#include <templatious/util/Functions.h>

namespace templatious {
namespace recursive {

struct IteratorMaker {
   private:

    template <class... Args>
    static auto simpleMakeIter(Args... args) -> RecursiveIterator<Args...> {
        return RecursiveIterator<Args...>(args...);
    }

    template <class... Args>
    static auto tupleArgumentDeductor(std::tuple<Args...>& tpl) -> RecursiveIterator<Args...> {
        namespace ut = templatious::util;
        return call_tuple(tpl,ut::make_function(simpleMakeIter<Args...>));
    }

   public:
    //template <class... Args>
    //static auto makeIter(Args & ... args)
        //-> decltype(tupleArgumentDeductor(Pointerizer<Args...>(args...)
                                              //.getTuple())) 
    //{
        //Pointerizer<Args...> ptr_maker(args...);
        //auto tpl = ptr_maker.getTuple();
        //return tupleArgumentDeductor(tpl);
    //}

    template <class... Args>
    static auto makeIter(Args & ... args)
    {
        return RecursiveIterator<Args...>(args...);
    }
};

}
}

#endif /* end of include guard: ITERMAKER_W1WSUFYD */

