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

#include "MiraiCPMacros.h"
// -----------------------
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "LoaderMacro.h"
#include "LoaderTaskQueue.h"
#include "PluginListManager.h"
#include "Scheduler.h"
#include "ThreadPool.h"
#include "loaderApiInternal.h"
#include "loaderTools.h"
#include <mutex>


namespace LibLoader {
    std::queue<loadertask> loader_thread_task_queue; // NOLINT(cert-err58-cpp)
    std::recursive_mutex task_mtx;
    std::mutex &loaderCVLock();
    std::condition_variable &loaderWakeCV();
} // namespace LibLoader

#ifdef LOADER_NATIVE
namespace LoaderAPIs {
    using OperFunc = char **(*) (const char *);
    extern OperFunc oper;
}; // namespace LoaderAPIs

inline MiraiCP::MiraiCPString *retrieveString(char **inPtr) {
    return reinterpret_cast<MiraiCP::MiraiCPString *>(inPtr);
}
#endif

namespace LibLoader::LoaderApi {
    using MiraiCP::MiraiCPString;
    /// interfaces for plugins

    MiraiCPString pluginOperation(MiraiCPStringview s) {
#ifdef LOADER_NATIVE
        auto nCharPtr = s.c_str();
        auto str = retrieveString(LoaderAPIs::oper(nCharPtr));
        MIRAICP_DEFER(delete str;);
        return std::move(*str);
#else
        auto env = JNIEnvManager::getEnv();
        auto tmp = jstring2str((jstring) env->CallStaticObjectMethod(JNIEnvs::Class_cpplib,
                                                                     JNIEnvs::koper,
                                                                     str2jstring(s.c_str())));
        return {tmp};
#endif
    }

    void loggerInterface(const MiraiCPString &content, const MiraiCPString &name, long long id, int level) {
        logger.call_logger(content, name, id, level);
    }

    MiraiCPString showAllPluginId() {
        return nlohmann::json(PluginListManager::getAllPluginId()).dump();
    }

    void pushTask(task_func func) {
        Antares::pool->push_task(func);
    }

    void pushTaskWithId(task_func_with_id func, size_t id) {
        Antares::pool->push_task(func, id);
    }

    void timer(const MiraiCPString &id, const MiraiCPString &content, size_t time) {
        Scheduler::timer(id, content, time);
    }

    void enablePluginById(const MiraiCPString &id) {
        {
            std::lock_guard lk(task_mtx);
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::ADD_THREAD, id));
        }
        {
            loaderWakeCV().notify_one();
        }
    }

    void disablePluginById(const MiraiCPString &id) {
        {
            std::lock_guard lk(task_mtx);
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::END_THREAD, id));
        }
        {
            loaderWakeCV().notify_one();
        }
    }

    void enableAllPlugins() {
        {
            std::lock_guard lk(task_mtx);
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::ENABLE_ALL, ""));
        }
        {
            loaderWakeCV().notify_one();
        }
    }

    void disableAllPlugins() {
        {
            std::lock_guard lk(task_mtx);
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::DISABLE_ALL, ""));
        }
        {
            loaderWakeCV().notify_one();
        }
    }

    void loadNewPlugin(const MiraiCPString &path, bool activateNow) {
        {
            std::lock_guard lk(task_mtx);
            if (activateNow)
                loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::LOAD_NEW_ACTIVATENOW, path));
            else
                loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::LOAD_NEW_DONTACTIVATE, path));
        }
        {
            loaderWakeCV().notify_one();
        }
    }

    void unloadPluginById(const MiraiCPString &id) {
        {
            std::lock_guard lk(task_mtx);
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::UNLOAD, id));
        }
        {
            loaderWakeCV().notify_one();
        }
    }

    void reloadPluginById(const MiraiCPString &id) {
        {
            std::lock_guard lk(task_mtx);
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::RELOAD, id));
        }
        {
            loaderWakeCV().notify_one();
        }
    }
} // namespace LibLoader::LoaderApi
