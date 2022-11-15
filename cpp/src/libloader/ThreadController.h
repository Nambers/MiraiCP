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
#include <mutex>
#include <queue>
#include <thread>


// multi thread controller
namespace LibLoader {
    void sendPluginException(std::string plugin_id);

    class ThreadController {
        typedef std::function<void()> void_callable;

        // represents a thread worker
        class threadWorker;

        // the address of "threadWorker" should not be moved,
        // use smart pointers to assure allocated at the same address;
        // also, to init "std::thread" lazily, use smart pointers
        struct workerThreadPair {
            std::shared_ptr<threadWorker> worker;
            std::shared_ptr<std::thread> thread;
        };
        // typedef std::pair<std::shared_ptr<threadWorker>, std::shared_ptr<std::thread>> workerThreadPair;

    private:
        std::unordered_map<std::string, workerThreadPair> thread_memory;
        std::unordered_map<std::thread::id, std::string> thread_id_indexes;
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

        /// 该函数阻塞地结束一个线程
        void callThreadEnd(const std::string &name, void_callable func);

        void submitJob(const std::string &name, void_callable func);

        void endThread(const std::string &name);

    private:
        static void joinAndShutdownThread(workerThreadPair &worker);

        static void detachThread(workerThreadPair &worker);

    public:
        static std::string getPluginIdFromThreadId(std::thread::id id);

    public:
        static ThreadController &getController() {
            return _threadController;
        }
    };
} // namespace LibLoader

#endif //MIRAICP_PRO_THREADCONTROLLER_H
