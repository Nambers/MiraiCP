//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_COMMONTYPES_H
#define MIRAICP_PRO_COMMONTYPES_H

#include "libInterface.h"
#include <unordered_map>


namespace LibLoader {
    struct MiraiCPPluginConfig;
    typedef std::unordered_map<std::string, MiraiCPPluginConfig> PluginList;
    typedef void *plugin_handle;
    typedef void (*plugin_func_ptr)(const interface_funcs *);
    typedef void (*void_func_ptr)();

    // implement MiraiCPPluginConfig
    struct MiraiCPPluginConfig {
        plugin_handle handle;
        std::string path;
    };

    // lib interface defs
    // typedef void *lib_interface_func_ptr;                // void*, erase function signatures
    // typedef lib_interface_func_ptr *lib_interface_funcs; // void**
} // namespace LibLoader


#endif //MIRAICP_PRO_COMMONTYPES_H
