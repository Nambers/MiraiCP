//
// Created by antares on 6/15/22.
//

#ifndef MIRAICP_PRO_LOADERPLUGINCONFIG_H
#define MIRAICP_PRO_LOADERPLUGINCONFIG_H


#include "commonTypes.h"
#include <string>

namespace LibLoader {
    // implement LoaderPluginConfig
    struct LoaderPluginConfig {
        std::string path;
        plugin_handle handle;
        plugin_func_ptr eventFunc;
        const MiraiCP::PluginConfig *config;
        PluginAuthority authority = PLUGIN_AUTHORITY_NORMAL;
        bool enable = true;

        const std::string &getId() const {
            return config->id;
        }

        void reset() {
            handle = nullptr;
            eventFunc = nullptr;
            config = nullptr;
            authority = PLUGIN_AUTHORITY_NORMAL;
            enable = true;
        }
    };

} // namespace LibLoader
#endif //MIRAICP_PRO_LOADERPLUGINCONFIG_H
