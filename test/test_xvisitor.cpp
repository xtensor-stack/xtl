/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xvisitor.hpp"

#include "gtest/gtest.h"

namespace xtl
{
    class root : public base_visitable<int, false>
    {
    public:

        XTL_DEFINE_VISITABLE()
    };

    class leaf_one : public root
    {
    public:

        XTL_DEFINE_VISITABLE()
    };

    class leaf_two : public root
    {
    public:

        XTL_DEFINE_VISITABLE()
    };

    class acyclic_visitor_tester
        : public base_visitor
        , public visitor<mpl::vector<leaf_one, leaf_two>, int, false>
    {
    public:

        int visit(leaf_one&) override
        {
            return 1;
        }

        int visit(leaf_two&) override
        {
            return 2;
        }
    };

    TEST(visitor, acyclic_visitor)
    {
        leaf_one l1;
        leaf_two l2;
        root* r1 = &l1;
        root* r2 = &l2;

        acyclic_visitor_tester t;

        int res1 = r1->accept(t);
        EXPECT_EQ(res1, 1);

        int res2 = r2->accept(t);
        EXPECT_EQ(res2, 2);
    }

    class const_root : public base_visitable<int, true>
    {
    public:

        XTL_DEFINE_CONST_VISITABLE()
    };

    class const_leaf_one : public const_root
    {
    public:

        XTL_DEFINE_CONST_VISITABLE()
    };

    class const_leaf_two : public const_root
    {
    public:

        XTL_DEFINE_CONST_VISITABLE()
    };

    class const_acyclic_visitor_tester
        : public base_visitor
        , public visitor<mpl::vector<const_leaf_one, const_leaf_two>, int, true>
    {
    public:

        int visit(const const_leaf_one&) override
        {
            return 1;
        }

        int visit(const const_leaf_two&) override
        {
            return 2;
        }
    };

    TEST(visitor, const_acyclic_visitor)
    {
        const_leaf_one l1;
        const_leaf_two l2;
        const_root* r1 = &l1;
        const_root* r2 = &l2;

        const_acyclic_visitor_tester t;

        int res1 = r1->accept(t);
        EXPECT_EQ(res1, 1);

        int res2 = r2->accept(t);
        EXPECT_EQ(res2, 2);

    }
}

