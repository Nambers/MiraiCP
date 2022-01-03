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

#include "Config.h"
#include "Exception.h"
#include "Tools.h"
#include "ThreadManager.h"

namespace MiraiCP::Config {
    jclass CPPLib = nullptr;
    jmethodID KOperation = nullptr;
    // 不可以赋值, 不然会覆盖
    int pluginId;
    /*
    配置类实现
    throw: InitxException 即找不到对应签名
    */
    void construct(JNIEnv *env) {
        if (env == nullptr) env = ThreadManager::getEnv();
        CPPLib = reinterpret_cast<jclass>(env->NewGlobalRef(
                env->FindClass("tech/eritquearcus/miraicp/shared/CPPLib")));
        if (CPPLib == nullptr) {
            MiraiCPThrow(APIException("初始化错误，找不到CPPLib类"));
        }
        KOperation = env->GetStaticMethodID(CPPLib, "KOperation", "(Ljava/lang/String;)Ljava/lang/String;");
    }

    void destruct() {
        ThreadManager::getEnv()->DeleteGlobalRef(CPPLib);
    }

    std::string koperation(operation_set type, const nlohmann::json &data, JNIEnv *env, bool catchErr, const std::string &errorInfo) {
        if (env == nullptr) env = ThreadManager::getEnv();
        nlohmann::json j;
        j["type"] = type;
        j["data"] = data;
        std::string re = Tools::jstring2str((jstring) env->CallStaticObjectMethod(CPPLib,
                                                                                  KOperation,
                                                                                  Tools::str2jstring(j.dump().c_str(), env)),
                                            env);
        if (catchErr) ErrorHandle(re, errorInfo);
        return re;
    }
} // namespace MiraiCP::Config
