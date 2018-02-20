/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include <complex>

#include "xtl/xcomplex.hpp"
#include "xtl/xclosure.hpp"

namespace xtl
{
    using namespace std::complex_literals;

    TEST(xcomplex, forward_offset)
    {
        // Test that lvalues can be modified
        std::complex<double> clv;
        forward_real(clv) = 3.0;
        EXPECT_EQ(std::real(clv), 3.0);

        forward_imag(clv) = 1.0;
        EXPECT_EQ(std::imag(clv), 1.0);

        double rlv = 2.0;
        forward_real(rlv) = 1.0;
        EXPECT_EQ(forward_imag(rlv), 0.0);
        EXPECT_EQ(forward_real(rlv), 1.0);
    }

    TEST(xcomplex, scalar)
    {
        double d = 1.0;
        ASSERT_EQ(1.0, real(d));
        ASSERT_EQ(0.0, imag(d));
        real(d) = 2.0;
        ASSERT_EQ(2.0, d);
    }

    TEST(xcomplex, closure)
    {
        double x = 5.0;
        auto x_closure = closure(x);
        std::complex<double> b(0, 5), c;
        EXPECT_EQ(b + x_closure, b + 5.0);
        EXPECT_EQ(x_closure + b, 5.0 + b);
        EXPECT_EQ(b - x_closure, b - 5.0);
        EXPECT_EQ(x_closure - b, 5.0 - b);
        EXPECT_EQ(b * x_closure, b * 5.0);
        EXPECT_EQ(x_closure * b, 5.0 * b);
        EXPECT_EQ(b / x_closure, b / 5.0);
        EXPECT_EQ(x_closure / b, 5.0 / b);
    }
}

