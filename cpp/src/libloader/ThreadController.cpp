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

#include "ThreadController.h"
#include "JNIEnvManager.h"
#include "LoaderLogger.h"
#include "LoaderTaskQueue.h"
#include "PluginListManager.h"
#include "commonTools.h"


namespace LibLoader {
    ThreadController ThreadController::_threadController;

    void sendPluginException(std::string plugin_id) {
        std::lock_guard lk(task_mtx);
        loader_thread_task_queue.emplace(loadertask(LOADER_TASKS::EXCEPTION_PLUGINEND, std::move(plugin_id)));
    }

    // threadWorker
    void ThreadController::threadWorker::give_job(void_callable newjob) {
        std::lock_guard lk(worker_mtx);
        job_queue.emplace(std::move(newjob));
    }

    // 被调用后必须立刻结束run()
    void ThreadController::threadWorker::exception_call_this_thread_end() {
        logger.error("Error: plugin " + pluginid + " crashed (caused by throwing an unknown exception)");

        sendPluginException(std::move(pluginid));
    }

    void ThreadController::threadWorker::run() {
        // clean up at function end
        // try to detach this thread from JVM
        MIRAICP_DEFER(JNIEnvManager::detach(););

        // set thread name so the debugger can see the plugin id from binding thread
        pthread_setname_np(pthread_self(), pluginid.c_str());

        while (!exit) {
            if (job_queue.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(10));
            else {
                void_callable nowjob;
                {
                    std::lock_guard lk(worker_mtx);
                    nowjob = std::move(job_queue.front());
                    job_queue.pop();
                }
                if (!_do_job(nowjob)) {
                    exception_call_this_thread_end();
                    return;
                }
            }
        }

        std::lock_guard lk(worker_mtx);
        while (!job_queue.empty()) {
            auto job = std::move(job_queue.front());
            job_queue.pop();
            if (!_do_job(job)) {
                exception_call_this_thread_end();
                return;
            }
        }
    }

    // ThreadController methods
    // ThreadController helper functions, with no mutex lock

    // safely shutdown one thread using std::thread::join()
    void ThreadController::joinAndShutdownThread(workerThread &worker) {
        worker.first->end();
        worker.second->join();
    }

    // detach thread even if there is a job working. Better call at program exiting
    void ThreadController::detachThread(workerThread &worker) {
        worker.first->end();
        worker.second->detach();
    }

    // data maintain methods, use mutex

    // add one worker thread
    void ThreadController::addThread(const std::string &name, void_callable func) {
        std::lock_guard lk(_mtx);
        auto it = thread_memory.find(name);
        if (it != thread_memory.end()) {
            logger.error("Plugin " + name + " thread already activated!");
            return;
        }
        auto &worker = thread_memory[name];
        worker.first.reset(new threadWorker(name));
        worker.first->give_job(std::move(func));
        auto worker_ptr = worker.first.get();
        worker.second = std::make_shared<std::thread>([=]() { worker_ptr->run(); });
        thread_id_indexes[worker.second->get_id()] = name;
    }

    // safely shutdown
    void ThreadController::callThreadEnd(const std::string &name, void_callable func) {
        std::lock_guard lk(_mtx);
        auto it = thread_memory.find(name);
        if (it == thread_memory.end()) {
            logger.error("Plugin " + name + " thread not found!");
            return;
        }

        auto &worker = it->second;
        auto threadid = worker.second->get_id();

        worker.first->give_job(std::move(func));
        // shut down the thread
        joinAndShutdownThread(it->second);
        // thread is dead, safely remove it
        thread_memory.erase(it);
        thread_id_indexes.erase(threadid);
    }

    // only call this if unexpectedly shutdown happens
    void ThreadController::end_all_thread() {
        std::lock_guard lk(_mtx);
        for (auto &&[k, v]: thread_memory) {
            detachThread(v);
        }
        thread_memory.clear();
        thread_id_indexes.clear();
    }

    void ThreadController::submitJob(const std::string &name, void_callable func) {
        std::lock_guard lk(_mtx);
        auto it = thread_memory.find(name);
        if (it == thread_memory.end()) {
            logger.error("Submit job: plugin " + name + " thread not found!");
            return;
        }
        it->second.first->give_job(std::move(func));
    }

    void ThreadController::endThread(const std::string &name) {
        std::lock_guard lk(_mtx);
        auto it = thread_memory.find(name);
        if (it == thread_memory.end()) {
            logger.error("End thread: plugin " + name + " thread not found!");
            return;
        }
        auto threadid = it->second.second->get_id();
        // detach the thread
        detachThread(it->second);
        // thread is dead, safely remove it
        thread_memory.erase(it);
        thread_id_indexes.erase(threadid);
    }

    std::string ThreadController::getPluginIdFromThreadId(std::thread::id id) {
        const auto &threadidQuery = getController().thread_id_indexes;
        auto it = threadidQuery.find(id);
        if (it == threadidQuery.end()) {
            return "";
        }
        return it->second;
    }
} // namespace LibLoader
