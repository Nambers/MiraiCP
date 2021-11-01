#include "Config.h"
#include "Exception.h"

namespace MiraiCP {
    // 静态成员
    jclass Config::CPP_lib = nullptr;
    jmethodID Config::KOperation = nullptr;
    // 结束静态成员

    /*
    配置类实现
    throw: InitxException 即找不到对应签名
    */
    void Config::construct(JNIEnv *env) {
        Config::CPP_lib = reinterpret_cast<jclass>(env->NewGlobalRef(
                env->FindClass("tech/eritquearcus/miraicp/shared/CPP_lib")));
        if (Config::CPP_lib == nullptr) {
            throw APIException("初始化错误，找不到CPP_lib类");
        }
        Config::KOperation = env->GetStaticMethodID(CPP_lib, "KOperation", "(Ljava/lang/String;)Ljava/lang/String;");
    }

    void Config::destruct() {
        ThreadManager::getEnv(__FILE__, __LINE__)->DeleteGlobalRef(Config::CPP_lib);
    }

    std::string Config::koperation(operation_set type, json &data, JNIEnv *env, bool catchErr, const std::string &errorInfo) {
        json j;
        j["type"] = type;
        j["data"] = data;
        std::string re = Tools::jstring2str((jstring) env->CallStaticObjectMethod(Config::CPP_lib,
                                                                                  Config::KOperation,
                                                                                  Tools::str2jstring(j.dump().c_str(), env)),
                                            env);
        if (catchErr) ErrorHandle(re, errorInfo);
        return re;
    }

} // namespace MiraiCP
