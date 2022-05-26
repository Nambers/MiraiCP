//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_LOADERAPI_H
#define MIRAICP_PRO_LOADERAPI_H


#include <string>
#include <vector>

// the API defs to be exposed
namespace LibLoader {
    std::vector<std::string> _showAllPluginName();

    void _enablePluginByName(const std::string &);

    void _disablePluginByName(const std::string &);

    void _enableAllPlugins();

    void _disableAllPlugins();

    void _loadNewPlugin(const std::string &, bool);

    struct interface_funcs {
        decltype(&_showAllPluginName) showAllPluginName;
        decltype(&_enablePluginByName) enablePluginByName;
        decltype(&_disablePluginByName) disablePluginByName;
        decltype(&_enableAllPlugins) enableAllPlugins;
        decltype(&_disableAllPlugins) disableAllPlugins;
        decltype(&_loadNewPlugin) loadNewPlugin;
    };

    constexpr inline interface_funcs collect_interface_functions() {
        static_assert(sizeof(interface_funcs) == sizeof(void *) * 6);
        return {
                _showAllPluginName,
                _enablePluginByName,
                _disablePluginByName,
                _enableAllPlugins,
                _disableAllPlugins,
                _loadNewPlugin,
        };
    }
} // namespace LibLoader


#endif //MIRAICP_PRO_LOADERAPI_H
