/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xhierarchy_generator.hpp"

#include <type_traits>

#include "gtest/gtest.h"


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

        node_implem(int i, double d)
            : B(i, d)
        {
        }

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

        scatter_node(int i, double d)
            : m_i(i), m_d(d)
        {
        }

        virtual ~scatter_node() {}

        template <class T>
        int do_get_value(T arg) const
        {
            const node<T>& n = *this;
            return n.get_value(arg);
        }

        int get_int() const { return m_i; }
        double get_double() const { return m_d; }

    private:

        int m_i;
        double m_d;
    };

    using implementation_test = xlinear_hierarchy_generator<node_list, node_implem, scatter_node>;

    TEST(xhierarchy, linear)
    {
        implementation_test t(1, 2.5);
        EXPECT_EQ(0, t.do_get_value(int(0)));
        EXPECT_EQ(1, t.do_get_value(double(0)));
        EXPECT_EQ(2, t.do_get_value(std::string()));
        EXPECT_EQ(1, t.get_int());
        EXPECT_EQ(2.5, t.get_double());
    }
}
