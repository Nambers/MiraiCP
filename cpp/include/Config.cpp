// Copyright (c) 2021-2021. Eritque arcus and contributors.
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

namespace MiraiCP::Config {
    jclass CPPLib = nullptr;
    jmethodID KOperation = nullptr;
    /*
    配置类实现
    throw: InitxException 即找不到对应签名
    */
    void construct(JNIEnv *env) {
        CPPLib = reinterpret_cast<jclass>(env->NewGlobalRef(
                env->FindClass("tech/eritquearcus/miraicp/shared/CPPLib")));
        if (CPPLib == nullptr) {
            MiraiCPThrow(APIException("初始化错误，找不到CPPLib类"));
        }
        KOperation = env->GetStaticMethodID(CPPLib, "KOperation", "(Ljava/lang/String;)Ljava/lang/String;");
    }

    void destruct() {
        getEnv()->DeleteGlobalRef(CPPLib);
    }

    std::string koperation(operation_set type, const nlohmann::json &data, JNIEnv *env, bool catchErr, const std::string &errorInfo) {
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
