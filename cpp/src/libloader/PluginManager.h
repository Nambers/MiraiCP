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

#ifndef MIRAICP_PRO_PLUGINMANAGER_H
#define MIRAICP_PRO_PLUGINMANAGER_H


#include "commonTypes.h"
#include <functional>
#include <memory>
#include <shared_mutex>
#include <unordered_map>


namespace LibLoader {
    class Plugin;

    /// Plugin 对象的管理接口，全静态
    /// 不应提供接口可以使外部获取 Plugin 对象，所有功能应委托 PluginManager 完成
    class PluginManager {
        typedef std::unordered_map<std::string, std::shared_ptr<Plugin>> PluginMap;
        typedef PluginMap::iterator iterator;

    private:
        PluginMap id_plugin_map;
        std::shared_mutex pluginlist_mtx;

    private:
        void changeKey(const std::string &key, const std::string &new_key);

        void changeKeyInternal(const std::string &key, const std::string &new_key);

    public:
        void unloadAll();

    public:
        //        PluginManager() = delete;
        //        ~PluginManager() = delete;

        static PluginManager &Get() {
            static PluginManager instance;
            return instance;
        }

    public:
        std::vector<std::string> getAllPluginId();

        /// 将所有插件信息格式化为一个string
        std::string pluginListInfo(const std::function<bool(const Plugin &)> &);

        std::vector<std::string> getAllPluginPath();

        /// 返回目前记录的插件个数，使用前请先获取锁
        auto count() { return id_plugin_map.size(); }
        /// 返回目前记录的插件个数，使用前请先获取锁
        bool empty() { return id_plugin_map.empty(); }

    public: // load
        bool addNewPlugin(const std::shared_ptr<Plugin> &cfg);

    public: // unload
        void unloadById(const std::string &);
        void unloadWhenException(const std::string &);

    public: // reload
        void reloadAllPlugin();
        void reloadById(const std::string &);

    public: // enable
        void enableAll();
        void enableById(const std::string &);

    public: // disable
        void disableAll();
        void disableById(const std::string &);

    public:
        void run_over_pluginlist(const std::function<void(const Plugin &)> &f);

    private:
        std::string &threadRunningPluginId();

    public:
        std::string getThreadRunningPluginId() {
            return threadRunningPluginId();
        }

    public:
        void broadcastToAllEnabledPlugins(const MiraiCP::MiraiCPString &strPtr);

        void broadcastToOnePlugin(const std::string &id, MiraiCP::MiraiCPString str);

        void loadNewPluginByPath(const std::string &_path, bool activateNow);

        bool pluginNameLookup(const std::string &_id);

        std::chrono::time_point<std::chrono::system_clock> getPluginTimeStamp(const std::string &_id);
    };
} // namespace LibLoader


#endif //MIRAICP_PRO_PLUGINMANAGER_H
