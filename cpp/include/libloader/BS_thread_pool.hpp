// Copyright (c) 2023. Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

/**
 *  Modified version of Barak Shoshany's thread pool library
 *  link: https://github.com/bshoshany/thread-pool
 *  License: MIT
 */
#pragma once

/**
 * @file BS_thread_pool.hpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 3.3.0
 * @date 2022-08-03
 * @copyright Copyright (c) 2022 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library. This header file contains the entire library, including the main BS::thread_pool class and the helper classes BS::multi_future, BS::blocks, BS:synced_stream, and BS::timer.
 */

#define BS_THREAD_POOL_VERSION "v3.3.0 (2022-08-03)"

#include "LoaderLogger.h"
#include "PlatformThreading.h" // platform_set_thread_name, platform_thread_self
#include <atomic>              // std::atomic
#include <chrono>              // std::chrono
#include <condition_variable>  // std::condition_variable
#include <exception>           // std::current_exception
#include <functional>          // std::bind, std::function, std::invoke
#include <future>              // std::future, std::promise
#include <iostream>            // std::cout, std::endl, std::flush, std::ostream
#include <limits>              // std::numeric_limits
#include <memory>              // std::make_shared, std::make_unique, std::shared_ptr, std::unique_ptr
#include <mutex>               // std::mutex, std::scoped_lock, std::unique_lock
#include <queue>               // std::queue
#include <thread>              // std::thread
#include <type_traits>         // std::common_type_t, std::conditional_t, std::decay_t, std::invoke_result_t, std::is_void_v
#include <utility>             // std::forward, std::move, std::swap
#include <vector>              // std::vector


namespace BS {
    /**
 * @brief A convenient shorthand for the type of std::thread::hardware_concurrency(). Should evaluate to unsigned int.
 */
    using concurrency_t = std::invoke_result_t<decltype(std::thread::hardware_concurrency)>;

    // ============================================================================================= //
    //                                    Begin class multi_future                                   //

    /**
     * @brief A helper class to facilitate waiting for and/or getting the results of multiple futures at once.
     *
     * @tparam T The return type of the futures.
     */
    template<typename T>
    class [[nodiscard]] multi_future {
    public:
        /**
     * @brief Construct a multi_future object with the given number of futures.
     *
     * @param num_futures_ The desired number of futures to store.
     */
        multi_future(const size_t num_futures_ = 0) : futures(num_futures_) {}

        /**
     * @brief Get the results from all the futures stored in this multi_future object, rethrowing any stored exceptions.
     *
     * @return If the futures return void, this function returns void as well. Otherwise, it returns a vector containing the results.
     */
        [[nodiscard]] std::conditional_t<std::is_void_v<T>, void, std::vector<T>> get() {
            if constexpr (std::is_void_v<T>) {
                for (size_t i = 0; i < futures.size(); ++i)
                    futures[i].get();
                return;
            } else {
                std::vector<T> results(futures.size());
                for (size_t i = 0; i < futures.size(); ++i)
                    results[i] = futures[i].get();
                return results;
            }
        }

        /**
     * @brief Get a reference to one of the futures stored in this multi_future object.
     *
     * @param i The index of the desired future.
     * @return The future.
     */
        [[nodiscard]] std::future<T> &operator[](const size_t i) {
            return futures[i];
        }

        /**
     * @brief Append a future to this multi_future object.
     *
     * @param future The future to append.
     */
        void push_back(std::future<T> future) {
            futures.push_back(std::move(future));
        }

        /**
     * @brief Get the number of futures stored in this multi_future object.
     *
     * @return The number of futures.
     */
        [[nodiscard]] size_t size() const {
            return futures.size();
        }

        /**
     * @brief Wait for all the futures stored in this multi_future object.
     */
        void wait() const {
            for (size_t i = 0; i < futures.size(); ++i)
                futures[i].wait();
        }

    private:
        /**
     * @brief A vector to store the futures.
     */
        std::vector<std::future<T>> futures;
    };

    //                                     End class multi_future                                    //
    // ============================================================================================= //

    // ============================================================================================= //
    //                                       Begin class blocks                                      //

