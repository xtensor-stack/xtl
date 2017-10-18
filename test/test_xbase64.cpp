/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include <complex>

#include "xtl/xbase64.hpp"

namespace xtl
{
    TEST(xbase64, encode)
    {
       base64encode("") == "";
       base64encode("f") == "Zg==";
       base64encode("fo") == "Zm8=";
       base64encode("foo") == "Zm9v";
       base64encode("foob") == "Zm9vYg==";
       base64encode("fooba") == "Zm9vYmE=";
       base64encode("foobar") == "Zm9vYmFy";
    }

    TEST(xbase64, decode)
    {
       base64decode("") == "";
       base64decode("f") == "Zg==";
       base64decode("Zm8=") == "fo";
       base64decode("Zm9v") == "foo";
       base64decode("Zm9vYg==") == "foob";
       base64decode("Zm9vYmE=") == "fooba";
       base64decode("Zm9vYmFy") == "foobar";
    }
}

