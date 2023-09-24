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
#include "PluginManager.h"
#include "Scheduler.h"
#include "ThreadPool.h"
#include "loaderApiInternal.h"
#include "loaderTools.h"
#include <mutex>


namespace LibLoader {
    std::queue<loadertask> loader_thread_task_queue; // NOLINT(cert-err58-cpp)
    std::recursive_mutex task_mtx;
    void loader_enableAllPlugins();
    void loader_disableAllPlugins();
    void loader_loadNewPlugin(const std::string &path, bool activateNow);
    void loader_unloadPluginById(const std::string &id);
    void loader_enablePluginById(const std::string &id);
    void loader_disablePluginById(const std::string &id);
    void loader_reloadPluginById(const std::string &id);
    void loader_pluginEndByException(const std::string &id);
    void loader_resetThreadByIndex(const std::string &index);
} // namespace LibLoader

namespace LibLoader::LoaderApi {
    using MiraiCP::MiraiCPString;
    /// interfaces for plugins

    MiraiCPString pluginOperation(MiraiCPStringview s) {
        auto env = JNIEnvManager::getEnv();
        auto tmp = jstring2str((jstring) env->CallStaticObjectMethod(JNIEnvs::Class_cpplib,
                                                                     JNIEnvs::koper,
                                                                     str2jstring(s.c_str())));
        return {tmp};
    }

    void loggerInterface(const MiraiCPString &content, const MiraiCPString &name, long long id, int level) {
        logger.call_logger(content, name, id, level);
    }

    MiraiCPString showAllPluginId() {
        return nlohmann::json(PluginManager::Get().getAllPluginId()).dump();
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
        loader_enablePluginById(id);
    }

    void disablePluginById(const MiraiCPString &id) {
        loader_disablePluginById(id);
    }

    void enableAllPlugins() {
        loader_enableAllPlugins();
    }

    void disableAllPlugins() {
        loader_disableAllPlugins();
    }

    void loadNewPlugin(const MiraiCPString &path, bool activateNow) {
        loader_loadNewPlugin(path, activateNow);
    }

    void unloadPluginById(const MiraiCPString &id) {
        loader_unloadPluginById(id);
    }

    void reloadPluginById(const MiraiCPString &id) {
        loader_reloadPluginById(id);
    }
} // namespace LibLoader::LoaderApi
