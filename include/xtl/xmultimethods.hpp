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
#include <cstdint>
#include <functional>
#include <map>
#include <typeindex>
#include <type_traits>
#include <vector>

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

    template
    <
        class return_type,
        class callback_type,
        class... B
    >
    class basic_dispatcher<mpl::vector<B...>, return_type, callback_type>
    {
    private:

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
                XTL_THROW(std::runtime_error, "callback not found");
            }
            return (it->second)(args...);
        }
    };

    /*************************
     * basic_fast_dispatcher *
     *************************/

#define XTL_IMPLEMENT_INDEXABLE_CLASS()         \
    static size_t& get_class_static_index()     \
    {                                           \
        static size_t index = SIZE_MAX;         \
        return index;                           \
    }                                           \
    virtual size_t get_class_index() const      \
    {                                           \
        return get_class_static_index();        \
    }

    namespace detail
    {
        template <class T>
        class recursive_container_impl : private std::vector<T>
        {
        public:

            using base_type = std::vector<T>;

            using base_type::base_type;
            using base_type::operator[];
            using base_type::size;
            using base_type::resize;
        };

        template <class callback_type, size_t level>
        class recursive_container
            : public recursive_container_impl<recursive_container<callback_type, level-1>>
        {
        };

        template <class callback>
        class recursive_container<callback, 0>
            : public recursive_container_impl<callback>
        {
        };
      }

    template
    <
        class type_list,
        class return_type,
        class callback_type
    >
    class basic_fast_dispatcher;

    template
    <
        class return_type,
        class callback_type,
        class... B
    >
    class basic_fast_dispatcher<mpl::vector<B...>, return_type, callback_type>
    {
    private:

        static constexpr size_t nb_args = sizeof...(B);

        using storage_type = detail::recursive_container<callback_type, sizeof...(B) - 1>;
        using index_type = std::array<size_t, nb_args>;
        using index_ref_type = std::array<std::reference_wrapper<size_t>, nb_args>;

        storage_type m_callbacks;
        size_t m_next_index;

        template <size_t I, class C>
        void resize_container(C& c, const index_ref_type& index)
        {
            size_t& idx = index[I];
            if (idx == SIZE_MAX)
            {
                c.resize(++m_next_index);
                idx = c.size() - 1u;
            }
            else if(c.size() <= idx)
            {
                c.resize(idx + 1u);
            }
        }

        template <size_t I, class C>
        std::enable_if_t<I + 1 == nb_args>
        insert_impl(callback_type&& cb, C& c, const index_ref_type& index)
        {
            resize_container<I>(c, index);
            c[index[I]] = std::move(cb);
        }

        template <size_t I, class C>
        std::enable_if_t<I + 1 != nb_args>
        insert_impl(callback_type&& cb, C& c, const index_ref_type& index)
        {
            resize_container<I>(c, index);
            insert_impl<I+1>(std::move(cb), c[index[I]], index);
        }

        template <size_t I, class C>
        void check_size(C& c, const index_type& index) const
        {
            if (index[I] >= c.size())
            {
                XTL_THROW(std::runtime_error, "callback not found");
            }
        }

        template <size_t I, class C>
        std::enable_if_t<I + 1 == nb_args, return_type>
        dispatch_impl(C& c, const index_type& index, B&... args) const
        {
            check_size<I>(c, index);
            return c[index[I]](args...);
        }

        template <size_t I, class C>
        std::enable_if_t<I + 1 != nb_args, return_type>
        dispatch_impl(C& c, const index_type& index, B&... args) const
        {
            check_size<I>(c, index);
            return dispatch_impl<I+1>(c[index[I]], index, args...);
        }

    public:

        inline basic_fast_dispatcher()
            : m_next_index(0)
        {
        }

        template <class... D>
        void insert(callback_type&& cb)
        {
            static_assert(sizeof...(D) == sizeof...(B),
                          "Number of callback arguments must match dispatcher dimension");
            index_ref_type index = {{std::ref(D::get_class_static_index())...}};
            insert_impl<0>(std::move(cb), m_callbacks, index);
        }

        inline return_type dispatch(B&... args) const
        {
            index_type index = {{args.get_class_index()...}};
            return dispatch_impl<0>(m_callbacks, index, args...);
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
        template <class, class> class casting_policy = dynamic_caster,
        template <class, class, class> class dispatcher = basic_dispatcher
    >
    class functor_dispatcher;

    template
    <
        class return_type, 
        template <class, class> class casting_policy,
        template <class, class, class> class dispatcher,
        class... B
    >
    class functor_dispatcher<mpl::vector<B...>, return_type, casting_policy, dispatcher>
    {
    private:

        using functor_type = std::function<return_type (B&...)>;
        using backend = dispatcher<mpl::vector<B...>,
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

