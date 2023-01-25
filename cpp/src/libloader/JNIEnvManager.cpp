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

#ifndef LOADER_NATIVE

#include "LoaderMacro.h"
// -----------------------
#include "JNIEnvManager.h"


class JNIEnvManager::ThreadInfo {
    friend class JNIEnvManager;

    JNIEnv *env_ptr = nullptr;

    JNIEnv *getEnv() {
        return env_ptr;
    }

    void setEnv(JNIEnv *env) {
        env_ptr = env;
    };

public:
    ~ThreadInfo() {
        if (env_ptr) gvm->DetachCurrentThread();
    }

private:
    static JavaVM *gvm; ///< 全局JavaVM对象，用于多线程管理中新建线程的JNIEnv.
};

JavaVM *JNIEnvManager::ThreadInfo::gvm = nullptr;

thread_local JNIEnvManager::ThreadInfo thread_info;

/// @brief 为当前线程创建一个env.
inline JNIEnv *newEnv() {
    JNIEnv *env = nullptr;
    JavaVMAttachArgs args{static_cast<int>(MIRAICP_JVER),
                          nullptr,
                          nullptr};
    JNIEnvManager::getGvm()->AttachCurrentThread(MIRAICP_JNIPPTR &env, &args);
    return env;
}

bool JNIEnvManager::setEnv(JNIEnv *e) {
    return (nullptr == thread_info.env_ptr) && ((thread_info.env_ptr = e) || true);
}

JNIEnv *JNIEnvManager::getEnv() {
    if (nullptr == thread_info.env_ptr) thread_info.env_ptr = newEnv();
    return thread_info.env_ptr;
}

void JNIEnvManager::detach() {
    if (nullptr != thread_info.env_ptr) {
        getGvm()->DetachCurrentThread();
        thread_info.env_ptr = nullptr;
    }
}


JavaVM *JNIEnvManager::getGvm() {
    return ThreadInfo::gvm;
}

void JNIEnvManager::setGvm(JavaVM *_gvm) {
    ThreadInfo::gvm = _gvm;
}

namespace EnvMocker {
    /// @note dev: this function is only used for tests;
    ///  should never be declared in source headers
    MIRAICP_EXPORT void force_reset_env() {
        thread_info = JNIEnvManager::ThreadInfo();
    }
} // namespace EnvMocker

#endif //LOADER_NATIVE
