// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
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

#include "loaderMain.h"
#include "LoaderExceptions.h"
#include "LoaderLogger.h"
#include "LoaderTaskQueue.h"
#include "MessageManager.h"
#include "MessageProcessor.h"
#include "Plugin.h"
#include "PluginListManager.h"
#include "Scheduler.h"
#include "ThreadIdentify.h"
#include "ThreadPool.h"
#include "redirectCout.h"
#include <chrono>


namespace LibLoader {
    LoaderMain &LoaderMain::get() {
        static LoaderMain loaderMain;
        return loaderMain;
    }

    std::mutex &loaderCVLock() {
        static std::mutex m;
        return m;
    }

    std::condition_variable &loaderWakeCV() {
        static std::condition_variable cv;
        return cv;
    }

    /// LoaderMain实现开始

    void LoaderMain::loaderMain() {
        ThreadIdentify::IAmLoaderThread();
        logger.info("libLoader thread start");

        Antares::pool = std::make_unique<Antares::ThreadPool<>>();

        PluginListManager::enableAll();

        MessageManager::get().init(&MessageProcessor::get());

        while (!is_loader_exited()) mainloop();

        shutdownLoader();

        Antares::pool.reset();
    }

    ////////////////////////////////////
    /// 这里是loader线程调用的实际操作pluginlist的接口
    /// 每次操作都必须保证原子性。如果有多个函数调用，需要getLock()并自行创建lock_guard对象，
    /// 见getLock()的注释

    void loader_enableAllPlugins() {
        PluginListManager::enableAll();
    }

    void loader_disableAllPlugins() {
        PluginListManager::disableAll();
    }

    void loader_loadNewPlugin(const std::string &path, bool activateNow) {
        PluginListManager::loadNewPluginByPath(path, activateNow);
    }

    void loader_unloadPluginById(const std::string &id) {
        PluginListManager::unloadById(id);
    }

    void loader_enablePluginById(const std::string &id) {
        PluginListManager::enableById(id);
    }

    void loader_disablePluginById(const std::string &id) {
        PluginListManager::disableById(id);
    }

    void loader_reloadPluginById(const std::string &id) {
        PluginListManager::reloadById(id);
    }

    void loader_pluginEndByException(const std::string &id) {
        PluginListManager::unloadWhenException(id);
    }

    void loader_resetThreadByIndex(const std::string &index) {
        Antares::pool->resetThreadByIndex(std::stoull(index));
    }

    ////////////////////////////////////


    void LoaderMain::tick() const noexcept {
        Scheduler::popSchedule();

        MessageManager::get().tick();

        process_task_queue();
    }

    void LoaderMain::mainloop() const noexcept {
        constexpr auto tick_time = std::chrono::milliseconds(33);
        using Clock = std::chrono::steady_clock;
        auto loop_start = Clock::now();

        tick();

        auto loop_end = Clock::now();
        auto loop_duration = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start);
        if (loop_duration >= tick_time) {
            logger.warning("libLoader线程运行时间过长：" + std::to_string(loop_duration.count()) + "ms");
        } else {
            std::unique_lock cvuniquelock(loaderCVLock());
            auto diff_time = tick_time - loop_duration;
            loaderWakeCV().wait_for(cvuniquelock, diff_time,
                                    [this]() { return is_loader_exited() || Scheduler::timeup() || !loader_thread_task_queue.empty(); });
            cvuniquelock.unlock();
        }
    }

    void LoaderMain::shutdownLoader() {
        loader_disableAllPlugins();
        MiraiCP::Redirector::reset();
        // 为了删除 Win 下复制的缓存
        PluginListManager::unloadAll();
    }

    void LoaderMain::process_task_queue() const {
        if (!loader_thread_task_queue.empty()) {
            loadertask task;
            {
                std::lock_guard lk(task_mtx);
                std::swap(task, loader_thread_task_queue.front());
                loader_thread_task_queue.pop();
            }
            try {
                switch (task.first) {
                    case LOADER_TASKS::ADD_THREAD:
                        loader_enablePluginById(task.second);
                        break;
                    case LOADER_TASKS::END_THREAD:
                        loader_disablePluginById(task.second);
                        break;
                    case LOADER_TASKS::ENABLE_ALL:
                        loader_enableAllPlugins();
                        break;
                    case LOADER_TASKS::DISABLE_ALL:
                        loader_disableAllPlugins();
                        break;
                    case LOADER_TASKS::LOAD_NEW_ACTIVATENOW:
                        loader_loadNewPlugin(task.second, true);
                        break;
                    case LOADER_TASKS::LOAD_NEW_DONTACTIVATE:
                        loader_loadNewPlugin(task.second, false);
                        break;
                    case LOADER_TASKS::UNLOAD:
                        loader_unloadPluginById(task.second);
                        break;
                    case LOADER_TASKS::RELOAD:
                        loader_reloadPluginById(task.second);
                        break;
                    case LOADER_TASKS::EXCEPTION_PLUGINEND:
                        loader_pluginEndByException(task.second);
                        break;
                    case LOADER_TASKS::RESET_THREAD:
                        loader_resetThreadByIndex(task.second);
                        break;
                    default:
                        throw std::exception();
                }
            } catch (LoaderBaseException &e) {
                e.raise();
            } catch (std::exception &e) {
                logger.error("libLoader线程遇到无法预料的错误：" + std::string(e.what()) + "，程序将继续执行，如果发生意料之外的状况，请提交issue");
            }
        }
    }
} // namespace LibLoader
