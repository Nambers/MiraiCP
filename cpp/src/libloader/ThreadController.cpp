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
#include "JNIEnvs.h"
#include "LoaderLogger.h"


namespace LibLoader {
    ThreadController ThreadController::_threadController;
    // threadWorker
    void ThreadController::threadWorker::give_job(void_callable newjob) {
        std::lock_guard lk(worker_mtx);
        job_queue.emplace(std::move(newjob));
    }

    inline void _do_job(const void_callable &job) {
        try {
            if (job) job();
        } catch (...) { // do not let any exception raise to avoid crash
        }
    }

    void ThreadController::threadWorker::run() {
        while (!exit) {
            if (job_queue.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(10));
            else {
                void_callable nowjob;
                {
                    std::lock_guard lk(worker_mtx);
                    nowjob = std::move(job_queue.front());
                    job_queue.pop();
                }
                _do_job(nowjob);
            }
        }

        while (!job_queue.empty()) {
            auto job = std::move(job_queue.front());
            job_queue.pop();
            _do_job(job);
        }

        // clean up
        // try to detach this thread from JVM
        JNIEnvManager::detach();
    }

    // ThreadController methods
    // ThreadController helper functions, with no mutex lock

    // safely shutdown one thread using std::thread::join()
    void ThreadController::shutdownThread(workerThread &worker) {
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
        worker.first.reset(new threadWorker);
        worker.first->give_job(std::move(func));
        auto worker_ptr = worker.first.get();
        worker.second = std::make_shared<std::thread>([=]() { worker_ptr->run(); });
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
        worker.first->give_job(std::move(func));
        // shut down the thread
        shutdownThread(it->second);
        // thread is dead, safely remove it
        thread_memory.erase(it);
    }

    // only call this if unexpectedly shutdown happens
    void ThreadController::end_all_thread() {
        std::lock_guard lk(_mtx);
        for (auto &&[k, v]: thread_memory) {
            detachThread(v);
        }
        thread_memory.clear();
    }

    void ThreadController::submitJob(const std::string &name, void_callable func) {
        std::lock_guard lk(_mtx);
        auto it = thread_memory.find(name);
        if (it == thread_memory.end()) {
            logger.error("Plugin " + name + " thread not found!");
            return;
        }
        it->second.first->give_job(std::move(func));
    }
} // namespace LibLoader
