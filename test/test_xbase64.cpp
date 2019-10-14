/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xbase64.hpp"

#include <complex>

#include "gtest/gtest.h"

namespace xtl
{
    TEST(xbase64, encode)
    {
       EXPECT_EQ(base64encode(""), "");
       EXPECT_EQ(base64encode("f"), "Zg==");
       EXPECT_EQ(base64encode("fo"), "Zm8=");
       EXPECT_EQ(base64encode("foo"), "Zm9v");
       EXPECT_EQ(base64encode("foob"), "Zm9vYg==");
       EXPECT_EQ(base64encode("fooba"), "Zm9vYmE=");
       EXPECT_EQ(base64encode("foobar"), "Zm9vYmFy");
    }

    TEST(xbase64, decode)
    {
       EXPECT_EQ(base64decode(""), "");
       EXPECT_EQ(base64decode("Zg=="), "f");
       EXPECT_EQ(base64decode("Zm8="), "fo");
       EXPECT_EQ(base64decode("Zm9v"), "foo");
       EXPECT_EQ(base64decode("Zm9vYg=="), "foob");
       EXPECT_EQ(base64decode("Zm9vYmE="), "fooba");
       EXPECT_EQ(base64decode("Zm9vYmFy"), "foobar");
    }
}