    /**
     * @brief A helper class to divide a range into blocks. Used by parallelize_loop() and push_loop().
     *
     * @tparam T1 The type of the first index in the range. Should be a signed or unsigned integer.
     * @tparam T2 The type of the index after the last index in the range. Should be a signed or unsigned integer. If T1 is not the same as T2, a common type will be automatically inferred.
     * @tparam T The common type of T1 and T2.
     */
    template<typename T1, typename T2, typename T = std::common_type_t<T1, T2>>
    class [[nodiscard]] blocks {
    public:
        /**
     * @brief Construct a blocks object with the given specifications.
     *
     * @param first_index_ The first index in the range.
     * @param index_after_last_ The index after the last index in the range.
     * @param num_blocks_ The desired number of blocks to divide the range into.
     */
        blocks(const T1 first_index_, const T2 index_after_last_, const size_t num_blocks_) : first_index(static_cast<T>(first_index_)), index_after_last(static_cast<T>(index_after_last_)), num_blocks(num_blocks_) {
            if (index_after_last < first_index)
                std::swap(index_after_last, first_index);
            total_size = static_cast<size_t>(index_after_last - first_index);
            block_size = static_cast<size_t>(total_size / num_blocks);
            if (block_size == 0) {
                block_size = 1;
                num_blocks = (total_size > 1) ? total_size : 1;
            }
        }

        /**
     * @brief Get the first index of a block.
     *
     * @param i The block number.
     * @return The first index.
     */
        [[nodiscard]] T start(const size_t i) const {
            return static_cast<T>(i * block_size) + first_index;
        }

        /**
     * @brief Get the index after the last index of a block.
     *
     * @param i The block number.
     * @return The index after the last index.
     */
        [[nodiscard]] T end(const size_t i) const {
            return (i == num_blocks - 1) ? index_after_last : (static_cast<T>((i + 1) * block_size) + first_index);
        }

        /**
     * @brief Get the number of blocks. Note that this may be different than the desired number of blocks that was passed to the constructor.
     *
     * @return The number of blocks.
     */
        [[nodiscard]] size_t get_num_blocks() const {
            return num_blocks;
        }

        /**
     * @brief Get the total number of indices in the range.
     *
     * @return The total number of indices.
     */
        [[nodiscard]] size_t get_total_size() const {
            return total_size;
        }

    private:
        /**
     * @brief The size of each block (except possibly the last block).
     */
        size_t block_size = 0;

        /**
     * @brief The first index in the range.
     */
        T first_index = 0;

        /**
     * @brief The index after the last index in the range.
     */
        T index_after_last = 0;

        /**
     * @brief The number of blocks.
     */
        size_t num_blocks = 0;

        /**
     * @brief The total number of indices in the range.
     */
        size_t total_size = 0;
    };

    //                                        End class blocks                                       //
    // ============================================================================================= //

    // ============================================================================================= //
    //                                    Begin class thread_pool                                    //

    /**
     * @brief A fast, lightweight, and easy-to-use C++17 thread pool class.
     */
    class [[nodiscard]] thread_pool {
    public:
        // ============================
        // Constructors and destructors
        // ============================

        /**
         * @brief Construct a new thread pool.
         *
         * @param thread_count_ The number of threads to use. The default value is the total number of hardware threads available, as reported by the implementation. This is usually determined by the number of cores in the CPU. If a core is hyperthreaded, it will count as two threads.
         */
        thread_pool(const concurrency_t thread_count_ = 0) : thread_count(determine_thread_count(thread_count_)), threads(std::make_unique<std::thread[]>(determine_thread_count(thread_count_))) {
            create_threads();
        }

        /**
         * @brief Destruct the thread pool. Waits for all tasks to complete, then destroys all threads. Note that if the pool is paused, then any tasks still in the queue will never be executed.
         */
        ~thread_pool() {
            wait_for_tasks();
            destroy_threads();
        }

        // =======================
        // Public member functions
        // =======================

        /**
         * @brief Get the number of tasks currently waiting in the queue to be executed by the threads.
         *
         * @return The number of queued tasks.
         */
        [[nodiscard]] size_t get_tasks_queued() const {
            const std::scoped_lock tasks_lock(tasks_mutex);
            return tasks.size();
        }

        /**
         * @brief Get the number of tasks currently being executed by the threads.
         *
         * @return The number of running tasks.
         */
        [[nodiscard]] size_t get_tasks_running() const {
            const std::scoped_lock tasks_lock(tasks_mutex);
            return tasks_total - tasks.size();
        }

        /**
         * @brief Get the total number of unfinished tasks: either still in the queue, or running in a thread. Note that get_tasks_total() == get_tasks_queued() + get_tasks_running().
         *
         * @return The total number of tasks.
         */
        [[nodiscard]] size_t get_tasks_total() const {
            return tasks_total;
        }

