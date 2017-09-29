/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include "xtl/xproxy_wrapper.hpp"

namespace xtl
{
    using double_wrapper = xproxy_wrapper<double>;

    double_wrapper func_double()
    {
        return proxy_wrapper(2.5);
    }

    struct Tester
    {
        using value_type = double;

        value_type func() { return 1.; }
        value_type func() const { return 2.; }
    };

    using tester_wrapper = xproxy_wrapper<Tester>;

    tester_wrapper func_tester()
    {
        return proxy_wrapper(Tester());
    }


    TEST(xproxy_wrapper, fundamental_type)
    {
        auto wd = func_double();
        auto pd = &wd;
        
        double d1 = wd;
        double d2 = *pd;
        EXPECT_EQ(2.5, d1);
        EXPECT_EQ(2.5, d2);

        wd = 3.5;
        EXPECT_EQ(3.5, *pd);
    }

    TEST(xproxy_wrapper, user_type)
    {
        auto wu = func_tester();
        auto pu = &wu;

        using wrapper_type = decltype(wu);
        using value_type = wrapper_type::value_type;
        static_assert(std::is_same<value_type, double>::value, "value_type should be double");

        double res = wu.func();
        double res2 = pu->func();

        EXPECT_EQ(1., res);
        EXPECT_EQ(1., res2);
    }
}

