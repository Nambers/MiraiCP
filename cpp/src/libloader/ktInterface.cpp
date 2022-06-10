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

#include "ktInterface.h"
#include "JNIEnvManager.h"
#include "JNIEnvs.h"
#include "commonTypes.h"
#include "eventHandle.h"
#include "loaderMain.h"
#include "loaderTools.h"
#include <exception>
#include <future>
#include <json.hpp>


namespace LibLoader {
    void registerAllPlugin(jstring);
    std::string activateAllPlugins();
    std::thread loaderThread;
    std::vector<plugin_func_ptr> getEntrances(JNIEnv *env) {
        return {};
    }
} // namespace LibLoader


/// 实际初始化函数
/// 1. 设置全局变量
/// 2. 开启loader线程并获取插件入口函数的返回值
jobject Verify(JNIEnv *env, jobject, jstring _version, jstring _cfgPath) {
    using json = nlohmann::json;

    assert(JNIEnvManager::getGvm() != nullptr);

    LibLoader::JNIEnvs::setJNIVersion();

    std::string ans;
    try {
        //初始化日志模块
        LibLoader::JNIEnvs::initializeMiraiCPLoader();

        // 测试有效的插件
        LibLoader::registerAllPlugin(_cfgPath);

        // 激活插件。创建loader thread。
        // loader thread中创建多线程加载所有插件，调用入口函数
        LibLoader::loaderThread = std::thread(LibLoader::LoaderMain::loaderMain);
    } catch (std::exception &e) {
    }

    return nullptr;
}

jobject Event(JNIEnv *env, jobject, jstring content) {
    std::string str = LibLoader::jstring2str(content);
    if (str.find("\"id\":1001") != std::string::npos) {
        LibLoader::builtInCommand(str);
        return nullptr;
    }

    auto entrances = LibLoader::getEntrances(env);
    for (auto a: entrances) {
        // a(str);
        // 解析交给MiraiCP吧
        // 异常也应该在MiraiCP中捕获
    }
    return nullptr;
}

jobject PluginDisable(JNIEnv *env, jobject) {
    LibLoader::LoaderMain::loaderExit();
    LibLoader::loaderThread.join();
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
extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);
    JNIEnvManager::setGvm(vm);
    // 注册native方法
    if (!registerMethods(env, "tech/eritquearcus/miraicp/shared/CPPLib", method_table, 3)) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
