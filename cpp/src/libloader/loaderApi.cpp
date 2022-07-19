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

#include "loaderApi.h"
#include "JNIEnvs.h"
#include "LoaderLogger.h"
#include "LoaderTaskQueue.h"
#include "PluginListManager.h"
#include "loaderTools.h"
#include <mutex>


namespace LibLoader {
    std::queue<loadertask> loader_thread_task_queue; // NOLINT(cert-err58-cpp)
    std::recursive_mutex task_mtx;
} // namespace LibLoader


namespace LibLoader::LoaderApi {
    /// interfaces for plugins
    std::vector<const char *> showAllPluginId() {
        std::vector<const char *> re;
        for (auto a: PluginListManager::getAllPluginId()) {
            re.emplace_back(a.c_str());
        }
        return re;
    }

    void enablePluginById(const char *id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::ADD_THREAD, std::string(id)));
    }

    void disablePluginById(const char *id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::END_THREAD, std::string(id)));
    }

    void enableAllPlugins() {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::ENABLE_ALL, ""));
    }

    void disableAllPlugins() {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::DISABLE_ALL, ""));
    }

    void loadNewPlugin(const char *path, bool activateNow) {
        std::lock_guard lk(task_mtx);
        if (activateNow)
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::LOAD_NEW_ACTIVATENOW, std::string(path)));
        else
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::LOAD_NEW_DONTACTIVATE, std::string(path)));
    }

    void unloadPluginById(const char *id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::UNLOAD, std::string(id)));
    }

    void reloadPluginById(const char *id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::RELOAD, std::string(id)));
    }

    const char *pluginOperation(const char *s) {
        auto env = JNIEnvManager::getEnv();
        auto tmp = jstring2str((jstring) env->CallStaticObjectMethod(JNIEnvs::Class_cpplib,
                                                                     JNIEnvs::koper,
                                                                     str2jstring(s)));
        std::unique_ptr<char[]> re(new char[tmp.length() + 1]);
        strcpy(re.get(), tmp.c_str());
        return re.get();
    }

    void loggerInterface(const char *content, const char *name, long long id, int level) {
        logger.call_logger(content, name, id, level);
    }
} // namespace LibLoader::LoaderApi
