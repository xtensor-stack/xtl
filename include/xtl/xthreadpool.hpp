#ifndef XTL_XTHREADPOOL_HPP
#define XTL_XTHREADPOOL_HPP

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <condition_variable>
#include <future>
#include <mutex>
#include <vector>
#include <queue>
#include <stdexcept>
#include <cmath>
#include <functional>
#include <thread>
#include <numeric>


#define XTL_XTHREADPOOL_USE_RAW_PTR 1

namespace xtl
{
    enum class n_thread_settings{
        default_n_threads ,  ///< Determine number of threads automatically (from <tt>std::thread::hardware_concurrency()</tt>)
        nice_n_threads    ,  ///< Use half as many threads as <tt>Auto</tt> would.
        no_threads           ///< Switch off multi-threading (i.e. execute tasks sequentially)
    };




    /// \cond
    namespace detail{

        typedef std::function<void(std::size_t)> xthreadpool_basefunction;

        class prio_function : public xthreadpool_basefunction{
        public:
            //using xthreadpool_basefunction::xthreadpool_basefunction;

            prio_function()
            :   xthreadpool_basefunction(),
                m_priority(){
            }

            template<class functor>
            prio_function(functor && f, float priority)
            :   xthreadpool_basefunction(std::forward<functor>(f)),
                m_priority(priority){

            }

            // void set_priority(const float priority){
            //     m_priority = priority;
            // }
            // bool operator>(const prio_function & other)const{
            //     return m_priority > other.m_priority;
            // }
            bool operator<(const prio_function & other)const{
                return m_priority < other.m_priority;
            }
        private:
            float m_priority;
        };

        // class normal_function : public xthreadpool_basefunction{
        // public:
        //     using xthreadpool_basefunction::xthreadpool_basefunction;
        // };

        // to unify std::queue and std::priority_queue
        // and possible other queues

        template<class ... arguments>
        class threadpool_queue : public std::queue<arguments ...>{
            typedef std::queue<arguments ...> base_type;
        public:
            using base_type::base_type;
            const auto& next()const{
                return this->front();
            }
            void remove_next(){
                this->pop();
            }
        };

        template<class ... arguments>
        class threadpool_priority_queue : public std::priority_queue<arguments ...>{
            typedef std::priority_queue<arguments ...> base_type;
        public:
            using base_type::base_type;
            const auto& next()const{
                return this->top();
            }
            void remove_next(){
                this->pop();
            }
        };


        // The class xthreadpool_impl is based on https://github.com/progschj/ThreadPool/ 
        // with the crucial difference that any  enqueued function get a consecutive worker
        // index as argument. is allows for ``thread-private`` / per-thread-storage.
        // Furthermore we allow for different queues (including priority queues)  
        //
        // The implementation of https://github.com/progschj/ThreadPool/ 
        // has the following copyright notice:
        //========================================================
        // Copyright (c) 2012 Jakob Progsch, Václav Zeman
        // 
        // This software is provided 'as-is', without any express or implied
        // warranty. In no event will the authors be held liable for any damages
        // arising from the use of this software.
        // 
        // Permission is granted to anyone to use this software for any purpose,
        // including commercial applications, and to alter it and redistribute it
        // freely, subject to the following restrictions:
        // 
        //   1. The origin of this software must not be misrepresented; you must not
        //   claim that you wrote the original software. If you use this software
        //   in a product, an acknowledgment in the product documentation would be
        //   appreciated but is not required.
        // 
        //   2. Altered source versions must be plainly marked as such, and must not be
        //   misrepresented as being the original software.
        // 
        //   3. This notice may not be removed or altered from any source
        //   distribution.
        //==========================================================
        //

        template< class queue >
        class xthreadpool_impl
        {
        public:
            

            explicit xthreadpool_impl(const std::size_t n);
            explicit xthreadpool_impl(const n_thread_settings & settings);
            ~xthreadpool_impl();

            void wait();
    
            std::size_t n_worker_threads() const;
            std::size_t n_threads() const;  

        protected:
            template<class functor, class ... arguments>
            auto enqueue_impl(functor&& f, arguments&& ... args);// -> std::future<decltype(f(0))>;
        
        private:
            typedef std::unique_lock<std::mutex> unique_lock_type;   
            typedef typename queue::value_type function_type;

            // helper function to init the thread pool
            void init(std::size_t);

            // helper function which is called from each worker thread
            void run(std::size_t worker_index);
            
     
            std::vector<std::thread> m_threads;
            queue m_queue;
            //std::queue<function_type> m_queue;
            std::mutex m_queue_mutex;
            std::condition_variable m_worker_condition;
            std::condition_variable m_finish_condition;
            bool m_destructed;
            std::atomic_long m_busy;
        };


    } // close namespace xtl::detail
    /// \endcond

    class xthreadpool : public  detail::xthreadpool_impl<detail::threadpool_queue<detail::xthreadpool_basefunction> > {
        typedef detail::xthreadpool_impl<detail::threadpool_queue<detail::xthreadpool_basefunction> > base_type;
    public:
        using base_type::base_type;
        template<class functor>
        auto enqueue(functor&& f);
    };


