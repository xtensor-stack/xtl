/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <algorithm>

#include "gtest/gtest.h"

#include "xtl/xthreadpool.hpp"
#include "xtl/xparallel_foreach.hpp"

namespace xtl
{
    
    TEST(xthreadpool, with_workers)
    {

        std::vector<int> visited(5, 0);
        xthreadpool pool(int(std::thread::hardware_concurrency()));

        for(std::size_t i=0; i<visited.size(); ++i){
            auto res = pool.enqueue([i,&visited,&pool](auto thread_index){
                EXPECT_LT(std::size_t(thread_index), pool.n_threads());
                visited[i] = 1;
            });
        }

        // wait for threadpool to complete
        pool.wait();

        EXPECT_EQ(visited[0], int(1));
        EXPECT_EQ(visited[1], int(1));
        EXPECT_EQ(visited[2], int(1));
        EXPECT_EQ(visited[3], int(1));
        EXPECT_EQ(visited[4], int(1));
    }

    TEST(xthreadpool, no_workers)
    {

        std::vector<int> visited(5, 0);
        xthreadpool pool(0);

        for(std::size_t i=0; i<visited.size(); ++i){
            auto res = pool.enqueue([i,&visited,&pool](auto thread_index){
                EXPECT_LT(std::size_t(thread_index), pool.n_threads());
                visited[i] = 1;
            });
        }

        // wait for threadpool to complete
        pool.wait();

        EXPECT_EQ(visited[0], int(1));
        EXPECT_EQ(visited[1], int(1));
        EXPECT_EQ(visited[2], int(1));
        EXPECT_EQ(visited[3], int(1));
        EXPECT_EQ(visited[4], int(1));
    }


    TEST(xparallel_foreach, simple_small)
    {
        const auto n_workers = 3;
        const auto size = 10;
        std::vector<int> vec(size, 0);  

        xparallel_foreach(vec.size(), n_workers, [&](auto thread_index, auto i){
            vec[i] = int(i);
            EXPECT_GE(int(thread_index), int(0));
            EXPECT_LT(int(thread_index), int(n_workers));

        });

        EXPECT_EQ(vec[0], 0);
        EXPECT_EQ(vec[1], 1);
        EXPECT_EQ(vec[2], 2);
        EXPECT_EQ(vec[3], 3);
        EXPECT_EQ(vec[4], 4);
        EXPECT_EQ(vec[5], 5);
        EXPECT_EQ(vec[6], 6);
        EXPECT_EQ(vec[7], 7);
        EXPECT_EQ(vec[8], 8);
        EXPECT_EQ(vec[9], 9);

    }

    TEST(xparallel_foreach, simple_big)
    {
        const auto n_workers = 7;
        const auto size = 1000;
        std::vector<int> vec(size, 0);  

        xparallel_foreach(vec.size(), n_workers, [&](auto thread_index, auto i){
            vec[i] = int(i);
            EXPECT_GE(int(thread_index), int(0));
            EXPECT_LT(int(thread_index), int(n_workers));
        });

        for(std::size_t i=0; i<std::size_t(size); ++i){
            EXPECT_EQ(vec[i], int(i));
        }
    }

    TEST(xparallel_foreach, check_unique_visit)
    {
        const auto n_workers = 7;
        const auto size = 1000;
        std::vector<int> vec(size, 1);  

        xparallel_foreach(vec.size(), n_workers, [&](auto thread_index, auto i){
            vec[i] += 1;
            EXPECT_GE(int(thread_index), int(0));
            EXPECT_LT(int(thread_index), int(n_workers));
        });

        for(std::size_t i=0; i<size; ++i){
            EXPECT_EQ(vec[i], 2);
        }



    }
    
    TEST(xparallel_foreach, check_unique_visit_strict)
    {
        const auto n_workers = 7;
        const auto size = 1000;
        std::vector<int> vec(size, 1);  

        std::mutex add_mutex;
        xparallel_foreach(vec.size(), n_workers, [&](auto thread_index, auto i){
            add_mutex.lock();
            vec[i] += 1;
            add_mutex.unlock();
            EXPECT_GE(int(thread_index), int(0));
            EXPECT_LT(int(thread_index), int(n_workers));
        });

        for(std::size_t i=0; i<size; ++i){
            EXPECT_EQ(vec[i], 2);
        }
    }
}

