/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xcomplex_sequence.hpp"

#include "gtest/gtest.h"

namespace xtl
{
    using complex_vector = xcomplex_vector<double, false>;

    TEST(xcomplex_sequence, constructor)
    {
        complex_vector v1;
        EXPECT_TRUE(v1.empty());
        EXPECT_EQ(v1.size(), std::size_t(0));

        complex_vector v2(10);
        EXPECT_FALSE(v2.empty());
        EXPECT_EQ(v2.size(), std::size_t(10));

        complex_vector v3(10, xcomplex<double>(1., 1.));
        EXPECT_FALSE(v3.empty());
        EXPECT_EQ(v3.size(), std::size_t(10));
    }

    TEST(xcomplex_sequence, resize)
    {
        complex_vector v1(2);
        v1.resize(4);
        EXPECT_EQ(v1.size(), 4u);

        v1.resize(8, xcomplex<double>(1., 1.));
        EXPECT_EQ(v1.size(), 8u);
    }

    TEST(xcomplex_sequence, access)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);
        
        complex_vector v = { c0, c1 };
        EXPECT_EQ(v[0], c0);
        EXPECT_EQ(v[1], c1);
    }

    TEST(xcomplex_sequence, at)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);

        complex_vector v = { c0, c1 };
        EXPECT_EQ(v.at(0), c0);
        EXPECT_EQ(v.at(1), c1);
    }

    TEST(xcomplex_sequence, front)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);

        complex_vector v = { c0, c1 };
        EXPECT_EQ(v.front(), c0);
    }

    TEST(xcomplex_sequence, back)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);

        complex_vector v = { c0, c1 };
        EXPECT_EQ(v.back(), c1);
    }

    TEST(xcomplex_sequence, iterator)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);
        xcomplex<double> c2(4., 6.);
        xcomplex<double> c3(8., 12.);

        complex_vector v = { c0, c1, c2, c3 };
        auto iter = v.begin();
        auto citer = v.cbegin();

        EXPECT_EQ(*iter, c0);
        EXPECT_EQ(*citer, c0);

        ++iter, ++citer;
        EXPECT_EQ(*iter, c1);
        EXPECT_EQ(*citer, c1);

        ++iter, ++citer;
        EXPECT_EQ(*iter, c2);
        EXPECT_EQ(*citer, c2);

        ++iter, ++citer;
        EXPECT_EQ(*iter, c3);
        EXPECT_EQ(*citer, c3);

        ++iter, ++citer;
        EXPECT_EQ(iter, v.end());
        EXPECT_EQ(citer, v.cend());
    }

    TEST(xcomplex_sequence, reverse_iterator)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);
        xcomplex<double> c2(4., 6.);
        xcomplex<double> c3(8., 12.);

        complex_vector v = { c0, c1, c2, c3 };
        auto iter = v.rbegin();
        auto citer = v.crbegin();

        EXPECT_EQ(*iter, c3);
        EXPECT_EQ(*citer, c3);

        ++iter, ++citer;
        EXPECT_EQ(*iter, c2);
        EXPECT_EQ(*citer, c2);

        ++iter, ++citer;
        EXPECT_EQ(*iter, c1);
        EXPECT_EQ(*citer, c1);

        ++iter, ++citer;
        EXPECT_EQ(*iter, c0);
        EXPECT_EQ(*citer, c0);

        ++iter, ++citer;
        EXPECT_EQ(iter, v.rend());
        EXPECT_EQ(citer, v.crend());
    }

    TEST(xcomplex_sequence, comparison)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);
        xcomplex<double> c2(4., 6.);
        xcomplex<double> c3(8., 12.);
        complex_vector v1 = { c0, c1, c2, c3 };

        EXPECT_TRUE(v1 == v1);
        EXPECT_FALSE(v1 != v1);
    }

    TEST(xcomplex_sequence, real)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);

        complex_vector v = { c0, c1 };
        EXPECT_EQ(v.real()[0], 1.);
        EXPECT_EQ(v.real()[1], 2.);
    }

    TEST(xcomplex_sequence, imag)
    {
        xcomplex<double> c0(1., 2.);
        xcomplex<double> c1(2., 3.);

        complex_vector v = { c0, c1 };
        EXPECT_EQ(v.imag()[0], 2.);
        EXPECT_EQ(v.imag()[1], 3.);
    }
}
