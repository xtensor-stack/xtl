/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XTL_TYPE_TRAITS_HPP
#define XTL_TYPE_TRAITS_HPP

#include <type_traits>
#include <complex>

#include "xtl_config.hpp"

namespace xtl
{
    /************************************
     * arithmetic type promotion traits *
     ************************************/

    /**
     * Traits class for the result type of mixed arithmetic expressions.
     * For example, <tt>promote_type<unsigned char, unsigned char>::type</tt> tells
     * the user that <tt>unsigned char + unsigned char => int</tt>.
     */
    template <class... T>
    struct promote_type;

    template <>
    struct promote_type<>
    {
        using type = void;
    };

    template <class T>
    struct promote_type<T>
    {
        using type = typename promote_type<T, T>::type;
    };

    template <class T0, class T1>
    struct promote_type<T0, T1>
    {
        using type = decltype(std::declval<std::decay_t<T0>>() + std::declval<std::decay_t<T1>>());
    };

    template <class T0, class... REST>
    struct promote_type<T0, REST...>
    {
        using type = decltype(std::declval<std::decay_t<T0>>() + std::declval<typename promote_type<REST...>::type>());
    };

    template <>
    struct promote_type<bool>
    {
        using type = bool;
    };

    template <class T>
    struct promote_type<bool, T>
    {
        using type = T;
    };

    template <class... REST>
    struct promote_type<bool, REST...>
    {
        using type = typename promote_type<bool, typename promote_type<REST...>::type>::type;
    };

    /**
     * Abbreviation of 'typename promote_type<T>::type'.
     */
    template <class... T>
    using promote_type_t = typename promote_type<T...>::type;

    /**
     * Traits class to find the biggest type of the same kind.
     *
     * For example, <tt>big_promote_type<unsigned char>::type</tt> is <tt>unsigned long long</tt>.
     * The default implementation only supports built-in types and <tt>std::complex</tt>. All
     * other types remain unchanged unless <tt>big_promote_type</tt> gets specialized for them.
     */
    template <class T>
    struct big_promote_type
    {
    private:

        using V = std::decay_t<T>;
        static constexpr bool is_arithmetic = std::is_arithmetic<V>::value;
        static constexpr bool is_signed = std::is_signed<V>::value;
        static constexpr bool is_integral = std::is_integral<V>::value;
        static constexpr bool is_long_double = std::is_same<V, long double>::value;

    public:

        using type = std::conditional_t<is_arithmetic,
                        std::conditional_t<is_integral,
                            std::conditional_t<is_signed, long long, unsigned long long>,
                            std::conditional_t<is_long_double, long double, double>
                        >,
                        V
                     >;
    };

    template <class T>
    struct big_promote_type<std::complex<T>>
    {
        using type = std::complex<typename big_promote_type<T>::type>;
    };

    /**
     * Abbreviation of 'typename big_promote_type<T>::type'.
     */
    template <class T>
    using big_promote_type_t = typename big_promote_type<T>::type;

    namespace traits_detail
    {
        using std::sqrt;

        template <class T>
        using real_promote_type_t = decltype(sqrt(std::declval<std::decay_t<T>>()));
    }

    /**
     * Result type of algebraic expressions.
     *
     * For example, <tt>real_promote_type<int>::type</tt> tells the
     * user that <tt>sqrt(int) => double</tt>.
     */
    template <class T>
    struct real_promote_type
    {
        using type = traits_detail::real_promote_type_t<T>;
    };

    /**
     * Abbreviation of 'typename real_promote_type<T>::type'.
     */
    template <class T>
    using real_promote_type_t = typename real_promote_type<T>::type;

    /**
     * Traits class to replace 'bool' with 'uint8_t' and keep everything else.
     *
     * This is useful for scientific computing, where a boolean mask array is
     * usually implemented as an array of bytes.
     */
    template <class T>
    struct bool_promote_type
    {
        using type = typename std::conditional<std::is_same<T, bool>::value, uint8_t, T>::type;
    };

    /**
     * Abbreviation for typename bool_promote_type<T>::type
     */
    template <class T>
    using bool_promote_type_t = typename bool_promote_type<T>::type;

    /************
     * apply_cv *
     ************/

    namespace detail
    {
        template <class T, class U, bool = std::is_const<std::remove_reference_t<T>>::value,
                  bool = std::is_volatile<std::remove_reference_t<T>>::value>
        struct apply_cv_impl
        {
            using type = U;
        };

        template <class T, class U>
        struct apply_cv_impl<T, U, true, false>
        {
            using type = const U;
        };

        template <class T, class U>
        struct apply_cv_impl<T, U, false, true>
        {
            using type = volatile U;
        };

        template <class T, class U>
        struct apply_cv_impl<T, U, true, true>
        {
            using type = const volatile U;
        };

        template <class T, class U>
        struct apply_cv_impl<T&, U, false, false>
        {
            using type = U&;
        };

        template <class T, class U>
        struct apply_cv_impl<T&, U, true, false>
        {
            using type = const U&;
        };

        template <class T, class U>
        struct apply_cv_impl<T&, U, false, true>
        {
            using type = volatile U&;
        };

        template <class T, class U>
        struct apply_cv_impl<T&, U, true, true>
        {
            using type = const volatile U&;
        };
    }

    template <class T, class U>
    struct apply_cv
    {
        using type = typename detail::apply_cv_impl<T, U>::type;
    };

    template <class T, class U>
    using apply_cv_t = typename apply_cv<T, U>::type;

    /****************************************************************
     * C++17 logical operators (disjunction, conjunction, negation) *
     ****************************************************************/

    /********************
     * disjunction - or *
     ********************/

    template <class...>
    struct disjunction;

    template <>
    struct disjunction<> : std::false_type
    {
    };

    template <class Arg>
    struct disjunction<Arg> : Arg
    {
    };

    template <class Arg1, class Arg2, class... Args>
    struct disjunction<Arg1, Arg2, Args...> : std::conditional_t<Arg1::value, Arg1, disjunction<Arg2, Args...>>
    {
    };

    /*********************
     * conjunction - and *
     *********************/

    template <class...>
    struct conjunction;

    template <>
    struct conjunction<> : std::true_type
    {
    };

    template <class Arg1>
    struct conjunction<Arg1> : Arg1
    {
    };

    template <class Arg1, class Arg2, class... Args>
    struct conjunction<Arg1, Arg2, Args...> : std::conditional_t<Arg1::value, conjunction<Arg2, Args...>, Arg1>
    {
    };

    /******************
     * negation - not *
     ******************/

    template <class Arg>
    struct negation : std::integral_constant<bool, !Arg::value>
    {
    };

    /************
     * concepts *
     ************/

    template <class... C>
    using check_concept = std::enable_if_t<conjunction<C...>::value, int>;

#define XTL_REQUIRES(...) xtl::check_concept<__VA_ARGS__> = 0
#define XTL_REQUIRES_IMPL(...) xtl::check_concept<__VA_ARGS__>

    /**************
     * all_scalar *
     **************/

    template <class... Args>
    struct all_scalar : conjunction<std::is_scalar<Args>...>
    {
    };

    /************
     * constify *
     ************/

    // Adds const to the underlying type of a reference or pointer, or to the type itself
    // if it's not a reference nor a pointer

    template <class T>
    struct constify
    {
        using type = std::add_const_t<T>;
    };

    template <class T>
    struct constify<T*>
    {
        using type = std::add_const_t<T>*;
    };

    template <class T>
    struct constify<T&>
    {
        using type = std::add_const_t<T>&;
    };

    template <class T>
    using constify_t = typename constify<T>::type;
}

#endif
