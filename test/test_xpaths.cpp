/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xpaths.hpp"

#include <iostream>

#include "gtest/gtest.h"

namespace xtl
{
    TEST(paths, executable_path)
    {
        std::cout << "Executable path:" << std::endl;
        std::cout << executable_path() << std::endl;
    }

    TEST(paths, prefix_path)
    {
        std::cout << "Prefix path:" << std::endl;
        std::cout << prefix_path() << std::endl;
    }
}

