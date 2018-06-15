/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include "xtl/xcomplex_sequence.hpp"

namespace xtl
{
    using complex_vector = xcomplex_vector<double, false>;

    TEST(xcomplex_sequence, constructor)
    {
        complex_vector c1;
        EXPECT_TRUE(c1.empty());
        EXPECT_EQ(c1.size(), std::size_t(0));
    }
}