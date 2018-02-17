#ifndef XTL_XPARALLEL_FOR_EACH_HPP
#define XTL_XPARALLEL_FOR_EACH_HPP

#define XTL_DEFAULT_CHUNKING_FACTOR 3.0

#include <future>
#include <type_traits>
#include "xtl/xthreadpool.hpp"


namespace xtl
{      
    ///\cond
    namespace detail{





        template<class integer, class functor,typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
        void xparallel_integer_foreach_impl(
            const integer start,
            const integer stop,
            const integer step, 
            xthreadpool& pool,
            functor&& f
        ){
            const auto dist = stop - start;
            const auto size = static_cast<integer>(std::ceil(static_cast<float>(dist)/static_cast<float>(step)));

            auto workload = size;
            const auto work_per_thread = workload / pool.n_worker_threads();
            const auto tmp = std::llround(double(work_per_thread) / double(XTL_DEFAULT_CHUNKING_FACTOR));
            const auto chunk_size = std::max(static_cast<integer>(tmp), static_cast<integer>(1));
            
            std::vector<std::future<void> > futures;
            futures.reserve(workload/chunk_size + 1);

            // TODO: this reads horrible!!!
            // rewrite me
            uint64_t real_index = start;
            for(integer i=0; i<size; i += chunk_size){
                const size_t lc = std::min(workload, chunk_size);
                workload -= lc;
                futures.emplace_back(
                    pool.enqueue([&f, i, lc, real_index, step](int thread_id){

                        uint64_t real_index_cp = real_index; // i get a const copy of real_index?

                        for(integer j=0; j<lc; ++j){
                            f(thread_id, real_index_cp);
                            real_index_cp += step;
                        }
                    })
                );
                real_index += lc*step;
            }
            for (auto& fut : futures){
                fut.get();
            }
        }


        template<class integer, class functor,typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
        void xparallel_integer_foreach_impl(
            const integer size,
            xthreadpool& pool,
            functor&& f
        ){
            auto workload = size;
            const auto work_per_thread = workload / pool.n_worker_threads();
            const auto tmp = std::llround(double(work_per_thread) / double(XTL_DEFAULT_CHUNKING_FACTOR));
            const auto chunk_size = std::max(integer(tmp), integer(1));
            
            std::vector<std::future<void> > futures;
            futures.reserve(workload/chunk_size + 1);

            for(integer i=0; i<size; i += chunk_size){
                const size_t lc = std::min(workload, chunk_size);
                workload -= lc;
                futures.emplace_back(
                    pool.enqueue([&f, i, lc](int thread_id){
                        for(integer j=i; j<i+lc; ++j){
                            f(thread_id, j);
                        }
                    })
                );
            }
            for (auto& fut : futures){
                fut.get();
            }
        }

        template<class integer, class functor,typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
        void seriell_integer_foreach_impl(
            const integer size,
            functor&& f
        ){
            for(integer i=0; i<size; ++i){
                f(int(0), i);
            }
        }

        template<class integer, class functor,typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
        void seriell_integer_foreach_impl(
            const integer start,
            const integer stop,
            const integer step,
            functor&& f
        ){
            for(integer i=start; i<stop; i += step){
                f(int(0), i);
            }
        }

        template<class ITERATOR, class functor>
        void xparallel_iterator_foreach_impl(
            ITERATOR iter,
            ITERATOR end,
            const uint64_t size,
            xthreadpool& pool,
            functor&& f,
            const std::random_access_iterator_tag
        ){
            uint64_t workload = size > 0 ? size : static_cast<uint64_t>(std::distance(iter, end));
            const uint64_t work_per_thread = workload / pool.n_worker_threads();
            const uint64_t tmp = static_cast<uint64_t>(std::llround(double(work_per_thread) / double(XTL_DEFAULT_CHUNKING_FACTOR)));
            const uint64_t chunk_size = std::max(uint64_t(tmp), uint64_t(1));
            
            std::vector<std::future<void> > futures;
            futures.reserve(workload/chunk_size + 1);

            for(;iter<end; iter+= static_cast<std::ptrdiff_t>(chunk_size)){

                const std::size_t lc = std::min(workload, chunk_size);
                workload -= lc;
                futures.emplace_back(
                    pool.enqueue(
                        [&f, iter, lc](int worker_index)
                        {
                            for(std::size_t i=0; i<lc; ++i)
                                f(worker_index, iter[i]);
                        }
                    )
                );
            }

            for (auto& fut : futures){
                fut.get();
            }
        }


