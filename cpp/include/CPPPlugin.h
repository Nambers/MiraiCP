#ifndef MIRAICP_PRO_CPPPLUGIN_H
#define MIRAICP_PRO_CPPPLUGIN_H

#include "Logger.h"
#include "PluginConfig.h"
namespace MiraiCP {
    /// 插件父类
    class CPPPlugin {
    public:
        /// @brief 插件信息
        PluginConfig config;
        /// @brief 插件级logger
        static PluginLogger *pluginLogger;

        static CPPPlugin *plugin;

        virtual void onEnable() {}

        virtual void onDisable() {}

        explicit CPPPlugin(PluginConfig c) : config(std::move(c)) {}
    };

} // namespace MiraiCP

#endif //MIRAICP_PRO_CPPPLUGIN_H
