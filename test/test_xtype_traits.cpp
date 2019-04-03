/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
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

    template <class T, XTL_REQUIRES(std::is_integral<T>, std::is_signed<T>)>
    int test_concept(T);

    template <class T, XTL_REQUIRES_IMPL(std::is_integral<T>, std::is_signed<T>)>
    int test_concept(T)
    {
        return 0;
    }

    template <class T, XTL_REQUIRES(std::is_integral<T>, xtl::negation<std::is_signed<T>>)>
    int test_concept(T)
    {
        return 1;
    }

    template <class T, XTL_REQUIRES(xtl::negation<std::is_integral<T>>)>
    int test_concept(T)
    {
        return 2;
    }

    TEST(xtype_traits, concepts)
    {
        int i = 0;
        unsigned int ui = 1u;
        double d = 1.;
        EXPECT_EQ(test_concept(i), 0);
        EXPECT_EQ(test_concept(ui), 1);
        EXPECT_EQ(test_concept(d), 2);
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
}

