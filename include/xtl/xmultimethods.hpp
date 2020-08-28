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
        class lhs_type_list,
        bool symmetric = false,
        class return_type = void,
        class base_rhs = base_lhs,
        class rhs_type_list = lhs_type_list
    >
    class static_dispatcher
    {
    private:

        template <class lhs_type, class rhs_type>
        static return_type invoke_executor(lhs_type& lhs,
                                           rhs_type& rhs,
                                           executor& exec,
                                           std::false_type)
        {
            return exec.run(lhs, rhs);
        }

        template <class lhs_type, class rhs_type>
        static return_type invoke_executor(lhs_type& lhs,
                                           rhs_type& rhs,
                                           executor& exec,
                                           std::true_type)
        {
            return exec.run(rhs, lhs);
        }

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
                constexpr size_t lhs_index = mpl::index_of<lhs_type_list, lhs_type>::value;
                constexpr size_t rhs_index = mpl::index_of<rhs_type_list, T>::value;

                using invoke_flag = std::integral_constant<bool,
                    symmetric && (rhs_index < lhs_index)>;
                return invoke_executor(lhs, *p, exec, invoke_flag());
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
                return dispatch_rhs(*p, rhs, exec, rhs_type_list());
            }
            return dispatch_lhs(lhs, rhs, exec, mpl::vector<U...>());
        }

    public:

        static return_type dispatch(base_lhs& lhs, base_rhs& rhs, executor& exec)
        {
            return dispatch_lhs(lhs, rhs, exec, lhs_type_list());
        }
    };
}

#endif

