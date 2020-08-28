/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xmultimethods.hpp"

#include "gtest/gtest.h"

namespace xtl
{
    enum class shape_id
    {
        unknown_id,
        rectangle_id,
        circle_id,
        triangle_id
    };

    class shape
    {
    public:

        virtual ~shape() = default;

        shape(const shape&) = delete;
        shape(shape&&) = delete;

        shape& operator=(const shape&) = delete;
        shape& operator=(shape&&) = delete;
        
        virtual shape_id get_id() const = 0;

    protected:

        shape() = default;
    };

    template <shape_id id>
    class shape_impl : public shape
    {
    public:

        shape_impl() = default;

        shape_id get_id() const override
        {
            return id;
        }
    };

    using rectangle = shape_impl<shape_id::rectangle_id>;
    using circle = shape_impl<shape_id::circle_id>;
    using triangle = shape_impl<shape_id::triangle_id>;

    class static_dispatch_tester
    {
    public:

        using return_type = std::pair<shape_id, shape_id>;

        template <class T1, class T2>
        return_type run(const T1& t1, const T2& t2) const
        {
            return std::make_pair(t1.get_id(), t2.get_id());
        }

        return_type on_error(const shape& t1, const shape& t2) const
        {
            return std::make_pair(t1.get_id(), t2.get_id());
        }
    };

    TEST(multimethods, static_dispatch)
    {
        using return_type = static_dispatch_tester::return_type;
        using dispatcher_type = static_dispatcher
        <
            static_dispatch_tester,
            const shape,
            mpl::vector<const rectangle, const circle, const triangle>,
            false,
            return_type
        >;
        rectangle r;
        circle c;
        triangle t;
        shape* p1 = &r;
        shape* p2 = &c;
        shape* p3 = &t;
        static_dispatch_tester tester;

        return_type r1 = dispatcher_type::dispatch(*p1, *p2, tester);
        return_type r2 = dispatcher_type::dispatch(*p1, *p3, tester);
        return_type r3 = dispatcher_type::dispatch(*p2, *p1, tester);
        return_type r4 = dispatcher_type::dispatch(*p2, *p3, tester);
        return_type r5 = dispatcher_type::dispatch(*p3, *p1, tester);
        return_type r6 = dispatcher_type::dispatch(*p3, *p2, tester);

        EXPECT_EQ(r1, return_type(shape_id::rectangle_id, shape_id::circle_id));
        EXPECT_EQ(r2, return_type(shape_id::rectangle_id, shape_id::triangle_id));
        EXPECT_EQ(r3, return_type(shape_id::circle_id, shape_id::rectangle_id));
        EXPECT_EQ(r4, return_type(shape_id::circle_id, shape_id::triangle_id));
        EXPECT_EQ(r5, return_type(shape_id::triangle_id, shape_id::rectangle_id));
        EXPECT_EQ(r6, return_type(shape_id::triangle_id, shape_id::circle_id));
    }

    TEST(multimethods, static_dispatch_symmetry)
    {
        using return_type = static_dispatch_tester::return_type;
        using dispatcher_type = static_dispatcher
        <
            static_dispatch_tester,
            const shape,
            mpl::vector<const rectangle, const circle, const triangle>,
            true,
            return_type
        >;
        rectangle r;
        circle c;
        triangle t;
        shape* p1 = &r;
        shape* p2 = &c;
        shape* p3 = &t;
        static_dispatch_tester tester;

        return_type r1 = dispatcher_type::dispatch(*p1, *p2, tester);
        return_type r2 = dispatcher_type::dispatch(*p1, *p3, tester);
        return_type r3 = dispatcher_type::dispatch(*p2, *p1, tester);
        return_type r4 = dispatcher_type::dispatch(*p2, *p3, tester);
        return_type r5 = dispatcher_type::dispatch(*p3, *p1, tester);
        return_type r6 = dispatcher_type::dispatch(*p3, *p2, tester);

        EXPECT_EQ(r1, return_type(shape_id::rectangle_id, shape_id::circle_id));
        EXPECT_EQ(r2, return_type(shape_id::rectangle_id, shape_id::triangle_id));
        EXPECT_EQ(r3, return_type(shape_id::rectangle_id, shape_id::circle_id));
        EXPECT_EQ(r4, return_type(shape_id::circle_id, shape_id::triangle_id));
        EXPECT_EQ(r5, return_type(shape_id::rectangle_id, shape_id::triangle_id));
        EXPECT_EQ(r6, return_type(shape_id::circle_id, shape_id::triangle_id));
    }
}

