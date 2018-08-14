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
    bool result(const std::array<int, 2>& lval)
    {
        return false;
    }

    bool result(std::array<int, 2>&& lval)
    {
        return true;
    }

    template <class T>
    bool test(T&& arg)
    {
        using shape_type = std::array<int, 2>;
        return result(xtl::forward_sequence<shape_type, T>(arg));
    }

    template <class T>
    bool test_wrong(T&& arg)
    {
        using shape_type = std::array<int, 2>;
        return result(xtl::forward_sequence<shape_type>(arg));
    }

    TEST(xsequence, forward)
    {
        std::array<int, 2> a, b;
        std::vector<int> c;
        EXPECT_TRUE(test(std::move(a)));
        EXPECT_FALSE(test(a));
        EXPECT_TRUE(test(c));
        EXPECT_TRUE(test(std::move(c)));

        EXPECT_FALSE(test_wrong(std::move(a)));
        EXPECT_FALSE(test_wrong(a));
        EXPECT_TRUE(test_wrong(c));
        EXPECT_TRUE(test_wrong(std::move(c)));
    }
}

