/***************************************************************************
* Copyright (c) 2016, Johan Mabille, Sylvain Corlay and Wolf Vollprecht    *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <algorithm>
#include <vector>
#include <list>
#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "xtl/xthreadpool.hpp"
#include "xtl/xparallel_foreach.hpp"

namespace xtl
{
    
    TEST(xthreadpool, with_workers)
    {

        std::vector<int> visited(5, 0);
        xthreadpool pool((std::thread::hardware_concurrency()));

        for(std::size_t i=0; i<visited.size(); ++i)
        {
            auto res = pool.enqueue([i,&visited,&pool](auto thread_index)
            {
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
        xthreadpool pool((0));

        for(std::size_t i=0; i<visited.size(); ++i)
        {
            auto res = pool.enqueue([i,&visited,&pool](auto thread_index)
            {
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

    TEST(xprio_threadpool, single_workers){
        // this test might not be valid for single core machines



        xpriority_threadpool pool((1));


        // enqueue some sleeping task which should start immediately
        pool.enqueue(10.0, [](std::size_t ){
            std::this_thread::sleep_for(   std::chrono::seconds(3));
        });
        pool.enqueue(10.0, [](std::size_t ){
            std::this_thread::sleep_for(   std::chrono::milliseconds(10));
        });
        pool.enqueue(10.0, [](std::size_t ){
            std::this_thread::sleep_for(   std::chrono::milliseconds(10));
        });
         pool.enqueue(10.0, [](std::size_t ){
            std::this_thread::sleep_for(   std::chrono::milliseconds(10));
        });

        // wait a tiny to make sure tasks above is started
        // (this might be superfluous)
        std::this_thread::sleep_for(   std::chrono::milliseconds(100)); 


        // since there is only one worker we need no mutex
        std::vector<int> order;

        pool.enqueue(2.0, [&order](std::size_t){
            // since there is only one worker we need no mutex
            order.push_back(2);
            //std::this_thread::sleep_for(   std::chrono::milliseconds(100));
        });

        pool.enqueue(4.0, [&order](std::size_t){
            // since there is only one worker we need no mutex
            order.push_back(4);
            //std::this_thread::sleep_for(   std::chrono::milliseconds(100));
        });

        pool.enqueue(3.0, [&order](std::size_t){
            // since there is only one worker we need no mutex
            order.push_back(3);
            //std::this_thread::sleep_for(   std::chrono::milliseconds(100));
        });

        pool.enqueue(5.0, [&order](std::size_t){
            // since there is only one worker we need no mutex
            order.push_back(5);
            //std::this_thread::sleep_for(   std::chrono::milliseconds(100));
        });



        pool.wait();

        EXPECT_EQ(order[0],5);
        EXPECT_EQ(order[1],4);
        EXPECT_EQ(order[2],3);
        EXPECT_EQ(order[3],2);
    }
}

