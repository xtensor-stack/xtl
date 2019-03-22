/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xcomplex.hpp"

#include <complex>

#include "gtest/gtest.h"
#include "xtl/xclosure.hpp"

namespace xtl
{
    using namespace std::complex_literals;

    using complex_type = xcomplex<double>;
    using complex_ref_type = xcomplex<double&, double&>;

    TEST(xcomplex, constructor)
    {
        complex_type c0;
        EXPECT_EQ(c0.real(), 0.);
        EXPECT_EQ(c0.imag(), 0.);

        complex_type c1(2.);
        EXPECT_EQ(c1.real(), 2.);
        EXPECT_EQ(c1.imag(), 0.);

        complex_type c2(2., 3.);
        EXPECT_EQ(c2.real(), 2.);
        EXPECT_EQ(c2.imag(), 3.);

        complex_type c3(c2);
        EXPECT_EQ(c3.real(), c2.real());
        EXPECT_EQ(c3.imag(), c2.imag());

        complex_type c4(std::move(c3));
        EXPECT_EQ(c4.real(), c2.real());
        EXPECT_EQ(c4.imag(), c2.imag());

        std::complex<double> sc(1., 2.);
        complex_type c5(sc);
        EXPECT_EQ(c5.real(), sc.real());
        EXPECT_EQ(c5.imag(), sc.imag());

        complex_type c6(std::move(sc));
        EXPECT_EQ(c6.real(), c5.real());
        EXPECT_EQ(c6.imag(), c5.imag());

        double re = 1.2, im = 4.5;
        complex_ref_type c7(re, im);
        EXPECT_EQ(c7.real(), re);
        EXPECT_EQ(c7.imag(), im);

        xcomplex<float> f(1.f, 2.f);
        complex_type c8(f);
        EXPECT_EQ(c8.real(), 1.);
        EXPECT_EQ(c8.imag(), 2.);
    }

    TEST(xcomplex, assign)
    {
        complex_type c0(1., 2.);
        complex_type c1(4., 5.);

        c0 = 3.;
        EXPECT_EQ(c0.real(), 3.);
        EXPECT_EQ(c0.imag(), 0.);

        c0 = std::move(4.);
        EXPECT_EQ(c0.real(), 4.);
        EXPECT_EQ(c0.imag(), 0.);

        c0 = c1;
        EXPECT_EQ(c0.real(), c1.real());
        EXPECT_EQ(c0.imag(), c1.imag());

        c0 = complex_type(2., 3.);
        EXPECT_EQ(c0.real(), 2.);
        EXPECT_EQ(c0.imag(), 3.);

        std::complex<double> c2(1.4, 5.2);
        c0 = c2;
        EXPECT_EQ(c0.real(), c2.real());
        EXPECT_EQ(c0.imag(), c2.imag());

        c0 = std::complex<double>(1.2, 2.5);
        EXPECT_EQ(c0.real(), 1.2);
        EXPECT_EQ(c0.imag(), 2.5);
    }

    TEST(xcomplex, conversion)
    {
        complex_type c(1., 2.);
        std::complex<double> sc = c;
        EXPECT_EQ(c.real(), sc.real());
        EXPECT_EQ(c.imag(), sc.imag());
    }

    TEST(xcomplex, comparison)
    {
        complex_type vc0(1., 2.);
        complex_type vc1(2., 4.);

        EXPECT_TRUE(vc0 == vc0);
        EXPECT_FALSE(vc0 == vc1);

        EXPECT_FALSE(vc0 != vc0);
        EXPECT_TRUE(vc0 != vc1);
    }

    TEST(xcomplex, computed_assign)
    {
        complex_type vc0(1., 2.);
        complex_type vc1(2., 4.);

        complex_type c0(vc0);
        c0 += vc1;
        EXPECT_EQ(c0.real(), 3.);
        EXPECT_EQ(c0.imag(), 6.);

        complex_type c1(vc0);
        c1 -= vc1;
        EXPECT_EQ(c1.real(), -1.);
        EXPECT_EQ(c1.imag(), -2.);

        complex_type c2(vc0);
        c2 *= vc1;
        EXPECT_EQ(c2.real(), -6.);
        EXPECT_EQ(c2.imag(), 8.);

        complex_type c3(vc0);
        c3 /= vc1;
        EXPECT_EQ(c3.real(), 0.5);
        EXPECT_EQ(c3.imag(), 0.);

        double v = 0.5;

        complex_type c4(vc0);
        c4 += v;
        EXPECT_EQ(c4.real(), 1.5);
        EXPECT_EQ(c4.imag(), 2.);

        complex_type c5(vc0);
        c5 -= v;
        EXPECT_EQ(c5.real(), 0.5);
        EXPECT_EQ(c5.imag(), 2.);

        complex_type c6(vc0);
        c6 *= v;
        EXPECT_EQ(c6.real(), 0.5);
        EXPECT_EQ(c6.imag(), 1.);

        complex_type c7(vc0);
        c7 /= v;
        EXPECT_EQ(c7.real(), 2.);
        EXPECT_EQ(c7.imag(), 4.);

        xcomplex<double, double, true> sc(vc1);
        xcomplex<double, double, true> c8(vc0);
        c8 *= sc;
        EXPECT_EQ(c8.real(), -6.);
        EXPECT_EQ(c8.imag(), 8.);

        xcomplex<double, double, true> c9(vc0);
        c9 /= sc;
        EXPECT_EQ(c9.real(), 0.5);
        EXPECT_EQ(c9.imag(), 0.);
    }

