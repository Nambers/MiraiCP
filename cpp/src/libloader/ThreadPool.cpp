//
// Created by antares on 3/13/23.
//

#include "ThreadPool.h"
#include "ThreadIdentify.h"

namespace Antares {
    constexpr auto order_relaxed = std::memory_order_relaxed;

    void ThreadPoolBase::worker() {
        ThreadIdentify::IAmPoolThread();
        setThreadIndex();
        std::unique_lock<std::mutex> tasks_lock(tasks_mtx);
        while (running.load(order_relaxed)) {
            std::function<void()> task;
            while (tasks.empty() && running.load(order_relaxed)) {
                task_available_cv.wait_for(tasks_lock, std::chrono::milliseconds(100));
            }

            while (!is_paused() && !tasks.empty()) {
                task = std::move(tasks.front());
                tasks.pop();
                tasks_lock.unlock();
                task();
                tasks_total.fetch_sub(1, std::memory_order_release);
                tasks_lock.lock();
                if (waiting.load(order_relaxed))
                    task_done_cv.notify_one();
            }
        }
    }

    ThreadPoolBase::ThreadPoolBase() = default;

    concurrency_t ThreadPoolBase::determine_thread_count(concurrency_t thread_count_) {
        if (thread_count_ > 0)
            return thread_count_;
        else {
            if (std::thread::hardware_concurrency() > 0)
                return std::thread::hardware_concurrency();
            else
                return 1;
        }
    }

    void ThreadPoolBase::wait_for_tasks() {
        std::unique_lock<std::mutex> tasks_lock(tasks_mtx);
        waiting = true;
        while ((get_tasks_total() != (is_paused() ? tasks.size() : 0))) {
            // if you require a faster way to know the tasks are over, you should implement it in your tasks,
            // instead of calling this stupid function to help you.
            task_done_cv.wait_for(tasks_lock, std::chrono::milliseconds(10));
        }
        waiting = false;
    }

    void ThreadPoolBase::pause() {
        paused = true;
    }

    size_t ThreadPoolBase::get_tasks_total() const {
        return tasks_total.load(order_relaxed);
    }

    bool ThreadPoolBase::is_paused() const {
        return paused.load(order_relaxed);
    }

    void ThreadPoolBase::unpause() {
        paused = false;
    }

    size_t &ThreadPoolBase::getThreadIndex() {
        // for windows compatibility, use parentheses to avoid calling the macro "max"
        static thread_local size_t index = (std::numeric_limits<size_t>::max)();
        return index;
    }

    void ThreadPoolBase::setThreadIndex() {
        getThreadIndex() = threadIndexCounter++;
    }
} // namespace Antares
