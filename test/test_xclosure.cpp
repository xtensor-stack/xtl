/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xclosure.hpp"

#include <type_traits>
#include <vector>

#include "gtest/gtest.h"

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

    TEST(xclosure, copy)
    {
        std::vector<int> v = { 1, 2, 3 };
        auto cl1 = closure(v);
        auto cl2(cl1);

        cl2.get()[0] = 4;
        EXPECT_EQ(v[0], 4);

        auto cl3(const_cast<const std::decay_t<decltype(cl1)>&>(cl1));
        cl3.get()[0] = 5;
        EXPECT_EQ(v[0], 5);
    }

    TEST(xclosure, assign)
    {
        std::vector<int> v1 = { 1, 2, 3 };
        std::vector<int> v2 = { 3, 2, 1 };
        std::vector<int> v3 = v1;

        auto cl1 = closure(v1);
        auto cl2 = closure(v2);
        auto cl3 = closure(v3);

        cl1 = cl2;
        EXPECT_EQ(v1, v2);

        cl3 = const_cast<const std::decay_t<decltype(cl2)>&>(cl2);
        EXPECT_EQ(v3, v2);
    }

    TEST(xclosure, move)
    {
        std::vector<int> v = { 1, 2, 3 };

        auto cl1 = closure(v);
        auto cl2(std::move(cl1));

        cl2.get()[0] = 4;
        EXPECT_EQ(v[0], 4);
    }

    TEST(xclosure, move_assign)
    {
        std::vector<int> v1 = { 1, 2, 3 };
        std::vector<int> v2 = { 3, 2, 1 };
        std::vector<int> v3 = v2;

        auto cl1 = closure(v1);
        auto cl2 = closure(v2);

        cl1 = std::move(cl2);
        EXPECT_EQ(v1, v3);
        
    }

    TEST(xclosure, swap)
    {
        std::vector<int> v1 = { 1, 2, 3 };
        std::vector<int> v2 = { 3, 2, 1 };
        std::vector<int> v3 = v1;
        std::vector<int> v4 = v2;

        auto cl1 = closure(v1);
        auto cl2 = closure(v2);

        using std::swap;
        swap(cl1, cl2);
        EXPECT_EQ(v1, v4);
        EXPECT_EQ(v2, v3);
    }

    TEST(xclosure, swap_pointers)
    {
        int* d1 = new int[3];
        int* d2 = new int[3];

        int* d3 = d1;
        int* d4 = d2;

        auto cl1 = closure(d1);
        auto cl2 = closure(d2);

        using std::swap;
        swap(cl1, cl2);

        EXPECT_EQ(d1, d4);
        EXPECT_EQ(d2, d3);

        delete[] d2;
        delete[] d1;
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

