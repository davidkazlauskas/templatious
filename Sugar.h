/*
 * =====================================================================================
 *
 *       Filename:  Sugar.h
 *
 *    Description:  Syntax sugar
 *
 *        Version:  1.0
 *        Created:  07/07/2014 07:13:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Deividas Kazlauskas (dk), ne-bobu@reikalas.lt
 *        Company:  Company and shit
 *
 * =====================================================================================
 */

#ifndef SUGAR_RISZR4GH
#define SUGAR_RISZR4GH

#include <templatious/CollectionAdapter.h>

namespace templatious {

#define TEMPLATIOUS_FOREACH(var,col) \
    for (auto _tmp_i = templatious::StaticAdapter::begin(col);      \
            _tmp_i != templatious::StaticAdapter::end(col);         \
            ++_tmp_i)                                               \
        for (var = *_tmp_i; ; ({break;}))

}

#endif /* end of include guard: SUGAR_RISZR4GH */
