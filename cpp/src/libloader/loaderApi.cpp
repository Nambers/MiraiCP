//
// Created by antares on 6/3/22.
//


#include "loaderApi.h"
#include "JNIEnvManager.h"
#include "LoaderTaskQueue.h"
#include "PluginList.h"
#include <mutex>
#include <queue>


namespace LibLoader {
    std::queue<loadertask> loader_thread_task_queue;
    std::recursive_mutex task_mtx;


    /// interfaces for plugins
    std::vector<std::string> _showAllPluginName() {
        return getAllPluginName();
    }

    void _enablePluginByName(const std::string &name) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::ADD_THREAD, name));
    }

    void _disablePluginByName(const std::string &name) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::END_THREAD, name));
    }

    void _enableAllPlugins() {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::ENABLE_ALL, ""));
    }

    void _disableAllPlugins() {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::DISABLE_ALL, ""));
    }

    void _loadNewPlugin(const std::string &path, bool activateNow) {
        std::lock_guard lk(task_mtx);
        if (activateNow)
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::LOAD_NEW_ACTIVATENOW, path));
        else
            loader_thread_task_queue.push(std::make_pair(LOADER_TASKS::LOAD_NEW_DONTACTIVATE, path));
    }

    JNIEnv *_getEnv() {
        return JNIEnvManager::getEnv();
    }
} // namespace LibLoader