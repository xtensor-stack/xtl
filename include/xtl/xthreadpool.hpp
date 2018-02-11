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

namespace xtl
{

/** \addtogroup ParallelProcessing
*/

//@{

    /**\brief Option base class for parallel algorithms.

        <b>\#include</b> \<xtl/xthreadpool.hpp\><br>
        Namespace: xtl
    */
class xparallel_options
{
    public:

        /** Constants for special settings.
        */
    enum {
        Auto       = -1, ///< Determine number of threads automatically (from <tt>std::thread::hardware_concurrency()</tt>)
        Nice       = -2, ///< Use half as many threads as <tt>Auto</tt> would.
        NoThreads  =  0  ///< Switch off multi-threading (i.e. execute tasks sequentially)
    };

    xparallel_options(int numberOfThreads = int(-1))
    :   numThreads_(xparallel_options::n_worker_threads(numberOfThreads))
    {}

        /** \brief Get desired number of threads.

            <b>Note:</b> This function may return 0, which means that multi-threading
            shall be switched off entirely. If an algorithm receives this value,
            it should revert to a sequential implementation. In contrast, if
            <tt>numThread() == 1</tt>, the parallel algorithm version shall be
            executed with a single thread.
        */
    int n_worker_threads() const
    {
        return numThreads_;
    }

        /** \brief Get desired number of threads.

            In contrast to <tt>numThread()</tt>, this will always return a value <tt>>=1</tt>.
        */
    int n_threads() const
    {
        return std::max(1,numThreads_);
    }


  private:
    // helper function to compute the actual number of threads
    static int n_worker_threads(const int desired_n_threads)
    {
        #ifdef xtl_SINGLE_THREADED
            return 0;
        #else
            return  desired_n_threads >= 0  ? 
                        desired_n_threads : 
                        desired_n_threads == Nice ? 
                            int(std::thread::hardware_concurrency()) / 2 : 
                            int(std::thread::hardware_concurrency());
        #endif
    }

    int numThreads_;
};

/********************************************************/
/*                                                      */
/*                      xthreadpool                      */
/*                                                      */
/********************************************************/

    /**\brief Thread pool class to manage a set of parallel workers.

        <b>\#include</b> \<xtl/xthreadpool.hpp\><br>
        Namespace: xtl
    */
class xthreadpool
{
  public:

    /** Create a thread pool from xparallel_options. The constructor just launches
        the desired number of workers. If the number of threads is zero,
        no workers are started, and all tasks will be executed in synchronously
        in the present thread.
     */
    xthreadpool(const xparallel_options & options = xparallel_options())
    :   options_(options),
        stop(false)        
    {
        init();
    }



    /** Create a thread pool with n threads. The constructor just launches
        the desired number of workers. If \arg n is <tt>xparallel_options::Auto</tt>,
        the number of threads is determined by <tt>std::thread::hardware_concurrency()</tt>.
        <tt>xparallel_options::Nice</tt> will create half as many threads.
        If <tt>n = 0</tt>, no workers are started, and all tasks will be executed
        synchronously in the present thread. If the preprocessor flag
        <tt>xtl_SINGLE_THREADED</tt> is defined, the number of threads is always set
        to zero (i.e. synchronous execution), regardless of the value of \arg n. This
        is useful for debugging.
     */
    xthreadpool(const int n)
    :   options_(n),
        stop(false)
    {
        init();
    }


    /**
     * The destructor joins all threads.
     */
    ~xthreadpool();

    /**
     * Enqueue a task that will be executed by the thread pool.
     * The task result can be obtained using the get() function of the returned future.
     * If the task throws an exception, it will be raised on the call to get().
     */
    template<class F>
    auto enqueue(F&& f) -> std::future<decltype(f(0))>;


    /**
     * Block until all tasks are finished.
     */
    void wait()
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        finish_condition.wait(lock, [this](){ return tasks.empty() && (busy == 0); });
    }

    /**
     * Return the number of worker threads.
     */
    size_t n_worker_threads() const
    {
        return workers.size();
    }

    size_t n_threads() const
    {
        return workers.size() == 0 ? std::size_t(1) : workers.size();
    }

private:

    // helper function to init the thread pool
    void init();

    xparallel_options options_;

    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;

    // the task queue
    std::queue<std::function<void(int)> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable worker_condition;
    std::condition_variable finish_condition;
    bool stop;
    std::atomic_long busy, processed;
};

inline void xthreadpool::init()
{
    busy.store(0);
    processed.store(0);

    const auto actualNThreads = options_.n_worker_threads();
    for(auto ti = 0; ti<actualNThreads; ++ti)
    {
        workers.emplace_back(
            [ti,this]
            {
                for(;;)
                {
                    std::function<void(int)> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // will wait if : stop == false  AND queue is empty
                        // if stop == true AND queue is empty thread function will return later
                        //
                        // so the idea of this wait, is : If where are not in the destructor
                        // (which sets stop to true, we wait here for new jobs)
                        this->worker_condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                        if(!this->tasks.empty())
                        {
                            ++busy;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                            lock.unlock();
                            task(ti);
                            ++processed;
                            --busy;
                            finish_condition.notify_one();
                        }
                        else if(stop)
                        {
                            return;
                        }
                    }
                }
            }
        );
    }
}

inline xthreadpool::~xthreadpool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    worker_condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}

template<class F>
inline auto
xthreadpool::enqueue(F&& f) -> std::future<decltype(f(0))>
{
    typedef decltype(f(0)) result_type;
    typedef std::packaged_task<result_type(int)> PackageType;

    auto task = std::make_shared<PackageType>(f);
    auto res = task->get_future();

    if(workers.size()>0){
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            // don't allow enqueueing after stopping the pool
            if(stop)
                throw std::runtime_error("enqueue on stopped xthreadpool");

            tasks.emplace(
                [task](int tid)
                {
                    (*task)(std::move(tid));
                }
            );
        }
        worker_condition.notify_one();
    }
    else{
        (*task)(0);
    }

    return res;
}


} // Close namespace.
#endif // XTL_XTHREADPOOL_HPP
