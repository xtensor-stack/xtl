/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xtype_traits.hpp"

#include "gtest/gtest.h"


namespace xtl
{
    using std::true_type;
    using std::false_type;

    TEST(xtype_traits, conjunction)
    {
        bool res;
        res = conjunction<>::value;
        EXPECT_EQ(res, true);
        res = conjunction<false_type, false_type>::value;
        EXPECT_EQ(res, false);
        res = conjunction<false_type, true_type>::value;
        EXPECT_EQ(res, false);
        res = conjunction<true_type, false_type>::value;
        EXPECT_EQ(res, false);
        res = conjunction<true_type, true_type>::value;
        EXPECT_EQ(res, true);
        res = conjunction<true_type, true_type, true_type>::value;
        EXPECT_EQ(res, true);
        res = conjunction<true_type, true_type, false_type>::value;
        EXPECT_EQ(res, false);
    }

    TEST(xtype_traits, disjunction)
    {
        bool res;
        res = disjunction<>::value;
        EXPECT_EQ(res, false);
        res = disjunction<false_type, false_type>::value;
        EXPECT_EQ(res, false);
        res = disjunction<false_type, true_type>::value;
        EXPECT_EQ(res, true);
        res = disjunction<true_type, false_type>::value;
        EXPECT_EQ(res, true);
        res = disjunction<true_type, true_type>::value;
        EXPECT_EQ(res, true);
        res = disjunction<true_type, true_type, true_type>::value;
        EXPECT_EQ(res, true);
        res = disjunction<true_type, true_type, false_type>::value;
        EXPECT_EQ(res, true);
        res = disjunction<false_type, false_type, false_type>::value;
        EXPECT_EQ(res, false);
    }

    template <class T, XTL_REQUIRES(xtl::is_integral<T>, xtl::is_signed<T>)>
    int test_requires(T);

    template <class T, XTL_REQUIRES_IMPL(xtl::is_integral<T>, xtl::is_signed<T>)>
    int test_requires(T)
    {
        return 0;
    }

    template <class T, XTL_REQUIRES(xtl::is_integral<T>, xtl::negation<xtl::is_signed<T>>)>
    int test_requires(T)
    {
        return 1;
    }

    template <class T, XTL_DISALLOW(xtl::is_integral<T>)>
    int test_requires(T)
    {
        return 2;
    }

    TEST(xtype_traits, requires)
    {
        int i = 0;
        unsigned int ui = 1u;
        double d = 1.;
        EXPECT_EQ(test_requires(i), 0);
        EXPECT_EQ(test_requires(ui), 1);
        EXPECT_EQ(test_requires(d), 2);
    }

    template <class T, XTL_EITHER(xtl::is_integral<T>, xtl::is_floating_point<T>)>
    int test_either_disallow(T)
    {
        return 0;
    }

    template <class T, XTL_DISALLOW_ONE(xtl::is_integral<T>, xtl::is_floating_point<T>)>
    int test_either_disallow(T)
    {
        return 1;
    }

    TEST(xtype_traits, either_disallow)
    {
        int i = 0;
        float d = 1.;
        int* p = &i;
        EXPECT_EQ(test_either_disallow(i), 0);
        EXPECT_EQ(test_either_disallow(d), 0);
        EXPECT_EQ(test_either_disallow(p), 1);
    }

    TEST(xtype_traits, constify)
    {
        bool res = std::is_same<constify_t<int&>, const int&>::value;
        EXPECT_TRUE(res);

        res = std::is_same<constify_t<const int&>, const int&>::value;
        EXPECT_TRUE(res);

        res = std::is_same<constify_t<int*>, const int*>::value;
        EXPECT_TRUE(res);

        res = std::is_same<constify_t<const int*>, const int*>::value;
        EXPECT_TRUE(res);

        res = std::is_same<constify_t<int>, const int>::value;
        EXPECT_TRUE(res);

        res = std::is_same<constify_t<const int>, const int>::value;
        EXPECT_TRUE(res);
    }

    TEST(xtype_traits, promote_type)
    {
        using time_type = std::chrono::system_clock::time_point;
        EXPECT_TRUE((std::is_same<time_type, promote_type_t<time_type, time_type>>::value));
        EXPECT_TRUE((std::is_same<int, promote_type_t<unsigned char, unsigned char>>::value));
        EXPECT_TRUE((std::is_same<std::complex<double>, promote_type_t<unsigned char, std::complex<double>>>::value));
        EXPECT_TRUE((std::is_same<std::complex<double>, promote_type_t<std::complex<float>, std::complex<double>>>::value));
    }
}

