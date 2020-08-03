/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xvariant.hpp"

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "xtl/xtl_config.hpp"

#ifdef HAVE_NLOHMANN_JSON
#include "xtl/xjson.hpp"
#endif

using namespace std::literals;

namespace xtl
{
    TEST(xvariant, basic)
    {
        xtl::variant<int, float> v, w;
        v = 12;
        int i = xtl::get<int>(v);
        EXPECT_EQ(12, i);

        w = xtl::get<int>(v);
        EXPECT_EQ(12, xtl::get<int>(w));
        w = xtl::get<0>(v);
        EXPECT_EQ(12, xtl::get<int>(w));
        w = v;
        EXPECT_EQ(12, xtl::get<int>(w));
    }

    TEST(xvariant, bad_variant_access)
    {
        xtl::variant<int, float> v;
        v = 12;
#if defined(XTL_NO_EXCEPTIONS)
        EXPECT_DEATH_IF_SUPPORTED(xtl::get<float>(v), "");
#else
        EXPECT_THROW(xtl::get<float>(v), xtl::bad_variant_access);
#endif
    }

    TEST(xvariant, converting_constructor)
    {
        xtl::variant<std::string> x("abc");
        x = "def";
        EXPECT_EQ(std::string("def"), xtl::get<std::string>(x));
    }

    TEST(xvariant, holds_alternative)
    {
        xtl::variant<std::string, bool> y("abc");
        EXPECT_TRUE(xtl::holds_alternative<bool>(y));
        y = "xyz"s;
        EXPECT_TRUE(xtl::holds_alternative<std::string>(y));
    }

    TEST(xvariant, visit)
    {
        using var_t = xtl::variant<int, long, double, std::string>;
        std::vector<var_t> vec = { 10, 15l, 1.5, "hello" };
        std::vector<var_t> vec_res(4);
        for (std::size_t i = 0; i < vec.size(); ++i)
        {
            var_t v = vec[i];
            var_t w = xtl::visit([](auto&& arg) -> var_t {return arg + arg; }, v);
            vec_res[i] = w;
        }
        EXPECT_EQ(20, xtl::get<0>(vec_res[0]));
        EXPECT_EQ(30l, xtl::get<1>(vec_res[1]));
        EXPECT_EQ(3., xtl::get<2>(vec_res[2]));
        EXPECT_EQ("hellohello", xtl::get<3>(vec_res[3]));
    }

    using variant_ref = xtl::variant<xclosure_wrapper<int&>, xclosure_wrapper<double&>>;

    variant_ref build_test_variant(int& ref)
    {
        variant_ref res(closure(ref));
        return res;
    }

    const variant_ref build_test_cvariant(int& ref)
    {
        variant_ref res(closure(ref));
        return res;
    }

    TEST(xvariant, closure_wrapper)
    {
        {
            int i = 2;
            variant_ref v = closure(i);
            int& ir = xtl::xget<int&>(v);
            EXPECT_EQ(&ir, &i);
            ir = 4;
            EXPECT_EQ(i, 4);
            int ir2 = xtl::xget<int&>(v);
            EXPECT_EQ(ir2, i);

            const int& cir = xtl::xget<const int&>(v);
            EXPECT_EQ(&cir, &i);
        }

        {
            int i = 2;
            const variant_ref v = closure(i);
            const int& ir = xtl::xget<int&>(v);
            EXPECT_EQ(&ir, &i);
            int ir2 = xtl::xget<int&>(v);
            EXPECT_EQ(ir2, i);

            const int& cir = xtl::xget<const int&>(v);
            EXPECT_EQ(&cir, &i);
        }

        {
            int i = 2;
            int& ir = xtl::xget<int&>(build_test_variant(i));
            EXPECT_EQ(&ir, &i);
            ir = 4;
            EXPECT_EQ(i, 4);

            const int& cir = xtl::xget<const int&>(build_test_variant(i));
            EXPECT_EQ(&cir,&i); 
        }

        {
            int i = 2;
            const int& ir = xtl::xget<int&>(build_test_cvariant(i));
            EXPECT_EQ(&ir, &i);
            i = 4;
            EXPECT_EQ(ir, i);

            const int& cir = xtl::xget<const int&>(build_test_cvariant(i));
            EXPECT_EQ(&cir,&i); 
        }
    }

    using variant_cref = xtl::variant<xclosure_wrapper<const int&>, xclosure_wrapper<const double&>>;

    variant_cref build_test_variant(const int& ref)
    {
        variant_cref res(closure(ref));
        return res;
    }

    const variant_cref build_test_cvariant(const int& ref)
    {
        variant_cref res(closure(ref));
        return res;
    }


    TEST(xvariant, const_closure_wrapper)
    {
        {
            const int i = 2;
            variant_cref v = closure(i);
            const int& cir = xtl::xget<const int&>(v);
            EXPECT_EQ(&cir, &i);
        }

        {
            const int i = 2;
            const variant_cref v = closure(i);
            const int& cir = xtl::xget<const int&>(v);
            EXPECT_EQ(&cir, &i);
        }

        {
            const int i = 2;
            const int& cir = xtl::xget<const int&>(build_test_variant(i));
            EXPECT_EQ(&cir, &i);
        }

        {
            const int i = 2;
            const int& cir = xtl::xget<const int&>(build_test_cvariant(i));
            EXPECT_EQ(&cir, &i);
        }
    }

    TEST(xvariant, overloaded_lambdas)
    {
        using var_t = xtl::variant<int, double, std::string>;
        std::vector<var_t> vec = { 1, 2.5, "hello" };

        bool res = true;

        for (auto& v : vec)
        {
            res &= xtl::visit(xtl::make_overload([](int arg) { return arg == 1; },
                                                 [](double arg) { return arg == 2.5; },
                                                 [](const std::string& arg) { return arg == "hello"; }),
                              v);
        }

        EXPECT_TRUE(res);
    }

#ifdef HAVE_NLOHMANN_JSON
    TEST(xvariant, json)
    {
        typedef xtl::variant<double, std::string> stringNumType;

        nlohmann::json j1;
        stringNumType m1 = "Hello";
        j1["salutation"] = m1;
        EXPECT_EQ(j1.at("salutation"), "Hello");

        nlohmann::json j2;
        stringNumType m2 = 3.0;
        j2["num"] = m2;
        EXPECT_EQ(j2.at("num"), 3.0);
    }
#endif

}
