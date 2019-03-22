/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xoptional.hpp"
#include "xtl/xoptional_sequence.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "xtl/xany.hpp"
#include "gtest/gtest.h"

#ifdef HAVE_NLOHMANN_JSON
#include "xtl/xjson.hpp"
#endif


namespace xtl
{
    TEST(xoptional, scalar_tests)
    {
        // Test uninitialized == missing
        xoptional<double, bool> v0;
        ASSERT_FALSE(v0.has_value());

        // Test initialization from value
        xoptional<double, bool> v1(1.0);
        ASSERT_TRUE(v1.has_value());
        ASSERT_EQ(v1.value(), 1.0);

        // Test lvalue closure types
        double value1 = 3.0;
        int there = 0;
        auto opt1 = optional(value1, there);
        ASSERT_FALSE(opt1.has_value());
        opt1 = 1.0;
        ASSERT_TRUE(opt1.has_value());
        ASSERT_EQ(value1, 1.0);

        // Test rvalue closure type for boolean
        double value2 = 3.0;
        auto opt2 = optional(value2, true);
        opt2 = 2.0;
        ASSERT_TRUE(opt2.has_value());
        ASSERT_EQ(value2, 2.0);

        auto ptr_opt2 = &opt2;
        EXPECT_TRUE(ptr_opt2->has_value());
        EXPECT_EQ(ptr_opt2->value(), 2.0);
    }

    TEST(xoptional, string)
    {
        xoptional<std::string, bool> opt1;
        opt1 = "foo";
        ASSERT_TRUE(opt1.has_value());

        xoptional<std::string, bool> opt2 = "bar";
        ASSERT_TRUE(opt2.has_value());
    }

    TEST(xoptional, vector)
    {
        xoptional_vector<double> v(3, 2.0);
        ASSERT_TRUE(v.front().has_value());
        ASSERT_TRUE(v[0].has_value());
        ASSERT_EQ(v[0].value(), 2.0);
        v[1] = missing<double>();
        ASSERT_FALSE(v[1].has_value());
        ASSERT_TRUE(v.has_value().front());
        ASSERT_FALSE(v.has_value()[1]);
    }

    TEST(xoptional, vector_iteration)
    {
        xoptional_vector<double> v(4, 2.0);
        v[0] = missing<double>();
        std::vector<double> res;
        for (auto it = v.cbegin(); it != v.cend(); ++it)
        {
            res.push_back(it->value_or(0.0));
        }
        std::vector<double> expect = {0.0, 2.0, 2.0, 2.0};
        ASSERT_TRUE(std::equal(res.begin(), res.end(), expect.begin()));
    }

    TEST(xoptional, comparison)
    {
        ASSERT_TRUE(optional(1.0, true) == 1.0);
        ASSERT_TRUE(optional(1.0, false) == missing<double>());
        ASSERT_FALSE(missing<double>() == 1.0);
        ASSERT_TRUE(missing<double>() != 1.0);
    }

    TEST(xoptional, vector_comparison)
    {
        xoptional_vector<double> v1(4, 2.0);
        v1[0] = missing<double>();

        xoptional_vector<double> v2(4, 1.0);
        v2[0] = missing<double>();

        EXPECT_TRUE(v1 == v1);
        EXPECT_FALSE(v1 == v2);
        EXPECT_TRUE(v1 != v2);
        EXPECT_FALSE(v1 != v1);
        EXPECT_TRUE(v2 < v1);
        EXPECT_FALSE(v1 < v1);
        EXPECT_TRUE(v1 <= v1);
        EXPECT_FALSE(v1 <= v2);
        EXPECT_TRUE(v1 > v2);
        EXPECT_FALSE(v2 > v1);
        EXPECT_TRUE(v1 >= v1);
        EXPECT_FALSE(v2 >= v1);
    }

    TEST(xoptional, io)
    {
        std::ostringstream oss;
        oss << missing<int>();
        ASSERT_EQ(oss.str(), std::string("N/A"));
    }

    struct implicit_double
    {
        implicit_double(double v) : m_value(v) {}
        double m_value;
    };

    struct explicit_double
    {
        explicit explicit_double(double v) : m_value(v) {}
        double m_value;
    };

    struct implicit_bool
    {
        implicit_bool(bool b) : m_value(b) {}
        bool m_value;
    };

    struct explicit_bool
    {
        explicit explicit_bool(bool b) : m_value(b) {}
        bool m_value;
    };

    TEST(xoptional, implicit_constructor)
    {
        bool res1 = std::is_convertible<double, implicit_double>::value;
        bool res2 = std::is_convertible<double, explicit_double>::value;
        EXPECT_TRUE(res1);
        EXPECT_FALSE(res2);

        bool res3 = std::is_convertible<double, xoptional<implicit_double>>::value;
        bool res4 = std::is_convertible<double, xoptional<explicit_double>>::value;
        EXPECT_TRUE(res3);
        EXPECT_FALSE(res4);

        bool res5 = std::is_convertible<xoptional<double>, xoptional<implicit_double >>::value;
        bool res6 = std::is_convertible<xoptional<double>, xoptional<explicit_double >>::value;
        EXPECT_TRUE(res5);
        EXPECT_FALSE(res6);

        bool res7 = std::is_convertible<xoptional<double, bool>, xoptional<double, implicit_bool>>::value;
        bool res8 = std::is_convertible<xoptional<double, bool>, xoptional<double, explicit_bool>>::value;
        EXPECT_TRUE(res7);
        EXPECT_FALSE(res8);
    }

