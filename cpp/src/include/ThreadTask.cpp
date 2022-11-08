//
// Created by antares on 11/8/22.
//

#include "ThreadTask.h"
#include "Logger.h"
#include "loaderApiInternal.h"
#include <atomic>
#include <deque>
#include <shared_mutex>


namespace MiraiCP::ThreadTask {
    // todo(Antares): add verbose log
    using functionType = std::function<void()>;
    using functionPointerType = std::shared_ptr<functionType>;
    using storerType = std::unordered_map<size_t, functionPointerType>;
    using storerIteratorType = storerType::iterator;

    size_t getAutoIncrId() {
        static std::atomic<size_t> id = 0;
        return id++;
    }

    inline auto &get_function_storer() {
        static storerType function_storer;
        return function_storer;
    }

    inline auto &get_mutex() {
        static std::shared_mutex mtx;
        return mtx;
    }

    void push_task(size_t id, functionPointerType func) {
        {
            std::unique_lock lk(get_mutex());
            get_function_storer().insert({id, std::move(func)});
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


} // namespace MiraiCP::ThreadTask
