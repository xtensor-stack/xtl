/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xplatform.hpp"


#include "gtest/gtest.h"

namespace xtl
{

    TEST(platform, endian)
    {
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN
        EXPECT_TRUE(endianness() == endian::big_endian);
#endif

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN
        EXPECT_TRUE(endianness() == endian::little_endian);
#endif
    }
}

