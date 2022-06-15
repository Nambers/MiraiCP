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
#include "ThreadController.h"
#include "libOpen.h"
#include "loaderTools.h"
#include <future>
#include <string>


namespace LibLoader {
    PluginListManager::PluginList PluginListManager::id_plugin_list;
    std::recursive_mutex PluginListManager::pluginlist_mtx;

    ////////////////////////////////////
    /// PluginListController部分
    /// 这部分必须保证每个函数都是原子操作
    /// 如果有第一行不是创建lock_guard对象的请务必检查

    std::vector<std::string> PluginListManager::getAllPluginId() {
        std::lock_guard lk(pluginlist_mtx);
        std::vector<std::string> ans;
        for (auto &&[k, v]: id_plugin_list) {
            ans.emplace_back(k);
        }
        return ans;
    }

    /// PluginList 仅存储plugin的信息，addNewPlugin 接收的是一个已经加载好的 Plugin
    /// cfg 中已经存储了handle，id等信息
    /// 如果插件id重复则应该取消加载
    void PluginListManager::addNewPlugin(LoaderPluginConfig cfg, bool activateNow) {
        std::lock_guard lk(pluginlist_mtx);

        auto id = cfg.getId();
        auto handle = cfg.handle;
        auto pr = id_plugin_list.insert(std::make_pair(id, std::make_shared<LoaderPluginConfig>(std::move(cfg))));

        if (!pr.second) {
            logger.error("Plugin with id: " + id + " Already exists");
            LoaderApi::libClose(handle);
            return;
        }

        auto &plugincfg = *(pr.first->second);

        plugincfg.disable();

        if (activateNow) enable_plugin(plugincfg);
    }

    /// unload全部插件，但不会修改任何key
    void PluginListManager::unloadAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            unload_plugin(*v);
        }
    }

    void PluginListManager::unloadById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        unload_plugin(*(it->second));
    }

    // todo(Antares): 考虑是否删除这个函数
    void PluginListManager::reloadAllPlugin(jstring _cfgPath) {
        std::lock_guard lk(pluginlist_mtx);
        unloadAll();
        id_plugin_list.clear();

        std::string cfgPath = jstring2str(_cfgPath);
        // todo(antares): this function is not finished
    }

    void PluginListManager::enableAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            v->enable();
        }
    }

    void PluginListManager::enableById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        it->second->enable();
    }

    void PluginListManager::disableAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            v->disable();
        }
    }

    void PluginListManager::disableById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        it->second->disable();
    }

    /// 遍历所有插件，by id（默认是不会by path的，path是用于id变更的特殊情况的备份）
    /// 注意：不会检查插件是否enable，请自行检查
    void PluginListManager::run_over_pluginlist(const std::function<void(const std::string &, const LoaderPluginConfig &)> &f) {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            f(k, *v);
        }
    }
} // namespace LibLoader
