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

#ifndef MIRAICP_PRO_PLUGIN_H
#define MIRAICP_PRO_PLUGIN_H


#include "PluginData.h"
#include "commonTypes.h"
#include <atomic>
#include <shared_mutex>
#include <string>


namespace LibLoader {
    class Plugin : public PluginData {
        std::atomic<int> _runCounter = {0};
        mutable std::shared_mutex _mtx;

    public:
        explicit Plugin(std::string inPath) : PluginData(std::move(inPath)) {}

    private:
        static plugin_handle loadNewPlugin(const std::string &path) noexcept;

        static void loadsAll(const std::vector<std::string> &paths, const std::vector<PluginAuthority> &authorities) noexcept;

    private:
        void callEntranceFuncAdmin();

        void callEntranceFuncNormal();

    private: // internal, no lock; literally
        void enableInternal();

        void disableInternal();

        void loadInternal(bool alsoEnablePlugin);

        void unloadInternal();

        void unloadWhenExceptionInternal();

    public: // external usage, with lock; literally
        // todo(Antares): 检查完load等用法后重命名
        void load_plugin(bool alsoEnablePlugin);

        void enable_plugin();

        void unload_plugin();

        void disable_plugin();

        void unload_when_exception();

    public:
        /// 只应被线程池worker调用
        void pushEvent_worker(const MiraiCP::MiraiCPString &event);

    public:
        [[nodiscard]] bool checkValid() const { return handle != nullptr; }
        [[nodiscard]] bool checkLoaded() const { return checkValid() && infoFunc != nullptr; }

    public:
        /// 安全、带锁地获取id
        [[nodiscard]] std::string getIdSafe() const;

        void formatTo(std::vector<std::string> &, size_t (&charNum)[4]);

    public:
        static PluginFuncAddrData testSymbolExistance(plugin_handle handle, const std::string &path);

        /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用一次
        /// 实际的入口，id_plugin_list 必须在这里初始化，该函数只会被调用一次
        static void registerAllPlugin(const std::string &cfgPath) noexcept;
    };

    // plugin_func_ptr get_plugin_disable_ptr(Plugin &plugin);
} // namespace LibLoader
#endif //MIRAICP_PRO_PLUGIN_H