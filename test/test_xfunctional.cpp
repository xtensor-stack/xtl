/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xfunctional.hpp"

#include "gtest/gtest.h"
#include "xtl/xoptional.hpp"

namespace xtl
{
    TEST(xfunctional, select_scalar)
    {
        EXPECT_EQ(select(true, 2., 3.), 2.);
        EXPECT_EQ(select(false, 2., 3.), 3.);
    }
}
