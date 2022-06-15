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
#include "JNIEnvManager.h"
#include "LoaderTaskQueue.h"
#include "PluginListImplements.h"
#include "PluginListManager.h"
#include "ThreadController.h"


namespace LibLoader::LoaderApi {
    extern std::queue<loadertask> loader_thread_task_queue;
    extern std::recursive_mutex task_mtx;
} // namespace LibLoader::LoaderApi
namespace LibLoader {
    volatile bool LoaderMain::loader_exit = false;
    void LoaderMain::loaderMain() {
        PluginListManager::enableAll();

        while (!is_loader_exited()) mainloop();

        shutdownLoader();
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
        loadNewPluginByPath(path, activateNow);
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

    ////////////////////////////////////

    void LoaderMain::mainloop() {
        if (LoaderApi::loader_thread_task_queue.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(70));
        else {
            loadertask task;
            {
                std::lock_guard lk(LoaderApi::task_mtx);
                task = std::move(LoaderApi::loader_thread_task_queue.front());
                LoaderApi::loader_thread_task_queue.pop();
            }

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
                default:
                    throw std::exception();
            }
        }
    }

    void LoaderMain::shutdownLoader() {
        loader_disableAllPlugins();
    }
} // namespace LibLoader
