#ifndef XTL_XPARALLEL_FOR_EACH_HPP
#define XTL_XPARALLEL_FOR_EACH_HPP

#define XTL_DEFAULT_CHUNKING_FACTOR 3.0

#include <future>

#include "xtl/xthreadpool.hpp"

namespace xtl
{      
    ///\cond
    namespace detail{
        template<class INTEGER, class F>
        void xparallel_foreach_impl(
            const INTEGER size,
            xthreadpool & pool,
            F && f
        ){
            auto workload = size;
            const auto work_per_thread = pool.n_worker_threads();
            const auto tmp = std::llround(double(work_per_thread)/double(XTL_DEFAULT_CHUNKING_FACTOR));
            const auto chunked_work_per_thread = std::max<INTEGER>(INTEGER(tmp), INTEGER(1));
            
            std::vector<std::future<void> > futures;
            futures.reserve(workload/chunked_work_per_thread + 1);

            for(INTEGER i=0; i<size; i += chunked_work_per_thread){
                const size_t lc = std::min(workload, chunked_work_per_thread);
                workload -= lc;
                futures.emplace_back(
                    pool.enqueue([&f, i, lc](int thread_id){
                        for(INTEGER j=i; j<i+lc; ++j){
                            f(thread_id, j);
                        }
                    })
                );
            }
            for (auto & fut : futures){
                fut.get();
            }
        }

        template<class INTEGER, class F>
        void xserial_foreach_impl(
            const INTEGER size,
            F && f
        ){
            for(INTEGER i=0; i<size; ++i){
                f(int(0), i);
            }
        }
    }



    template<class INTEGER, class F>
    void xparallel_foreach(
        const INTEGER size,
        const int n_threads,
        F && f
    ){
        if(n_threads !=0){
            xthreadpool pool(n_threads);
            detail::xparallel_foreach_impl(size, pool, std::forward<F>(f));
        }
        else{
            detail::xserial_foreach_impl(size, std::forward<F>(f));
        }
    }

    template<class INTEGER, class F>
    void xparallel_foreach(
        const INTEGER size,
        const xparallel_options & options,
        F && f
    ){
        if(options.n_worker_threads() == 0){
            xthreadpool pool(options);
        }
        else{
            detail::xparallel_foreach_impl(size, std::forward<F>(f));
        }
    }

    template<class INTEGER, class F>
    void xparallel_foreach(
        const INTEGER size,
        F && f
    ){
        xparallel_options options(-1);
        if(options.n_worker_threads() == 0){
            xthreadpool pool(options);
        }
        else{
            detail::xparallel_foreach_impl(size, std::forward<F>(f));
        }
    }

    ///\endcond
} // end namespace xtl

#endif // XTL_XPARALLEL_FOR_EACH_HPP