    class xpriority_threadpool : 
        public detail::xthreadpool_impl<detail::threadpool_priority_queue<detail::prio_function>> 
    {
        typedef detail::xthreadpool_impl<detail::threadpool_priority_queue<detail::prio_function>> base_type;
    public:
        using base_type::base_type;

        template<class functor>
        auto enqueue(float priority, functor&& f);
    };


    // implementation 
    template<class functor>
    auto xthreadpool::enqueue(functor&& f){
        return this->enqueue_impl(std::forward<functor>(f));
    }


    template<class functor>
    auto xpriority_threadpool::enqueue(float priority, functor&& f){
        return this->enqueue_impl(std::forward<functor>(f), priority);
    }




    // implementation of detail
    namespace detail{

        template< class queue >
        inline xthreadpool_impl<queue>::xthreadpool_impl(const n_thread_settings & settings)
        :   m_destructed(false)        
        {
            if(settings == n_thread_settings::nice_n_threads){
                init(std::thread::hardware_concurrency()/2);
            }
            else if(settings == n_thread_settings::default_n_threads){
                init(std::thread::hardware_concurrency());
            }
            else{ // settings == n_thread_settings::no_threads
                init(0);
            }
        }

        template< class queue >
        inline xthreadpool_impl<queue>::xthreadpool_impl(const std::size_t n)
        :   m_destructed(false)
        {
            init(n);
        }


        template< class queue >
        inline void xthreadpool_impl<queue>::init(const std::size_t n)
        {
            m_busy.store(0);
            for(auto worker_index = 0; worker_index<n; ++worker_index)
            {   
                m_threads.emplace_back(&xthreadpool_impl<queue>::run, this, worker_index);
            }
        }

        template< class queue >
        void xthreadpool_impl<queue>::run(std::size_t worker_index){
            while(true){
                function_type task;
                {
                    unique_lock_type lock(this->m_queue_mutex);

                    // wait for new jobs or destructor
                    this->m_worker_condition.wait(lock, [this]{ return this->m_destructed || !this->m_queue.empty(); });
                    if(!this->m_queue.empty())
                    {
                        ++m_busy;
                        task = std::move(this->m_queue.next());
                        this->m_queue.remove_next();
                        lock.unlock();
                        task(worker_index);
                        --m_busy;
                        m_finish_condition.notify_one();
                    }
                    else if(m_destructed)
                    {
                        return;
                    }
                }
            }
        }

        template< class queue >
        inline xthreadpool_impl<queue>::~xthreadpool_impl()
        {
            {
                unique_lock_type lock(m_queue_mutex);
                m_destructed = true;
            }
            m_worker_condition.notify_all();
            for(auto & worker: m_threads)
            {
                worker.join();
            }
        }

        template< class queue >
        template<class functor, class  ... arguments>
        inline auto
        xthreadpool_impl<queue>::enqueue_impl(
            functor&& f, 
            arguments&& ... prio_or_none
        )// -> std::future<decltype(f(0))>
        {
            typedef decltype(f(0)) result_type;
            typedef std::packaged_task<result_type(std::size_t)> packaged_task_type;

            #if XTL_XTHREADPOOL_USE_RAW_PTR
            auto task_ptr = new packaged_task_type(f);
            #else
            auto task_ptr = std::make_shared<packaged_task_type>(f);
            #endif
            

            auto res = task_ptr->get_future();

            if(m_threads.size()>0)
            {
                {
                    unique_lock_type lock(m_queue_mutex);
      
                    if(m_destructed)
                        throw std::runtime_error("enqueue on a destructed xthreadpool_impl is disallowed");

                    static_assert(sizeof ... (prio_or_none)==0 || sizeof ... (prio_or_none) == 1, "internal error");
                    
                    #if XTL_XTHREADPOOL_USE_RAW_PTR
                    auto task_lambda = [task_ptr](std::size_t worker_index){
                        (*task_ptr)(worker_index);
                        delete task_ptr;
                    };
                    #else
                    auto task_lambda = [task_ptr](std::size_t worker_index){
                        (*task_ptr)(worker_index);
                    };
                    #endif

                    m_queue.emplace(task_lambda,prio_or_none...);
                }
                m_worker_condition.notify_one();
            }
            else
            {
                (*task_ptr)(0);
                #if XTL_XTHREADPOOL_USE_RAW_PTR     
                delete task_ptr;
                #endif      
            }

            return res;
        }

        template< class queue >
        inline void xthreadpool_impl<queue>::wait()
        {
            unique_lock_type lock(m_queue_mutex);
            m_finish_condition.wait(lock, [this]()
            { 
                return m_queue.empty() && (m_busy == 0); 
            });
        }

        template< class queue >
        inline std::size_t xthreadpool_impl<queue>::n_worker_threads() const
        {
            return m_threads.size();
        }

        template< class queue >
        inline std::size_t xthreadpool_impl<queue>::n_threads() const
        {
            return m_threads.size() == 0 ? std::size_t(1) : m_threads.size();
        }

    } // Close namespace xtl::detail.

} // Close namespace xtl.
#endif // XTL_XTHREADPOOL_HPP
