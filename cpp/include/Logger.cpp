#include "CPPPlugin.h"
#include "Config.h"

namespace MiraiCP {
    // 静态成员

    Logger Logger::logger = Logger();
    [[deprecated("Use Logger::logger instead")]] Logger *const logger = &Logger::logger;

    // 结束静态成员

    /*
    日志类实现
    throw: InitException 即找不到签名
    */
    void Logger_interface::init(JNIEnv *env) {
        this->log = env->GetStaticMethodID(Config::CPP_lib, "KSendLog", "(Ljava/lang/String;I)V");
    }

    void Logger_interface::registerHandle(Logger_interface::Action action) {
        this->loggerhandler.action = std::move(action);
    }

    void Logger_interface::setHandleState(bool state) {
        this->loggerhandler.enable = state;
    }

    void Logger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = -2;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

    void IdLogger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = id;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

    void PluginLogger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = -1;
        j["name"] = CPPPlugin::plugin->config.id;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }
} // namespace MiraiCP
