//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_COMMONTYPES_H
#define MIRAICP_PRO_COMMONTYPES_H
// don't create cpp for this header


#include "loaderApi.h"
#include <jni.h>
#include <unordered_map>


namespace LibLoader {
    struct MiraiCPPluginConfig;
    typedef std::unordered_map<std::string, MiraiCPPluginConfig> PluginList;
    typedef void *plugin_handle;
    typedef void (*plugin_entrance_func_ptr)(const interface_funcs &, JavaVM *);
    typedef void (*plugin_func_ptr)();

    // implement MiraiCPPluginConfig
    struct MiraiCPPluginConfig {
        plugin_handle handle;
        std::string path;
    };
} // namespace LibLoader


#endif //MIRAICP_PRO_COMMONTYPES_H
