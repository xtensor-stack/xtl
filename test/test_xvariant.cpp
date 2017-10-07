/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include "xtl/xvariant.hpp"

using namespace std::literals;

namespace xtl
{
    TEST(xvariant, basic)
    {
        xtl::variant<int, float> v, w;
        v = 12;
        int i = xtl::get<int>(v);
        EXPECT_EQ(12, i);

        w = xtl::get<int>(v);
        EXPECT_EQ(12, xtl::get<int>(w));
        w = xtl::get<0>(v);
        EXPECT_EQ(12, xtl::get<int>(w));
        w = v;
        EXPECT_EQ(12, xtl::get<int>(w));
    }

    TEST(xvariant, bad_variant_access)
    {
        xtl::variant<int, float> v;
        v = 12;
        EXPECT_THROW(xtl::get<float>(v), xtl::bad_variant_access);
    }

    TEST(xvariant, converting_constructor)
    {
        xtl::variant<std::string> x("abc");
        x = "def";
        EXPECT_EQ(std::string("def"), xtl::get<std::string>(x));
    }

    TEST(xvariant, holds_alternative)
    {
        xtl::variant<std::string, bool> y("abc");
        EXPECT_TRUE(xtl::holds_alternative<bool>(y));
        y = "xyz"s;
        EXPECT_TRUE(xtl::holds_alternative<std::string>(y));
    }

    TEST(xvariant, visit)
    {
        using var_t = xtl::variant<int, long, double, std::string>;
        std::vector<var_t> vec = { 10, 15l, 1.5, "hello" };
        std::vector<var_t> vec_res(4);
        for (std::size_t i = 0; i < vec.size(); ++i)
        {
            var_t v = vec[i];
            var_t w = xtl::visit([](auto&& arg) -> var_t {return arg + arg; }, v);
            vec_res[i] = w;
        }
        EXPECT_EQ(20, xtl::get<0>(vec_res[0]));
        EXPECT_EQ(30l, xtl::get<1>(vec_res[1]));
        EXPECT_EQ(3., xtl::get<2>(vec_res[2]));
        EXPECT_EQ("hellohello", xtl::get<3>(vec_res[3]));
    }
}
