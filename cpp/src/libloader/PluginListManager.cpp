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
#include "BS_thread_pool.hpp"
#include "LoaderExceptions.h"
#include "LoaderLogger.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "commonTools.h"
#include "libOpen.h"
#include "loaderTools.h"
#include <sstream>
#include <string>
#include <utility>


namespace LibLoader {
    PluginListManager::PluginList PluginListManager::id_plugin_list;
    std::shared_mutex PluginListManager::pluginlist_mtx;

    ////////////////////////////////////
    // 一个工具函数
    std::string PluginInfoStream(const std::vector<std::string> &plugin_info, size_t (&charNum)[4]) {
        std::ostringstream out;
        out << std::setiosflags(std::ios::left) << "\n";
        int index = 0;
        for (const auto &one_plugin_info: plugin_info) {
            if (index == 0) out << '|';
            if (one_plugin_info != "\n") {
                out << std::setfill(' ') << std::setw(int(charNum[index])) << one_plugin_info;
                out << '|';
            } else {
                out << "\n";
            }
            index++;
            if (index == 5) {
                index = 0;
                out << std::setw(int(charNum[0] + charNum[1] + charNum[2] + charNum[3] + 4 + 1)) << std::setfill('-') << "";
                out << "\n";
            }
        }
        return out.str();
    }
    ////////////////////////////////////
    /// PluginListController部分
    /// 这部分必须保证每个函数都是原子操作
    /// 如果有第一行不是创建lock_guard对象的请务必检查

    std::vector<std::string> PluginListManager::getAllPluginId() {
        std::shared_lock lk(pluginlist_mtx);
        std::vector<std::string> ans;
        for (auto &&[k, v]: id_plugin_list) {
            if (v->checkValid()) ans.emplace_back(v->getIdSafe());
        }
        return ans;
    }

    std::string PluginListManager::pluginListInfo(const std::function<bool(const Plugin &)> &filter) {
        std::shared_lock lk(pluginlist_mtx);
        std::vector<std::string> ans = {"id", "name or path", "author", "description", "\n"};
        size_t charNum[4] = {2 + 1, 12 + 1, 6 + 1, 11 + 1};
        for (auto &&[k, v]: id_plugin_list) {
            v->formatTo(ans, charNum);
        }
        return PluginInfoStream(ans, charNum);
    }

    std::vector<std::string> PluginListManager::getAllPluginPath() {
        std::shared_lock lk(pluginlist_mtx);
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
    bool PluginListManager::addNewPlugin(const std::shared_ptr<Plugin> &pluginPtr) {
        std::unique_lock lk(pluginlist_mtx);

        if (!pluginPtr->isLoaded()) {
            throw PluginNotLoadedException(pluginPtr->path, MIRAICP_EXCEPTION_WHERE);
        }

        // 该函数语境下，插件不会被其他线程访问到，因为获得的Plugin智能指针刚刚被创建，无需考虑锁的问题

        auto id = pluginPtr->getIdSafe();

        auto pr = id_plugin_list.insert(std::make_pair(id, pluginPtr));

        if (!pr.second) {
            logger.error("Plugin with id: " + id + " Already exists");
            pluginPtr->unload_plugin();
            throw PluginIdDuplicateException(id, pr.first->first, pluginPtr->path, MIRAICP_EXCEPTION_WHERE);
        }

        return true;
    }

    /// unload全部插件，但不会修改任何key
    void PluginListManager::unloadAll() {
        std::shared_lock lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            v->unload_plugin();
        }
    }

    /// 插件正常工作时的卸载函数，不会修改任何key
    void PluginListManager::unloadById(const std::string &id) {
        std::shared_lock lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        it->second->unload_plugin();
    }

    /// 插件异常时的卸载函数，输入插件id，不会修改任何key
    void PluginListManager::unloadWhenException(const std::string &id) {
        std::shared_lock lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        it->second->unload_when_exception();
    }

    /// reload 所有插件。
    /// 该函数设计时需要注意，我们假设插件仍然保存在原路径，但是不能保证id仍然不变
    /// 所有的key必须被清除然后重新添加
    void PluginListManager::reloadAllPlugin() {
        std::unique_lock lk(pluginlist_mtx);
        unloadAll();

        std::vector<std::shared_ptr<Plugin>> cfgs;
        for (auto &&[k, v]: id_plugin_list) {
            cfgs.emplace_back(v);
        }

        id_plugin_list.clear();
        for (auto &&cfg: cfgs) {
            loadNewPluginByPath(cfg->path, true);
            auto pr = id_plugin_list.insert(std::make_pair(cfg->getIdSafe(), cfg));
            if (!pr.second) {
                logger.error("插件id: " + cfg->getIdSafe() + " 重复加载，将自动unload加载较晚的插件");
                cfg->unload_plugin();
            }
        }
    }

