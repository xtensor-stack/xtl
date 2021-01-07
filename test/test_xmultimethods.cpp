/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include "xtl/xmultimethods.hpp"

#include "gtest/gtest.h"

namespace xtl
{
    struct undispatched_type
    {
    };

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

        XTL_IMPLEMENT_INDEXABLE_CLASS()

        virtual ~shape() = default;

        shape(const shape&) = delete;
        shape(shape&&) = delete;

        shape& operator=(const shape&) = delete;
        shape& operator=(shape&&) = delete;

    protected:

        shape() = default;
    };

    template <shape_id id>
    class shape_impl : public shape
    {
    public:

        XTL_IMPLEMENT_INDEXABLE_CLASS()

        shape_impl() = default;
        virtual ~shape_impl() = default;

        shape_id get_id() const
        {
            return id;
        }
    };

    using rectangle = shape_impl<shape_id::rectangle_id>;
    using circle = shape_impl<shape_id::circle_id>;
    using triangle = shape_impl<shape_id::triangle_id>;

    using dispatch_return_type = std::pair<shape_id, shape_id>;

    namespace
    {
        template <class T1, class T2>
        dispatch_return_type dispatch_shape_impl(const T1& t1, const T2& t2)
        {
            return std::make_pair(t1.get_id(), t2.get_id());
        }

#define DEFINE_DISPATCH_SHAPE(T1, T2) \
        dispatch_return_type dispatch_##T1##_##T2(const T1& t1, const T2& t2) \
        { return dispatch_shape_impl(t1, t2); }

        DEFINE_DISPATCH_SHAPE(rectangle, circle)
        DEFINE_DISPATCH_SHAPE(rectangle, triangle)
        DEFINE_DISPATCH_SHAPE(circle, rectangle)
        DEFINE_DISPATCH_SHAPE(circle, triangle)
        DEFINE_DISPATCH_SHAPE(triangle, rectangle)
        DEFINE_DISPATCH_SHAPE(triangle, circle)

        template <class T1, class T2>
        dispatch_return_type partial_dispatch_shape_impl(const T1& t1, const T2& t2, const undispatched_type&)
        {
            return std::make_pair(t1.get_id(), t2.get_id());
        }

#define DEFINE_PARTIAL_DISPATCH_SHAPE(T1, T2) \
        dispatch_return_type partial_dispatch_##T1##_##T2(const T1& t1, const T2& t2, const undispatched_type& i) \
        { return partial_dispatch_shape_impl(t1, t2, i); }

        DEFINE_PARTIAL_DISPATCH_SHAPE(rectangle, circle)
        DEFINE_PARTIAL_DISPATCH_SHAPE(rectangle, triangle)
        DEFINE_PARTIAL_DISPATCH_SHAPE(circle, rectangle)
        DEFINE_PARTIAL_DISPATCH_SHAPE(circle, triangle)
        DEFINE_PARTIAL_DISPATCH_SHAPE(triangle, rectangle)
        DEFINE_PARTIAL_DISPATCH_SHAPE(triangle, circle)
    }

    class dispatch_tester
    {
    public:

        using return_type = dispatch_return_type;

        template <class T1, class T2>
        return_type run(const T1& t1, const T2& t2) const
        {
            return dispatch_shape_impl(t1, t2);
        }

        return_type on_error(const shape&, const shape&) const
        {
            return std::make_pair(shape_id::unknown_id, shape_id::unknown_id);
        }
    };

    TEST(multimethods, static_dispatch)
    {
        using return_type = dispatch_tester::return_type;
        using dispatcher_type = static_dispatcher
        <
            dispatch_tester,
            const shape,
            mpl::vector<const rectangle, const circle, const triangle>,
            return_type
        >;
        rectangle r;
        circle c;
        triangle t;
        shape* p1 = &r;
        shape* p2 = &c;
        shape* p3 = &t;
        dispatch_tester tester;

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
        using return_type = dispatch_tester::return_type;
        using dispatcher_type = static_dispatcher
        <
            dispatch_tester,
            const shape,
            mpl::vector<const rectangle, const circle, const triangle>,
            return_type,
            symmetric_dispatch
        >;
        rectangle r;
        circle c;
        triangle t;
        shape* p1 = &r;
        shape* p2 = &c;
        shape* p3 = &t;
        dispatch_tester tester;

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

    template <class D>
    void test_function_dispatcher()
    {
        using dispatcher_type = D;
        using return_type = dispatch_return_type;
        dispatcher_type d;
        d.template insert<const rectangle, const circle>(&dispatch_rectangle_circle);
        d.template insert<const rectangle, const triangle>(&dispatch_rectangle_triangle);
        d.template insert<const circle, const rectangle>(&dispatch_circle_rectangle);
        d.template insert<const circle, const triangle>(&dispatch_circle_triangle);
        d.template insert<const triangle, const rectangle>(&dispatch_triangle_rectangle);
        d.template insert<const triangle, const circle>(&dispatch_triangle_circle);

        rectangle r;
        circle c;
        triangle t;
        shape* p1 = &r;
        shape* p2 = &c;
        shape* p3 = &t;

        return_type r1 = d.dispatch(*p1, *p2);
        return_type r2 = d.dispatch(*p1, *p3);
        return_type r3 = d.dispatch(*p2, *p1);
        return_type r4 = d.dispatch(*p2, *p3);
        return_type r5 = d.dispatch(*p3, *p1);
        return_type r6 = d.dispatch(*p3, *p2);

        EXPECT_EQ(r1, return_type(shape_id::rectangle_id, shape_id::circle_id));
        EXPECT_EQ(r2, return_type(shape_id::rectangle_id, shape_id::triangle_id));
        EXPECT_EQ(r3, return_type(shape_id::circle_id, shape_id::rectangle_id));
        EXPECT_EQ(r4, return_type(shape_id::circle_id, shape_id::triangle_id));
        EXPECT_EQ(r5, return_type(shape_id::triangle_id, shape_id::rectangle_id));
        EXPECT_EQ(r6, return_type(shape_id::triangle_id, shape_id::circle_id));
    }

    TEST(multimethods, function_dispatcher)
    {
        using return_type = dispatch_return_type;
        using dispatcher_type = functor_dispatcher
        <
            mpl::vector<const shape, const shape>,
            return_type
        >;

        test_function_dispatcher<dispatcher_type>();
    }

    TEST(multimethods, function_dispatcher_static_cast)
    {
        using return_type = dispatch_return_type;
        using dispatcher_type = functor_dispatcher
        <
            mpl::vector<const shape, const shape>,
            return_type,
            mpl::vector<>,
            static_caster
        >;

        test_function_dispatcher<dispatcher_type>();
    }

    TEST(multimethods, fast_function_dispatcher)
    {
        using return_type = dispatch_return_type;
        using dispatcher_type = functor_dispatcher
        <
            mpl::vector<const shape, const shape>,
            return_type,
            mpl::vector<>,
            static_caster,
            basic_fast_dispatcher
        >;

        test_function_dispatcher<dispatcher_type>();
    }

    template <class D>
    void test_function_partial_dispatcher()
    {
        using dispatcher_type = D;
        using return_type = dispatch_return_type;
        dispatcher_type d;
        d.template insert<const rectangle, const circle>(&partial_dispatch_rectangle_circle);
        d.template insert<const rectangle, const triangle>(&partial_dispatch_rectangle_triangle);
        d.template insert<const circle, const rectangle>(&partial_dispatch_circle_rectangle);
        d.template insert<const circle, const triangle>(&partial_dispatch_circle_triangle);
        d.template insert<const triangle, const rectangle>(&partial_dispatch_triangle_rectangle);
        d.template insert<const triangle, const circle>(&partial_dispatch_triangle_circle);

        undispatched_type i;

        rectangle r;
        circle c;
        triangle t;
        shape* p1 = &r;
        shape* p2 = &c;
        shape* p3 = &t;

        return_type r1 = d.dispatch(*p1, *p2, i);
        return_type r2 = d.dispatch(*p1, *p3, i);
        return_type r3 = d.dispatch(*p2, *p1, i);
        return_type r4 = d.dispatch(*p2, *p3, i);
        return_type r5 = d.dispatch(*p3, *p1, i);
        return_type r6 = d.dispatch(*p3, *p2, i);

        EXPECT_EQ(r1, return_type(shape_id::rectangle_id, shape_id::circle_id));
        EXPECT_EQ(r2, return_type(shape_id::rectangle_id, shape_id::triangle_id));
        EXPECT_EQ(r3, return_type(shape_id::circle_id, shape_id::rectangle_id));
        EXPECT_EQ(r4, return_type(shape_id::circle_id, shape_id::triangle_id));
        EXPECT_EQ(r5, return_type(shape_id::triangle_id, shape_id::rectangle_id));
        EXPECT_EQ(r6, return_type(shape_id::triangle_id, shape_id::circle_id));
    }

    TEST(multimethods, fast_function_partial_dispatcher)
    {
        using return_type = dispatch_return_type;
        using dispatcher_type = functor_dispatcher
        <
            mpl::vector<const shape, const shape>,
            return_type,
            mpl::vector<const undispatched_type>,
            static_caster,
            basic_fast_dispatcher
        >;

        test_function_partial_dispatcher<dispatcher_type>();
    }
}
