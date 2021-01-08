/***************************************************************************
* Copyright (c) 2016, Sylvain Corlay and Johan Mabille                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/


#include "xtl/xbasic_fixed_string.hpp"

#ifdef HAVE_NLOHMANN_JSON
#include "xtl/xjson.hpp"
#endif

#include "gtest/gtest.h"
#include "xtl/xtl_config.hpp"

namespace xtl
{
    using string_type = xbasic_fixed_string<char, 16, buffer | store_size, string_policy::throwing_error>;
    using numpy_string = xbasic_fixed_string<char, 16, buffer, string_policy::throwing_error>;
    using size_type = string_type::size_type;

    TEST(xfixed_string, constructors)
    {
        // Default constructor
        {
            string_type s;
            EXPECT_EQ(s.size(), size_type(0));
            EXPECT_STREQ(s.c_str(), "");
        }

        // ch repeated n times
        {
            string_type s(size_type(4), 'a');
            EXPECT_EQ(s.size(), size_type(4));
            EXPECT_STREQ(s.c_str(), "aaaa");
        }

        // From C string
        {
            const char* cstr = "constructor";
            string_type s(cstr);
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), cstr);
        }

        // From const char* with \0 inside
        {
            const char* cstr = "construc\0tor";
            string_type s(cstr, 12);
            EXPECT_EQ(s.size(), size_type(12));
            EXPECT_STREQ(s.c_str(), "construc");
        }

        // From substring
        {
            string_type s1("constructor");
            string_type s2(s1, size_type(1), size_type(4));
            EXPECT_EQ(s2.size(), size_type(4));
            EXPECT_STREQ(s2.c_str(), "onst");
            string_type s3(s1, size_type(2), size_type(45));
            EXPECT_EQ(s3.size(), size_type(9));
            EXPECT_STREQ(s3.c_str(), "nstructor");
        }

        // From initializer list
        {
            string_type s = { 'c', 'o', 'n', 's', 't' };
            EXPECT_EQ(s.size(), size_type(5));
            EXPECT_STREQ(s.c_str(), "const");
        }

        // From iterators pair
        {
            std::string s1 = "constructor";
            string_type s2(s1.cbegin(), s1.cend());
            EXPECT_EQ(s2.size(), size_type(11));
            EXPECT_STREQ(s2.c_str(), s1.c_str());
        }

        // From string
        {
            std::string s1 = "constructor";
            string_type s2(s1);
            EXPECT_EQ(s2.size(), size_type(11));
            EXPECT_STREQ(s2.c_str(), s1.c_str());

            string_type s3(s1, size_type(1), size_type(4));
            EXPECT_EQ(s3.size(), size_type(4));
            EXPECT_STREQ(s3.c_str(), "onst");

            std::string s4 = s1;
            EXPECT_EQ(s4.size(), size_type(11));
            EXPECT_STREQ(s4.c_str(), s4.c_str());
        }
    }

    TEST(xfixed_string, copy_semantic)
    {
        string_type ref("constructor");
        string_type ref2("operator=");
        string_type s(ref);
        EXPECT_EQ(s.size(), ref.size());
        EXPECT_STREQ(s.c_str(), ref.c_str());
        s = ref2;
        EXPECT_EQ(s.size(), ref2.size());
        EXPECT_STREQ(s.c_str(), ref2.c_str());
    }

    TEST(xfixed_string, move_semantic)
    {
        string_type ref("constructor");
        string_type ref_tmp(ref);
        string_type ref2("operator=");
        string_type ref2_tmp(ref2);
        string_type s(std::move(ref_tmp));
        EXPECT_EQ(s.size(), ref.size());
        EXPECT_STREQ(s.c_str(), ref.c_str());
        s = std::move(ref2_tmp);
        EXPECT_EQ(s.size(), ref2.size());
        EXPECT_STREQ(s.c_str(), ref2.c_str());
    }

    TEST(xfixed_string, assign_operator)
    {
        string_type ref("reference");

        // From const char*
        {
            const char* cstr = "constructor";
            string_type s(ref);
            s = cstr;
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), cstr);
        }

        // From char
        {
            string_type s(ref);
            s = 'b';
            EXPECT_EQ(s.size(), size_type(1));
            EXPECT_STREQ(s.c_str(), "b");
        }

        // From initializer_list
        {
            string_type s(ref);
            s = { 'c', 'o', 'n', 's', 't' };
            EXPECT_EQ(s.size(), size_type(5));
            EXPECT_STREQ(s.c_str(), "const");
        }

        // From std::string
        {
            string_type s(ref);
            std::string a("const");
            s = a;
            EXPECT_EQ(s.size(), size_type(5));
            EXPECT_STREQ(s.c_str(), "const");
        }
    }

    TEST(xfixed_string, assign)
    {
        string_type ref("reference");

        // ch repeated n times
        {
            string_type s(ref);
            s.assign(size_type(4), 'a');
            EXPECT_EQ(s.size(), size_type(4));
            EXPECT_STREQ(s.c_str(), "aaaa");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.assign(size_type(17), 'a'), "");
#else
            EXPECT_THROW(s.assign(size_type(17), 'a'), std::length_error);
#endif
        }

        // From substring
        {
            string_type to_assign("constructor");
            string_type s(ref);
            s.assign(to_assign, 1, size_type(4));
            EXPECT_EQ(s.size(), size_type(4));
            EXPECT_STREQ(s.c_str(), "onst");
        }

        // From const char* with \0 inside
        {
            const char* cstr = "construc\0tor";
            string_type s(ref);
            s.assign(cstr, size_type(12));
            EXPECT_EQ(s.size(), size_type(12));
            EXPECT_STREQ(s.c_str(), "construc");
            const char* ctothrow = "abcdefghij\0klmnopq";
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.assign(ctothrow, size_type(18)), "");
#else
            EXPECT_THROW(s.assign(ctothrow, size_type(18)), std::length_error);
#endif
        }

        // From const char*
        {
            const char* cstr = "constructor";
            string_type s(ref);
            s.assign(cstr);
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), cstr);
            const char* ctothrow = "abcdefghijklmnopq";
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.assign(ctothrow, size_type(17)), "");
#else
            EXPECT_THROW(s.assign(ctothrow, size_type(17)), std::length_error);
#endif
        }

        // From initializer_list
        {
            string_type s(ref);
            s.assign({ 'c', 'o', 'n', 's', 't' });
            EXPECT_EQ(s.size(), size_type(5));
            EXPECT_STREQ(s.c_str(), "const");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.assign({ 'a','b','c','a','b','c','a','b','c','a','b','c','a','b','c','a','b','c' }), "");
#else
            EXPECT_THROW(s.assign({ 'a','b','c','a','b','c','a','b','c','a','b','c','a','b','c','a','b','c' }), std::length_error);
#endif
        }

        // From iterators pair
        {
            string_type s(ref);
            std::string s1 = "constructor";
            s.assign(s1.cbegin(), s1.cend());
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), s1.c_str());
            std::string ctothrow = "abcdefghijklmnopq";
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.assign(ctothrow.cbegin(), ctothrow.cend()), "");
#else
            EXPECT_THROW(s.assign(ctothrow.cbegin(), ctothrow.cend()), std::length_error);
#endif
        }

        // From lvalue reference
        {
            string_type s(ref);
            string_type s1 = "constructor";
            s.assign(s1);
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), s1.c_str());
        }

        // From rvalue reference
        {
            string_type s(ref);
            string_type s1 = "constructor";
            string_type s1_tmp(s1);
            s.assign(std::move(s1_tmp));
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), s1.c_str());
        }

        // From string
        {
            string_type s(ref);
            std::string s1 = "constructor";
            s.assign(s1);
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), s1.c_str());
            s.assign(s1, 1, size_type(4));
            EXPECT_EQ(s.size(), size_type(4));
            EXPECT_STREQ(s.c_str(), "onst");
        }
    }

    TEST(xfixed_string, element_access)
    {
        string_type s = "element_access";
        s[2] = 'E';
        EXPECT_EQ(s[2], 'E');
        EXPECT_STREQ(s.c_str(), "elEment_access");

        s.at(3) = 'M';
        EXPECT_EQ(s.at(3), 'M');
        EXPECT_STREQ(s.c_str(), "elEMent_access");

        s.front() = 'E';
        EXPECT_EQ(s.front(), 'E');
        EXPECT_STREQ(s.c_str(), "ElEMent_access");

        s.back() = 'S';
        EXPECT_EQ(s.back(), 'S');
        EXPECT_STREQ(s.c_str(), "ElEMent_accesS");

#if defined(XTL_NO_EXCEPTIONS)
        EXPECT_DEATH_IF_SUPPORTED(s.at(15), "");
#else
        EXPECT_THROW(s.at(15), std::out_of_range);
#endif

        EXPECT_STREQ(s.data(), s.c_str());
    }

    TEST(xfixed_string, iterator)
    {
        string_type s = "iterator";
        *(s.begin()) = 'I';
        EXPECT_EQ(*(s.begin()), 'I');
        EXPECT_EQ(*(s.cbegin()), 'I');

        auto iter = s.begin();
        auto citer = s.cbegin();
        for (size_type count = 0; count < s.size(); ++iter, ++citer, ++count) {}
        EXPECT_EQ(iter, s.end());
        EXPECT_EQ(citer, s.cend());

        *(s.rbegin()) = 'R';
        EXPECT_EQ(*(s.rbegin()), 'R');
        EXPECT_EQ(*(s.crbegin()), 'R');

        auto riter = s.rbegin();
        auto criter = s.crbegin();
        for (size_type count = 0; count < s.size(); ++riter, ++criter, ++count) {}
        EXPECT_EQ(riter, s.rend());
        EXPECT_EQ(criter, s.crend());
    }

    TEST(xfixed_string, capacity)
    {
        string_type s = "capacity";
        EXPECT_EQ(s.size(), s.length());
        EXPECT_FALSE(s.empty());
        EXPECT_EQ(s.max_size(), size_type(16));
    }

    TEST(xfixed_string, clear)
    {
        string_type s = "capacity";
        s.clear();
        EXPECT_EQ(s.size(), size_type(0));
        EXPECT_STREQ(s.c_str(), "");
    }

    TEST(xfixed_string, push_back)
    {
        string_type s = "operation";
        s.push_back('s');
        EXPECT_EQ(s.size(), size_type(10));
        EXPECT_STREQ(s.c_str(), "operations");
    }

    TEST(xfixed_string, pop_back)
    {
        string_type s = "operation";
        s.push_back('s');
        EXPECT_EQ(s.size(), size_type(10));
        EXPECT_STREQ(s.c_str(), "operations");
    }

    TEST(xfixed_string, substr)
    {
        string_type ref = "operation";
        string_type s1 = ref.substr(2, 5);
        EXPECT_STREQ(s1.c_str(), "erati");
        string_type s2 = ref.substr(2, 45);
        EXPECT_STREQ(s2.c_str(), "eration");
#if defined(XTL_NO_EXCEPTIONS)
        EXPECT_DEATH_IF_SUPPORTED(ref.substr(15, 4), "");
#else
        EXPECT_THROW(ref.substr(15, 4), std::out_of_range);
#endif
    }

    TEST(xfixed_string, copy)
    {
        string_type ref = "operation";
        string_type dst1 = "aaaa";
        string_type dst2 = "bbbbbb";

        size_type cp1 = ref.copy(dst1.data(), 4, 2);
        EXPECT_EQ(cp1, size_type(4));
        EXPECT_STREQ(dst1.c_str(), "erat");

        size_type cp2 = ref.copy(dst2.data(), 45, 3);
        EXPECT_EQ(cp2, size_type(6));
        EXPECT_STREQ(dst2.c_str(), "ration");

#if defined(XTL_NO_EXCEPTIONS)
        EXPECT_DEATH_IF_SUPPORTED(ref.copy(dst2.data(), 4, 15), "");
#else
        EXPECT_THROW(ref.copy(dst2.data(), 4, 15), std::out_of_range);
#endif
    }

    TEST(xfixed_string, resize)
    {
        string_type s1 = "operation";
        string_type s2 = s1;
        EXPECT_EQ(s1.size(), size_type(9));
        s1.resize(11);
        EXPECT_EQ(s1.size(), size_type(11));
        s2.resize(11, 'a');
        EXPECT_EQ(s2.size(), size_type(11));
        EXPECT_STREQ(s2.c_str(), "operationaa");
    }

    TEST(xfixed_string, swap)
    {
        string_type s1 = "operation";
        string_type s2 = "swap";

        s1.swap(s2);
        EXPECT_STREQ(s1.c_str(), "swap");
        EXPECT_STREQ(s2.c_str(), "operation");

        using std::swap;
        swap(s1, s2);
        EXPECT_STREQ(s1.c_str(), "operation");
        EXPECT_STREQ(s2.c_str(), "swap");
    }

    TEST(xfixed_string, insert)
    {
        string_type ref = "operation";
        {
            string_type s = ref;
            s.insert(3, 2, 'a');
            EXPECT_STREQ(s.c_str(), "opeaaration");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.insert(45, 2, 'b'), "");
#else
            EXPECT_THROW(s.insert(45, 2, 'b'), std::out_of_range);
#endif
        }

        {
            string_type s = ref;
            s.insert(3, "bb");
            EXPECT_STREQ(s.c_str(), "opebbration");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.insert(45, "bb"), "");
#else
            EXPECT_THROW(s.insert(45, "bb"), std::out_of_range);
#endif
        }

        {
            string_type s = ref;
            s.insert(3, "b\0b", 3);
            EXPECT_EQ(s.size(), size_type(12));
            EXPECT_STREQ(s.c_str(), "opeb");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.insert(45, "b\0b", 3), "");
#else
            EXPECT_THROW(s.insert(45, "b\0b", 3), std::out_of_range);
#endif
        }

        {
            string_type s = ref;
            string_type ins = "aa";
            s.insert(3, ins);
            EXPECT_STREQ(s.c_str(), "opeaaration");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.insert(45, ins), "");
#else
            EXPECT_THROW(s.insert(45, ins), std::out_of_range);
#endif
        }

        {
            string_type s = ref;
            string_type ins = "abcdefgh";
            s.insert(3, ins, 2, 2);
            EXPECT_STREQ(s.c_str(), "opecdration");
            s.insert(3, ins, 5, 15);
            EXPECT_STREQ(s.c_str(), "opefghcdration");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.insert(45, ins, 2, 2), "");
#else
            EXPECT_THROW(s.insert(45, ins, 2, 2), std::out_of_range);
#endif
        }

        {
            string_type s = ref;
            std::string ins = "aa";
            s.insert(3, ins);
            EXPECT_STREQ(s.c_str(), "opeaaration");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.insert(45, ins), "");
#else
            EXPECT_THROW(s.insert(45, ins), std::out_of_range);
#endif
        }

        {
            string_type s = ref;
            std::string ins = "abcdefgh";
            s.insert(3, ins, 2, 2);
            EXPECT_STREQ(s.c_str(), "opecdration");
            s.insert(3, ins, 5, 15);
            EXPECT_STREQ(s.c_str(), "opefghcdration");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.insert(45, ins, 2, 2), "");
#else
            EXPECT_THROW(s.insert(45, ins, 2, 2), std::out_of_range);
#endif
        }

        {
            string_type s = ref;
            s.insert(s.begin() + 3, 'b');
            EXPECT_STREQ(s.c_str(), "opebration");
        }

        {
            string_type s = ref;
            s.insert(s.begin() + 3, 2, 'b');
            EXPECT_STREQ(s.c_str(), "opebbration");
        }

        {
            string_type s = ref;
            s.insert(s.begin() + 3, { 'a', 'b', 'c' });
            EXPECT_STREQ(s.c_str(), "opeabcration");
        }

        {
            string_type s = ref;
            string_type ins = "insert";
            s.insert(s.begin() + 3, ins.cbegin() + 2, ins.cbegin() + 4);;
            EXPECT_STREQ(s.c_str(), "opeseration");
        }
    }

    TEST(xfixed_string, erase)
    {
        string_type ref = "operation";

        {
            string_type s = ref;
            s.erase(2, 2);
            EXPECT_STREQ(s.c_str(), "opation");
            s.erase();
            EXPECT_STREQ(s.c_str(), "");
        }

        {
            string_type s = ref;
            s.erase(s.cbegin() + 2);
            EXPECT_STREQ(s.c_str(), "opration");
        }

        {
            string_type s = ref;
            s.erase(s.cbegin() + 2, s.cbegin() + 4);
            EXPECT_STREQ(s.c_str(), "opation");
        }
    }

    TEST(xfixed_string, append)
    {
        string_type ref = "operation";

        {
            string_type s = ref;
            s.append(2, 'a');
            EXPECT_STREQ(s.c_str(), "operationaa");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.append(15, 'a'), "");
#else
            EXPECT_THROW(s.append(15, 'a'), std::length_error);
#endif
        }

        {
            string_type s = ref;
            string_type ap = "abc";
            s.append(ap);
            EXPECT_STREQ(s.c_str(), "operationabc");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.append(string_type("operation")), "");
#else
            EXPECT_THROW(s.append(string_type("operation")), std::length_error);
#endif
        }

        {
            string_type s = ref;
            string_type ap = "abc";
            s.append(ap, 1, 2);
            EXPECT_STREQ(s.c_str(), "operationbc");
            s.append(ap, 1, 15);
            EXPECT_STREQ(s.c_str(), "operationbcbc");
        }

        {
            string_type s = ref;
            std::string ap = "abc";
            s.append(ap);
            EXPECT_STREQ(s.c_str(), "operationabc");
#if defined(XTL_NO_EXCEPTIONS)
            EXPECT_DEATH_IF_SUPPORTED(s.append(string_type("operation")), "");
#else
            EXPECT_THROW(s.append(string_type("operation")), std::length_error);
#endif
        }

        {
            string_type s = ref;
            std::string ap = "abc";
            s.append(ap, 1, 2);
            EXPECT_STREQ(s.c_str(), "operationbc");
            s.append(ap, 1, 15);
            EXPECT_STREQ(s.c_str(), "operationbcbc");
        }

        {
            string_type s = ref;
            const char* ap = "ab\0cde";
            s.append(ap, 6);
            EXPECT_EQ(s.size(), size_type(15));
            EXPECT_STREQ(s.c_str(), "operationab");
        }

        {
            string_type s = ref;
            const char* ap = "ab\0cde";
            s.append(ap);
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), "operationab");
        }

        {
            string_type s = ref;
            s.append({ 'a', 'b', 'c' });
            EXPECT_STREQ(s.c_str(), "operationabc");
        }

        {
            string_type s = ref;
            std::string ap = "append";
            s.append(ap.cbegin() + 1, ap.cbegin() + 5);
            EXPECT_STREQ(s.c_str(), "operationppen");
        }
    }

    TEST(xfixed_string, operator_append)
    {
        string_type ref = "operation";

        {
            string_type s = ref;
            string_type ap = "abc";
            s += (ap);
            EXPECT_STREQ(s.c_str(), "operationabc");
        }

        {
            string_type s = ref;
            std::string ap = "abc";
            s += (ap);
            EXPECT_STREQ(s.c_str(), "operationabc");
        }

        {
            string_type s = ref;
            s += 'a';
            EXPECT_STREQ(s.c_str(), "operationa");
        }

        {
            string_type s = ref;
            const char* ap = "ab";
            s += (ap);
            EXPECT_EQ(s.size(), size_type(11));
            EXPECT_STREQ(s.c_str(), "operationab");
        }

        {
            string_type s = ref;
            s += { 'a', 'b', 'c' };
            EXPECT_STREQ(s.c_str(), "operationabc");
        }
    }

    TEST(xfixed_string, compare)
    {
        string_type s1 = "aabcdef";
        string_type s2 = "abcdefg";
        string_type s3 = "aabcd";

        {
            int r1 = s1.compare(s2);
            EXPECT_LT(r1, 0);
            int r2 = s1.compare(s1);
            EXPECT_EQ(r2, 0);
            int r3 = s2.compare(s1);
            EXPECT_GT(r3, 0);
            int r4 = s1.compare(s3);
            EXPECT_GT(r4, 0);
            int r5 = s3.compare(s1);
            EXPECT_LT(r5, 0);
        }

        {
            int r1 = s1.compare(s2.c_str());
            EXPECT_LT(r1, 0);
            int r2 = s1.compare(s1.c_str());
            EXPECT_EQ(r2, 0);
            int r3 = s2.compare(s1.c_str());
            EXPECT_GT(r3, 0);
            int r4 = s1.compare(s3.c_str());
            EXPECT_GT(r4, 0);
            int r5 = s3.compare(s1.c_str());
            EXPECT_LT(r5, 0);
        }

        {
            int r1 = s2.compare(0, 5, s3);
            EXPECT_GT(r1, 0);
            int r2 = s1.compare(0, 5, s3);
            EXPECT_EQ(r2, 0);
        }

        {
            int r1 = s2.compare(0, 5, s3.c_str());
            EXPECT_GT(r1, 0);
            int r2 = s1.compare(0, 5, s3.c_str());
            EXPECT_EQ(r2, 0);
        }

        {
            int r1 = s1.compare(2, 3, s2, 2, 3);
            EXPECT_LT(r1, 0);
            int r2 = s1.compare(2, 3, s1, 2, 3);
            EXPECT_EQ(r2, 0);
            int r3 = s2.compare(2, 3, s1, 2, 3);
            EXPECT_GT(r3, 0);
            int r4 = s1.compare(2, 3, s3, 1, 2);
            EXPECT_GT(r4, 0);
            int r5 = s3.compare(1, 2, s1, 2, 3);
            EXPECT_LT(r5, 0);
        }

        {
            int r1 = s1.compare(2, 3, s2.c_str() + 2, 3);
            EXPECT_LT(r1, 0);
            int r2 = s1.compare(2, 3, s1.c_str() + 2, 3);
            EXPECT_EQ(r2, 0);
            int r3 = s2.compare(2, 3, s1.c_str() + 2, 3);
            EXPECT_GT(r3, 0);
            int r4 = s1.compare(2, 3, s3.c_str() + 1, 2);
            EXPECT_GT(r4, 0);
            int r5 = s3.compare(1, 2, s1.c_str() + 2, 3);
            EXPECT_LT(r5, 0);
        }

        {
            std::string str1(s1.cbegin(), s1.cend());
            std::string str2(s2.cbegin(), s2.cend());
            std::string str3(s3.cbegin(), s3.cend());

            int r1 = s1.compare(str2);
            EXPECT_LT(r1, 0);
            int r2 = s1.compare(str1);
            EXPECT_EQ(r2, 0);
            int r3 = s2.compare(str1);
            EXPECT_GT(r3, 0);
            int r4 = s1.compare(str3);
            EXPECT_GT(r4, 0);
            int r5 = s3.compare(str1);
            EXPECT_LT(r5, 0);
        }
    }

    TEST(xfixed_string, replace)
    {
        string_type ref = "replace";
        string_type s = ref;

        string_type rep1 = "abc";
        s.replace(1, 4, rep1);
        EXPECT_STREQ(s.c_str(), "rabcce");

        string_type rep2 = "epla";
        s.replace(s.cbegin() + 1, s.cbegin() + 4, rep2);
        EXPECT_STREQ(s.c_str(), ref.c_str());

        string_type rep3 = "operation";
        s.replace(2, 2, rep3, 3, 4);
        EXPECT_STREQ(s.c_str(), "reratiace");

        const char* rep4 = "plabc";
        s.replace(2, 4, rep4, 2);
        EXPECT_STREQ(s.c_str(), ref.c_str());

        s.replace(s.cbegin() + 2, s.cbegin() + 4, rep3.c_str() + 3, 4);
        EXPECT_STREQ(s.c_str(), "reratiace");

        const char* rep5 = "pl";
        s.replace(2, 4, rep5);
        EXPECT_STREQ(s.c_str(), ref.c_str());

        s.replace(s.cbegin() + 2, s.cbegin() + 4, rep4);
        EXPECT_STREQ(s.c_str(), "replabcace");

        s.replace(2, 5, 3, 'c');
        EXPECT_STREQ(s.c_str(), "recccace");

        s.replace(s.cbegin() + 2, s.cbegin() + 5, 2, 'b');
        EXPECT_STREQ(s.c_str(), "rebbace");

        s.replace(s.cbegin() + 2, s.cbegin() + 4, { 'p', 'l' });
        EXPECT_STREQ(s.c_str(), ref.c_str());

        s.replace(s.cbegin() + 2, s.cbegin() + 4, rep4 + 2, rep4 + 4);
        EXPECT_STREQ(s.c_str(), "reabace");

        std::string rep6 = "pl";
        s.replace(2, 2, rep6);
        EXPECT_STREQ(s.c_str(), ref.c_str());

        std::string rep7 = "zyx";
        s.replace(s.cbegin() + 2, s.cbegin() + 4, rep7);
        EXPECT_STREQ(s.c_str(), "rezyxace");

        std::string rep8 = "epla";
        s.replace(2, 3, rep8, 1, 2);
        EXPECT_STREQ(s.c_str(), ref.c_str());
    }

    TEST(xfixed_string, find)
    {
        string_type ref = "operationftionf";
        string_type sub = "tionf";

        size_type r1 = ref.find(sub, 2);
        EXPECT_EQ(r1, size_type(5));
        size_type r2 = ref.find(sub, 11);
        EXPECT_EQ(r2, string_type::npos);

        size_type r3 = ref.find(sub.c_str(), 2, 3);
        EXPECT_EQ(r3, size_type(5));
        size_type r4 = ref.find(sub.c_str(), 11, 3);
        EXPECT_EQ(r4, string_type::npos);

        size_type r5 = ref.find(sub.c_str(), 2);
        EXPECT_EQ(r5, size_type(5));
        size_type r6 = ref.find(sub.c_str(), 11);
        EXPECT_EQ(r6, string_type::npos);

        size_type r7 = ref.find('a', 2);
        EXPECT_EQ(r7, size_type(4));
        size_type r8 = ref.find('a', 8);
        EXPECT_EQ(r8, string_type::npos);

        std::string ssub(sub.cbegin(), sub.cend());
        size_type r9 = ref.find(ssub, 2);
        EXPECT_EQ(r9, size_type(5));
        size_type r10 = ref.find(ssub, 11);
        EXPECT_EQ(r10, string_type::npos);
    }

    TEST(xfixed_string, rfind)
    {
        string_type ref = "operateration";
        string_type sub = "erat";

        size_type r1 = ref.rfind(sub, 12);
        EXPECT_EQ(r1, size_type(6));
        size_type r2 = ref.rfind(sub, 1);
        EXPECT_EQ(r2, string_type::npos);

        size_type r3 = ref.rfind(sub.c_str(), 12, 3);
        EXPECT_EQ(r3, size_type(6));
        size_type r4 = ref.rfind(sub.c_str(), 1, 3);
        EXPECT_EQ(r4, string_type::npos);

        size_type r5 = ref.rfind(sub.c_str(), 12);
        EXPECT_EQ(r5, size_type(6));
        size_type r6 = ref.rfind(sub.c_str(), 1);
        EXPECT_EQ(r6, string_type::npos);

        size_type r7 = ref.rfind('a');
        EXPECT_EQ(r7, size_type(8));
        size_type r8 = ref.rfind('a', 2);
        EXPECT_EQ(r8, string_type::npos);

        std::string ssub(sub.cbegin(), sub.cend());
        size_type r9 = ref.rfind(ssub, 12);
        EXPECT_EQ(r9, size_type(6));
        size_type r10 = ref.rfind(ssub, 1);
        EXPECT_EQ(r10, string_type::npos);
    }

    TEST(xfixed_string, find_first_of)
    {
        string_type ref = "Hello World!";
        string_type sub = "Good Bye!";
        string_type sub2 = "eo";

        size_type r1 = ref.find_first_of(sub, 1);
        EXPECT_EQ(r1, size_type(1));
        size_type r2 = ref.find_first_of(sub, 3);
        EXPECT_EQ(r2, size_type(4));
        size_type r3 = ref.find_first_of(sub2, 8);
        EXPECT_EQ(r3, string_type::npos);

        size_type r4 = ref.find_first_of(sub.c_str(), 1);
        EXPECT_EQ(r4, size_type(1));
        size_type r5 = ref.find_first_of(sub.c_str(), 3);
        EXPECT_EQ(r5, size_type(4));
        size_type r6 = ref.find_first_of(sub2.c_str(), 8);
        EXPECT_EQ(r6, string_type::npos);

        size_type r7 = ref.find_first_of(sub.c_str(), 1, 4);
        EXPECT_EQ(r7, size_type(4));
        size_type r8 = ref.find_first_of(sub.c_str(), 5, 4);
        EXPECT_EQ(r8, size_type(7));
        size_type r9 = ref.find_first_of(sub2.c_str(), 8, 2);
        EXPECT_EQ(r9, string_type::npos);

        size_type r10 = ref.find_first_of('o', 2);
        EXPECT_EQ(r10, size_type(4));
        size_type r11 = ref.find_first_of('o', 5);
        EXPECT_EQ(r11, size_type(7));
        size_type r12 = ref.find_first_of('o', 8);
        EXPECT_EQ(r12, string_type::npos);

        std::string ssub = "Good Bye!";
        std::string ssub2 = "eo";
        size_type r13 = ref.find_first_of(ssub, 1);
        EXPECT_EQ(r13, size_type(1));
        size_type r14 = ref.find_first_of(ssub, 3);
        EXPECT_EQ(r14, size_type(4));
        size_type r15 = ref.find_first_of(ssub2, 8);
        EXPECT_EQ(r15, string_type::npos);
    }

    TEST(xfixed_string, find_first_not_of)
    {
        string_type ref = "Hello World!";
        string_type sub = "eo";

        size_type r1 = ref.find_first_not_of(sub, 1);
        EXPECT_EQ(r1, size_type(2));
        size_type r2 = ref.find_first_not_of(sub, 4);
        EXPECT_EQ(r2, size_type(5));
        size_type r3 = ref.find_first_not_of(ref, 4);
        EXPECT_EQ(r3, string_type::npos);

        size_type r4 = ref.find_first_not_of(sub.c_str(), 1);
        EXPECT_EQ(r4, size_type(2));
        size_type r5 = ref.find_first_not_of(sub.c_str(), 4);
        EXPECT_EQ(r5, size_type(5));
        size_type r6 = ref.find_first_not_of(ref.c_str(), 4);
        EXPECT_EQ(r6, string_type::npos);

        size_type r7 = ref.find_first_not_of(sub.c_str(), 1, 2);
        EXPECT_EQ(r7, size_type(2));
        size_type r8 = ref.find_first_not_of(sub.c_str(), 4, 2);
        EXPECT_EQ(r8, size_type(5));
        size_type r9 = ref.find_first_not_of(ref.c_str(), 4, 12);
        EXPECT_EQ(r9, string_type::npos);

        size_type r10 = ref.find_first_not_of('l', 2);
        EXPECT_EQ(r10, size_type(4));

        std::string ssub = "eo";
        size_type r11 = ref.find_first_not_of(ssub, 1);
        EXPECT_EQ(r11, size_type(2));
        size_type r12 = ref.find_first_not_of(ssub, 4);
        EXPECT_EQ(r12, size_type(5));
    }

    TEST(xfixed_string, find_last_of)
    {
        string_type ref = "path/to/file";
        string_type sub = "/f";

        size_type r1 = ref.find_last_of(sub, 11);
        EXPECT_EQ(r1, size_type(8));
        size_type r2 = ref.find_last_of(sub, 6);
        EXPECT_EQ(r2, size_type(4));
        size_type r3 = ref.find_last_of(sub, 3);
        EXPECT_EQ(r3, string_type::npos);

        size_type r4 = ref.find_last_of(sub.c_str(), 11);
        EXPECT_EQ(r4, size_type(8));
        size_type r5 = ref.find_last_of(sub.c_str(), 6);
        EXPECT_EQ(r5, size_type(4));
        size_type r6 = ref.find_last_of(sub.c_str(), 3);
        EXPECT_EQ(r6, string_type::npos);

        size_type r7 = ref.find_last_of(sub.c_str(), 11, 2);
        EXPECT_EQ(r7, size_type(8));
        size_type r8 = ref.find_last_of(sub.c_str(), 6, 2);
        EXPECT_EQ(r8, size_type(4));
        size_type r9 = ref.find_last_of(sub.c_str(), 3, 2);
        EXPECT_EQ(r9, string_type::npos);

        size_type r10 = ref.find_last_of('/', 11);
        EXPECT_EQ(r10, size_type(7));
        size_type r11 = ref.find_last_of('/', 6);
        EXPECT_EQ(r11, size_type(4));
        size_type r12 = ref.find_last_of('/', 3);
        EXPECT_EQ(r12, string_type::npos);

        std::string ssub = "/f";
        size_type r13 = ref.find_last_of(ssub, 11);
        EXPECT_EQ(r13, size_type(8));
        size_type r14 = ref.find_last_of(ssub, 6);
        EXPECT_EQ(r14, size_type(4));
        size_type r15 = ref.find_last_of(ssub, 3);
        EXPECT_EQ(r15, string_type::npos);
    }

    TEST(xfixed_string, find_last_not_of)
    {
        string_type ref = "path/to/file";
        string_type sub = "/f";

        size_type r1 = ref.find_last_not_of(sub, 11);
        EXPECT_EQ(r1, size_type(11));
        size_type r2 = ref.find_last_not_of(sub, 4);
        EXPECT_EQ(r2, size_type(3));

        size_type r3 = ref.find_last_not_of(sub.c_str(), 11);
        EXPECT_EQ(r3, size_type(11));
        size_type r4 = ref.find_last_not_of(sub.c_str(), 4);
        EXPECT_EQ(r4, size_type(3));

        size_type r5 = ref.find_last_not_of(sub.c_str(), 11, 2);
        EXPECT_EQ(r5, size_type(11));
        size_type r6 = ref.find_last_not_of(sub.c_str(), 4, 2);
        EXPECT_EQ(r6, size_type(3));

        size_type r10 = ref.find_last_not_of('/', 11);
        EXPECT_EQ(r10, size_type(11));
        size_type r11 = ref.find_last_not_of('/', 4);
        EXPECT_EQ(r11, size_type(3));

        std::string ssub = "/f";
        size_type r12 = ref.find_last_not_of(ssub, 11);
        EXPECT_EQ(r12, size_type(11));
        size_type r13 = ref.find_last_not_of(ssub, 4);
        EXPECT_EQ(r13, size_type(3));
    }

    TEST(xfixed_string, concatenation)
    {
        string_type s1 = "opera";
        string_type s2 = "tions";
        string_type ref = "operations";

        string_type res1 = s1 + s2;
        EXPECT_STREQ(res1.c_str(), ref.c_str());
        string_type res2 = s1 + s2.c_str();
        EXPECT_STREQ(res2.c_str(), ref.c_str());
        string_type res3 = s1 + 'b';
        EXPECT_STREQ(res3.c_str(), "operab");
        string_type res4 = s1.c_str() + s2;
        EXPECT_STREQ(res4.c_str(), ref.c_str());
        string_type res5 = 'v' + s1;
        EXPECT_STREQ(res5.c_str(), "vopera");

        string_type s1bu = s1;
        string_type res6 = std::move(s1bu) + s2;
        EXPECT_STREQ(res6.c_str(), ref.c_str());

        string_type s2bu = s2;
        string_type res7 = s1 + std::move(s2bu);
        EXPECT_STREQ(res7.c_str(), ref.c_str());

        s1bu = s1;
        s2bu = s2;
        string_type res8 = std::move(s1bu) + std::move(s2bu);
        EXPECT_STREQ(res8.c_str(), ref.c_str());

        s1bu = s1;
        string_type res9 = std::move(s1bu) + s2.c_str();
        EXPECT_STREQ(res9.c_str(), ref.c_str());

        s1bu = s1;
        string_type res10 = std::move(s1bu) + 'b';
        EXPECT_STREQ(res10.c_str(), "operab");

        s2bu = s2;
        string_type res11 = s1.c_str() + std::move(s2bu);
        EXPECT_STREQ(res11.c_str(), ref.c_str());

        s2bu = s2;
        string_type res12 = 'b' + std::move(s2bu);
        EXPECT_STREQ(res12.c_str(), "btions");
    }

    TEST(xfixed_string, comparison_operators)
    {
        string_type s1 = "aabcdef";
        string_type s2 = "abcdefg";
        string_type s3 = "aabcd";

        EXPECT_TRUE(s1 < s2);
        EXPECT_TRUE(s1 <= s2);
        EXPECT_TRUE(s1 <= s1);
        EXPECT_TRUE(s1 == s1);
        EXPECT_TRUE(s1 >= s1);
        EXPECT_TRUE(s2 > s1);
        EXPECT_TRUE(s2 >= s1);

        EXPECT_TRUE(s1.c_str() < s2);
        EXPECT_TRUE(s1.c_str() <= s2);
        EXPECT_TRUE(s1.c_str() <= s1);
        EXPECT_TRUE(s1.c_str() == s1);
        EXPECT_TRUE(s1.c_str() >= s1);
        EXPECT_TRUE(s2.c_str() > s1);
        EXPECT_TRUE(s2.c_str() >= s1);

        EXPECT_TRUE(s1 < s2.c_str());
        EXPECT_TRUE(s1 <= s2.c_str());
        EXPECT_TRUE(s1 <= s1.c_str());
        EXPECT_TRUE(s1 == s1.c_str());
        EXPECT_TRUE(s1 >= s1.c_str());
        EXPECT_TRUE(s2 > s1.c_str());
        EXPECT_TRUE(s2 >= s1.c_str());

        std::string ss1 = "aabcdef";
        std::string ss2 = "abcdefg";
        std::string ss3 = "aabcd";

        EXPECT_TRUE(s1 < ss2);
        EXPECT_TRUE(s1 <= ss2);
        EXPECT_TRUE(s1 <= ss1);
        EXPECT_TRUE(s1 == ss1);
        EXPECT_TRUE(s1 >= ss1);
        EXPECT_TRUE(s2 > ss1);
        EXPECT_TRUE(s2 >= ss1);
    }

    TEST(xfixed_string, input_output)
    {
        std::string s("input_output");
        string_type ref = "input_output";
        string_type res;

        std::istringstream iss(s);
        iss >> res;
        EXPECT_STREQ(ref.c_str(), res.c_str());

        std::ostringstream oss;
        oss << ref;
        std::string res2 = oss.str();
        EXPECT_STREQ(res2.c_str(), ref.c_str());
    }

    TEST(xfixed_string, hash)
    {
        std::hash<string_type> h;
        std::size_t res = h("test");
        EXPECT_TRUE(res != std::size_t(0));
    }

    TEST(numpy_string, constructor)
    {
        std::string s = "thisisatest";
        char buf[16];
        strcpy(buf, s.c_str());
        numpy_string* x = reinterpret_cast<numpy_string*>(buf);

        EXPECT_EQ(*x, s);
        (*x)[4] = '!';
        EXPECT_EQ(buf[4], '!');
        EXPECT_EQ(buf, *x);
    }

    TEST(numpy_string, element_access)
    {
        char buf[16] = "element_access";
        numpy_string& s = *reinterpret_cast<numpy_string*>(buf);
        s[2] = 'E';
        EXPECT_EQ(s[2], 'E');
        EXPECT_STREQ(s.c_str(), "elEment_access");

        s.at(3) = 'M';
        EXPECT_EQ(s.at(3), 'M');
        EXPECT_STREQ(s.c_str(), "elEMent_access");

        s.front() = 'E';
        EXPECT_EQ(s.front(), 'E');
        EXPECT_STREQ(s.c_str(), "ElEMent_access");

        s.back() = 'S';
        EXPECT_EQ(s.back(), 'S');
        EXPECT_STREQ(s.c_str(), "ElEMent_accesS");

#if defined(XTL_NO_EXCEPTIONS)
        EXPECT_DEATH_IF_SUPPORTED(s.at(15), "");
#else
        EXPECT_THROW(s.at(15), std::out_of_range);
#endif

        EXPECT_STREQ(s.data(), s.c_str());
        EXPECT_STREQ(s.data(), buf);
    }

    TEST(numpy_string, iterator)
    {
        numpy_string s("iterator");
        *(s.begin()) = 'I';
        EXPECT_EQ(*(s.begin()), 'I');
        EXPECT_EQ(*(s.cbegin()), 'I');

        auto iter = s.begin();
        auto citer = s.cbegin();
        for (size_type count = 0; count < s.size(); ++iter, ++citer, ++count) {}
        EXPECT_EQ(iter, s.end());
        EXPECT_EQ(citer, s.cend());

        *(s.rbegin()) = 'R';
        EXPECT_EQ(*(s.rbegin()), 'R');
        EXPECT_EQ(*(s.crbegin()), 'R');

        auto riter = s.rbegin();
        auto criter = s.crbegin();
        for (size_type count = 0; count < s.size(); ++riter, ++criter, ++count) {}
        EXPECT_EQ(riter, s.rend());
        EXPECT_EQ(criter, s.crend());
    }

    TEST(numpy_string, find)
    {
        numpy_string ref("operationftionf");
        numpy_string sub("tionf");

        size_type r1 = ref.find(sub, 2);
        EXPECT_EQ(r1, size_type(5));
        size_type r2 = ref.find(sub, 11);
        EXPECT_EQ(r2, string_type::npos);

        size_type r3 = ref.find(sub.c_str(), 2, 3);
        EXPECT_EQ(r3, size_type(5));
        size_type r4 = ref.find(sub.c_str(), 11, 3);
        EXPECT_EQ(r4, string_type::npos);

        size_type r5 = ref.find(sub.c_str(), 2);
        EXPECT_EQ(r5, size_type(5));
        size_type r6 = ref.find(sub.c_str(), 11);
        EXPECT_EQ(r6, string_type::npos);

        size_type r7 = ref.find('a', 2);
        EXPECT_EQ(r7, size_type(4));
        size_type r8 = ref.find('a', 8);
        EXPECT_EQ(r8, string_type::npos);

        std::string ssub(sub.cbegin(), sub.cend());
        size_type r9 = ref.find(ssub, 2);
        EXPECT_EQ(r9, size_type(5));
        size_type r10 = ref.find(ssub, 11);
        EXPECT_EQ(r10, string_type::npos);
    }

#ifdef HAVE_NLOHMANN_JSON
    TEST(xfixed_string, json)
    {
        string_type s1 = "hello";
        nlohmann::json j1 = s1;
        EXPECT_FALSE(j1.is_null());
        EXPECT_EQ(j1.get<std::string>(), "hello");
    }
#endif

    TEST(xfixed_string, limit)
    {
      using string_type = xbasic_fixed_string<char, 255, buffer | store_size, string_policy::throwing_error>;
      string_type s1 = "hello";
      static_assert(sizeof(s1) == 256 * sizeof(char), "minimal storage");
    }

} // xtl
