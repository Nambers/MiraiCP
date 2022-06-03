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

#ifndef MIRAICP_PRO_THREADCONTROLLER_H
#define MIRAICP_PRO_THREADCONTROLLER_H


#include "commonTypes.h"
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


// multi thread controller
namespace LibLoader {
    typedef std::function<void()> void_callable;

    class ThreadController {
        class threadWorker {
            std::queue<void_callable> job_queue;
            std::recursive_mutex worker_mtx;
            const volatile bool exit = false;

        public:
            threadWorker() = default;
            // DO NOT copy (to avoid wild pointers)!
            threadWorker(threadWorker &&) = delete;
            threadWorker(const threadWorker &) = delete;

        public:
            /// only for main thread
            void give_job(void_callable newjob);

            /// only for worker thread
            void run();

            /// for main thread
            void end() { const_cast<bool &>(exit) = true; }
        };

        // the address of "threadWorker" should not be moved,
        // use smart pointers to assure allocated on heap;
        // also, to init "std::thread" lazily, use smart pointers
        typedef std::pair<std::shared_ptr<threadWorker>, std::shared_ptr<std::thread>> workerThread;


    private:
        std::unordered_map<std::string, workerThread> thread_memory;
        std::recursive_mutex _mtx;

    private:
        static ThreadController _threadController;

    private:
        ThreadController() = default;
        ~ThreadController() {
            // have to shut down immediately
            end_all_thread();
        }

    public:
        // no copy; global unique
        ThreadController(ThreadController &&) = delete;
        ThreadController(const ThreadController &) = delete;

    private:
        // only call this if program exits
        void end_all_thread();

    public:
        void addThread(const std::string &name, void_callable func);

        void callThreadEnd(const std::string &name, void_callable func);

        void submitJob(const std::string &name, void_callable func);

    private:
        static void shutdownThread(workerThread &worker);

        static void detachThread(workerThread &worker);

    public:
        static ThreadController &getController() {
            return _threadController;
        }
    };
} // namespace LibLoader

#endif //MIRAICP_PRO_THREADCONTROLLER_H
