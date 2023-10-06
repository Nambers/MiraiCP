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
#include "PluginManager.h"
#include "Scheduler.h"
#include "ThreadIdentify.h"
#include "ThreadPool.h"
#include "loaderTools.h"
#include "redirectCout.h"
#include <chrono>
#include <nlohmann/json.hpp>
#include <utility>


extern "C" {
void plugin_raw_API_try_wake_loader() {
    LibLoader::loaderWakeCV().notify_one();
}
}

constexpr size_t US_IN_SEC = 1000000;


namespace LibLoader {
    class InvalidConfException : public LoaderExceptionCRTP<InvalidConfException> {
    public:
        InvalidConfException(const string &conf_name, string _filename, int _lineNum) : LoaderExceptionCRTP("无效的配置" + conf_name, std::move(_filename), _lineNum) {}

        static string exceptionType() { return "InvalidConfException"; }
    };
    LoaderMain &LoaderMain::get() {
        static LoaderMain loaderMain;
        return loaderMain;
    }

    std::condition_variable &loaderWakeCV() {
        return LoaderMain::get().get_cv();
    }

    /// LoaderMain实现开始

    void LoaderMain::loaderMain() {
        ThreadIdentify::IAmLoaderThread();
        logger.info("libLoader thread start");
        readConfig();
        if (!Antares::pool) Antares::pool = std::make_unique<Antares::ThreadPool<>>();


        PluginManager::Get().enableAll();

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
        PluginManager::Get().enableAll();
    }

    void loader_disableAllPlugins() {
        PluginManager::Get().disableAll();
    }

    void loader_loadNewPlugin(const std::string &path, bool activateNow) {
        PluginManager::Get().loadNewPluginByPath(path, activateNow);
    }

    void loader_unloadPluginById(const std::string &id) {
        PluginManager::Get().unloadById(id);
    }

    void loader_enablePluginById(const std::string &id) {
        PluginManager::Get().enableById(id);
    }

    void loader_disablePluginById(const std::string &id) {
        PluginManager::Get().disableById(id);
    }

    void loader_reloadPluginById(const std::string &id) {
        PluginManager::Get().reloadById(id);
    }

    void loader_pluginEndByException(const std::string &id) {
        PluginManager::Get().unloadWhenException(id);
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
        auto tick_time = std::chrono::microseconds(US_IN_SEC / tickRate);
        using Clock = std::chrono::steady_clock;
        auto loop_start = Clock::now();

        tick();

        auto loop_end = Clock::now();
        auto loop_duration = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start);
        if (loop_duration >= tick_time) {
            logger.warning("libLoader线程运行时间过长：" + std::to_string(loop_duration.count()) + "ms");
        } else {
            std::unique_lock cvUniqueLock(loaderCVLock);
            auto diff_time = tick_time - loop_duration;
            loaderWakeCV().wait_for(cvUniqueLock, diff_time,
                                    [this]() { return is_loader_exited() || Scheduler::timeup() || !loader_thread_task_queue.empty(); });
            cvUniqueLock.unlock();
        }
    }

    void LoaderMain::shutdownLoader() { // NOLINT(*-convert-member-functions-to-static)
        loader_disableAllPlugins();
        MiraiCP::Redirector::reset();
        // 为了删除 Win 下复制的缓存
        PluginManager::Get().unloadAll();
    }

    void LoaderMain::process_task_queue() const { // NOLINT(*-convert-member-functions-to-static)
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

    void LoaderMain::readConfig() {
        nlohmann::json j = readJsonFile(config_path);
        if (j.contains("loaderConfigs")) {
            auto &loader_conf = j["loaderConfigs"];
            if (loader_conf.contains("threadPoolSize")) {
                size_t thread_pool_size = loader_conf["threadPoolSize"];
                Antares::pool = std::make_unique<Antares::ThreadPool<>>(thread_pool_size);
            }
            if (loader_conf.contains("tickRate")) {
                size_t _tickRate = loader_conf["tickRate"];
                if (_tickRate > US_IN_SEC) {
                    throw InvalidConfException("tickRate", MIRAICP_EXCEPTION_WHERE);
                }
                if (_tickRate == 0) {
                    tickRate = 100;
                } else {
                    tickRate = _tickRate;
                }
            }
        }
    }
} // namespace LibLoader
