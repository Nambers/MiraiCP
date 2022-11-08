//
// Created by antares on 11/8/22.
//

#ifndef MIRAICP_PRO_THREADTASK_H
#define MIRAICP_PRO_THREADTASK_H

#include "Logger.h"
#include "loaderApiInternal.h"
#include <future>

namespace MiraiCP::ThreadTask {
    namespace internal {
        void task_interface(size_t id);

        size_t get_auto_incr_id();

        void remove_task(size_t id);

        void push_task(size_t id, std::shared_ptr<std::function<void()>> func);
    } // namespace internal

    /// @brief 提交一个任务到线程池，参数与构造 std::thread 以及 MiraiCPNewThread 用的参数相同。
    ///  任务在线程池可用时会被立刻执行
    /// @return 对应任务返回类型的 std::future 对象
    /// @see addTask, MiraiCPNewThread
    template<typename F, typename... Args, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    std::future<R> promiseTask(F &&func, Args &&...args) {
        auto promise = std::make_shared<std::promise<R>>();
        auto future = promise->get_future();
        std::function<R()> taskfunction = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
        size_t function_id = internal::get_auto_incr_id();
        auto to_push_func = std::make_shared<std::function<void()>>(
                [promise = std::move(promise), function_id, taskfunction = std::move(taskfunction)]() {
                    try {
                        if constexpr (std::is_void_v<R>) {
                            std::invoke(taskfunction);
                            promise->set_value();
                        } else {
                            promise->set_value(std::invoke(taskfunction));
                        }
                    } catch (...) {
                        try {
                            promise->set_exception(std::current_exception());
                            Logger::logger.error("Failed to run task: " + std::to_string(function_id));
                        } catch (...) {
                        }
                    }
                    try {
                        internal::remove_task(function_id);
                    } catch (...) {
                    }
                });
        internal::push_task(function_id, std::move(to_push_func));
        return future;
    }

    /// @brief 提交一个任务到线程池，参数与构造 std::thread 以及 MiraiCPNewThread 用的参数相同。
    ///  任务在线程池可用时会被立刻执行。与 promiseTask 不同，不会给出任务返回值
    /// @see promiseTask, MiraiCPNewThread
    template<typename F, typename... Args, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    void addTask(F &&func, Args &&...args) {
        if constexpr (std::is_same_v<std::decay_t<F>, void (*)()>) {
            LibLoader::LoaderApi::pushTask(func);
        } else {
            auto function_id = internal::get_auto_incr_id();
            std::function<void()> taskfunction = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
            auto to_push_func = std::make_shared<std::function<void()>>(
                    [function_id, taskfunction = std::move(taskfunction)]() {
                        try {
                            std::invoke(taskfunction);
                        } catch (...) {
                            try {
                                Logger::logger.error("Failed to run task: " + std::to_string(function_id));
                            } catch (...) {
                            }
                        }
                        try {
                            internal::remove_task(function_id);
                        } catch (...) {
                        }
                    });
            internal::push_task(function_id, std::move(to_push_func));
        }
    }
}; // namespace MiraiCP::ThreadTask


#endif //MIRAICP_PRO_THREADTASK_H
