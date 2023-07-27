// Copyright (c) 2023. Eritque arcus and contributors.
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

//
// Created by antares on 11/8/22.
//

#include "ThreadTask.h"
#include "Logger.h"
#include "loaderApiInternal.h"
#include <atomic>
#include <shared_mutex>
#include <unordered_map>


namespace MiraiCP::ThreadTask::internal {
    // todo(Antares): add verbose log
    using functionType = std::function<void()>;
    using functionPointerType = std::shared_ptr<functionType>;
    using storerType = std::unordered_map<size_t, functionPointerType>;
    using storerIteratorType = storerType::iterator;

    inline auto &get_function_storer() {
        static storerType function_storer;
        return function_storer;
    }

    inline auto &get_mutex() {
        static std::shared_mutex mtx;
        return mtx;
    }

    size_t get_auto_incr_id() {
        static std::atomic<size_t> id = 0;
        return id++;
    }

    void push_task(size_t id, functionPointerType func) {
        bool insert;
        {
            std::unique_lock lk(get_mutex());
            insert = get_function_storer().insert({id, std::move(func)}).second;
        }

        if (!insert) {
            Logger::logger.error("Fatal: task id " + std::to_string(id) + " is already taken by another task!");
            return;
        }

        LibLoader::LoaderApi::pushTaskWithId(task_interface, id);
    }

    void remove_task(size_t id) {
        size_t erased;
        {
            std::unique_lock lk(get_mutex());
            erased = get_function_storer().erase(id);
        }
        if (0 == erased) {
            Logger::logger.error("Invalid task id " + std::to_string(id) + " is requested to erase");
        }
    }

    /// @note dev: we can't send a std::function object to libLoader,
    ///  so one appropriate way is that store the function in a data structure and call it in a wrapper function.
    void task_interface(size_t id) {
        static_assert(std::is_same_v<LibLoader::LoaderApi::task_func_with_id, decltype(&task_interface)>);
        storerIteratorType iter;
        functionPointerType fPtr;
        {
            std::shared_lock lck(get_mutex());
            iter = get_function_storer().find(id);
            if (iter == get_function_storer().end()) {
                Logger::logger.error("Invalid task id " + std::to_string(id) + " is requested to run");
                return;
            }
            fPtr = iter->second;
        }
        // running task!
        (*fPtr)();
    }

    void raw_push_task(void (*func)()) {
        LibLoader::LoaderApi::pushTask(func);
    }

    void logerror(const std::string &content) {
        Logger::logger.error(content);
    }
} // namespace MiraiCP::ThreadTask::internal
