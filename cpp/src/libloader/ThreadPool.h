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

#ifndef TESTPROJECT_THREADPOOL_H
#define TESTPROJECT_THREADPOOL_H


#include "LoaderLogger.h"
#include <functional>
#include <future>
#include <queue>
#include <type_traits>


namespace Antares {
    using concurrency_t = decltype(std::thread::hardware_concurrency());

    struct ThreadPoolDefaultTraits {
        static inline void *malloc(size_t size) {
            return ::malloc(size);
        }

        static inline void free(void *ptr) {
            ::free(ptr);
        }
    };

    class ThreadPoolBase {
    protected:
        template<typename T1, typename T2, typename T = std::common_type_t<T1, T2>>
        class [[nodiscard]] blocks {
            size_t block_size = 0;
            size_t num_blocks = 0;
            size_t total_size = 0;
            T first_index = 0;
            T index_after_last = 0;

        public:
            blocks(const T1 first_index_, const T2 index_after_last_, const size_t num_blocks_)
                : num_blocks(num_blocks_),
                  first_index(static_cast<T>(first_index_)),
                  index_after_last(static_cast<T>(index_after_last_)) {
                if (index_after_last < first_index)
                    std::swap(index_after_last, first_index);
                total_size = static_cast<size_t>(index_after_last - first_index);
                block_size = static_cast<size_t>(total_size / num_blocks);
                if (block_size == 0) {
                    block_size = 1;
                    num_blocks = (total_size > 1) ? total_size : 1;
                }
            }

            [[nodiscard]] T start(const size_t i) const {
                return static_cast<T>(i * block_size) + first_index;
            }

            [[nodiscard]] T end(const size_t i) const {
                return (i == num_blocks - 1) ? index_after_last : (static_cast<T>((i + 1) * block_size) + first_index);
            }

            [[nodiscard]] size_t get_num_blocks() const {
                return num_blocks;
            }

            [[nodiscard]] size_t get_total_size() const {
                return total_size;
            }
        };

    protected:
        std::mutex tasks_mtx{};
        std::queue<std::function<void()>> tasks;
        std::condition_variable task_available_cv = {};
        std::condition_variable task_done_cv = {};
        std::atomic<size_t> tasks_total = 0;
        std::atomic<size_t> threadIndexCounter = 0;
        std::atomic<bool> running = false;
        std::atomic<bool> waiting = false;
        std::atomic<bool> paused = false;

    public:
        ThreadPoolBase();

        ~ThreadPoolBase() = default;

        /// a bad design, should avoid calling this
        void wait_for_tasks();

        void pause();

        [[nodiscard]] size_t get_tasks_total() const;

        /// may not be accurate!
        [[nodiscard]] size_t get_tasks_queued() const {
            return tasks.size();
        }

        /// may not be accurate!
        [[nodiscard]] size_t get_tasks_running() const {
            return get_tasks_total() - tasks.size();
        }

        [[nodiscard]] bool is_paused() const;

        void unpause();

        static size_t &getThreadIndex();

        void setThreadIndex();

    protected:
        [[nodiscard]] static concurrency_t determine_thread_count(concurrency_t thread_count_);

        void worker();
    };

    template<typename Traits = ThreadPoolDefaultTraits>
    class ThreadPool : public ThreadPoolBase {
    protected:
        std::vector<std::thread> threads;

    public:
        ThreadPool(concurrency_t thread_count_ = 0) // NOLINT(google-explicit-constructor)
            : ThreadPoolBase(),
              threads(determine_thread_count(thread_count_)) {
            create_threads();
        }

        ~ThreadPool() {
            wait_for_tasks();
            destroy_threads();
        }

        template<typename T>
        static T *Create() {
            auto ptr = (T *) Traits::malloc(sizeof(T));
            new (ptr) T;
            return ptr;
        }

        template<typename T>
        static void Destroy(T *promise) {
            promise->~T();
            Traits::free(promise);
        }

        template<typename F, typename... A>
        void push_task(F &&task, A &&...args) {
            std::function<void()> task_function = std::bind(std::forward<F>(task), std::forward<A>(args)...);
            {
                std::unique_lock tasks_lock(tasks_mtx);
                tasks.push(std::move(task_function));
            }
            tasks_total.fetch_add(1, std::memory_order_acq_rel);
            task_available_cv.notify_one();
        }

        template<typename F, typename... A, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>
        [[nodiscard]] std::future<R> submit(F &&task, A &&...args) {
            auto task_promise = Create<std::promise<R>>();
            auto future = task_promise->get_future();
            push_task(
                    [task_promise](auto &&taskInner, auto &&...argss) {
                        try {
                            if constexpr (std::is_void_v<R>) {
                                std::invoke(std::forward<decltype(taskInner)>(taskInner),
                                            std::forward<decltype(argss)>(argss)...);
                                task_promise->set_value();
                            } else {
                                task_promise->set_value(std::invoke(std::forward<decltype(taskInner)>(taskInner),
                                                                    std::forward<decltype(argss)>(argss)...));
                            }
                        } catch (...) {
                            try {
                                task_promise->set_exception(std::current_exception());
                            } catch (...) {
                            }
                        }
                        Destroy(task_promise);
                    },
                    std::forward<F>(task), std::forward<A>(args)...);
            return future;
        }

        size_t get_thread_count() {
            return threads.size();
        }

        void reset(concurrency_t thread_count_ = 0) {
            bool was_paused = paused.exchange(true);
            wait_for_tasks();
            destroy_threads();
            auto thread_count = determine_thread_count(thread_count_);
            threads.resize(thread_count);
            paused.exchange(was_paused);
            create_threads();
        }


        static size_t getCurrentThreadIndexView() { return getThreadIndex(); }

        /// @brief force reset a thread, only should be called when a deadly problem happen
        void resetThreadByIndex(size_t index) {
            static std::mutex resetThreadMtx;
            std::lock_guard lk(resetThreadMtx);
            size_t oldThCounter = threadIndexCounter;
            threadIndexCounter = index;
            LibLoader::logger.warning("resetting thread in pool at index " + std::to_string(index) + " due to a user caused error");
            new (&threads[index]) std::thread(&ThreadPool::worker, this);
            while (threadIndexCounter != index + 1) {
                std::this_thread::yield();
            }
            threadIndexCounter = oldThCounter;
        }

    private:
        void create_threads() {
            running = true;
            for (auto &th: threads) {
                th = std::thread(&ThreadPool::worker, this);
            }
        }

        void destroy_threads() {
            running = false;
            task_available_cv.notify_all();
            for (auto &thread: threads) {
                thread.join();
            }
        }
    };

    inline std::unique_ptr<ThreadPool<>> pool;
} // namespace Antares
#endif //TESTPROJECT_THREADPOOL_H
