/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xtl/xdynamic_bitset.hpp"

#include "gtest/gtest.h"

namespace xtl
{
    using bitset = xdynamic_bitset<uint64_t>;
    using bitset_view = xdynamic_bitset_view<uint64_t>;

    TEST(xdynamic_bitset, constructors)
    {
        // empty constructor
        {
            bitset b;
            EXPECT_TRUE(b.empty());
            EXPECT_EQ(0u, b.size());
            EXPECT_EQ(0u, b.block_count());
        }

        // Size constructor
        {
            bitset b(80u);
            EXPECT_FALSE(b.empty());
            EXPECT_EQ(80u, b.size());
            EXPECT_EQ(2u, b.block_count());
            bool res = false;
            for (std::size_t i = 0; i < b.size(); ++i)
            {
                res = res || b[i];
            }
            EXPECT_FALSE(res);
        }

        // Value constructor
        {
            bitset b(80u, true);
            EXPECT_FALSE(b.empty());
            EXPECT_EQ(80u, b.size());
            EXPECT_EQ(2u, b.block_count());
            bool res = true;
            for (std::size_t i = 0; i < b.size(); ++i)
            {
                res = res && b[i];
            }
            EXPECT_TRUE(res);
        }

        // Iterator pair constructor
        {
            std::vector<uint64_t> src(2u, 0xAAAAAAAAAAAAAAAA);
            bitset b(src.begin(), src.end());
            bool res = true;
            std::size_t size(128u);
            for (std::size_t i = 0; i < size; i += 2)
            {
                res = res && !b[i];
                res = res && b[i + 1];
            }
            EXPECT_EQ(size, b.size());
            EXPECT_TRUE(res);
        }

        // Initializer_list
        {
            bitset b = { true, true, false, true };
            EXPECT_EQ(4u, b.size());
            EXPECT_TRUE(b[0]);
            EXPECT_TRUE(b[1]);
            EXPECT_FALSE(b[2]);
            EXPECT_TRUE(b[3]);
        }
    }

    TEST(xdynamic_bitset, assign)
    {
        // same value repeated
        {
            bitset b(40u, false);
            b.assign(80u, true);
            EXPECT_EQ(80u, b.size());
            EXPECT_TRUE(b.all());
        }

        // Block iterator
        {
            bitset b(40u, false);
            std::vector<uint64_t> src(2u, 0xAAAAAAAAAAAAAAAA);
            b.assign(src.begin(), src.end());
            bool res = true;
            std::size_t size(128u);
            for (std::size_t i = 0; i < size; i += 2)
            {
                res = res && !b[i];
                res = res && b[i + 1];
            }
            EXPECT_EQ(size, b.size());
            EXPECT_TRUE(res);
        }

        // Initializer list
        {
            bitset b(40u, true);
            b.assign({ true, true, false, true });
            EXPECT_EQ(4u, b.size());
            EXPECT_TRUE(b[0]);
            EXPECT_TRUE(b[1]);
            EXPECT_FALSE(b[2]);
            EXPECT_TRUE(b[3]);
        }
    }

    TEST(xdynamic_bitset, resize)
    {
        std::size_t size1 = 80u;
        bitset b(size1, false);
        std::size_t size2 = 100u;
        b.resize(size2, true);
        bool res = true;
        for (std::size_t i = 0; i < size1; ++i)
        {
            res = res && !b[i];
        }
        for (std::size_t i = size1; i < size2; ++i)
        {
            res = res && b[i];
        }
        EXPECT_EQ(size2, b.size());
        EXPECT_TRUE(res);

        std::size_t size3 = 40u;
        b.resize(size3);
        res = false;
        for (std::size_t i = 0; i < size3; ++i)
        {
            res = res && b[i];
        }
        EXPECT_EQ(size3, b.size());
        EXPECT_FALSE(res);
    }

    TEST(xdynamic_bitset, clear)
    {
        std::size_t size1 = 80u;
        bitset b(size1, false);
        b.clear();
        EXPECT_EQ(0u, b.size());
    }

    TEST(xdynamic_bitset, reserve)
    {
        std::size_t size = 80u;
        std::size_t cap = 128u;
        bitset b;
        b.reserve(size);
        EXPECT_EQ(0u, b.size());
        EXPECT_EQ(cap, b.capacity());
    }

    TEST(xdynamic_bitset, push_back)
    {
        bitset b(80u, false);
        b.push_back(true);
        EXPECT_EQ(81u, b.size());
        EXPECT_TRUE(b.back());
        b.push_back(false);
        EXPECT_EQ(82u, b.size());
        EXPECT_FALSE(b.back());
    }

