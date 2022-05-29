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


namespace MiraiCP {
    std::unordered_map<ThreadManager::threadid, ThreadManager::ThreadInfo> ThreadManager::threads;
    std::recursive_mutex ThreadManager::mtx;
    JavaVM *ThreadManager::gvm = nullptr;
    long ThreadManager::JNIVersion = 0;


    void ThreadManager::setEnv(JNIEnv *e) {
        std::lock_guard lk(mtx);
        if (!included(getThreadId())) {
            ThreadInfo tmp{e, false};
            threads.insert(std::make_pair(getThreadId(), tmp));
        } else {
            threads[getThreadId()].e = e;
        }
    }

    void ThreadManager::newEnv(const char *threadName) {
        JNIEnv *env = nullptr;
        JavaVMAttachArgs args{static_cast<int>(JNIVersion),
                              const_cast<char *>(threadName),
                              nullptr};
        gvm->AttachCurrentThread((void **) &env, &args);
        ThreadInfo tmp{env, true};
        threads.insert(std::make_pair(getThreadId(), tmp));
    }

    void ThreadManager::detach() {
        std::lock_guard lk(mtx);
        if (included(getThreadId())) {
            bool att = threads[getThreadId()].attach;
            threads.erase(getThreadId());
            if (att)
                gvm->DetachCurrentThread();
        }
    }

    JNIEnv *ThreadManager::getEnv() {
        JNIEnv *tmp;
        {
            std::lock_guard lk(mtx);
            if (!included(getThreadId())) {
                newEnv();
            }
            tmp = threads[getThreadId()].e;
        }
        return tmp;
    }
} // namespace MiraiCP