        /**
         * @brief Get the number of threads in the pool.
         *
         * @return The number of threads.
         */
        [[nodiscard]] concurrency_t get_thread_count() const {
            return thread_count;
        }

        /**
         * @brief Check whether the pool is currently paused.
         *
         * @return true if the pool is paused, false if it is not paused.
         */
        [[nodiscard]] bool is_paused() const {
            return paused;
        }

        /**
         * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Returns a multi_future object that contains the futures for all of the blocks.
         *
         * @tparam F The type of the function to loop through.
         * @tparam T1 The type of the first index in the loop. Should be a signed or unsigned integer.
         * @tparam T2 The type of the index after the last index in the loop. Should be a signed or unsigned integer. If T1 is not the same as T2, a common type will be automatically inferred.
         * @tparam T The common type of T1 and T2.
         * @tparam R The return value of the loop function F (can be void).
         * @param first_index The first index in the loop.
         * @param index_after_last The index after the last index in the loop. The loop will iterate from first_index to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = first_index; i < index_after_last; ++i)". Note that if index_after_last == first_index, no blocks will be submitted.
         * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
         * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
         * @return A multi_future object that can be used to wait for all the blocks to finish. If the loop function returns a value, the multi_future object can also be used to obtain the values returned by each block.
         */
        template<typename It, typename R = std::invoke_result_t<std::decay_t<decltype(*std::declval<It>())>>>
        [[nodiscard]] multi_future<R> run_over(It &&begin, It &&end, size_t inSize) {
            It ptr = begin;
            if (inSize) {
                multi_future<R> mf(inSize);
                {
                    std::scoped_lock lk(tasks_mutex);
                    size_t i = 0;
                    while (ptr != end && i < inSize) {
                        mf[i++] = submit_nolock(*ptr);
                    }
                }
                return mf;
            } else {
                return multi_future<R>();
            }
        }

        /**
         * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Returns a multi_future object that contains the futures for all of the blocks.
         *
         * @tparam F The type of the function to loop through.
         * @tparam T1 The type of the first index in the loop. Should be a signed or unsigned integer.
         * @tparam T2 The type of the index after the last index in the loop. Should be a signed or unsigned integer. If T1 is not the same as T2, a common type will be automatically inferred.
         * @tparam T The common type of T1 and T2.
         * @tparam R The return value of the loop function F (can be void).
         * @param first_index The first index in the loop.
         * @param index_after_last The index after the last index in the loop. The loop will iterate from first_index to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = first_index; i < index_after_last; ++i)". Note that if index_after_last == first_index, no blocks will be submitted.
         * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
         * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
         * @return A multi_future object that can be used to wait for all the blocks to finish. If the loop function returns a value, the multi_future object can also be used to obtain the values returned by each block.
         */
        template<typename F, typename T1, typename T2, typename T = std::common_type_t<T1, T2>, typename R = std::invoke_result_t<std::decay_t<F>, T, T>>
        [[nodiscard]] multi_future<R> parallelize_loop(const T1 first_index, const T2 index_after_last, F &&loop, const size_t num_blocks = 0) {
            blocks blks(first_index, index_after_last, num_blocks ? num_blocks : thread_count);
            if (blks.get_total_size() > 0) {
                multi_future<R> mf(blks.get_num_blocks());
                {
                    std::scoped_lock lk(tasks_mutex);
                    for (size_t i = 0; i < blks.get_num_blocks(); ++i)
                        mf[i] = submit_nolock(std::forward<F>(loop), blks.start(i), blks.end(i));
                }
                return mf;
            } else {
                return multi_future<R>();
            }
        }

        /**
         * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Returns a multi_future object that contains the futures for all of the blocks. This overload is used for the special case where the first index is 0.
         *
         * @tparam F The type of the function to loop through.
         * @tparam T The type of the loop indices. Should be a signed or unsigned integer.
         * @tparam R The return value of the loop function F (can be void).
         * @param index_after_last The index after the last index in the loop. The loop will iterate from 0 to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = 0; i < index_after_last; ++i)". Note that if index_after_last == 0, no blocks will be submitted.
         * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
         * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
         * @return A multi_future object that can be used to wait for all the blocks to finish. If the loop function returns a value, the multi_future object can also be used to obtain the values returned by each block.
         */
        template<typename F, typename T, typename R = std::invoke_result_t<std::decay_t<F>, T, T>>
        [[nodiscard]] multi_future<R> parallelize_loop(const T index_after_last, F &&loop, const size_t num_blocks = 0) {
            return parallelize_loop(0, index_after_last, std::forward<F>(loop), num_blocks);
        }