    /// reload 一个插件。
    /// 该函数设计时需要注意，我们假设插件仍然保存在原路径，但是不能保证id仍然不变
    /// key必须被清除然后重新添加
    void PluginListManager::reloadById(const std::string &id) {
        std::unique_lock lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }

        auto &cfg = *(it->second);
        cfg.unload_plugin();
        cfg.load_plugin(true);

        if (cfg.getIdSafe() != it->first) {
            logger.warning("插件id: " + it->first + " 被修改为: " + cfg.getIdSafe());
            changeKeyInternal(it->first, cfg.getIdSafe());
        }
    }

    void PluginListManager::enableAll() {
        std::shared_lock lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            v->enable_plugin();
        }
    }

    void PluginListManager::enableById(const std::string &id) {
        std::shared_lock lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        it->second->enable_plugin();
    }

    void PluginListManager::disableAll() {
        std::shared_lock lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            v->disable_plugin();
        }
    }

    void PluginListManager::disableById(const std::string &id) {
        std::shared_lock lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }
        it->second->disable_plugin();
    }

    void PluginListManager::disableByIdVanilla(const std::string &id) {
        std::shared_lock lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end()) {
            logger.error(id + "尚未加载");
            return;
        }

        it->second->forceCallExit();
    }

    /// 遍历所有插件，by id（默认是不会by path的，path是用于id变更的特殊情况的备份）
    /// 注意：不会检查插件是否enable，请自行检查
    void PluginListManager::run_over_pluginlist(const std::function<void(const Plugin &)> &f) {
        std::shared_lock lk(pluginlist_mtx);
        for (auto &&[k, v]: id_plugin_list) {
            f(*v);
        }
    }

    void PluginListManager::changeKeyInternal(const std::string &key, const std::string &new_key) {
        auto ptr = id_plugin_list[key];
        id_plugin_list.erase(key);
        auto pr = id_plugin_list.insert(std::make_pair(new_key, ptr));
        if (!pr.second) {
            logger.error("Reload失败！插件id: " + new_key + " 已经被另一个插件占用。当前插件将被unload");
            ptr->unload_plugin();
            return;
        }
    }

    void PluginListManager::changeKey(const std::string &key, const std::string &new_key) {
        std::unique_lock lk(pluginlist_mtx);
        changeKeyInternal(key, new_key);
    }

    std::string &PluginListManager::threadRunningPluginId() {
        thread_local static std::string id;
        return id;
    }

    void PluginListManager::broadcastToAllEnabledPlugins(const MiraiCP::MiraiCPString &strPtr) {
        std::shared_lock lk(pluginlist_mtx);
        for (auto &&[id, pluginConfig]: id_plugin_list) {
            if (pluginConfig->isEnabled()) {
                auto pluginConfigCopy = pluginConfig;
                //                auto eventPtr = pluginConfig->eventFunc;
                //                // 禁止捕获和plugin本身有关的东西，因为不知道谁先运行完
                BS::pool->push_task([strPtrCopy = strPtr, pluginConfig = std::move(pluginConfigCopy)]() mutable {
                    pluginConfig->pushEvent_worker(strPtrCopy);
                });
            }
        }
    }

    void PluginListManager::broadcastToOnePlugin(const std::string &id, MiraiCP::MiraiCPString str) {
        std::shared_lock lk(pluginlist_mtx);
        auto it = id_plugin_list.find(id);
        if (it == id_plugin_list.end() || !it->second->isEnabled()) {
            logger.error("插件不存在或尚未加载！插件id: " + id);
        }
        BS::pool->push_task([strCopy = std::move(str), pluginConfig = it->second]() mutable {
            pluginConfig->pushEvent_worker(strCopy);
        });
    }

    void PluginListManager::loadNewPluginByPath(const std::string &_path, bool activateNow) {
        auto pluginPtr = std::make_shared<Plugin>(_path);

        pluginPtr->load_plugin(activateNow);

        addNewPlugin(pluginPtr);
    }

    bool PluginListManager::pluginNameLookup(const std::string &_id) {
        std::shared_lock lk(pluginlist_mtx);
        return id_plugin_list.find(_id) != id_plugin_list.end();
    }


} // namespace LibLoader
