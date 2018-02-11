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
        xthreadpool pool((int(std::thread::hardware_concurrency())));

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


        // enqueue a sleeping task which should start immediately
        // which is sleeping
        // 
        // while this thread sleeps we add new tasks 
        //
        pool.enqueue(10.0, [](auto worker_index){
            std::this_thread::sleep_for(   std::chrono::seconds(3));
        });

        // wait a tiny to make sure task above is stared
        std::this_thread::sleep_for(   std::chrono::milliseconds(100)); 


        // since there is only one worker we need no mutex
        std::vector<int> order;

        pool.enqueue(2.0, [&order](auto worker_index){
            // since there is only one worker we need no mutex
            order.push_back(2);
            //std::this_thread::sleep_for(   std::chrono::milliseconds(100));
        });

        pool.enqueue(4.0, [&order](auto worker_index){
            // since there is only one worker we need no mutex
            order.push_back(4);
            //std::this_thread::sleep_for(   std::chrono::milliseconds(100));
        });

        pool.enqueue(3.0, [&order](auto worker_index){
            // since there is only one worker we need no mutex
            order.push_back(3);
            //std::this_thread::sleep_for(   std::chrono::milliseconds(100));
        });

        pool.enqueue(5.0, [&order](auto worker_index){
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

    TEST(xparallel_foreach, simple_small)
    {
        const auto n_workers = 3;
        const auto size = 10;
        std::vector<int> vec(size, 0);  

        xparallel_foreach(vec.size(), n_workers, [&](auto thread_index, auto i)
        {
            vec[i] = int(i);
            EXPECT_GE(int(thread_index), int(0));
       
            auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);

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
                                                 // n_workers only for travis
        xparallel_foreach(vec.size(), n_workers, [n_workers, &vec](auto thread_index, auto i)
        {
            vec[i] = int(i);
            EXPECT_GE(int(thread_index), int(0));
            auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<std::size_t(size); ++i)
        {
            EXPECT_EQ(vec[i], int(i));
        }
    }




    TEST(xparallel_foreach, check_unique)
    {
        const auto n_workers = 7;
        const auto size = 1000;
        std::vector<int> vec(size, 1);  

        xparallel_foreach(vec.size(), n_workers, [n_workers, &vec](auto thread_index, auto i)
        {
            vec[i] += 1;
            EXPECT_GE(int(thread_index), int(0));
            auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[i], 2);
        }
    }


    TEST(xparallel_foreach, strided)
    {
        {
            const auto n_workers = 7;
            const auto size = 8;
            std::vector<int> vec(size, 0);  

            xparallel_foreach(0,8,1,n_workers, [n_workers, &vec](auto thread_index, auto i)
            {
                vec[i] += 1;
                EXPECT_GE(int(thread_index), int(0));
                auto tmp = int(n_workers); // travis fails otherwise?!?
                EXPECT_LT(int(thread_index), tmp);
            });

            EXPECT_EQ(vec[0], 1);
            EXPECT_EQ(vec[1], 1);
            EXPECT_EQ(vec[2], 1);
            EXPECT_EQ(vec[3], 1);
            EXPECT_EQ(vec[4], 1);
            EXPECT_EQ(vec[5], 1);
            EXPECT_EQ(vec[6], 1);
            EXPECT_EQ(vec[7], 1);
        }

        {
            const auto n_workers = 5;
            const auto size = 8;
            std::vector<int> vec(size, 0);  

            xparallel_foreach(1,7,2,n_workers, [n_workers, &vec](auto thread_index, auto i)
            {
                vec[i] += 1;
                EXPECT_GE(int(thread_index), int(0));
                auto tmp = int(n_workers); // travis fails otherwise?!?
                EXPECT_LT(int(thread_index), tmp);
            });

            EXPECT_EQ(vec[0], 0);
            EXPECT_EQ(vec[1], 1);
            EXPECT_EQ(vec[2], 0);
            EXPECT_EQ(vec[3], 1);
            EXPECT_EQ(vec[4], 0);
            EXPECT_EQ(vec[5], 1);
            EXPECT_EQ(vec[6], 0);
            EXPECT_EQ(vec[7], 0);
        }


        {
            const auto n_workers = 1;
            const auto size = 8;
            std::vector<int> vec(size, 0);  

            xparallel_foreach(1,7,3,n_workers,  [n_workers, &vec](auto thread_index, auto i)
            {
                vec[i] += 1;
                EXPECT_GE(int(thread_index), int(0));
              auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
            });

            EXPECT_EQ(vec[0], 0);
            EXPECT_EQ(vec[1], 1);
            EXPECT_EQ(vec[2], 0);
            EXPECT_EQ(vec[3], 0);
            EXPECT_EQ(vec[4], 1);
            EXPECT_EQ(vec[5], 0);
            EXPECT_EQ(vec[6], 0);
            EXPECT_EQ(vec[7], 0);
        }

        {
            const auto n_workers = 11;
            const auto size = 8;
            std::vector<int> vec(size, 0);  

            xparallel_foreach(1,8,3,n_workers, [n_workers, &vec](auto thread_index, auto i)
            {
                vec[i] += 1;
                EXPECT_GE(int(thread_index), int(0));
              auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
            });

            EXPECT_EQ(vec[0], 0);
            EXPECT_EQ(vec[1], 1);
            EXPECT_EQ(vec[2], 0);
            EXPECT_EQ(vec[3], 0);
            EXPECT_EQ(vec[4], 1);
            EXPECT_EQ(vec[5], 0);
            EXPECT_EQ(vec[6], 0);
            EXPECT_EQ(vec[7], 1);
        }

        {
            const auto n_workers = 2;
            const auto size = 8;
            std::vector<int> vec(size, 0);  

            xparallel_foreach(0,8,4,n_workers, [n_workers, &vec](auto thread_index, auto i)
            {
                vec[i] += 1;
                EXPECT_GE(int(thread_index), int(0));
              auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
            });

            EXPECT_EQ(vec[0], 1);
            EXPECT_EQ(vec[1], 0);
            EXPECT_EQ(vec[2], 0);
            EXPECT_EQ(vec[3], 0);
            EXPECT_EQ(vec[4], 1);
            EXPECT_EQ(vec[5], 0);
            EXPECT_EQ(vec[6], 0);
            EXPECT_EQ(vec[7], 0);
        }
        
    }



    TEST(xparallel_foreach, check_unique_visit_strict)
    {
        const auto n_workers = 7;
        const auto size = 1000;
        std::vector<int> vec(size, 1);  

        std::mutex add_mutex;
        xparallel_foreach(vec.size(), n_workers, [n_workers, &vec,&add_mutex](auto thread_index, auto i)
        {
            add_mutex.lock();
            vec[i] += 1;
            add_mutex.unlock();
            EXPECT_GE(int(thread_index), int(0));
            auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[i], 2);
        }
    }


    TEST(xparallel_foreach, container_impl_random_access)
    {
        const auto n_workers = 7;
        const auto size = 100;
        std::vector<int> vec(size, 1);  

        xparallel_foreach(vec, n_workers, [&](auto thread_index, int & val)
        {
            val = 2;
          auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[i], 2);
        }

    }

    TEST(xparallel_foreach, container_impl_bidirectional_iterator)
    {
        const auto n_workers = 7;
        const auto size = 100;
        std::list<int> container(size, 1);  

        xparallel_foreach(container, n_workers, [&](auto thread_index, int & val)
        {
            val = 2;
          auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        auto iter = container.begin();
        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(*iter, 2);
            ++iter;
        }

    }

    struct Foo
    {
        Foo()
        {
            val = 0;
        }
        int val;
    };


    TEST(xparallel_foreach, container_impl_nondefault_type)
    {
        const auto n_workers = 7;
        const auto size = 100;
        std::vector<Foo> vec(size);  

        xparallel_foreach(vec, n_workers, [&](auto thread_index, Foo & foo)
        {
            foo.val = 1;
          auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[i].val, 1);
        }

        xparallel_foreach(vec, n_workers, [&](auto thread_index, Foo & foo)
        {
            foo.val = 2;
          auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[i].val, 2);
        }

        xparallel_foreach(vec, [&](auto thread_index, Foo & foo)
        {
            foo.val = 3;
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[i].val, 3);
        }

    }
}