        /**
         * @brief Pause the pool. The workers will temporarily stop retrieving new tasks out of the queue, although any tasks already executed will keep running until they are finished.
         */
        void pause() {
            paused = true;
        }

        /**
         * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Does not return a multi_future, so the user must use wait_for_tasks() or some other method to ensure that the loop finishes executing, otherwise bad things will happen.
         *
         * @tparam F The type of the function to loop through.
         * @tparam T1 The type of the first index in the loop. Should be a signed or unsigned integer.
         * @tparam T2 The type of the index after the last index in the loop. Should be a signed or unsigned integer. If T1 is not the same as T2, a common type will be automatically inferred.
         * @tparam T The common type of T1 and T2.
         * @param first_index The first index in the loop.
         * @param index_after_last The index after the last index in the loop. The loop will iterate from first_index to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = first_index; i < index_after_last; ++i)". Note that if index_after_last == first_index, no blocks will be submitted.
         * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
         * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
         */
        template<typename F, typename T1, typename T2, typename T = std::common_type_t<T1, T2>>
        void push_loop(const T1 first_index, const T2 index_after_last, F &&loop, const size_t num_blocks = 0) {
            blocks blks(first_index, index_after_last, num_blocks ? num_blocks : thread_count);
            if (blks.get_total_size() > 0) {
                for (size_t i = 0; i < blks.get_num_blocks(); ++i)
                    push_task(std::forward<F>(loop), blks.start(i), blks.end(i));
            }
        }

        /**
         * @brief Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. Does not return a multi_future, so the user must use wait_for_tasks() or some other method to ensure that the loop finishes executing, otherwise bad things will happen. This overload is used for the special case where the first index is 0.
         *
         * @tparam F The type of the function to loop through.
         * @tparam T The type of the loop indices. Should be a signed or unsigned integer.
         * @param index_after_last The index after the last index in the loop. The loop will iterate from 0 to (index_after_last - 1) inclusive. In other words, it will be equivalent to "for (T i = 0; i < index_after_last; ++i)". Note that if index_after_last == 0, no blocks will be submitted.
         * @param loop The function to loop through. Will be called once per block. Should take exactly two arguments: the first index in the block and the index after the last index in the block. loop(start, end) should typically involve a loop of the form "for (T i = start; i < end; ++i)".
         * @param num_blocks The maximum number of blocks to split the loop into. The default is to use the number of threads in the pool.
         */
        template<typename F, typename T>
        void push_loop(const T index_after_last, F &&loop, const size_t num_blocks = 0) {
            push_loop(0, index_after_last, std::forward<F>(loop), num_blocks);
        }

        /**
         * @brief Push a function with zero or more arguments, but no return value, into the task queue. Does not return a future, so the user must use wait_for_tasks() or some other method to ensure that the task finishes executing, otherwise bad things will happen.
         *
         * @tparam F The type of the function.
         * @tparam A The types of the arguments.
         * @param task The function to push.
         * @param args The zero or more arguments to pass to the function. Note that if the task is a class member function, the first argument must be a pointer to the object, i.e. &object (or this), followed by the actual arguments.
         */
        template<typename F, typename... A>
        void push_task(F &&task, A &&...args) {
            std::function<void()> task_function = std::bind(std::forward<F>(task), std::forward<A>(args)...);
            {
                const std::scoped_lock tasks_lock(tasks_mutex);
                tasks.push(std::move(task_function));
            }
            ++tasks_total;
            task_available_cv.notify_one();
        }

        /**
         * @brief Same as push_task, but without locking the mutex.
         */
        template<typename F, typename... A>
        void push_task_nolock(F &&task, A &&...args) {
            std::function<void()> task_function = std::bind(std::forward<F>(task), std::forward<A>(args)...);
            {
                tasks.push(std::move(task_function));
            }
            ++tasks_total;
            task_available_cv.notify_one();
        }

        /**
         * @brief Reset the number of threads in the pool. Waits for all currently running tasks to be completed, then destroys all threads in the pool and creates a new thread pool with the new number of threads. Any tasks that were waiting in the queue before the pool was reset will then be executed by the new threads. If the pool was paused before resetting it, the new pool will be paused as well.
         *
         * @param thread_count_ The number of threads to use. The default value is the total number of hardware threads available, as reported by the implementation. This is usually determined by the number of cores in the CPU. If a core is hyperthreaded, it will count as two threads.
         */
        void reset(const concurrency_t thread_count_ = 0) {
            const bool was_paused = paused;
            paused = true;
            wait_for_tasks();
            destroy_threads();
            thread_count = determine_thread_count(thread_count_);
            threads = std::make_unique<std::thread[]>(thread_count);
            paused = was_paused;
            create_threads();
        }

