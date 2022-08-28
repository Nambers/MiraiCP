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
#include "LoaderExceptions.h"
#include "LoaderLogger.h"
#include "PluginConfig.h"
#include "PluginListImplements.h"
#include "commonTools.h"
#include "libOpen.h"
#include "loaderTools.h"
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
            if (v->handle != nullptr) ans.emplace_back(v->config().getId());
        }
        return ans;
    }

    std::string PluginListManager::pluginListInfo(const std::function<bool(const LoaderPluginConfig &)> &filter) {
        std::lock_guard lk(pluginlist_mtx);
        std::vector<std::string> ans = {"id", "name", "author", "description", "\n"};
        int charNum[4] = {2 + 1, 4 + 1, 6 + 1, 11 + 1};
        for (auto &&[k, v]: id_plugin_list) {
            if (v->handle != nullptr) {
                if (filter(*v)) FormatPluginListInfo(v->config(), charNum, ans);
            } else {
                // todo(Antares): 显示path
            }
        }
        return PluginInfoStream(ans, charNum);
    }

    std::vector<std::string> PluginListManager::getAllPluginPath() {
        std::lock_guard lk(pluginlist_mtx);
        std::vector<std::string> ans;
        for (auto &&[k, v]: id_plugin_list) {
            ans.emplace_back(v->path);
        }
        return ans;
    }

    /// PluginList 仅存储plugin的信息，addNewPlugin 接收的是一个已经load 或者已经enable 的 Plugin
    /// 总之，addNewPlugin 只是把plugin加入到PluginList中，其他什么都不做
    /// cfg 中已经存储了handle，id等信息
    /// 如果插件id重复则应该取消加载
    bool PluginListManager::addNewPlugin(LoaderPluginConfig cfg) {
        std::lock_guard lk(pluginlist_mtx);

        if (cfg.handle == nullptr || cfg.config == nullptr) {
            throw PluginNotLoadedException(cfg.path, MIRAICP_EXCEPTION_WHERE);
        }

        auto id = cfg.getId();
        auto handle = cfg.handle;
        auto path = cfg.path;
        auto pr = id_plugin_list.insert(std::make_pair(id, std::make_shared<LoaderPluginConfig>(std::move(cfg))));

        if (!pr.second) {
            logger.error("Plugin with id: " + id + " Already exists");
            LoaderApi::libClose(handle);
            throw PluginIdDuplicateException(id, pr.first->first, path, MIRAICP_EXCEPTION_WHERE);
        }

        return true;
    }

    /// unload全部插件，但不会修改任何key
    void PluginListManager::unloadAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            unload_plugin(*v);
        }
    }

    /// 插件正常工作时的卸载函数，不会修改任何key
    void PluginListManager::unloadById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        unload_plugin(*(it->second));
    }

    /// 插件异常时的卸载函数，输入插件id，不会修改任何key
    void PluginListManager::unloadWhenException(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        unload_when_exception(*(it->second));
    }

    /// reload 所有插件。
    /// 该函数设计时需要注意，我们假设插件仍然保存在原路径，但是不能保证id仍然不变
    /// 所有的key必须被清除然后重新添加
    void PluginListManager::reloadAllPlugin() {
        std::lock_guard lk(pluginlist_mtx);
        unloadAll();

        std::vector<std::shared_ptr<LoaderPluginConfig>> cfgs;
        for (auto &&[k, v]: id_plugin_list) {
            cfgs.emplace_back(v);
        }

        id_plugin_list.clear();
        for (auto &&cfg: cfgs) {
            loadNewPluginByPath(cfg->path, true);
            auto pr = id_plugin_list.insert(std::make_pair(cfg->getId(), cfg));
            if (!pr.second) {
                logger.error("插件id: " + cfg->getId() + " 重复加载，将自动unload加载较晚的插件");
                unload_plugin(*cfg);
            }
        }
    }

    /// reload 一个插件。
    /// 该函数设计时需要注意，我们假设插件仍然保存在原路径，但是不能保证id仍然不变
    /// key必须被清除然后重新添加
    void PluginListManager::reloadById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }

        auto &cfg = *(it->second);
        unload_plugin(cfg);
        load_plugin(cfg, true);

        if (cfg.getId() != it->first) {
            logger.warning("插件id: " + it->first + " 被修改为: " + cfg.getId());
            changeKey(it->first, cfg.getId());
        }
    }

    void PluginListManager::enableAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            enable_plugin(*v);
        }
    }

    void PluginListManager::enableById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        enable_plugin(*(it->second));
    }

    void PluginListManager::disableAll() {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            disable_plugin(*v);
        }
    }

    void PluginListManager::disableById(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        disable_plugin(*(it->second));
    }

    void PluginListManager::disableByIdVanilla(const std::string &id) {
        std::lock_guard lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }

        try {
            auto exit_ptr = get_plugin_disable_ptr(*(it->second));
            exit_ptr();
        } catch (...) {} // do not leak any exception
    }

    /// 遍历所有插件，by id（默认是不会by path的，path是用于id变更的特殊情况的备份）
    /// 注意：不会检查插件是否enable，请自行检查
    void PluginListManager::run_over_pluginlist(const std::function<void(const LoaderPluginConfig &)> &f) {
        std::lock_guard lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            f(*v);
        }
    }

    void PluginListManager::changeKey(const std::string &key, const std::string &new_key) {
        std::lock_guard lk(pluginlist_mtx);
        auto ptr = id_plugin_list[key];
        id_plugin_list.erase(key);
        auto pr = id_plugin_list.insert(std::make_pair(new_key, ptr));
        if (!pr.second) {
            logger.error("Reload失败！插件id: " + new_key + " 已经被另一个插件占用。当前插件将被unload");
            unload_plugin(*ptr);
            return;
        }
    }
} // namespace LibLoader