    TEST(xdynamic_bitset, pop_back)
    {
        bitset b(80u, false);
        b.push_back(true);
        b.push_back(false);
        b.pop_back();
        EXPECT_EQ(81u, b.size());
        EXPECT_TRUE(b.back());
        b.pop_back();
        EXPECT_EQ(80u, b.size());
        EXPECT_FALSE(b.back());
    }

    TEST(xdynamic_bitset, at)
    {
        bitset b(80u, false);
        b.at(4u) = true;
        EXPECT_TRUE(b[4u]);
    }

    TEST(xdynamic_bitset, front)
    {
        bitset b(80u, false);
        b.front() = true;
        EXPECT_TRUE(b[0]);
    }

    TEST(xdynamic_bitset, back)
    {
        bitset b(80u, false);
        b.back() = true;
        EXPECT_TRUE(b[79u]);
    }

    template <class B>
    void test_bitset_iterator(B& b)
    {
        b[17] = true;
        auto it = b.begin();
        auto cit = b.cbegin();
        EXPECT_FALSE(*it);
        EXPECT_FALSE(*cit);
        it += 16, cit += 16;
        ++it, ++cit;
        EXPECT_TRUE(*it);
        EXPECT_TRUE(*cit);
        it += 17, cit += 17;
        it -= 16, cit -= 16;
        --it, --cit;
        EXPECT_TRUE(*it);
        EXPECT_TRUE(*cit);
        it += 63, cit += 63;
        EXPECT_EQ(it, b.end());
        EXPECT_EQ(cit, b.cend());
    }

    TEST(xdynamic_bitset, iterator)
    {
        bitset b(80u, false);
        test_bitset_iterator(b);
    }

    TEST(xdynamic_bitset_view, iterator)
    {
        std::array<uint64_t, 2> bs = {0, 0};
        bitset_view b(bs.data(), 80);
        test_bitset_iterator(b);
    }

    template <class B>
    void test_bitset_reverse_iterator(B& b)
    {
        b[62] = true;
        auto it = b.rbegin();
        auto cit = b.crbegin();
        EXPECT_FALSE(*it);
        EXPECT_FALSE(*cit);
        it += 16, cit += 16;
        ++it, ++cit;
        EXPECT_TRUE(*it);
        EXPECT_TRUE(*cit);
        it += 17, cit += 17;
        it -= 16, cit -= 16;
        --it, --cit;
        EXPECT_TRUE(*it);
        EXPECT_TRUE(*cit);
        it += 63, cit += 63;
        EXPECT_EQ(it, b.rend());
        EXPECT_EQ(cit, b.crend());
    }

    TEST(xdynamic_bitset, reverse_iterator)
    {
        bitset b(80u, false);
        test_bitset_reverse_iterator(b);
    }

    TEST(xdynamic_bitset_view, reverse_iterator)
    {
        std::array<uint64_t, 2> bs = {0, 0};
        bitset_view b(bs.data(), 80);
        test_bitset_reverse_iterator(b);
    }

    template <class C>
    void print(C& c)
    {
        for (auto el : c) std::cout << el << ", ";
        std::cout << std::endl;
    }

    template <class B>
    void test_set(B& b)
    {
        b.set(47u, true);
        bool res = true;
        for (size_t i = 0; i < 80u; ++i)
        {
            res = res && (i != 47u ? !b[i] : b[i]);
        }
        EXPECT_TRUE(res);
        b.set();
        res = true;
        for (size_t i = 0; i < 80u; ++i)
        {
            res = res && b[i];
        }
        EXPECT_TRUE(res);
    }

    TEST(xdynamic_bitset, set)
    {
        bitset b(80u, false);
        test_set(b);
    }

    TEST(xdynamic_bitset_view, set)
    {
        std::array<uint64_t, 2> bs = {0, 0};
        bitset_view b(bs.data(), 80);
        test_set(b);
    }

    template <class B>
    void test_reset(B& b)
    {
        b.reset(47u);
        bool res = true;
        for (size_t i = 0; i < 80u; ++i)
        {
            res = res && (i != 47u ? b[i] : !b[i]);
        }
        EXPECT_TRUE(res);
        b.reset();
        res = true;
        for (size_t i = 0; i < 80u; ++i)
        {
            res = res && !b[i];
        }
        EXPECT_TRUE(res);
    }

    TEST(xdynamic_bitset, reset)
    {
        bitset b(80u, true);
        test_reset(b);
    }

    TEST(xdynamic_bitset_view, reset)
    {
        bitset bs(80u, true);
        bitset_view b(bs.data(), 80);
        test_reset(b);
    }

