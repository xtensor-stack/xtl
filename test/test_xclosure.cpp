/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include <type_traits>

#include "xtl/xclosure.hpp"

namespace xtl
{
    TEST(xclosure, closure_types)
    {
        bool rvalue_test = std::is_same<closure_type_t<double&&>, double>::value;
        bool lvalue_reference_test = std::is_same<closure_type_t<double&>, double&>::value;
        bool const_lvalue_reference_test = std::is_same<closure_type_t<const double&>, const double&>::value;

        EXPECT_TRUE(rvalue_test);
        EXPECT_TRUE(lvalue_reference_test);
        EXPECT_TRUE(const_lvalue_reference_test);
    }

    TEST(xclosure, lvalue_closure_wrappers)
    {
        double x = 0.0;
        auto x_closure = closure(x);
        x_closure = 1.0;
        EXPECT_EQ(x, 1.0);
        EXPECT_EQ(&x, &x_closure);
    }

    TEST(xclosure, rvalue_closure_wrappers)
    {
        double x = 0.0;
        auto x_closure = closure(std::move(x));
        x_closure = 1.0;
        EXPECT_EQ(x, 0.0);
        EXPECT_NE(&x, &x_closure);
    }

    TEST(xclosure_pointer, lvalue_closure_wrappers)
    {
        double x = 0.0;
        auto x_closure = closure_pointer(x);
        *x_closure = 1.0;
        EXPECT_EQ(x, 1.0);
        EXPECT_EQ(x_closure.operator->(), &x);
    }

    TEST(xclosure_pointer, rvalue_closure_wrappers)
    {
        double x = 0.0;
        auto x_closure = closure_pointer(std::move(x));
        *x_closure = 1.0;
        EXPECT_EQ(x, 0.0);
        EXPECT_NE(x_closure.operator->(), &x);
    }
}

