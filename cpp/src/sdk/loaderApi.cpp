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
#include "Exception.h"
#include "MsgDefine.h"
#include "loaderApiInternal.h"
#include <nlohmann/json.hpp>
#include <polym/Queue.hpp>
#include <string>
#include <vector>

namespace MiraiCP {
    PolyM::Queue *getMsgQueue();
} // namespace MiraiCP

namespace LibLoader {
    void try_wake_loader();
}

namespace LibLoader::LoaderApi {
    using namespace MiraiCP;
    using namespace PolyM;

    template<typename T>
    MiraiCPString send_payload(T &&payload) {
        using PayloadClass = typename std::decay_t<T>;
        if constexpr (PayloadClass::blocking) {
            auto ret_raw = getMsgQueue()->request(
                    DataMsg<PayloadClass>(PayloadClass::static_payload_class_id(),
                                          std::forward<T>(payload)),
                    0,
                    LibLoader::try_wake_loader);
            auto ret = static_cast<PolyM::DataMsg<MiraiCPString> *>(ret_raw.get());
            return std::move(ret->getPayload());
        } else {
            getMsgQueue()->put(DataMsg<PayloadClass>(PayloadClass::static_payload_class_id(), std::forward<T>(payload)));
            LibLoader::try_wake_loader();
            return {};
        }
    }

    static const interface_funcs *loader_apis = nullptr;

    MIRAICP_EXPORT void set_loader_apis(const LibLoader::LoaderApi::interface_funcs *apis) noexcept {
        loader_apis = apis;
    }

    MIRAICP_EXPORT void reset_loader_apis() noexcept {
        loader_apis = nullptr;
    }

    /// 这个函数是给本cpp以外的文件使用的，大概率用不到
    const interface_funcs *get_loader_apis() {
        return loader_apis;
    }

    using MiraiCP::PluginNotAuthorizedException;
    using MiraiCP::PluginNotEnabledException;

    /// interfaces for plugins

    MiraiCPString pluginOperation(MiraiCPStringview s) {
        using PayloadClass = MiraiCP::OperationMessage;
        PayloadClass tmp;
        tmp.msg_string = s;
        return send_payload(std::move(tmp));
    }

    void loggerInterface(const MiraiCPString &content, const MiraiCPString &name, long long id, int level) {
        using PayloadClass = MiraiCP::LoggerMessage;
        PayloadClass tmp;
        tmp.msg_string = content;
        tmp.name = name;
        tmp.msg_lid = id;
        tmp.msg_level = level;
        send_payload(std::move(tmp));
    }

    MiraiCPString showAllPluginId() {
        using PayloadClass = MiraiCP::PluginIdMessage;
        PayloadClass tmp;
        return send_payload(std::move(tmp));
    }

    void pushTask(task_func func) {
        using PayloadClass = MiraiCP::PushTaskMessage;
        PayloadClass tmp;
        tmp.task_func = func;
        send_payload(std::move(tmp));
    }

    void pushTaskWithId(task_func_with_id func, size_t id) {
        using PayloadClass = MiraiCP::PushTaskWithIdMessage;
        PayloadClass tmp;
        tmp.task_func_with_id = func;
        tmp.id = id;
        send_payload(std::move(tmp));
    }

    void timer(const MiraiCPString &id, const MiraiCPString &content, size_t sec) {
        using PayloadClass = MiraiCP::TimerMessage;
        PayloadClass tmp;
        tmp.timer_id = id;
        tmp.content = content;
        tmp.sec = sec;
        send_payload(std::move(tmp));
    }

    void _admin_command(int cmd_id, const MiraiCPString &arg, bool immediate = false) {
        using PayloadClass = MiraiCP::AdminMessage;
        PayloadClass tmp;
        tmp.cmd_id = cmd_id;
        tmp.cmd_arg = arg;
        tmp.immediate = immediate;
        send_payload(std::move(tmp));
    }

    void enablePluginById(const MiraiCPString &id) {
        _admin_command(MiraiCP::AdminMessage::ENABLE_ONE, id);
    }

    void disablePluginById(const MiraiCPString &id) {
        _admin_command(MiraiCP::AdminMessage::DISABLE_ONE, id);
    }

    void enableAllPlugins() {
        _admin_command(MiraiCP::AdminMessage::ENABLE_ALL, "");
    }

    void disableAllPlugins() {
        _admin_command(MiraiCP::AdminMessage::DISABLE_ALL, "");
    }

    void loadNewPlugin(const MiraiCPString &path, bool activateNow) {
        _admin_command(MiraiCP::AdminMessage::LOAD_NEW, path, activateNow);
    }

    void unloadPluginById(const MiraiCPString &id) {
        _admin_command(MiraiCP::AdminMessage::UNLOAD_ONE, id);
    }

    void reloadPluginById(const MiraiCPString &id) {
        _admin_command(MiraiCP::AdminMessage::RELOAD_ONE, id);
    }
} // namespace LibLoader::LoaderApi

namespace MiraiCP::LoaderApi {
    /// ---------- 封装接口 ----------
    void loggerInterface(const std::string &content, const std::string &name, long long int id, int level) {
        LibLoader::LoaderApi::loggerInterface(content, name, id, level);
    }

    std::vector<std::string> showAllPluginId() {
        nlohmann::json::array_t PluginIdList = nlohmann::json::parse(LibLoader::LoaderApi::showAllPluginId().toString());
        return {PluginIdList.begin(), PluginIdList.end()};
    }

    void enablePluginById(const std::string &id) {
        LibLoader::LoaderApi::enablePluginById(id);
    }

    void disablePluginById(const std::string &id) {
        LibLoader::LoaderApi::disablePluginById(id);
    }

    void enableAllPlugins() {
        LibLoader::LoaderApi::enableAllPlugins();
    }

    void disableAllPlugins() {
        LibLoader::LoaderApi::disableAllPlugins();
    }

    void loadNewPlugin(const std::string &path, bool enableNow) {
        LibLoader::LoaderApi::loadNewPlugin(path, enableNow);
    }

    void unloadPluginById(const std::string &id) {
        LibLoader::LoaderApi::unloadPluginById(id);
    }

    void reloadPluginById(const std::string &id) {
        LibLoader::LoaderApi::reloadPluginById(id);
    }
} // namespace MiraiCP::LoaderApi
