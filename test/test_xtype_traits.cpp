/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include "xtl/xtype_traits.hpp"

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
}