        template<class ITERATOR, class functor>
        void xparallel_iterator_foreach_impl(
            ITERATOR iter,
            ITERATOR end,
            const uint64_t size,
            xthreadpool& pool,
            functor&& f,
            const std::forward_iterator_tag
        ){
            uint64_t workload = size > 0 ? size : static_cast<uint64_t>(std::distance(iter, end));
            const uint64_t work_per_thread = workload / pool.n_worker_threads();
            const uint64_t tmp = static_cast<uint64_t>(std::llround(double(work_per_thread) / double(XTL_DEFAULT_CHUNKING_FACTOR)));
            const uint64_t chunk_size = std::max(uint64_t(tmp), uint64_t(1));
            
            std::vector<std::future<void> > futures;
            futures.reserve(workload/chunk_size + 1);

            for(;;){

                const uint64_t lc = std::min(chunk_size, workload);
                workload -= lc;
                futures.emplace_back(
                    pool.enqueue([&f, iter, lc] (int worker_index){
                        auto iter_copy = iter;

                        for(uint64_t i=0; i<lc; ++i){
                            f(worker_index, *iter_copy);
                            ++iter_copy;
                        }

                    })
                );
                for (uint64_t i = 0; i < lc; ++i)
                {
                    ++iter;
                    if (iter == end){
                        assert(workload == 0);
                        break;
                    }
                }
                if(workload==0)
                    break;
            }



            for (auto& fut : futures){
                fut.get();
            }
        }


        template<class ITERATOR, class functor>
        void xparallel_iterator_foreach_impl(
            ITERATOR iter,
            ITERATOR end,
            const uint64_t size,
            xthreadpool& pool,
            functor&& f,
            const std::input_iterator_tag
        ){
            // todo use better impl if size is known (aka size!=0)
            // also it could be very expensive to store 
            // a future for each single item

            std::vector<std::future<void> > futures;

            for (; iter != end; ++iter)
            {
                auto item = *iter;
                futures.emplace_back(
                    pool.enqueue(
                        [&f,&item](int id){
                            f(id, item);
                        }
                    )
                );
            }
            for (auto& fut : futures){
                fut.get();
            }
        }


        template<class ITERATOR, class functor, class TAG>
        void seriell_iterator_foreach_impl(
            ITERATOR iter,
            ITERATOR end,
            functor&& f,
            const TAG& 
        ){
            while(iter!=end){
                f(int(0), *iter);
                ++iter;
            }
        }

    }

    // strided
    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
    void xparallel_foreach(
        const integer start,
        const integer stop,
        const integer step,
        const std::size_t n_threads,
        functor&& f
    ){
        if(n_threads!=0){
            xthreadpool pool(n_threads);
            detail::xparallel_integer_foreach_impl(start,stop,step, pool, std::forward<functor>(f));
        }
        else{
            detail::seriell_integer_foreach_impl(start,stop,step, std::forward<functor>(f));
        }
    }


    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
    void xparallel_foreach(
        const integer start,
        const integer stop,
        const integer step,
        const n_thread_settings & settings,
        functor&& f
    ){
        if(settings != n_thread_settings::no_threads){
            xthreadpool pool(settings);
            detail::xparallel_integer_foreach_impl(start,stop,step, pool, std::forward<functor>(f));
        }
        else{
            detail::seriell_integer_foreach_impl(start,stop,step, std::forward<functor>(f));
        }
    }


    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
    void xparallel_foreach(
        const integer start,
        const integer stop,
        const integer step,
        functor&& f
    ){
        
        xthreadpool pool(n_thread_settings::default_n_threads);
        detail::xparallel_integer_foreach_impl(start,stop,step, pool, std::forward<functor>(f));
    }

    
    // non strided start at zero
    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
    void xparallel_foreach(
        const integer size,
        xthreadpool & pool,
        functor&& f
    ){
        if(pool.n_worker_threads()!=0){
            detail::xparallel_integer_foreach_impl(size, pool, std::forward<functor>(f));
        }
        else{
            detail::seriell_integer_foreach_impl(size, std::forward<functor>(f));
        }
    }



    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
    void xparallel_foreach(
        const integer size,
        const std::size_t n_threads,
        functor&& f
    ){
        if(n_threads!=0){
            xthreadpool pool(n_threads);
            detail::xparallel_integer_foreach_impl(size, pool, std::forward<functor>(f));
        }
        else{
            detail::seriell_integer_foreach_impl(size, std::forward<functor>(f));
        }
    }

    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
    void xparallel_foreach(
        const integer size,
        const n_thread_settings & settings,
        functor&& f
    ){
        if(settings!=n_thread_settings::no_threads){
            xthreadpool pool(settings);
            detail::xparallel_integer_foreach_impl(size, pool, std::forward<functor>(f));
        }
        else{
            detail::seriell_integer_foreach_impl(size, std::forward<functor>(f));
        }
    }



