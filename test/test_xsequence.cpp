/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include <array>
#include <vector>

#include "xtl/xsequence.hpp"

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
}

