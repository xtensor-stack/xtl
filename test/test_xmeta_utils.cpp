/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include "xtl/xmeta_utils.hpp"
#include "xtl/xvariant.hpp"

namespace xtl
{
    using vector_t = mpl::vector<int, double, char*>;
    using variant_t = variant<int, double, char*>;

    TEST(mpl, if_)
    {
        using true_cond = mpl::bool_<true>;
        using false_cond = mpl::bool_<false>;

        bool res1 = std::is_same<mpl::if_t<true_cond, vector_t, variant_t>, vector_t>::value;
        bool res2 = std::is_same<mpl::if_t<false_cond, vector_t, variant_t>, variant_t>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
    }

    TEST(mpl, cast)
    {
        using type = mpl::cast_t<vector_t, variant>;
        bool res = std::is_same<type, variant_t>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, size)
    {
        std::size_t size = mpl::size<vector_t>::value;
        EXPECT_EQ(size, 3u);
    }

    TEST(mpl, empty)
    {
        bool res1 = mpl::empty<vector_t>::value;
        bool res2 = mpl::empty<mpl::vector<>>::value;

        EXPECT_FALSE(res1);
        EXPECT_TRUE(res2);
    }

    TEST(mpl, count)
    {
        using type = mpl::vector<int, double, int, float, double, double>;
        std::size_t c1 = mpl::count<type, int>::value;
        std::size_t c2 = mpl::count<type, double>::value;
        std::size_t c3 = mpl::count<type, float>::value;
        std::size_t c4 = mpl::count<type, char>::value;

        EXPECT_EQ(c1, 2u);
        EXPECT_EQ(c2, 3u);
        EXPECT_EQ(c3, 1u);
        EXPECT_EQ(c4, 0u);
    }

    template <class T>
    struct is_double : std::is_same<T, double>
    {
    };

    TEST(mpl, count_if)
    {
        using type = mpl::vector<int, double, float, double>;
        std::size_t c1 = mpl::count_if<type, is_double>::value;
        EXPECT_EQ(c1, 2u);
    }

    TEST(mpl, contains)
    {
        using type = mpl::vector<int, double, double>;
        bool res1 = mpl::contains<type, int>::value;
        bool res2 = mpl::contains<type, double>::value;
        bool res3 = mpl::contains<type, float>::value;

        EXPECT_TRUE(res1);
        EXPECT_TRUE(res2);
        EXPECT_FALSE(res3);
    }

    TEST(mpl, front)
    {
        using type = mpl::front_t<variant_t>;
        bool res = std::is_same<type, int>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, back)
    {
        using type = mpl::back_t<variant_t>;
        bool res = std::is_same<type, char*>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, push_front)
    {
        using type = mpl::push_front_t<vector_t, double>;
        using res_type = mpl::vector<double, int, double, char*>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, push_back)
    {
        using type = mpl::push_back_t<vector_t, double>;
        using res_type = mpl::vector<int, double, char*, double>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, pop_front)
    {
        using type = mpl::pop_front_t<vector_t>;
        using res_type = mpl::vector<double, char*>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, pop_back)
    {
        using type = mpl::pop_back_t<vector_t>;
        using res_type = mpl::vector<int, double>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    TEST(mpl, transform)
    {
        using type = mpl::transform_t<std::add_pointer_t, vector_t>;
        using res_type = mpl::vector<int*, double*, char**>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);
    }

    template <class T>
    struct DEBUG
    {
        using type = typename T::coincoin;
    };

    TEST(mpl, merge_set)
    {
        using arg2 = mpl::vector<double, float, int, short>;
        using type = mpl::merge_set_t<vector_t, arg2>;
        using res_type = mpl::vector<int, double, char*, float, short>;
        bool res = std::is_same<type, res_type>::value;
        EXPECT_TRUE(res);

        using res_type2 = mpl::merge_set_t<vector_t, vector_t>;
        bool res2 = std::is_same<res_type2, vector_t>::value;
        EXPECT_TRUE(res2);
    }

    template <bool B>
    int static_if_tester()
    {
        int input = 0;
        return mpl::static_if<B == false>([&](auto self)
        {
            return input;
        }, /*else*/ [&](auto self)
        {
            return input + 1;
        });
    }

    TEST(mpl, static_if)
    {
        int output_0 = static_if_tester<false>();
        int output_1 = static_if_tester<true>();
        EXPECT_EQ(output_0, 0);
        EXPECT_EQ(output_1, 1);
    }
}

