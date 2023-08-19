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
#include <future>
#include <polym/Queue.hpp>
#include <shared_mutex>
#include <string>


namespace LibLoader {
    class Plugin : public PluginData {
        using timepoint = std::chrono::time_point<std::chrono::system_clock>;
        std::atomic<int> _runCounter = {0};
        mutable std::shared_mutex _mtx;
        PolyM::Queue *message_queue;
        timepoint timestamp;

    public:
        explicit Plugin(std::string inPath) : PluginData(std::move(inPath)) {}

    public: // external usage, with lock; literally
        void loadPlugin(bool alsoEnablePlugin);

        void enablePlugin();

        void unloadPlugin();

        void disablePlugin();

        void unloadWhenException();

    public:
        /// 只应被线程池worker调用
        void pushEvent_worker(const MiraiCP::MiraiCPString &event);

    public:
        [[nodiscard]] bool checkValid() const { return handle != nullptr; }
        [[nodiscard]] bool checkLoaded() const { return checkValid() && infoFunc != nullptr; }

    public:
        /// 安全、带锁地获取id
        [[nodiscard]] std::string getIdSafe() const;

        // 带锁地格式化
        void formatTo(std::vector<std::string> &, size_t (&charNum)[4]);

        void forceCallExit();

        timepoint getTimeStamp() const;

        std::unique_ptr<PolyM::Msg> popMessage() const;

        void popMessageTo(std::vector<std::unique_ptr<PolyM::Msg>> & messageList) const;

    public:
        static PluginFuncAddrData testSymbolExistance(plugin_handle handle, const std::string &path);

        /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用一次
        /// 实际的入口，id_plugin_list 必须在这里初始化，该函数只会被调用一次
        static void registerAllPlugin(const std::string &cfgPath) noexcept;

    private:
        static plugin_handle loadNewPlugin(const std::string &path) noexcept;

        static void loadsAll(const std::vector<std::string> &paths, const std::vector<PluginAuthority> &authorities) noexcept;

    private:
        message_queue_handle callEntranceFuncAdmin() const;

        message_queue_handle callEntranceFuncNormal() const;

        message_queue_handle callEntranceByAuthority() const;

    private: // internal, no lock; literally
        void updateTimeStamp();

        void enableInternal();

        std::shared_ptr<std::future<void>> disableInternal(bool lockedAndWait = false);

        void loadInternal(bool alsoEnablePlugin);

        void unloadInternal();

        void unloadWhenExceptionInternal();
    };
} // namespace LibLoader
#endif //MIRAICP_PRO_PLUGIN_H
