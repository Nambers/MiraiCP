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


JNIEnv *JNIEnvManager::newEnv() {
    JNIEnv *env = nullptr;
    // todo JNIVersion是否写死
    JavaVMAttachArgs args{static_cast<int>(JNI_VERSION_1_8),
                          nullptr,
                          nullptr};
    gvm->AttachCurrentThread((void **) &env, &args);
    return env;
}

bool JNIEnvManager::setEnv(JNIEnv *e) {
    std::lock_guard lk(mtx);
    auto pr = threadJNIEnvs.insert(std::make_pair(getThreadId(), ThreadInfo{e, false}));
    if (!pr.second) {
        pr.first->second.env_ptr = e;
        return true;
    }
    return false;
}

JNIEnv *JNIEnvManager::getEnv() {
    JNIEnv *tmp;
    {
        std::lock_guard lk(mtx);
        auto pr = threadJNIEnvs.insert(std::make_pair(getThreadId(), ThreadInfo{nullptr, true}));
        if (pr.second) {
            pr.first->second.env_ptr = newEnv();
        }
        tmp = pr.first->second.env_ptr;
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