    template<class integer, class functor, typename std::enable_if<std::is_integral<integer>::value ,int>::type = 0>
    void xparallel_foreach(
        const integer size,
        functor&& f
    ){
        xthreadpool pool(n_thread_settings::default_n_threads);
        detail::xparallel_integer_foreach_impl(size, std::forward<functor>(f));
    }




    // iterator overloads
    template<class ITERATOR, class functor, typename std::enable_if<!std::is_integral<ITERATOR>::value ,int>::type = 0>
    void xparallel_foreach(
        ITERATOR begin,
        ITERATOR end,
        const std::size_t n_threads,
        functor&& f
    ){
        const auto size = std::distance(begin, end);
        typedef typename std::iterator_traits<ITERATOR>::iterator_category iterator_category;  

        if(n_threads !=0){
            xthreadpool pool((n_threads));
            detail::xparallel_iterator_foreach_impl(begin ,end, size, pool, std::forward<functor>(f), iterator_category());
        }
        else{
            detail::seriell_iterator_foreach_impl(begin ,end, std::forward<functor>(f), iterator_category());
        }
    }


    template<class ITERATOR, class functor>
    void xparallel_foreach(
        ITERATOR begin,
        ITERATOR end,
        const n_thread_settings & settings,
        functor&& f
    ){
        const auto size = std::distance(begin, end);
        typedef typename std::iterator_traits<ITERATOR>::iterator_category iterator_category;  

        if(settings != n_thread_settings::no_threads){
            xthreadpool pool((settings));
            detail::xparallel_iterator_foreach_impl(begin ,end, size, pool, std::forward<functor>(f), iterator_category());
        }
        else{
            detail::seriell_iterator_foreach_impl(begin ,end, std::forward<functor>(f), iterator_category());
        }
    }

    template<class ITERATOR, class functor>
    void xparallel_foreach(
        ITERATOR begin,
        ITERATOR end,
        functor&& f
    ){
        const auto size = std::distance(begin, end);
        typedef typename std::iterator_traits<ITERATOR>::iterator_category iterator_category;  

        xthreadpool pool((n_thread_settings::default_n_threads));
        detail::xparallel_iterator_foreach_impl(begin ,end, size, pool, std::forward<functor>(f), iterator_category());
        
    }






    // container overloads

    template<class CONTAINER, class functor, typename std::enable_if<!std::is_integral<CONTAINER>::value ,int>::type = 0>
    void xparallel_foreach(
        CONTAINER& container,
        const std::size_t n_threads,
        functor&& f
    ){
        auto begin = std::begin(container);
        auto end  = std::end(container);
        typedef typename std::decay<decltype(begin)>::type iterator_type;
        typedef typename std::iterator_traits<iterator_type>::iterator_category iterator_category; 

        const uint64_t size = static_cast<uint64_t>(std::distance(begin, end));
    
        if(n_threads !=0){
            xthreadpool pool((n_threads));
            detail::xparallel_iterator_foreach_impl(begin ,end,size, pool, std::forward<functor>(f), iterator_category());
        }
        else{
            detail::seriell_iterator_foreach_impl(begin ,end, std::forward<functor>(f), iterator_category());
        }

    }


    template<class CONTAINER, class functor, typename std::enable_if<!std::is_integral<CONTAINER>::value ,int>::type = 0>
    void xparallel_foreach(
        CONTAINER& container,
        const n_thread_settings & settings,
        functor&& f
    ){
        auto begin = std::begin(container);
        auto end  = std::end(container);
        typedef typename std::decay<decltype(begin)>::type iterator_type;
        typedef typename std::iterator_traits<iterator_type>::iterator_category iterator_category; 

        const uint64_t size = static_cast<uint64_t>(std::distance(begin, end));

        if(settings != n_thread_settings::no_threads){  
            xthreadpool pool((settings));
            detail::xparallel_iterator_foreach_impl(begin ,end, size, pool, std::forward<functor>(f), iterator_category());
        }
        else{
            detail::seriell_iterator_foreach_impl(begin ,end, std::forward<functor>(f), iterator_category());
        }

    }

    template<class CONTAINER, class functor, typename std::enable_if<!std::is_integral<CONTAINER>::value ,int>::type = 0>
    void xparallel_foreach(
        CONTAINER& container,
        functor&& f
    ){
        auto begin = std::begin(container);
        auto end  = std::end(container);
        typedef typename std::decay<decltype(begin)>::type iterator_type;
        typedef typename std::iterator_traits<iterator_type>::iterator_category iterator_category; 

        const uint64_t size = static_cast<uint64_t>(std::distance(begin, end));
        xthreadpool pool((n_thread_settings::default_n_threads));
        detail::xparallel_iterator_foreach_impl(begin ,end, size, pool, std::forward<functor>(f), iterator_category());


    }



    ///\endcond
} // end namespace xtl

#endif // XTL_XPARALLEL_FOR_EACH_HPP