// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

#include "ThreadManager.h"
#include "Logger.h"

namespace MiraiCP {
    // 静态成员

    std::map<std::string, ThreadManager::ThreadInfo> ThreadManager::threads = std::map<std::string, ThreadInfo>();
    std::recursive_mutex ThreadManager::mtx = std::recursive_mutex();
    JavaVM *ThreadManager::gvm = nullptr;
    long ThreadManager::JNIVersion = 0;

    // 结束静态成员

    void ThreadManager::setEnv(JNIEnv *e) {
        mtx.lock();
        if (!ThreadManager::included(ThreadManager::getThreadId())) {
            ThreadInfo tmp{e, false};
            ThreadManager::threads.insert(std::pair<std::string, ThreadInfo>(ThreadManager::getThreadId(), tmp));
        } else {
            ThreadManager::threads[ThreadManager::getThreadId()].e = e;
        }
        mtx.unlock();
    }

    void ThreadManager::newEnv(const char *threadName) {
        JNIEnv *env = nullptr;
        JavaVMAttachArgs args{JNIVersion,
                              const_cast<char *>(threadName),
                              nullptr};
        gvm->AttachCurrentThread((void **) &env, &args);
        ThreadInfo tmp{env, true};
        ThreadManager::threads.insert(std::pair<std::string, ThreadInfo>(ThreadManager::getThreadId(), tmp));
        Logger::logger.info("refresh env");
    };

    void ThreadManager::detach() {
        mtx.lock();
        if (ThreadManager::included(ThreadManager::getThreadId())) {
            bool att = ThreadManager::threads[ThreadManager::getThreadId()].attach;
            ThreadManager::threads.erase(ThreadManager::getThreadId());
            if (att)
                gvm->DetachCurrentThread();
        }
        mtx.unlock();
    }

    bool ThreadManager::included(const std::string &id) {
        if (ThreadManager::threads.empty() || ThreadManager::threads.count(id) == 0)
            return false;
        return true;
    }

    JNIEnv *ThreadManager::getEnv() {
        mtx.lock();
        if (!ThreadManager::included(getThreadId())) {
            ThreadManager::newEnv();
        }
        JNIEnv *tmp = ThreadManager::threads[ThreadManager::getThreadId()].e;
        mtx.unlock();
        return tmp;
    }
} // namespace MiraiCP
