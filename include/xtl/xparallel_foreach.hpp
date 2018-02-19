#ifndef XTL_XPARALLEL_FOR_EACH_HPP
#define XTL_XPARALLEL_FOR_EACH_HPP

#define XTL_DEFAULT_CHUNKING_FACTOR 3.0

#include <future>
#include <type_traits>
#include "xtl/xthreadpool.hpp"


// todos:
// 
// make a faster, yet not exception save
// and void only impl
// 
// faster impl of function:
// https://github.com/skarupke/std_function/blob/master/function.h
// https://probablydance.com/2013/01/13/a-faster-implementation-of-stdfunction/
//
// funtion_queue as alternate to std::queue<function>
// https://groups.google.com/a/isocpp.org/forum/#!topic/sg14/O3bKyEJ7L04
//


namespace xtl
{      
    ///\cond
    namespace detail{

        template<class integer>
        inline integer get_chunk_size(integer workload, std::size_t n_workers)
        {
            // how much work needs to be done by each thread
            const auto work_per_thread = double(workload) / double(n_workers);
            const auto tmp = std::llround(work_per_thread / double(XTL_DEFAULT_CHUNKING_FACTOR));
            return std::max(integer(tmp), integer(1));
        }


        template<class integer, class functor,typename std::enable_if<std::is_integral<integer>::value, int>::type = 0>
        void xparallel_integer_foreach_impl(
            integer start,
            integer stop,
            integer step, 
            xthreadpool& pool,
            functor&& f
        )
        {
            // precondition
            assert(pool.n_worker_threads() > 0);

            // compute the size / workload
            const auto dist = stop - start;
            const auto size = static_cast<integer>(std::ceil(float(dist)/float(step)));
            

            // we bundle a chunk of functor calls to a task
            // which is enqueued to the threadpool to minimize 
            // overhead from pool.enqueue
            const auto chunk_size = get_chunk_size(size, pool.n_worker_threads());
            
            // all possible exceptions are stored 
            // into futures (todo: here we could speedup
            // with a stack allocated vector)
            // => also we need this to wait properly
            std::vector<std::future<void> > futures;
            futures.reserve(std::size_t(size/chunk_size + 1));


            auto workload = size;

            // the actual argument for functor
            integer f_arg = start;

            // dense loop starting from 0
            for(integer i=0; i<size; i += chunk_size)
            {

                // workload for this task
                const auto task_workload = std::min(workload, chunk_size);
               

                futures.emplace_back(
                    // mutable keyword is needed to get a NON-CONST COPY of f_arg
                    pool.enqueue([&f, task_workload, f_arg, step](auto thread_id) mutable
                    {
                        for(integer j=0; j<task_workload; ++j, f_arg += step)
                        {
                            f(thread_id, f_arg);
                        }
                    })
                );

                // update workload and functor arg
                workload -= task_workload;
                f_arg += task_workload*step;
            }

            // this will wait until all tasks above are done
            for (auto& fut : futures)
            {
                fut.get();
            }
        }

        template<class integer, class functor,typename std::enable_if<std::is_integral<integer>::value, int>::type = 0>
        void seriell_integer_foreach_impl(
            const integer start,
            const integer stop,
            const integer step,
            functor&& f
        )
        {
            for(integer i=start; i<stop; i += step)
            {
                f(std::size_t(0), i);
            }
        }

        template<class integer, class functor,typename std::enable_if<std::is_integral<integer>::value, int>::type = 0>
        void xparallel_integer_foreach_impl(
            const integer size,
            xthreadpool& pool,
            functor&& f
        )
        {
            // we bundle a chunk of functor calls to a task
            // which is enqueued to the threadpool to minimize 
            // overhead from pool.enqueue
            const auto chunk_size = get_chunk_size(size, pool.n_worker_threads());
            
            // all possible exceptions are stored 
            // into futures (todo: here we could speedup
            // with a stack allocated vector)
            // => also we need this to wait properly
            std::vector<std::future<void> > futures;
            futures.reserve(size/chunk_size + 1);

            auto workload = size;
            for(integer i=0; i<size; i += chunk_size)
            {

                // workload for this task
                const auto task_workload = std::min(workload, chunk_size);

                
                futures.emplace_back(
                    pool.enqueue([&f, i, task_workload](int thread_id)
                    {
                        for(integer f_arg=i; f_arg<i+task_workload; ++f_arg)
                        {
                            f(thread_id, f_arg);
                        }
                    })
                );
                // update workload
                workload -= task_workload;
            }

            // this will wait until all tasks above are done
            for (auto& fut : futures)
            {
                fut.get();
            }
        }