    TEST(xcomplex, arithmetic)
    {
        complex_type vc0(1., 2.);
        complex_type vc1(2., 4.);

        complex_type c0 = vc0 + vc1;
        EXPECT_EQ(c0.real(), 3.);
        EXPECT_EQ(c0.imag(), 6.);

        complex_type c1 = vc0 - vc1;
        EXPECT_EQ(c1.real(), -1.);
        EXPECT_EQ(c1.imag(), -2.);

        complex_type c2 = vc0 * vc1;
        EXPECT_EQ(c2.real(), -6.);
        EXPECT_EQ(c2.imag(), 8.);

        complex_type c3 = vc0 / vc1;
        EXPECT_EQ(c3.real(), 0.5);
        EXPECT_EQ(c3.imag(), 0.);

        double v = 0.5;

        complex_type c4 = vc0 + v;
        EXPECT_EQ(c4.real(), 1.5);
        EXPECT_EQ(c4.imag(), 2.);

        complex_type c5 = vc0 - v;
        EXPECT_EQ(c5.real(), 0.5);
        EXPECT_EQ(c5.imag(), 2.);

        complex_type c6 = vc0 * v;
        EXPECT_EQ(c6.real(), 0.5);
        EXPECT_EQ(c6.imag(), 1.);

        complex_type c7 = vc0 / v;
        EXPECT_EQ(c7.real(), 2.);
        EXPECT_EQ(c7.imag(), 4.);
    }

    TEST(xcomplex, real_imag)
    {
        complex_type c(1., 2.);

        auto d1 = real(c);
        EXPECT_EQ(d1, c.real());

        auto d2 = real(std::move(c));
        EXPECT_EQ(d2, c.real());

        auto d3 = imag(c);
        EXPECT_EQ(d3, c.imag());

        auto d4 = imag(std::move(c));
        EXPECT_EQ(d4, c.imag());
    }

    TEST(xcomplex, free_functions)
    {
        complex_type c(1., 2.);
        std::complex<double> sc(c);

        EXPECT_EQ(abs(c), std::abs(sc));
        EXPECT_EQ(arg(c), std::arg(sc));
        EXPECT_EQ(norm(c), std::norm(sc));
        EXPECT_EQ(conj(c), complex_type(std::conj(sc)));
        EXPECT_EQ(proj(c), complex_type(std::proj(sc)));
    }

    TEST(xcomplex, exponential)
    {
        complex_type c(1., 2.);
        std::complex<double> sc(c);
        EXPECT_EQ(exp(c), complex_type(std::exp(sc)));
        EXPECT_EQ(log(c), complex_type(std::log(sc)));
        EXPECT_EQ(log10(c), complex_type(std::log10(sc)));
    }

    TEST(xcomplex, power)
    {
        complex_type c(1., 2.);
        std::complex<double> sc(c);
        double d = 1.5;
        EXPECT_EQ(pow(c, c), complex_type(std::pow(sc, sc)));
        EXPECT_EQ(pow(c, d), complex_type(std::pow(sc, d)));
        EXPECT_EQ(pow(d, c), complex_type(std::pow(d, sc)));
        EXPECT_EQ(sqrt(c), complex_type(std::sqrt(sc)));
    }

    TEST(xcomplex, trigonometric)
    {
        complex_type c(1., 2.);
        std::complex<double> sc(c);
        EXPECT_EQ(sin(c), complex_type(std::sin(sc)));
        EXPECT_EQ(cos(c), complex_type(std::cos(sc)));
        EXPECT_EQ(tan(c), complex_type(std::tan(sc)));
        EXPECT_EQ(asin(c), complex_type(std::asin(sc)));
        EXPECT_EQ(acos(c), complex_type(std::acos(sc)));
        EXPECT_EQ(atan(c), complex_type(std::atan(sc)));
    }

    TEST(xcomplex, hyperbolic)
    {
        complex_type c(1., 2.);
        std::complex<double> sc(c);
        EXPECT_EQ(sinh(c), complex_type(std::sinh(sc)));
        EXPECT_EQ(cosh(c), complex_type(std::cosh(sc)));
        EXPECT_EQ(tanh(c), complex_type(std::tanh(sc)));
        EXPECT_EQ(asinh(c), complex_type(std::asinh(sc)));
        EXPECT_EQ(acosh(c), complex_type(std::acosh(sc)));
        EXPECT_EQ(atanh(c), complex_type(std::atanh(sc)));
    }

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

