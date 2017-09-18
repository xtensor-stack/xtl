/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTL_CLOSURE_HPP
#define XTL_CLOSURE_HPP

#include <type_traits>
#include <utility>

#include "xtl_config.hpp"

namespace xtl
{
    /***********
     * closure *
     ***********/

    template <class S>
    struct closure_type
    {
        using underlying_type = std::conditional_t<std::is_const<std::remove_reference_t<S>>::value,
                                                   const std::decay_t<S>,
                                                   std::decay_t<S>>;
        using type = typename std::conditional<std::is_lvalue_reference<S>::value,
                                               underlying_type&,
                                               underlying_type>::type;
    };

    template <class S>
    using closure_type_t = typename closure_type<S>::type;

    template <class S>
    struct const_closure_type
    {
        using underlying_type = const std::decay_t<S>;
        using type = typename std::conditional<std::is_lvalue_reference<S>::value,
                                               underlying_type&,
                                               underlying_type>::type;
    };

    template <class S>
    using const_closure_type_t = typename const_closure_type<S>::type;

    /***************
     * ptr_closure *
     ***************/

    template <class S>
    struct ptr_closure_type
    {
        using underlying_type = std::conditional_t<std::is_const<std::remove_reference_t<S>>::value,
                                                   const std::decay_t<S>,
                                                   std::decay_t<S>>;
        using type = std::conditional_t<std::is_lvalue_reference<S>::value,
                                        underlying_type*,
                                        underlying_type>;
    };

    template <class S>
    using ptr_closure_type_t = typename ptr_closure_type<S>::type;

    template <class S>
    struct const_ptr_closure_type
    {
        using underlying_type = const std::decay_t<S>;
        using type = std::conditional_t<std::is_lvalue_reference<S>::value,
                                        underlying_type*,
                                        underlying_type>;
    };

    template <class S>
    using const_ptr_closure_type_t = typename const_ptr_closure_type<S>::type;
}

#endif
