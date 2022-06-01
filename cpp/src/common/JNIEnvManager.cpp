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

#include "JNIEnvManager.h"


std::unordered_map<JNIEnvManager::threadid, JNIEnvManager::ThreadInfo> JNIEnvManager::threadJNIEnvs;
std::recursive_mutex JNIEnvManager::mtx;
JavaVM *JNIEnvManager::gvm = nullptr;
long JNIEnvManager::JNIVersion = 0;


JNIEnv *JNIEnvManager::newEnv() {
    JNIEnv *env = nullptr;
    JavaVMAttachArgs args{static_cast<int>(JNIVersion),
                          nullptr,
                          nullptr};
    gvm->AttachCurrentThread((void **) &env, &args);
    return env;
}

void JNIEnvManager::setEnv(JNIEnv *e) {
    std::lock_guard lk(mtx);
    auto pr = threadJNIEnvs.insert(std::make_pair(getThreadId(), ThreadInfo{e, false}));
    if (!pr.second) {
        pr.first->second.e = e;
    }
}

JNIEnv *JNIEnvManager::getEnv() {
    JNIEnv *tmp;
    {
        std::lock_guard lk(mtx);
        auto pr = threadJNIEnvs.insert(std::make_pair(getThreadId(), ThreadInfo{nullptr, true}));
        if (pr.second) {
            pr.first->second.e = newEnv();
        }
        tmp = pr.first->second.e;
    }
    return tmp;
}

void JNIEnvManager::detach() {
    std::lock_guard lk(mtx);
    auto it = threadJNIEnvs.find(getThreadId());
    if (it != threadJNIEnvs.end()) {
        if (it->second.attach) gvm->DetachCurrentThread();
        threadJNIEnvs.erase(it);
    }
}

void JNIEnvManager::detachAll() {
    std::lock_guard lk(mtx);
    for (auto &&[k, v]: threadJNIEnvs) {
        if (v.attach) gvm->DetachCurrentThread();
    }
    threadJNIEnvs.clear();
}