        template<class integer, class functor,typename std::enable_if<std::is_integral<integer>::value, int>::type = 0>
        void seriell_integer_foreach_impl(
            const integer size,
            functor&& f
        )
        {
            for(integer i=0; i<size; ++i)
            {
                f(std::size_t(0), i);
            }
        }

        

        template<class TAG>
        struct xparallel_iterator_foreach_impl;

        template<>
        struct xparallel_iterator_foreach_impl<std::random_access_iterator_tag>
        {
            template<class iterator, class functor>
            static void op(
                iterator iter,
                iterator end,
                const typename std::iterator_traits<iterator>::difference_type size,
                xthreadpool& pool,
                functor&& f
            )
            {

                typedef typename std::iterator_traits<iterator>::difference_type difference_type;

                // if size is already specified we do not call std::distance 
                // (this is not at all crucial for random_access_iterator_tag but
                // we use this trick for also for forward iterators and here for consistency)
                difference_type workload = size > 0 ? size : std::distance(iter, end);

                // we bundle a chunk of functor calls to a task
                // which is enqueued to the threadpool to minimize 
                // overhead from pool.enqueue
                const auto chunk_size = get_chunk_size(workload, pool.n_worker_threads());
                
                // all possible exceptions are stored 
                // into futures (todo: here we could speedup
                // with a stack allocated vector)
                // => also we need this to wait properly
                std::vector<std::future<void> > futures;
                futures.reserve(std::size_t(workload/chunk_size + 1));

                for( ;iter != end; iter += difference_type(chunk_size))
                {

                    const auto task_workload = difference_type(std::min(workload, chunk_size));
                    
                    futures.emplace_back(
                        pool.enqueue(
                            [&f, iter, task_workload](int worker_index)
                            {
                                for(difference_type i=0; i<task_workload; ++i)
                                    f(worker_index, iter[i]);
                            }
                        )
                    );
                    // update workload
                    workload -= task_workload;
                }

                for (auto& fut : futures)
                {
                    fut.get();
                }
            } 
        };

        template<>
        struct xparallel_iterator_foreach_impl<std::forward_iterator_tag>
        {
            template<class iterator, class functor>
            static void op(
                iterator iter,
                iterator end,
                const typename std::iterator_traits<iterator>::difference_type size,
                xthreadpool& pool,
                functor&& f
            )
            {
                typedef typename std::iterator_traits<iterator>::difference_type difference_type;
                auto workload = size > 0 ? size : std::distance(iter, end);
                const auto chunk_size = get_chunk_size(workload, pool.n_worker_threads());
                
                // all possible exceptions are stored 
                // into futures (todo: here we could speedup
                // with a stack allocated vector)
                // => also we need this to wait properly
                std::vector<std::future<void> > futures;
                futures.reserve(std::size_t(workload/chunk_size + 1));


                for(;;)
                {

                    // workload for a single task
                    const auto task_workload = std::min(chunk_size, workload);

                    // here we pass a copy of the current iterator to the 
                    // task function
                    futures.emplace_back(
                        // mutable keyword is needed to get a NON-CONST COPY captured iter
                        pool.enqueue([&f, iter, task_workload] (int worker_index) mutable
                        {
                            for(difference_type i=0; i<task_workload; ++i)
                            {
                                f(worker_index, *iter);
                                ++iter;
                            }

                        })
                    );

                    // increment iterator in a serial fashion
                    for (difference_type i = 0; i < task_workload; ++i)
                    {
                        ++iter;
                        if (iter == end)
                        {
                            break;
                        }
                    }

                    // update workload
                    workload -= task_workload;
                    if(workload==0)
                        break;
                }

                for (auto& fut : futures)
                {
                    fut.get();
                }
            }
        };

