/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTL_MULTIMETHODS_HPP
#define XTL_MULTIMETHODS_HPP

#include "xmeta_utils.hpp"

namespace xtl
{
    // Loki's multimethods ported to modern C++
    // Original implementation can be found at
    // https://github.com/snaewe/loki-lib/blob/master/include/loki/MultiMethods.h

    /*********************
     * static_dispatcher *
     *********************/

    template
    <
        class executor,
        class base_lhs,
        class type_lhs,
        bool symmetric = false,
        class return_type = void,
        class base_rhs = base_lhs,
        class type_rhs = type_lhs
    >
    class static_dispatcher
    {
    private:

        template <class lhs_type>
        static return_type dispatch_rhs(lhs_type& lhs,
                                        base_rhs& rhs,
                                        executor& exec,
                                        mpl::vector<>)
        {
            return exec.on_error(lhs, rhs);
        }

        template <class lhs_type, class T, class... U>
        static return_type dispatch_rhs(lhs_type& lhs,
                                        base_rhs& rhs,
                                        executor& exec,
                                        mpl::vector<T, U...>)
        {
            if (T* p = dynamic_cast<T*>(&rhs))
            {
                return exec.run(lhs, *p);
            }
            return dispatch_rhs(lhs, rhs, exec, mpl::vector<U...>());
        }

        static return_type dispatch_lhs(base_lhs& lhs,
                                        base_rhs& rhs,
                                        executor& exec,
                                        mpl::vector<>)
        {
            return exec.on_error(lhs, rhs);
        }

        template <class T, class... U>
        static return_type dispatch_lhs(base_lhs& lhs,
                                        base_rhs& rhs,
                                        executor& exec,
                                        mpl::vector<T, U...>)
        {
            if (T* p = dynamic_cast<T*>(&lhs))
            {
                return dispatch_rhs(*p, rhs, exec, type_rhs());
            }
            return dispatch_lhs(lhs, rhs, exec, mpl::vector<U...>());
        }

    public:

        static return_type dispatch(base_lhs& lhs, base_rhs& rhs, executor& exec)
        {
            return dispatch_lhs(lhs, rhs, exec, type_lhs());
        }
    };
}

#endif

