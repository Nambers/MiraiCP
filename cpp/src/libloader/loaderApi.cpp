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

#include "BS_thread_pool.hpp"
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "LoaderMacro.h"
#include "LoaderTaskQueue.h"
#include "PluginListManager.h"
#include "loaderApiInternal.h"
#include "loaderTools.h"
#include <mutex>

namespace LibLoader {
    std::queue<loadertask> loader_thread_task_queue; // NOLINT(cert-err58-cpp)
    std::recursive_mutex task_mtx;
} // namespace LibLoader

#ifdef LOADER_NATIVE
namespace LoaderAPIs {
    using OperFunc = const char *(*) (const char *);
    extern OperFunc oper;
}; // namespace LoaderAPIs
#endif

namespace LibLoader::LoaderApi {
    using MiraiCP::MiraiCPString;
    /// interfaces for plugins

    MiraiCPString pluginOperation(const MiraiCPString &s) {
#ifdef LOADER_NATIVE
        return {LoaderAPIs::oper(s.copyToCharPtr())}; // todo(Antares): 检查内存是否回收
#else
        auto env = JNIEnvManager::getEnv();
        auto tmp = jstring2str((jstring) env->CallStaticObjectMethod(JNIEnvs::Class_cpplib,
                                                                     JNIEnvs::koper,
                                                                     str2jstring(s.copyToCharPtr())));
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
        BS::pool->push_task(func);
    }

    void pushTaskWithId(task_func_with_id func, size_t id) {
        BS::pool->push_task(func, id);
    }

    void enablePluginById(const MiraiCPString &id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::ADD_THREAD, id));
    }

    void disablePluginById(const MiraiCPString &id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::END_THREAD, id));
    }

    void enableAllPlugins() {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::ENABLE_ALL, ""));
    }

    void disableAllPlugins() {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::DISABLE_ALL, ""));
    }

    void loadNewPlugin(const MiraiCPString &path, bool activateNow) {
        std::lock_guard lk(task_mtx);
        if (activateNow)
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::LOAD_NEW_ACTIVATENOW, path));
        else
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::LOAD_NEW_DONTACTIVATE, path));
    }

    void unloadPluginById(const MiraiCPString &id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::UNLOAD, id));
    }

    void reloadPluginById(const MiraiCPString &id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::RELOAD, id));
    }
} // namespace LibLoader::LoaderApi
