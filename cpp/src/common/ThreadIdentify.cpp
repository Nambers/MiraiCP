//
// Created by 60168 on 2023-1-25.
//
#include "ThreadIdentify.h"
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>


#if MIRAICP_MSVC /// Windows case
#include <Windows.h>
#include <processthreadsapi.h>
#include <string>
#define SET_THREADNAME(name)                                   \
    size_t len = strlen(name);                                 \
    std::wstring wst(len + 1, '#');                            \
    size_t num_convert;                                        \
    mbstowcs_s(&num_convert, &wst[0], len + 1, name, len + 1); \
    SetThreadDescription(GetCurrentThread(), &wst[0])
#define GET_THREADNAME(buffer, buffersize)                 \
    wchar_t *wbuf;                                         \
    GetThreadDescription(GetCurrentThread(), &wbuf);       \
    size_t num_convert;                                    \
    wcstombs_s(&num_convert, buf, bufsize, wbuf, bufsize); \
    LocalFree(wbuf);

#else /// pthread case

#ifdef PTHREAD_SET_SUPPORTED
#define SET_THREADNAME(name) pthread_setname_np(pthread_self(), name)
#else
#define SET_THREADNAME(name)
#endif

#ifdef PTHREAD_GET_SUPPORTED
#define GET_THREADNAME(buffer, buffersize) pthread_getname_np(pthread_self(), buffer, buffersize);
#else
#define GET_THREADNAME(buffer, buffersize) buffer[0] = 0
#endif

#endif

namespace ThreadIdentify {
    thread_local bool isPoolThread = false;
    thread_local bool isLoaderThread = false;

    void IAmPoolThread() {
        isPoolThread = true;
        SET_THREADNAME("LoaderWorker");
    }

    bool isMePoolThread() {
        return isPoolThread;
    }

    void IAmLoaderThread() {
        isLoaderThread = true;
        SET_THREADNAME("libLoader");
    }

    bool isMeLoaderThread() {
        return isLoaderThread;
    }

    using id = std::thread::id;
    std::unordered_map<id, std::string> Identify;
    std::shared_mutex mtx;

    void setMyThreadName(const std::string &name) {
        bool success;
        {
            std::unique_lock _lk(mtx);
            success = Identify.insert({std::this_thread::get_id(), name}).second;
        }
        if (!success) {
            // todo
        }
        std::string t;
#ifdef PTHREAD_SET_SUPPORTED
        // 受限于Linux内核，只取前15个char，否则会失败
        if (name.size() > 15) {
            t.append(std::string_view(name), 15);
        } else {
            t = name;
        }
        SET_THREADNAME(t.c_str());
#else
        t = name;
        SET_THREADNAME(t.c_str());
#endif
    }

    std::string identifyMe() {
        std::shared_lock _lk(mtx);
        return Identify[std::this_thread::get_id()];
    }

    std::unordered_map<id, std::string> Working;
    std::shared_mutex wk_mtx;

    void internalSetThreadWorkingName(std::string name) {
        {
            std::unique_lock _lk(wk_mtx);
            Working[std::this_thread::get_id()] = std::move(name);
        }
    }

    void setThreadWorkingName(std::string name) {
        internalSetThreadWorkingName(std::move(name));
    }

    void unsetThreadWorkingName() {
        internalSetThreadWorkingName("");
    }

    std::string getThreadWorkingName() {
        std::string name;
        {
            std::unique_lock _lk(wk_mtx);
            name = Working[std::this_thread::get_id()];
        }
        return name;
    }
} // namespace ThreadIdentify