    template <class B>
    void test_flip(B& b)
    {
        b.flip(47u);
        bool res = true;
        for (size_t i = 0; i < 80u; ++i)
        {
            res = res && (i != 47u ? !b[i] : b[i]);
        }
        EXPECT_TRUE(res);

        b.reset();
        for (std::size_t i = 0; i < 80u; i += 2)
        {
            b[i] = true;
        }
        b.flip();
        res = true;
        for (std::size_t i = 0; i < 80u; i += 2)
        {
            res = res && !b[i] && b[i + 1];
        }
        EXPECT_TRUE(res);
    }

    TEST(xdynamic_bitset, flip)
    {
        bitset b(80u, false);
        test_flip(b);
    }

    TEST(xdynamic_bitset_view, flip)
    {
        std::array<uint64_t, 2> bs = {0, 0};
        bitset_view b(bs.data(), 80);
        test_flip(b);
    }

    template <class B>
    void test_all(B& b)
    {
        EXPECT_FALSE(b.all());
        b.flip(47u);
        EXPECT_FALSE(b.all());
        b.flip();
        EXPECT_FALSE(b.all());
        b.flip(47u);
        EXPECT_TRUE(b.all());
    }

    TEST(xdynamic_bitset, all)
    {
        bitset b(80u, false);
        test_all(b);
    }

    TEST(xdynamic_bitset_view, all)
    {
        bitset bs(80u, false);
        bitset_view b(bs.data(), 80);
        test_all(b);
    }

    template <class B>
    void test_any(B& b)
    {
        EXPECT_FALSE(b.any());
        b.flip(47u);
        EXPECT_TRUE(b.any());
    }

    TEST(xdynamic_bitset, any)
    {
        bitset b(80u, false);
        test_any(b);
    }

    TEST(xdynamic_bitset_view, any)
    {
        bitset bs(80u, false);
        bitset_view b(bs.data(), 80);
        test_any(b);
    }

    template <class B>
    void test_none(B& b)
    {
        EXPECT_TRUE(b.none());
        b.flip(47u);
        EXPECT_FALSE(b.none());
    }

    TEST(xdynamic_bitset, none)
    {
        bitset b(80u, false);
        test_none(b);
    }

    TEST(xdynamic_bitset_view, none)
    {
        bitset bs(80u, false);
        bitset_view b(bs.data(), 80);
        test_none(b);
    }

    template <class B>
    void test_count(B& b)
    {
        b[0] = true;
        b[17] = true;
        b[45] = true;
        b[65] = true;
        b[78] = true;
        EXPECT_EQ(5u, b.count());
    }

    TEST(xdynamic_bitset, count)
    {
        bitset b(80u, false);
        test_count(b);
    }

    TEST(xdynamic_bitset_view, count)
    {
        std::array<uint64_t, 2> bs = {0, 0};
        bitset_view b(bs.data(), 80);
        test_count(b);
    }

    template <class B>
    void test_bitwise_not(B& b1)
    {
        bitset b2 = ~b1;
        bitset res(80u, true);
        EXPECT_TRUE(res == b2);
    }

    TEST(xdynamic_bitset, bitwise_not)
    {
        bitset b1(80u, false);
        test_bitwise_not(b1);
    }

    TEST(xdynamic_bitset_view, bitwise_not)
    {
        std::array<uint64_t, 2> bs = {0, 0};
        bitset_view b(bs.data(), 80);
        test_bitwise_not(b);
    }

    template <class B>
    void test_bitwise_and(B& b1)
    {
        bitset b2(80u, false);
        bitset bres(80u, false);
        bres.flip(2);

        b1[2] = true;
        b2[2] = true;
        b1[5] = true;
        b2[7] = true;

        bitset b11(b1);
        bitset b22(b2);

        b1 &= b2;
        EXPECT_TRUE(bres == b1);

        bitset b3 = b11 & b22;
        EXPECT_TRUE(bres == b3);
    }

    TEST(xdynamic_bitset, bitwise_and)
    {
        bitset b1(80u, false);
        test_bitwise_and(b1);
    }

    TEST(xdynamic_bitset_view, bitwise_and)
    {
        std::array<uint64_t, 2> bs = {0, 0};
        bitset_view b(bs.data(), 80);
        test_bitwise_and(b);
    }

