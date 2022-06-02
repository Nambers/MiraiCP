//
// Created by antares on 6/3/22.
//


#ifndef MIRAICP_PRO_PLUGINLIST_H
#define MIRAICP_PRO_PLUGINLIST_H


#include "commonTypes.h"
#include <jni.h>


// 字符串宏
#define STRINGIFY(A) __STRINGIFY(A)
#define __STRINGIFY(A) #A
// todo(Antares): 将这部分宏放到公共头文件中
#define FUNC_ENTRANCE enrollPlugin
#define FUNC_EVENT eventHandle
#define FUNC_EXIT onDisable
#define PLUGIN_INFO MiraiCPPluginInfo


namespace LibLoader {
    // todo(Antares): 文件过于复杂，需要重构

    // 暴露的接口，必须开放
    void loader_enablePluginByName(const std::string &name);
    void loader_disablePluginByName(const std::string &name);
    void loader_enableAllPlugins();
    void loader_disableAllPlugins();
    void loader_loadNewPlugin(const std::string &path, bool activateNow);

    // loaderMain调用
    void activateAllPlugins();

    // 插件端调用
    std::vector<std::string> getAllPluginName();

    // todo(Antares): 重构下列接口，可能的话尽量避免调用
    void reloadAllPlugin(jstring _cfgPath);
    void addPlugin(MiraiCPPluginConfig cfg);
    void enableAll();
    void disableAll();
} // namespace LibLoader


#endif //MIRAICP_PRO_PLUGINLIST_H
