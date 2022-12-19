/***************************************************************************
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xcompare.hpp"

#include "test_common_macros.hpp"

namespace xtl
{
    TEST(xcompare, cmp_equal)
    {
        EXPECT_TRUE(cmp_equal(0, 0));
        EXPECT_TRUE(cmp_equal(1ul, 1ul));
        EXPECT_TRUE(cmp_equal(1, 1ul));
        EXPECT_TRUE(cmp_equal(1ul, 1));
        EXPECT_TRUE(cmp_equal(-1, -1));

        EXPECT_FALSE(cmp_equal(0, 1));
        EXPECT_FALSE(cmp_equal(-1, 1ul));
        EXPECT_FALSE(cmp_equal(0ul, 1ul));
    }

    TEST(xcompare, cmp_less)
    {
        EXPECT_FALSE(cmp_less(0, 0));
        EXPECT_FALSE(cmp_less(1ul, 1ul));
        EXPECT_FALSE(cmp_less(1, 1ul));
        EXPECT_FALSE(cmp_less(1ul, 1));
        EXPECT_FALSE(cmp_less(-1, -1));

        EXPECT_TRUE(cmp_less(1, 2));
        EXPECT_TRUE(cmp_less(-2, -1));
        EXPECT_TRUE(cmp_less(-2, 1));
        EXPECT_FALSE(cmp_less(1, -1));
    }
}
