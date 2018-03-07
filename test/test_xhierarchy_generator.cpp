/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "gtest/gtest.h"

#include <type_traits>

#include "xtl/xhierarchy_generator.hpp"

namespace xtl
{
    template <class T>
    class node
    {
    public:

        virtual ~node() {}
        virtual int get_value(T) const = 0;
    };

    using node_list = mpl::vector<int, double, std::string>;
    using interface_type = xscatter_hierarchy_generator<node_list, node>;

    class scatter_test : public interface_type
    {
    public:

        virtual ~scatter_test() {}

        int get_value(int) const override { return 0; }
        int get_value(double) const override { return 1; }
        int get_value(std::string) const override { return 2; }
    };

    TEST(xhierarchy, scattered)
    {
        scatter_test t;
        EXPECT_EQ(0, t.get_value(int(0)));
        EXPECT_EQ(1, t.get_value(double(0)));
        EXPECT_EQ(2, t.get_value(std::string()));
    }

    template <class T, class B>
    class node_implem : public B
    {
    public:

        virtual ~node_implem() {}

        int get_value(T) const override
        {
            if (std::is_same<T, int>::value)
                return 0;
            else if (std::is_same<T, double>::value)
                return 1;
            else
                return 2;
        }
    };

    class scatter_node : public interface_type
    {
    public:

        virtual ~scatter_node() {}

        template <class T>
        int do_get_value(T arg) const
        {
            const node<T>& n = *this;
            return n.get_value(arg);
        }
    };
    using implementation_test = xlinear_hierarchy_generator<node_list, node_implem, scatter_node>;

    TEST(xhierarchy, linear)
    {
        implementation_test t;
        EXPECT_EQ(0, t.do_get_value(int(0)));
        EXPECT_EQ(1, t.do_get_value(double(0)));
        EXPECT_EQ(2, t.do_get_value(std::string()));
    }
}
