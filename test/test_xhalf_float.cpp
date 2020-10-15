/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include "xtl/xhalf_float.hpp"

namespace xtl
{
    TEST(half_float, traits)
    {
        EXPECT_TRUE(xtl::is_scalar<half_float>::value);
        EXPECT_TRUE(xtl::is_arithmetic<half_float>::value);
        EXPECT_TRUE(xtl::is_signed<half_float>::value);
        EXPECT_TRUE(xtl::is_floating_point<half_float>::value);
    }

    TEST(half_float, arithmetic)
    {
        auto f0 = 2.f;
        auto f1 = 3.f;
        half_float h0 = f0;
        half_float h1 = f1;

        EXPECT_EQ((half_float)f0+(half_float)f1, (float)(h0+h1));
        EXPECT_EQ((half_float)f0-(half_float)f1, (float)(h0-h1));
        EXPECT_EQ((half_float)f0*(half_float)f1, (float)(h0*h1));
        EXPECT_EQ((half_float)f0/(half_float)f1, (float)(h0/h1));
    }
}

#ifdef GCC
#pragma GCC diagnostic pop
#endif