        template<>
        struct xparallel_iterator_foreach_impl<std::bidirectional_iterator_tag>
        :   public xparallel_iterator_foreach_impl<std::forward_iterator_tag>
        {
        };

        template<>
        struct xparallel_iterator_foreach_impl<std::input_iterator_tag>
        {
            template<class iterator, class functor>
            static void op(
                iterator iter,
                iterator end,
                const typename std::iterator_traits<iterator>::difference_type size,
                xthreadpool& pool,
                functor&& f
            )
            {
                // todo use better impl if size is known (aka size!=0)
                // also it could be very expensive to store 
                // a future for each single item

                std::vector<std::future<void> > futures;

                for (; iter != end; ++iter)
                {
                    auto item = *iter;
                    futures.emplace_back(
                        pool.enqueue(
                            [&f,&item](int id)
                            {
                                f(id, item);
                            }
                        )
                    );
                }
                for (auto& fut : futures)
                {
                    fut.get();
                }
            } 
        };

        template<class iterator, class functor>
        void seriell_iterator_foreach_impl(
            iterator iter,
            iterator end,
            functor&& f
        )
        {
            while(iter!=end)
            {
                f(std::size_t(0), *iter);
                ++iter;
            }
        }
    } // end namespace xtl::detail
    ///\endcond


    // strided 
    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value, int>::type = 0>
    void xparallel_foreach(
        const integer start,
        const integer stop,
        const integer step,
        xthreadpool & pool,
        functor&& f
    )
    {
        if(pool.n_worker_threads()!=0)
        {
            detail::xparallel_integer_foreach_impl(start,stop,step, pool, std::forward<functor>(f));
        }
        else{
            detail::seriell_integer_foreach_impl(start,stop,step, std::forward<functor>(f));
        }
    }


    // non strided, start at zero
    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value, int>::type = 0>
    void xparallel_foreach(
        const integer size,
        xthreadpool & pool,
        functor&& f
    )
    {
        if(pool.n_worker_threads()!=0)
        {
            detail::xparallel_integer_foreach_impl(size, pool, std::forward<functor>(f));
        }
        else{
            detail::seriell_integer_foreach_impl(size, std::forward<functor>(f));
        }
    }


    // iterator overloads
    template<class iterator, class functor, typename std::enable_if<!std::is_integral<iterator>::value, int>::type = 0>
    void xparallel_foreach(
        iterator begin,
        iterator end,
        xthreadpool & pool,
        functor&& f
    )
    {
        
        typedef typename std::iterator_traits<iterator>::iterator_category iterator_category;  

        if(pool.n_worker_threads()!=0)
        {
            const auto size = std::distance(begin, end);
            detail::xparallel_iterator_foreach_impl<iterator_category>::op(
                begin, end, size, pool, std::forward<functor>(f), iterator_category());
        }
        else{
            detail::seriell_iterator_foreach_impl(begin, end, std::forward<functor>(f));
        }
    }


    // container overloads
    template<class CONTAINER, class functor, typename std::enable_if<!std::is_integral<CONTAINER>::value, int>::type = 0>
    void xparallel_foreach(
        CONTAINER& container,
        xthreadpool & pool,
        functor&& f
    )
    {
        auto begin = std::begin(container);
        auto end  = std::end(container);
        typedef typename std::decay<decltype(begin)>::type iterator_type;
        typedef typename std::iterator_traits<iterator_type>::iterator_category iterator_category; 

        if(pool.n_worker_threads()!=0)
        {
            const auto size = std::distance(begin, end);
            detail::xparallel_iterator_foreach_impl<iterator_category>::op(
                begin, end, size, pool, std::forward<functor>(f));
        }
        else{
            detail::seriell_iterator_foreach_impl(begin, end, std::forward<functor>(f));
        }

    }

    
} // end namespace xtl

#endif // XTL_XPARALLEL_FOR_EACH_HPP