        /**
         * @brief Submit a function with zero or more arguments into the task queue. If the function has a return value, get a future for the eventual returned value. If the function has no return value, get an std::future<void> which can be used to wait until the task finishes.
         *
         * @tparam F The type of the function.
         * @tparam A The types of the zero or more arguments to pass to the function.
         * @tparam R The return type of the function (can be void).
         * @param task The function to submit.
         * @param args The zero or more arguments to pass to the function. Note that if the task is a class member function, the first argument must be a pointer to the object, i.e. &object (or this), followed by the actual arguments.
         * @return A future to be used later to wait for the function to finish executing and/or obtain its returned value if it has one.
         */
        template<typename F, typename... A, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>
        [[nodiscard]] std::future<R> submit(F &&task, A &&...args) {
            auto task_promise = new std::promise<R>;
            auto future = task_promise->get_future();
            push_task(
                    [task_promise](auto &&task_inner, auto &&...argss) {
                        try {
                            if constexpr (std::is_void_v<R>) {
                                std::invoke(std::forward<decltype(task_inner)>(task_inner), std::forward<decltype(argss)>(argss)...);
                                task_promise->set_value();
                            } else {
                                task_promise->set_value(std::invoke(std::forward<decltype(task_inner)>(task_inner), std::forward<decltype(argss)>(argss)...));
                            }
                        } catch (...) {
                            try {
                                task_promise->set_exception(std::current_exception());
                            } catch (...) {
                            }
                        }
                        delete task_promise;
                    },
                    std::forward<F>(task), std::forward<A>(args)...);
            return future;
        }

        /**
         * @brief Same as submit, but without locking mutex.
         */
        template<typename F, typename... A, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>
        [[nodiscard]] std::future<R> submit_nolock(F &&task, A &&...args) {
            auto task_promise = new std::promise<R>;
            auto future = task_promise->get_future();
            push_task_nolock(
                    [task_promise](auto &&task_inner, auto &&...argss) {
                        try {
                            if constexpr (std::is_void_v<R>) {
                                std::invoke(std::forward<decltype(task_inner)>(task_inner), std::forward<decltype(argss)>(argss)...);
                                task_promise->set_value();
                            } else {
                                task_promise->set_value(std::invoke(std::forward<decltype(task_inner)>(task_inner), std::forward<decltype(argss)>(argss)...));
                            }
                        } catch (...) {
                            try {
                                task_promise->set_exception(std::current_exception());
                            } catch (...) {
                            }
                        }
                        delete task_promise;
                    },
                    std::forward<F>(task), std::forward<A>(args)...);
            return future;
        }

        /**
         * @brief Unpause the pool. The workers will resume retrieving new tasks out of the queue.
         */
        void unpause() {
            paused = false;
        }

        /**
         * @brief Wait for tasks to be completed. Normally, this function waits for all tasks, both those that are currently running in the threads and those that are still waiting in the queue. However, if the pool is paused, this function only waits for the currently running tasks (otherwise it would wait forever). Note: To wait for just one specific task, use submit() instead, and call the wait() member function of the generated future.
         */
        void wait_for_tasks() {
            waiting = true;
            std::unique_lock<std::mutex> tasks_lock(tasks_mutex);
            task_done_cv.wait(tasks_lock, [this] { return (tasks_total == (paused ? tasks.size() : 0)); });
            waiting = false;
        }

        static size_t getCurrentThreadIndexView() { return getThreadIndex(); }

        /// @brief force reset a thread, only should be called when a deadly problem happen
        void resetThreadByIndex(size_t index) {
            static std::mutex resetThreadMtx;
            std::lock_guard lk(resetThreadMtx);
            size_t oldThCounter = threadIndexCounter;
            threadIndexCounter = index;
            LibLoader::logger.warning("resetting thread in pool at index " + std::to_string(index)+" due to a user caused error");
            new (&threads[index]) std::thread(&thread_pool::worker, this);
            while (threadIndexCounter != index + 1);
            threadIndexCounter = oldThCounter;
        }

    private:
        // ========================
        // Private member functions
        // ========================

