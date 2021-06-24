#ifndef TEST_COMMON_MACROS_HPP
#define TEST_COMMON_MACROS_HPP

#if defined(XTL_NO_EXCEPTIONS)
 #define DOCTEST_CONFIG_NO_EXCEPTIONS
#endif

#ifdef __APPLE__
#include <iostream>  // https://github.com/onqtam/doctest/issues/126
#endif

#include <string>

#include "doctest/doctest.h"

#if defined(XTL_NO_EXCEPTIONS)

#define XT_EXPECT_THROW(x, y)  #warning "XT_EXPECT_THROW IS DISABLED WHEN EXCEPTIONS ARE DISABLED" ;
#define XT_ASSERT_THROW(x, y)  #warning "XT_ASSERT_THROW IS DISABLED WHEN EXCEPTIONS ARE DISABLED" ;
#define XT_EXPECT_ANY_THROW(x) #warning "XT_EXPECT_ANY_THROW IS DISABLED WHEN EXCEPTIONS ARE DISABLED" ;
#define XT_ASSERT_ANY_THROW(x) #warning "XT_ASSERT_ANY_THROW IS DISABLED WHEN EXCEPTIONS ARE DISABLED" ;
#define XT_EXPECT_NO_THROW(x) x;
#define XT_ASSERT_NO_THROW(x) x;

#else

#define XT_EXPECT_THROW(x, y) CHECK_THROWS_AS(x, y);
#define XT_ASSERT_THROW(x, y) REQUIRE_THROWS_AS(x, y);
#define XT_EXPECT_ANY_THROW(x) CHECK_THROWS_AS(x, std::exception);
#define XT_ASSERT_ANY_THROW(x) REQUIRE_THROWS_AS(x, std::exception);
#define XT_EXPECT_NO_THROW(x) x;
#define XT_ASSERT_NO_THROW(x) x;

#endif

#define EXPECT_NO_THROW(x) XT_EXPECT_NO_THROW(x)
#define EXPECT_THROW(x, y) XT_EXPECT_THROW(x,y)

#define TEST(A,B) TEST_CASE(#A"."#B )
#define EXPECT_EQ(A,B) CHECK_EQ(A,B)
#define EXPECT_NE(A,B) CHECK_NE(A,B)
#define EXPECT_LE(A,B) CHECK_LE(A,B)
#define EXPECT_GE(A,B) CHECK_GE(A,B)
#define EXPECT_LT(A,B) CHECK_LT(A,B)
#define EXPECT_GT(A,B) CHECK_GT(A,B)
#define EXPECT_TRUE(A) CHECK_EQ(A, true)
#define EXPECT_FALSE(A) CHECK_FALSE(A)
#define EXPECT_DOUBLE_EQ(x,y)   CHECK(x == doctest::Approx(y));
#define EXPECT_APPROX_EQ(x,y)   CHECK(x == doctest::Approx(y));
#define EXPECT_STREQ(x,y) EXPECT_EQ(std::string(x),std::string(y))
#define EXPECT_NEAR(x,y,t) CHECK(x == doctest::Approx(y).epsilon(t))


#define EXPECT_COMPLEX_APPROX_EQ(x,y)\
    CHECK((x).real() == doctest::Approx((y).real()));\
    CHECK((x).imag() == doctest::Approx((y).imag()))

#define ASSERT_EQ(A,B) REQUIRE_EQ(A,B)
#define ASSERT_NE(A,B) REQUIRE_NE(A,B)
#define ASSERT_LE(A,B) REQUIRE_LE(A,B)
#define ASSERT_GE(A,B) REQUIRE_GE(A,B)
#define ASSERT_LT(A,B) REQUIRE_LT(A,B)
#define ASSERT_GT(A,B) REQUIRE_GT(A,B)
#define ASSERT_TRUE(A) REQUIRE_EQ(A, true)
#define ASSERT_FALSE(A) REQUIRE_FALSE(A)
#define ASSERT_DOUBLE_EQ(x,y)   REQUIRE(x == doctest::Approx(y));
#define ASSERT_STREQ(x,y) ASSERT_EQ(x,y)
#define ASSERT_NEAR(x,y,t) REQUIRE(x == doctest::Approx(y).epsilon(t))

#endif