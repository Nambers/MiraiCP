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

#include "loaderApiInternal.h"
#include "ktInterface.h"
#include "LoaderLogger.h"
#include "LoaderTaskQueue.h"
#include "PluginListManager.h"
#include <mutex>


namespace LibLoader {
    std::queue<loadertask> loader_thread_task_queue; // NOLINT(cert-err58-cpp)
    std::recursive_mutex task_mtx;
} // namespace LibLoader


namespace LibLoader::LoaderApi {
    using MiraiCP::MiraiCPString;

    /// interfaces for plugins
    MiraiCPString showAllPluginId() {
        return nlohmann::json(PluginListManager::getAllPluginId()).dump();
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

    MiraiCPString pluginOperation(const MiraiCPString &s) {
        return {LoaderAPIs::oper(s.copyToCharPtr())};
    }

    void loggerInterface(const MiraiCPString &content, const MiraiCPString &name, long long id, int level) {
        logger.call_logger(content, name, id, level);
    }
} // namespace LibLoader::LoaderApi