    template <class B>
    void test_bitwise_or(B& b1)
    {
        bitset b2(80u, false);
        bitset bres(80u, false);
        bres.flip(2);
        bres.flip(5);
        bres.flip(7);

        b1[2] = true;
        b2[2] = true;
        b1[5] = true;
        b2[7] = true;

        bitset b11(b1);
        bitset b22(b2);

        b1 |= b2;
        EXPECT_TRUE(bres == b1);

        bitset b3 = b11 | b22;
        EXPECT_TRUE(bres == b3);
    }

    TEST(xdynamic_bitset, bitwise_or)
    {
        bitset b1(80u, false);
        test_bitwise_or(b1);
    }

    TEST(xdynamic_bitset_view, bitwise_or)
    {
        std::array<uint64_t, 2> bs = {0, 0};
        bitset_view b(bs.data(), 80);
        test_bitwise_or(b);
    }

    template <class B>
    void test_bitwise_xor(B& b1, B& b2)
    {
        bitset bres(80u, false);
        bres.flip(5);
        bres.flip(7);

        b1[2] = true;
        b2[2] = true;
        b1[5] = true;
        b2[7] = true;

        bitset b11(b1);
        bitset b22(b2);

        b1 ^= b2;
        EXPECT_TRUE(bres == b1);

        bitset b3 = b11 ^ b22;
        EXPECT_TRUE(bres == b3);
    }

    TEST(xdynamic_bitset, bitwise_xor)
    {
        bitset b1(80u, false);
        bitset b2(80u, false);
        test_bitwise_xor(b1, b2);
    }

    TEST(xdynamic_bitset_view, bitwise_xor)
    {
        std::array<uint64_t, 2> bs1 = {0, 0};
        std::array<uint64_t, 2> bs2 = {0, 0};
        bitset_view b1(bs1.data(), 80);
        bitset_view b2(bs2.data(), 80);
        test_bitwise_xor(b1, b2);
    }

    template <class B>
    void test_shift_left(B& b1)
    {
        b1[2] = true;
        b1[7] = true;
        b1[15] = true;
        b1[45] = true;
        b1[78] = true;

        bitset b2 = b1 << 4;

        bitset bres(80u, false);
        bres[6] = true;
        bres[11] = true;
        bres[19] = true;
        bres[49] = true;

        EXPECT_TRUE(b2 == bres);
    }

    TEST(xdynamic_bitset, shift_left)
    {
        bitset b1(80u, false);
        test_shift_left(b1);
    }

    TEST(xdynamic_bitset_view, shift_left)
    {
        bitset bs(80u, false);
        bitset_view b1(bs.data(), 80u);
        test_shift_left(b1);
    }

    template <class B>
    void test_shift_right(B& b1)
    {
        b1[2] = true;
        b1[7] = true;
        b1[15] = true;
        b1[45] = true;
        b1[78] = true;

        bitset b2 = b1 >> 4;

        bitset bres(80u, false);
        bres[3] = true;
        bres[11] = true;
        bres[41] = true;
        bres[74] = true;

        EXPECT_TRUE(b2 == bres);
    }

    TEST(xdynamic_bitset, shift_right)
    {
        bitset b1(80u, false);
        test_shift_right(b1);
    }

    TEST(xdynamic_bitset_view, shift_right)
    {
        bitset bs(80u, false);
        bitset_view b1(bs.data(), 80u);
        test_shift_right(b1);
    }

    template <class B>
    void test_comparison(B& b1)
    {
        bitset b2(80u, false);

        EXPECT_TRUE(b1 == b1);
        EXPECT_FALSE(b1 == b2);
        EXPECT_TRUE(b1 != b2);
        EXPECT_FALSE(b1 != b1);

        bitset b11 = b1;
        b11[79] = false;
        EXPECT_FALSE(b11 == b2);
        EXPECT_FALSE(b11 == b1);
    }

    TEST(xdynamic_bitset, comparison)
    {
        bitset b1(80u, true);
        test_comparison(b1);
    }

    TEST(xdynamic_bitset_view, comparison)
    {
        bitset bs(80u, true);
        bitset_view b1(bs.data(), 80u);
        test_comparison(b1);
    }

    TEST(xdynamic_bitset, view)
    {
        uint64_t i = 0b00001100;
        auto bv = bitset_view(&i, 8);

        EXPECT_EQ(bv[0], false);
        EXPECT_EQ(bv[1], false);
        EXPECT_EQ(bv[2], true);
        EXPECT_EQ(bv[3], true);

        auto be = bitset({false, false, true, true, false, false, false, false});

        uint64_t j = 0b11110011;
        auto jbv = bitset_view(&j, 8);
        EXPECT_EQ(jbv, ~bv);
        bv.flip();
        EXPECT_EQ(jbv, bv);
        EXPECT_EQ(i, j);
    }
}