        /**
         * @brief Create the threads in the pool and assign a worker to each thread.
         */
        void create_threads() {
            running = true;
            threadIndexCounter = 0;
            for (concurrency_t i = 0; i < thread_count; ++i) {
                while (threadIndexCounter != i) [[unlikely]] std::this_thread::sleep_for(std::chrono::milliseconds(10));
                threads[i] = std::thread(&thread_pool::worker, this);
            }
        }

        /**
         * @brief Destroy the threads in the pool.
         */
        void destroy_threads() {
            running = false;
            task_available_cv.notify_all();
            for (concurrency_t i = 0; i < thread_count; ++i) {
                threads[i].join();
            }
        }

        /**
         * @brief Determine how many threads the pool should have, based on the parameter passed to the constructor or reset().
         *
         * @param thread_count_ The parameter passed to the constructor or reset(). If the parameter is a positive number, then the pool will be created with this number of threads. If the parameter is non-positive, or a parameter was not supplied (in which case it will have the default value of 0), then the pool will be created with the total number of hardware threads available, as obtained from std::thread::hardware_concurrency(). If the latter returns a non-positive number for some reason, then the pool will be created with just one thread.
         * @return The number of threads to use for constructing the pool.
         */
        [[nodiscard]] concurrency_t determine_thread_count(const concurrency_t thread_count_) {
            if (thread_count_ > 0)
                return thread_count_;
            else {
                if (std::thread::hardware_concurrency() > 0)
                    return std::thread::hardware_concurrency();
                else
                    return 1;
            }
        }

        static size_t &getThreadIndex() {
            // for windows compatibility, use parentheses to avoid calling the macro "max"
            static thread_local size_t index = (std::numeric_limits<size_t>::max)();
            return index;
        }

        void setThreadIndex() {
            getThreadIndex() = threadIndexCounter++;
        }

        /**
         * @brief A worker function to be assigned to each thread in the pool. Waits until it is notified by push_task() that a task is available, and then retrieves the task from the queue and executes it. Once the task finishes, the worker notifies wait_for_tasks() in case it is waiting.
         */
        void worker() {
            platform_set_thread_name("LoaderWorker");
            setThreadIndex();
            while (running) {
                std::function<void()> task;
                std::unique_lock<std::mutex> tasks_lock(tasks_mutex);
                task_available_cv.wait(tasks_lock, [this] { return !tasks.empty() || !running; });
                if (running && !paused) {
                    task = std::move(tasks.front());
                    tasks.pop();
                    tasks_lock.unlock();
                    task();
                    tasks_lock.lock();
                    --tasks_total;
                    if (waiting)
                        task_done_cv.notify_one();
                }
            }
        }

        // ============
        // Private data
        // ============

        /**
         * @brief An atomic variable indicating whether the workers should pause. When set to true, the workers temporarily stop retrieving new tasks out of the queue, although any tasks already executed will keep running until they are finished. When set to false again, the workers resume retrieving tasks.
         */
        std::atomic<bool> paused = false;

        /**
         * @brief An atomic variable indicating to the workers to keep running. When set to false, the workers permanently stop working.
         */
        std::atomic<bool> running = false;

        /**
         * @brief A condition variable used to notify worker() that a new task has become available.
         */
        std::condition_variable task_available_cv = {};

        /**
         * @brief A condition variable used to notify wait_for_tasks() that a tasks is done.
         */
        std::condition_variable task_done_cv = {};

        /**
         * @brief A queue of tasks to be executed by the threads.
         */
        std::queue<std::function<void()>> tasks = {};

        /**
         * @brief An atomic variable to keep track of the total number of unfinished tasks - either still in the queue, or running in a thread.
         */
        std::atomic<size_t> tasks_total = 0;

        /**
         * @brief A mutex to synchronize access to the task queue by different threads.
         */
        mutable std::mutex tasks_mutex = {};

        /**
         * @brief The number of threads in the pool.
         */
        concurrency_t thread_count = 0;

        /**
         * @brief A smart pointer to manage the memory allocated for the threads.
         */
        std::unique_ptr<std::thread[]> threads = nullptr;

        /**
         * @brief An atomic variable indicating that wait_for_tasks() is active and expects to be notified whenever a task is done.
         */
        std::atomic<bool> waiting = false;

        std::atomic<size_t> threadIndexCounter = 0;
    };

    //                                     End class thread_pool                                     //
    // ============================================================================================= //

    inline std::unique_ptr<thread_pool> pool;
} // namespace BS
