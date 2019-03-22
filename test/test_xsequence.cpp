/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xsequence.hpp"

#include <array>
#include <vector>

#include "gtest/gtest.h"

namespace xtl
{
    template <class T, std::size_t N>
    struct aligned_array
        : std::array<T, N>
    {
    };

    bool result(const std::array<int, 2>& /*lval*/)
    {
        return false;
    }

    bool result(std::array<int, 2>&& /*lval*/)
    {
        return true;
    }

    template <class T>
    bool test(T&& arg)
    {
        using shape_type = std::array<int, 2>;
        return result(xtl::forward_sequence<shape_type, T>(arg));
    }

    TEST(xsequence, forward_type)
    {
        std::array<int, 2> a, b;
        std::vector<int> c;
        EXPECT_TRUE(test(std::move(a)));
        EXPECT_FALSE(test(a));
        EXPECT_TRUE(test(c));
        EXPECT_TRUE(test(std::move(c)));
    }

    template <class R, class T>
    R test_different_array(T& array)
    {
        return xtl::forward_sequence<R, T>(array);
    }

    TEST(xsequence, different_arrays)
    {
        std::array<int, 3> x, y;
        aligned_array<int, 3> aa, ab;

        auto res1 = test_different_array<aligned_array<int, 3>>(x);
        auto res2 = test_different_array<aligned_array<int, 3>>(aa);
    }

    TEST(xsequence, forward)
    {
        std::array<int, 2> a = { 1, 2 };
        std::vector<int> b = { 1, 2 };

        std::vector<int> resa = xtl::forward_sequence<std::vector<int>, decltype(a)>(a);
        EXPECT_EQ(resa, b);

        std::array<int, 2> resb = xtl::forward_sequence<std::array<int, 2>, decltype(b)>(b);
        EXPECT_EQ(resb, a);
    }

    TEST(xsequence, make_sequence)
    {
        using vector_type = std::vector<int>;
        using array_type = std::array<int, 3>;

        std::size_t size = 3u;
        int init_value = 2;
        std::initializer_list<int> init_list = { 3, 2, 4 };

        vector_type v0 = xtl::make_sequence<vector_type>(size);
        EXPECT_EQ(v0.size(), size);
        
        array_type a0 = xtl::make_sequence<array_type>(size);
        EXPECT_EQ(a0.size(), size);

        vector_type v1 = xtl::make_sequence<vector_type>(size, init_value);
        EXPECT_EQ(v1.size(), size);
        EXPECT_EQ(v1[0], init_value);
        EXPECT_EQ(v1[1], init_value);
        EXPECT_EQ(v1[2], init_value);

        array_type a1 = xtl::make_sequence<array_type>(size, init_value);
        EXPECT_EQ(a1.size(), size);
        EXPECT_EQ(a1[0], init_value);
        EXPECT_EQ(a1[1], init_value);
        EXPECT_EQ(a1[2], init_value);

        vector_type v2 = xtl::make_sequence<vector_type>(init_list);
        EXPECT_EQ(v2.size(), size);
        EXPECT_EQ(v2[0], 3);
        EXPECT_EQ(v2[1], 2);
        EXPECT_EQ(v2[2], 4);

        array_type a2 = xtl::make_sequence<array_type>(init_list);
        EXPECT_EQ(a2.size(), size);
        EXPECT_EQ(a2[0], 3);
        EXPECT_EQ(a2[1], 2);
        EXPECT_EQ(a2[2], 4);
    }
}

