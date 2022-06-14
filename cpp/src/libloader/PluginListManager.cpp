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

#include "PluginListManager.h"
#include "LoaderLogger.h"
#include "PluginConfig.h"
#include "PluginListImplements.h"
#include "libOpen.h"
#include "loaderTools.h"
#include <future>
#include <string>


namespace LibLoader {
    PluginListManager::PluginList PluginListManager::plugin_list;
    std::recursive_mutex PluginListManager::pluginlist_mtx;

    ////////////////////////////////////
    /// PluginListController部分
    /// 这部分必须保证每个函数都是原子操作
    /// 如果有第一行不是创建lock_guard对象的请务必检查

    void PluginListManager::run_over_pluginlist(const std::function<void(const std::string &, const MiraiCPPluginConfig &)> &f) {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: plugin_list) {
            f(k, v);
        }
    }

    void PluginListManager::addPlugin(MiraiCPPluginConfig cfg) {
        std::lock_guard lk(pluginlist_mtx);

        auto cfgptr = (MiraiCP::PluginConfig *) LoaderApi::libSymbolLookup(cfg.handle, STRINGIFY(PLUGIN_INFO));
        if (cfgptr == nullptr) {
            logger.error("Runtime Error: plugin config does not exist, did you forget to check symbol existance?");
            return;
        }
        plugin_list.insert(std::make_pair(cfgptr->id, std::move(cfg)));
    }

    std::vector<std::string> PluginListManager::getAllPluginName() {
        std::lock_guard lk(pluginlist_mtx);
        std::vector<std::string> ans;
        for (auto &&[k, v]: plugin_list) {
            ans.emplace_back(k);
        }
        return ans;
    }

    void PluginListManager::unloadAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: plugin_list) {
            unload_plugin(v);
        }
    }

    void PluginListManager::reloadAllPlugin(jstring _cfgPath) {
        std::lock_guard lk(pluginlist_mtx);
        unloadAll();
        plugin_list.clear();

        std::string cfgPath = jstring2str(_cfgPath);
        // todo(antares): this function is not finished
    }

    void PluginListManager::disableAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: plugin_list) {
            v.enable = false;
        }
    }

    void PluginListManager::enableAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: plugin_list) {
            enable_plugin(v);
        }
    }

    void PluginListManager::unloadById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = plugin_list.find(id);
        if (it == plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        unload_plugin(it->second);
    }

    void PluginListManager::enableById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = plugin_list.find(id);
        if (it == plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        enable_plugin(it->second);
    }

    void PluginListManager::disableById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = plugin_list.find(id);
        if (it == plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        it->second.enable = false;
    }
} // namespace LibLoader