    TEST(xoptional, xoptional_proxy)
    {
        using optional = xoptional<double, bool>;
        using optional_ref = xoptional<double&, bool&>;
        double d1 = 1.2;
        bool b1 = true;
        double d2 = 2.3;
        bool b2 = true;

        optional_ref o1(d1, b1);
        optional_ref o2(d2, b2);

        auto res1 = o1 + o2;
        EXPECT_EQ(res1, optional(d1 + d2, true));

        auto res2 = o1 - o2;
        EXPECT_EQ(res2, optional(d1 - d2, true));

        auto res3 = o1 * o2;
        EXPECT_EQ(res3, optional(d1 * d2, true));

        auto res4 = o1 / o2;
        EXPECT_EQ(res4, optional(d1 / d2, true));

        xoptional<bool> res7 = o1 < o2;
        EXPECT_TRUE(res7.value());

        double d3 = 4.5;
        bool b3 = true;
        optional_ref o3(d3, b3);

        auto res8 = fma(o1, o2, o3);
        EXPECT_EQ(res8, std::fma(d1, d2, d3));

        using optional_int = xoptional<int, bool>;
        using optional_int_ref = xoptional<int&, bool&>;
        int i1 = 9;
        int i2 = 4;

        optional_int_ref oi1(i1, b1);
        optional_int_ref oi2(i2, b2);

        auto res9 = oi1 % oi2;
        EXPECT_EQ(res9, optional(i1 % i2, true));

        auto res10 = oi1 & oi2;
        EXPECT_EQ(res10, optional(i1 & i2, true));

        auto res11 = oi1 | oi2;
        EXPECT_EQ(res11, optional(i1 | i2, true));

        auto res12 = oi1 ^ oi2;
        EXPECT_EQ(res12, optional(i1 ^ i2, true));

        auto res13 = ~oi1;
        EXPECT_EQ(res13, optional(~i1, true));
        
        auto res5 = oi1 || oi2;
        EXPECT_EQ(res5, optional(i1 || i2, true));

        auto res6 = oi1 && oi2;
        EXPECT_EQ(res6, optional(i1 && i2, true));
    }

    TEST(xoptional, free_functions)
    {
        // Test uninitialized == missing
        xoptional<double, bool> v0;
        ASSERT_FALSE(has_value(v0));

        // Test initialization from value
        xoptional<double, bool> v1(1.0);
        ASSERT_TRUE(has_value(v1));
        ASSERT_EQ(value(v1), 1.0);

        // Test lvalue closure types
        double value1 = 3.0;
        int there = 0;
        auto opt1 = optional(value1, there);
        ASSERT_FALSE(has_value(opt1));
        opt1 = 1.0;
        ASSERT_TRUE(has_value(opt1));
        ASSERT_EQ(value1, 1.0);

        // Test rvalue closure type for boolean
        double value2 = 3.0;
        auto opt2 = optional(value2, true);
        value(opt2) = 2.0;
        ASSERT_TRUE(has_value(opt2));
        ASSERT_EQ(value2, 2.0);
    }

    TEST(xoptional, any)
    {
        using opt_type = xoptional<const double&, const bool&>;
        double d = 1.;
        bool f = true;
        opt_type o(d, f);
        xtl::any a(o);

        opt_type res = any_cast<opt_type>(a);
        EXPECT_EQ(res.value(), o.value());
        EXPECT_EQ(res.has_value(), o.has_value());
    }

#ifdef HAVE_NLOHMANN_JSON
    TEST(xoptional, json)
    {
        xoptional<double> m1 = missing<double>();
        nlohmann::json j1 = m1;
        EXPECT_TRUE(j1.is_null());
        EXPECT_EQ(j1.get<xoptional<double>>(), missing<double>());

        xoptional<double> m2 = 3.0;
        nlohmann::json j2 = m2;
        EXPECT_EQ(j2.get<xoptional<double>>(), 3.0);
    }
#endif

    TEST(xoptional, select)
    {
        using opt_type = xoptional<double, bool>;
        using bool_opt_type = xoptional<bool, bool>;
        auto missing_val = missing<double>();

        EXPECT_EQ(select(true, missing_val, 3.), missing_val);
        EXPECT_EQ(select(false, missing_val, 3.).value(), 3.);
        EXPECT_TRUE(select(false, missing_val, 3.).has_value());
        EXPECT_EQ(select(bool_opt_type(true), 2., 3.).value(), 2.);
        EXPECT_EQ(select(bool_opt_type(false), 2., 3.).value(), 3.);
    }
}
