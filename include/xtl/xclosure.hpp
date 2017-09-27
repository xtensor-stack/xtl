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
    /****************
     * closure_type *
     ****************/

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

    /****************************
     * ptr_closure_closure_type *
     ****************************/

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

    /*******************
     * closure_wrapper *
     *******************/

    template <class CT>
    class closure_wrapper
    {
    public:

        using value_type = std::decay_t<CT>;
        using closure_type = CT;
        using self_type = closure_wrapper<CT>;

        using reference = std::conditional_t<
          std::is_const<std::remove_reference_t<CT>>::value,
          const value_type&, value_type&
        >;

        closure_wrapper(value_type&& e);
        closure_wrapper(reference e);

        template <class T>
        self_type& operator=(T&&);

        operator closure_type() noexcept;
        closure_type get() & noexcept;
        closure_type get() const & noexcept;
        closure_type get() && noexcept;        

        bool equal(const self_type& rhs) const;

    private:

        using storing_type = xtl::ptr_closure_type_t<CT>;
        storing_type m_wrappee;

        template <class T>
        std::enable_if_t<std::is_pointer<T>::value, std::add_lvalue_reference_t<std::remove_pointer_t<T>>>
        deref(T val) const;

        template <class T>
        std::enable_if_t<!std::is_pointer<T>::value, std::add_lvalue_reference_t<T>>
        deref(T& val) const;

        template <class T, class CTA>
        std::enable_if_t<std::is_pointer<T>::value, T>
        get_storage_init(CTA&& e) const;

        template <class T, class CTA>
        std::enable_if_t<!std::is_pointer<T>::value, T>
        get_storage_init(CTA&& e) const;
    };

    template <class CT>
    inline closure_wrapper<CT>::closure_wrapper(value_type&& e)
        : m_wrappee(get_storage_init<storing_type>(std::move(e)))
    {
    }

    template <class CT>
    inline closure_wrapper<CT>::closure_wrapper(reference e)
        : m_wrappee(get_storage_init<storing_type>(e))
    {
    }

    template <class CT>
    template <class T>
    inline auto closure_wrapper<CT>::operator=(T&& t)
        -> self_type& 
    {
        deref(m_wrappee) = std::forward<T>(t);
        return *this;
    }

    template <class CT>
    inline closure_wrapper<CT>::operator typename closure_wrapper<CT>::closure_type() noexcept
    {
        return deref(m_wrappee);
    }

    template <class CT>
    inline auto closure_wrapper<CT>::get() & noexcept -> closure_type
    {
        return deref(m_wrappee);
    }

    template <class CT>
    inline auto closure_wrapper<CT>::get() const & noexcept -> closure_type
    {
        return deref(m_wrappee);
    }

    template <class CT>
    inline auto closure_wrapper<CT>::get() && noexcept -> closure_type
    {
        return deref(m_wrappee);
    }

    template <class CT>
    template <class T>
    inline std::enable_if_t<std::is_pointer<T>::value, std::add_lvalue_reference_t<std::remove_pointer_t<T>>>
    closure_wrapper<CT>::deref(T val) const
    {
        return *val;
    }

    template <class CT>
    template <class T>
    inline std::enable_if_t<!std::is_pointer<T>::value, std::add_lvalue_reference_t<T>>
    closure_wrapper<CT>::deref(T& val) const
    {
        return val;
    }

    template <class CT>
    template <class T, class CTA>
    inline std::enable_if_t<std::is_pointer<T>::value, T>
    closure_wrapper<CT>::get_storage_init(CTA&& e) const
    {
        return &e;
    }

    template <class CT>
    template <class T, class CTA>
    inline std::enable_if_t<!std::is_pointer<T>::value, T>
    closure_wrapper<CT>::get_storage_init(CTA&& e) const
    {
        return e;
    }

    template <class CT>
    inline bool closure_wrapper<CT>::equal(const self_type& rhs) const
    {
        return m_wrappee == rhs.m_wrappee;
    }

    template <class CT>
    inline bool operator==(const closure_wrapper<CT>& lhs, const closure_wrapper<CT>& rhs)
    {
        return lhs.equal(rhs);
    }

    template <class CT>
    inline bool operator!=(const closure_wrapper<CT>& lhs, const closure_wrapper<CT>& rhs)
    {
        return !(lhs == rhs);
    }

    /*****************************
     * closure and const_closure *
     *****************************/

    template <class T>
    inline decltype(auto) closure(T&& t)
    {
        return closure_wrapper<closure_type_t<T>>(std::forward<T>(t));
    }

    template <class T>
    inline decltype(auto) const_closure(T&& t)
    {
        return closure_wrapper<const_closure_type_t<T>>(std::forward<T>(t));
    }
}

#endif
