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


#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <functional>



#include "gtest/gtest.h"

#include "xtl/xthreadpool.hpp"
#include "xtl/xparallel_foreach.hpp"

namespace xtl
{
    
    TEST(xparallel_foreach, simple_small)
    {
        const auto n_workers = 3;
        const auto size = 10;
        std::vector<int> vec(size, 0);  

        xthreadpool pool(n_workers);
        xparallel_foreach(vec.size(), pool , [&](auto thread_index, auto i)
        {
            vec[std::size_t(i)] = int(i);
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
        
        xthreadpool pool(n_workers);
        xparallel_foreach(vec.size(), pool, [n_workers, &vec](auto thread_index, auto i)
        {
            vec[std::size_t(i)] = int(i);
            EXPECT_GE(int(thread_index), int(0));
            auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<std::size_t(size); ++i)
        {
            EXPECT_EQ(vec[std::size_t(i)], int(i));
        }
    }

    TEST(xparallel_foreach, check_unique)
    {
        const auto n_workers = 7;
        const auto size = 1000;
        std::vector<int> vec(size, 1);  

        xthreadpool pool(n_workers);
        xparallel_foreach(vec.size(), pool, [n_workers, &vec](auto thread_index, auto i)
        {
            vec[std::size_t(i)] += 1;
            EXPECT_GE(int(thread_index), int(0));
            auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[std::size_t(i)], 2);
        }
    }

    TEST(xparallel_foreach, strided)
    {
        {
            const auto n_workers = 7;
            const auto size = 8;
            std::vector<int> vec(size, 0);  

            xthreadpool pool(n_workers);
            xparallel_foreach(0,8,1, pool, [n_workers, &vec](auto thread_index, auto i)
            {
                vec[std::size_t(i)] += 1;
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

            xthreadpool pool(n_workers);
            xparallel_foreach(1, 7, 2, pool, [n_workers, &vec](auto thread_index, auto i)
            {
                vec[std::size_t(i)] += 1;
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

            xthreadpool pool(n_workers);
            xparallel_foreach(1,7,3, pool,  [n_workers, &vec](auto thread_index, auto i)
            {
                vec[std::size_t(i)] += 1;
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

            xthreadpool pool(n_workers);
            xparallel_foreach(1,8,3, pool, [n_workers, &vec](auto thread_index, auto i)
            {
                vec[std::size_t(i)] += 1;
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

            xthreadpool pool(n_workers);
            xparallel_foreach(0,8,4,pool, [n_workers, &vec](auto thread_index, auto i)
            {
                vec[std::size_t(i)] += 1;
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
        xthreadpool pool(n_workers);
        xparallel_foreach(vec.size(), pool, [n_workers, &vec,&add_mutex](auto thread_index, auto i)
        {
            add_mutex.lock();
            vec[std::size_t(i)] += 1;
            add_mutex.unlock();
            EXPECT_GE(int(thread_index), int(0));
            auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[std::size_t(i)], 2);
        }
    }

    TEST(xparallel_foreach, container_impl_random_access)
    {
        const auto n_workers = 7;
        const auto size = 100;
        std::vector<int> vec(size, 1);  

        xthreadpool pool(n_workers);
        xparallel_foreach(vec, pool, [&](auto thread_index, int & val)
        {
            val = 2;
          auto tmp = int(n_workers); // travis fails otherwise?!?
            EXPECT_LT(int(thread_index), tmp);
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[std::size_t(i)], 2);
        }

    }

    TEST(xparallel_foreach, container_impl_bidirectional_iterator)
    {
        const auto n_workers = 7;
        const auto size = 100;
        std::list<int> container(size, 1);  

        xthreadpool pool(n_workers);
        xparallel_foreach(container, pool, [&](auto thread_index, int & val)
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

        xthreadpool pool(n_workers);

        xparallel_foreach(vec, pool, [&](auto thread_index, Foo & foo)
        {
            foo.val = 1;
            EXPECT_LT(int(thread_index), int(n_workers));
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[std::size_t(i)].val, 1);
        }

        xparallel_foreach(vec, pool, [&](auto thread_index, Foo & foo)
        {
            foo.val = 2;
            EXPECT_LT(int(thread_index), int(n_workers));
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[std::size_t(i)].val, 2);
        }

        xparallel_foreach(vec, pool, [&](auto thread_index, Foo & foo)
        {
            foo.val = 3;
            EXPECT_LT(int(thread_index), int(n_workers));
        });

        for(std::size_t i=0; i<size; ++i)
        {
            EXPECT_EQ(vec[std::size_t(i)].val, 3);
        }

    }


    TEST(xparallel_foreach, same_as_seriell)
    {

        std::random_device rnd_device;
        // Specify the engine and distribution.
        std::mt19937 mersenne_engine(rnd_device());
        std::uniform_int_distribution<int> dist(1, 10000000);
        xthreadpool pool(n_thread_settings::default_n_threads);

        auto gen = std::bind(dist, mersenne_engine);
        
        auto test_it = [&](const std::size_t  size)
        {

            std::vector<int> vec(size), res_p(size), res_s(size);
            std::generate(begin(vec), end(vec), gen);

            xtl::xparallel_foreach(size, pool, [&](std::size_t , auto i)
            {
                res_p[i] = vec[i];
            });



            for(std::size_t i=0; i<size; ++i){
                res_s[i] = vec[i];
            }

            for(std::size_t i=0; i<size; ++i){
                EXPECT_EQ(res_s[i], res_p[i]);
            }
        };

        test_it(std::size_t(10));
        test_it(std::size_t(101));
        test_it(std::size_t(999));
        test_it(std::size_t(2001));
        test_it(std::size_t(20003));

    }



}

