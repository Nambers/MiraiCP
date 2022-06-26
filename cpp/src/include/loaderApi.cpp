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

#include "loaderApi.h"
#include <string>
#include <utility>
#include <vector>


namespace LibLoader::LoaderApi {
    static const interface_funcs *loader_apis = nullptr;

    void set_loader_apis(const LibLoader::LoaderApi::interface_funcs *apis) {
        loader_apis = apis;
    }

    void reset_loader_apis() {
        loader_apis = nullptr;
    }

    /// 这个函数是给本cpp以外的文件使用的，大概率用不到
    const interface_funcs *get_loader_apis() {
        return loader_apis;
    }

    /// todo(Antares): 目前还没有实现失败时的处理，
    ///  比如如果showAllPluginId()里loader_apis == nullptr，
    ///  那么应该在logger或者其他地方输出，但是logger这个时候也是nullptr，获取不到
    ///  这个之后讨论一下

    /// interfaces for plugins

    std::string pluginOperation(const std::string &s) {
        if (loader_apis == nullptr || loader_apis->_pluginOperation == nullptr) {
            return "";
        }
        return loader_apis->_pluginOperation(s);
    }

    void loggerInterface(const std::string &content, std::string name, long long id, int level) {
        if (loader_apis == nullptr || loader_apis->_loggerInterface == nullptr) {
            return;
        }
        loader_apis->_loggerInterface(content, std::move(name), id, level);
    }

    std::vector<std::string> showAllPluginId() {
        if (loader_apis == nullptr || loader_apis->_showAllPluginId == nullptr) {
            return {};
        }
        return loader_apis->_showAllPluginId();
    }

    void enablePluginById(const std::string &id) {
        if (loader_apis == nullptr || loader_apis->_enablePluginById == nullptr) {
            return;
        }
        loader_apis->_enablePluginById(id);
    }

    void disablePluginById(const std::string &id) {
        if (loader_apis == nullptr || loader_apis->_disablePluginById == nullptr) {
            return;
        }
        loader_apis->_disablePluginById(id);
    }

    void enableAllPlugins() {
        if (loader_apis == nullptr || loader_apis->_enableAllPlugins == nullptr) {
            return;
        }

        loader_apis->_enableAllPlugins();
    }

    void disableAllPlugins() {
        if (loader_apis == nullptr || loader_apis->_disableAllPlugins == nullptr) {
            return;
        }
        loader_apis->_disableAllPlugins();
    }

    void loadNewPlugin(const std::string &path, bool activateNow) {
        if (loader_apis == nullptr || loader_apis->_loadNewPlugin == nullptr) {
            return;
        }
        loader_apis->_loadNewPlugin(path, activateNow);
    }

    void unloadPluginById(const std::string &id) {
        if (loader_apis == nullptr || loader_apis->_unloadPluginById == nullptr) {
            return;
        }
        loader_apis->_unloadPluginById(id);
    }

    void reloadPluginById(const std::string &id) {
        if (loader_apis == nullptr || loader_apis->_reloadPluginById == nullptr) {
            return;
        }
        loader_apis->_reloadPluginById(id);
    }
} // namespace LibLoader::LoaderApi
