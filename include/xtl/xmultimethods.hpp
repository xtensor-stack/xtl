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

#include <array>
#include <functional>
#include <map>
#include <typeindex>

#include "xmeta_utils.hpp"

namespace xtl
{
    // Loki's multimethods ported to modern C++ and generalized to N arguments
    // Original implementation can be found at
    // https://github.com/snaewe/loki-lib/blob/master/include/loki/MultiMethods.h

    struct symmetric_dispatch {};
    struct antisymmetric_dispatch {};

    /*********************
     * static_dispatcher *
     *********************/

    template
    <
        class executor,
        class base_lhs,
        class lhs_type_list,
        class return_type = void,
        class symmetric = antisymmetric_dispatch,
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
                    std::is_same<symmetric, symmetric_dispatch>::value && (rhs_index < lhs_index)>;
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

    // TODO: generalize to N-D with mpl::vector of mpl:vector
    // Warning: this is hardcore ;)

    /********************
     * basic_dispatcher *
     ********************/

    template
    <
        class type_list,
        class return_type,
        class callback_type
    >
    class basic_dispatcher;

    template <class return_type, class callback_type, class... B>
    class basic_dispatcher<mpl::vector<B...>, return_type, callback_type>
    {
    private:

        static_assert(sizeof...(B) > size_t(1), "dispatcher dimension must be greater than 1");

        using key_type = std::array<std::type_index, sizeof...(B)>;
        using map_type = std::map<key_type, callback_type>;
        map_type m_callback_map;

        template <class... U>
        key_type make_key() const
        {
            return {{std::type_index(typeid(U))...}};
        }

    public:

        template <class... D>
        void insert(callback_type&& cb)
        {
            static_assert(sizeof...(D) == sizeof...(B),
                          "Number of callback arguments must match dispatcher dimension");
            m_callback_map[make_key<D...>()] = std::move(cb);
        }

        template <class... D>
        void erase()
        {
            static_assert(sizeof...(D) == sizeof...(B),
                          "Number of callback arguments must match dispatcher dimension");
            m_callback_map.erase(make_key<D...>());
        }

        inline return_type dispatch(B&... args) const
        {
            key_type k = {{std::type_index(typeid(args))...}};
            auto it = m_callback_map.find(k);
            if (it == m_callback_map.end())
            {
                throw std::runtime_error("callback not found");
            }
            return (it->second)(args...);
        }
    };

    /******************************
     * dynamic and static casters *
     ******************************/

    template <class T, class F>
    struct static_caster
    {
        static T& cast(F& f)
        {
            return static_cast<T&>(f);
        }
    };

    template <class T, class F>
    struct dynamic_caster
    {
        static T& cast(F& f)
        {
            return dynamic_cast<T&>(f);
        }
    };

    /**********************
     * functor_dispatcher *
     **********************/

    template
    <
        class type_list,
        class return_type,
        template <class, class> class casting_policy = dynamic_caster
    >
    class functor_dispatcher;

    template <class return_type, template <class, class> class casting_policy, class... B>
    class functor_dispatcher<mpl::vector<B...>, return_type, casting_policy>
    {
    private:

        using functor_type = std::function<return_type (B&...)>;
        using backend = basic_dispatcher<mpl::vector<B...>,
                                         return_type,
                                         functor_type>;
        backend m_backend;

    public:

        template <class... D, class Fun>
        void insert(const Fun& fun)
        {
            functor_type f([fun](B&... args) -> return_type
            {
                return fun(casting_policy<D&, B&>::cast(args)...);
            });
            m_backend.template insert<D...>(std::move(f));

        }

        template <class... D>
        void erase()
        {
            m_backend.template erase<D...>();
        }

        inline return_type dispatch(B&... args) const
        {
            return m_backend.dispatch(args...);
        }
    };

}

#endif

