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
#else
// todo 测试linux
#include <dlfcn.h>
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
    int loadCore(const char *jvmPath, const std::string &corePath) {
        //java虚拟机启动时接收的参数
        JavaVMOption vmOption[2];
        // todo 自定义参数
        vmOption[0].optionString = (char *) "-Xmx2048M";
        vmOption[1].optionString = (char *) ("-Djava.class.path=.\\;" + corePath + ";").c_str();

        JavaVMInitArgs vmInitArgs;
        vmInitArgs.version = JNI_VERSION_1_8;
        vmInitArgs.options = vmOption;
        // 参数数
        vmInitArgs.nOptions = 2;
        // 忽略无法识别jvm的情况
        vmInitArgs.ignoreUnrecognized = JNI_TRUE;
        //加载JVM动态库
        jvmLib = ::LoadLibraryA(jvmPath);
        if (jvmLib == nullptr) {
            // 加载JVM动态库错误
            return 1;
        }
        //获取JVM函数地址
        JNICREATEPROC jvmProcAddress = (JNICREATEPROC) GetProcAddress((HINSTANCE) jvmLib, "JNI_CreateJavaVM");
        if (jvmLib == nullptr) {
            FreeLibrary((HINSTANCE) jvmLib);
            // 加载JVM动态库错误
            return 2;
        }
        JNIEnv *env = nullptr;
        jint jvmProc = (jvmProcAddress) (&MiraiCP::ThreadManager::gvm, (void **) &env, &vmInitArgs);
        if (jvmProc < 0 || MiraiCP::ThreadManager::gvm == nullptr || env == nullptr) {
            FreeLibrary((HINSTANCE) jvmLib);
            // 创建JVM错误
            return 3;
        }
        //加载启动类
        coreClaz = env->FindClass("tech/eritquearcus/miraicp/core/Core");
        if (env->ExceptionCheck() == JNI_TRUE || coreClaz == nullptr) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            FreeLibrary((HINSTANCE) jvmLib);
            // 加载启动类失败
            return 4;
        }
        //加载 main 启动方法
        jmethodID methodID = env->GetStaticMethodID(coreClaz, "load", "()V");
        if (env->ExceptionCheck() == JNI_TRUE || methodID == nullptr) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            FreeLibrary((HINSTANCE) jvmLib);
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
        FreeLibrary((HINSTANCE) jvmLib);
    }

    std::optional<Bot> login(QQID id, std::string_view password, bool md5, std::string_view protocol, std::string_view heartBeat, JNIEnv *env) {
        nlohmann::json tmp;
        tmp["id"] = id;
        tmp["password"] = password;
        tmp["md5"] = md5;
        tmp["protocol"] = protocol;
        tmp["heartBeat"] = heartBeat;
        if (env == nullptr)
            env = MiraiCP::ThreadManager::getEnv();
        static jmethodID loginMethodId = env->GetStaticMethodID(coreClaz, "login", "(Ljava/lang/String;)V");
        env->CallStaticVoidMethod(coreClaz, loginMethodId, MiraiCP::Tools::str2jstring(tmp.dump().c_str(), env));
        return Bot(id);
    }
} // namespace MiraiCP::Core