// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
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

/// 本文件的全部函数实现都是kt线程调用的函数

#include "MiraiCPMacros.h"
// -----------------------
#include "ktInterface.h"
#include "LoaderLogger.h"
#include "PluginListManager.h"
#include "ThreadController.h"
#include "commonTools.h"
#include "eventHandle.h"
#include "loaderMain.h"
#include "redirectCout.h"

#ifdef LOADER_NATIVE
#include <cstdio>
#else
#include "JNIEnvs.h"
#include "LoaderMacro.h"
#include "loaderTools.h"
#endif

namespace LibLoader {
    void registerAllPlugin(const std::string &) noexcept;
    std::thread loaderThread;
} // namespace LibLoader

void VerifyImpl(JSTRING _version, JSTRING _cfgPath) {
    //初始化日志模块
    MiraiCP::Redirector::start();

    LibLoader::logger.info("⭐libLoader 版本: " + MiraiCP::MiraiCPVersion);
    auto version = "v" + J_TO_STD_STRING(_version);
    if (version != MiraiCP::MiraiCPVersion) {
        LibLoader::logger.warning(
                "libLoader(" + MiraiCP::MiraiCPVersion + ")版本和MiraiCP启动器(" + version + ")不一致, 建议更新至最新");
    }

    // 测试有效的插件
    LibLoader::registerAllPlugin(J_TO_STD_STRING(_cfgPath));

    // 激活插件。创建loader thread。
    // loader thread中创建多线程加载所有插件，调用入口函数
    LibLoader::loaderThread = std::thread(LibLoader::LoaderMain::loaderMain);
}

void EventImpl(JSTRING content) {
    static std::string str;

    std::lock_guard lk(LibLoader::PluginListManager::getLock());
    str = J_TO_STD_STRING(content);

    if (str.find(R"("type":1000)") != std::string::npos) {
        LibLoader::builtInCommand(str);
        return;
    }

    // static lambda，不可以捕获参数！str被声明为static了会被自动捕获
    static std::function broadcast_func = [](const LibLoader::LoaderPluginConfig &cfg) {
        if (cfg.handle && cfg.enabled) {
            LibLoader::ThreadController::getController().submitJob(cfg.getId(), [&cfg]() {
                cfg.eventFunc(str);
            });
        }
    };

    LibLoader::PluginListManager::run_over_pluginlist(broadcast_func);
}

void PluginDisableImpl() {
    LibLoader::LoaderMain::loaderExit();
    LibLoader::loaderThread.join();
}

#ifdef LOADER_NATIVE
namespace LoaderAPIs {
    using LogFunc = void (*)(const char *, int);
    using OperFunc = const char *(*) (const char *);
    LogFunc log = nullptr;
    OperFunc oper = nullptr;
} // namespace LoaderAPIs

extern "C" {
MIRAICP_EXPORT void Verify(const char *a, const char *b, const char *(*oper)(const char *), void (*log)(const char *, int)) {
    printf("call Verify(%s, %s)\n", a, b);
    LoaderAPIs::log = log;
    LoaderAPIs::oper = oper;
    VerifyImpl(a, b);
}

MIRAICP_EXPORT void Event(const char *a) {
    printf("call Event(%s)\n", a);
    EventImpl(a);
}

MIRAICP_EXPORT void PluginDisable() {
    PluginDisableImpl();
}
}
#else
/// 实际初始化函数
/// 1. 设置全局变量
/// 2. 开启loader线程并获取插件入口函数的返回值
jobject Verify(JNIEnv *env, jobject, JSTRING _version, JSTRING _cfgPath) {
    JNIEnvManager::setEnv(env);

    //初始化日志模块
    LibLoader::JNIEnvs::initializeMiraiCPLoader();

    VerifyImpl(_version, _cfgPath);

    return nullptr;
}

/// 事件广播，由kt（主）线程调用
/// 广播过程中应当锁住 plugin list，以防止内存访问冲突
/// 广播给插件的过程中由插件线程完成所有任务
/// 插件线程无法给plugin list加锁，因为插件端只能向loader线程发送某个任务的申请
/// loader线程可能会尝试获取 plugin list 的锁，
/// 但Event函数在派发任务后是会立刻退出并释放锁的，
/// 不会造成死锁
jobject Event(JNIEnv *env, jobject, jstring content) {
    SET_ENV(env);
    EventImpl(content);
    return nullptr;
}

jobject PluginDisable(JNIEnv *env, jobject) {
    SET_ENV(env);
    PluginDisableImpl();
    return nullptr;
}

int registerMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethods, int numMethods) {
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        return JNI_FALSE;
    }
    //注册native方法
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

// register
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_8) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);
    JNIEnvManager::setGvm(vm);
    JNIEnvManager::setEnv(env);
    // 注册native方法
    if (!registerMethods(env, "tech/eritquearcus/miraicp/shared/CPPLibMultiplatform", method_table, 3)) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_8;
}
#endif
