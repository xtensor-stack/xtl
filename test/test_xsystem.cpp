/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xsystem.hpp"

#include "gtest/gtest.h"

namespace xtl
{
    TEST(system, executable_path)
    {
        std::string exec_path = executable_path();
        EXPECT_TRUE(!exec_path.empty());
    }

    TEST(system, prefix_path)
    {
        std::string prefix = prefix_path();
        std::string exec_path = executable_path();

        EXPECT_NE(prefix.size(), exec_path.size());
        EXPECT_TRUE(std::equal(prefix.cbegin(), prefix.cend(), exec_path.cbegin()));
        EXPECT_TRUE((exec_path.find("test_xtl") != std::string::npos) || (exec_path.find("test_xsystem") != std::string::npos));
    }
}
