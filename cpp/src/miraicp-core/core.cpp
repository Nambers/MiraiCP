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
#include "core.h"
#include <MiraiCP.hpp>
#include <jni.h>
#ifdef _WIN32
#include <windows.h>
void freeLibrary(void *pointer) {
    ::FreeLibrary((HINSTANCE) pointer);
}
#else
void freeLibrary(void *pointer) {
    MiraiCP::ThreadManager::gvm->DestroyJavaVM();
}
#endif
typedef jint(JNICALL *JNICREATEPROC)(JavaVM **, void **, void *);
namespace MiraiCP::Core {
    void *jvmLib;
    jclass coreClaz;

    JNIEXPORT jstring enroll(JNIEnv *env, jobject) {
        JNINativeMethod tmp[]{
                {(char *) "Verify", (char *) "(Ljava/lang/String;)Ljava/lang/String;", (jstring *) MiraiCP::JNIApi::Verify},
                {(char *) "Event", (char *) "(Ljava/lang/String;)Ljava/lang/String;", (jstring *) MiraiCP::JNIApi::Event},
                {(char *) "PluginDisable", (char *) "()Ljava/lang/Void;", (jobject *) MiraiCP::JNIApi::PluginDisable}};
        MiraiCP::JNIApi::registerMethods(env, "tech/eritquearcus/miraicp/shared/CPPLib", tmp, 3);
        return MiraiCP::Tools::str2jstring("MIRAICP_NULL", env);
    }
    int loadCore(const std::string &corePath) {
        //java虚拟机启动时接收的参数
        JavaVMOption vmOption[2];

        vmOption[0].optionString = (char *) "-Xmx2048M";
#ifdef _WIN32
        std::string tmp = "-Djava.class.path=.\\;" + corePath + ";";
#else
        std::string tmp = "-Djava.class.path=./:" + corePath;
#endif
        vmOption[1].optionString = (char *) tmp.c_str();

        // for debugging, add "-verbose:jni"
        // note: change length of vmOption to 3
        // vmOption[2].optionString = (char *) "-verbose:jni";

        JavaVMInitArgs vmInitArgs;
        vmInitArgs.version = JNI_VERSION_1_8;
        vmInitArgs.options = vmOption;
        // 参数数
        vmInitArgs.nOptions = 2;
        // 忽略无法识别jvm的情况
        // vmInitArgs.ignoreUnrecognized = JNI_TRUE;
        // 不忽略
        vmInitArgs.ignoreUnrecognized = JNI_FALSE;
        //加载JVM动态库
        JNIEnv *env = nullptr;
#ifdef _WIN32
        jvmLib = ::LoadLibraryA(_JVM_DLL_PATH);
        if (jvmLib == nullptr) {
            // 加载JVM动态库错误
            return 1;
        }
        //获取JVM函数地址
        JNICREATEPROC jvmProcAddress = (JNICREATEPROC) GetProcAddress((HINSTANCE) jvmLib, "JNI_CreateJavaVM");
        if (jvmLib == nullptr) {
            freeLibrary(jvmLib);
            // 加载JVM动态库错误
            return 2;
        }
        jint jvmProc = (jvmProcAddress) (&MiraiCP::ThreadManager::gvm, (void **) &env, &vmInitArgs);
        if (jvmProc < 0 || MiraiCP::ThreadManager::gvm == nullptr || env == nullptr) {
            freeLibrary(jvmLib);
            // 创建JVM错误
            return 3;
        }
#else
        long flag = JNI_CreateJavaVM(&MiraiCP::ThreadManager::gvm, (void **) &env, &vmInitArgs);
        if (flag != 0) {
            // Error creating VM. Exiting...
            freeLibrary(nullptr);
            return 3;
        }
#endif
        //加载启动类
        coreClaz = env->FindClass("tech/eritquearcus/miraicp/core/Core");
        if (env->ExceptionCheck() == JNI_TRUE || coreClaz == nullptr) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            freeLibrary(jvmLib);
            // 加载启动类失败
            return 4;
        }
        //加载 main 启动方法
        jmethodID methodID = env->GetStaticMethodID(coreClaz, "load", "()V");
        if (env->ExceptionCheck() == JNI_TRUE || methodID == nullptr) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            freeLibrary(jvmLib);
            // 加载启动方法失败
            return 5;
        }
        JNINativeMethod e[] = {{(char *) "enroll", (char *) "()Ljava/lang/Void;", (jstring *) enroll}};
        MiraiCP::JNIApi::registerMethods(env, "tech/eritquearcus/miraicp/core/Core", e, 1);
        env->CallStaticVoidMethod(coreClaz, methodID, nullptr);
        return 0;
    }

    void exitCore() {
        //jvm释放
        MiraiCP::ThreadManager::gvm->DestroyJavaVM();
        freeLibrary(jvmLib);
    }

    Bot login(QQID id, std::string_view password, bool md5, std::string_view protocol, std::string_view heartBeat, JNIEnv *env) {
        nlohmann::json tmp;
        tmp["id"] = id;
        tmp["passwords"] = password;
        tmp["md5"] = md5;
        tmp["protocol"] = protocol;
        tmp["heartBeat"] = heartBeat;
        if (env == nullptr)
            env = MiraiCP::ThreadManager::getEnv();
        jmethodID loginMethodId = env->GetStaticMethodID(coreClaz, "login", "(Ljava/lang/String;)Ljava/lang/String;");
        std::string re = MiraiCP::Tools::jstring2str((jstring) env->CallStaticObjectMethod(coreClaz, loginMethodId, MiraiCP::Tools::str2jstring(tmp.dump().c_str(), env)));
        if (re != "200")
            throw BotException("登录异常:" + re, MIRAICP_EXCEPTION_WHERE);
        return Bot(id);
    }
} // namespace MiraiCP::Core
void MiraiCP::enrollPlugin() {}