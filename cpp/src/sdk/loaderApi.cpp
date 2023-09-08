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
#include "CPPPlugin.h"
#include "Exception.h"
#include "MsgDefine.h"
#include "MsgSender.h"
#include "commonTools.h"
#include "loaderApiInternal.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>


namespace MiraiCP {
    /// 插件没有权限时抛出该异常
    /// 该异常仅可能在插件尝试调用libLoader 高级权限的Api接口时抛出
    /// 如插件尝试重载、加载、卸载插件等操作，但配置文件中并没有赋予该插件权限时
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT PluginNotAuthorizedException : public MiraiCPExceptionCRTP<PluginNotAuthorizedException> {
    public:
        explicit PluginNotAuthorizedException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("插件" + CPPPlugin::config.getId() + "没有管理权限", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "PluginNotAuthorizedException"; }
    };

    /// 插件未加载抛出该异常
    /// 在插件能正常运行时不会抛出，出现该异常事件时请不要再次尝试收发消息等Mirai操作，
    /// 否则可能导致异常处理时再次抛出异常
    /// @see MiraiCPExceptionBase
    class MIRAICP_EXPORT PluginNotEnabledException : public MiraiCPExceptionCRTP<PluginNotEnabledException> {
    public:
        explicit PluginNotEnabledException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("插件" + CPPPlugin::config.getId() + "未加载", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "PluginNotEnabledException"; }
    };
} // namespace MiraiCP


namespace LibLoader::LoaderApi {
    using namespace MiraiCP;
    using namespace PolyM;
#define PUT_MSG \
    if constexpr (PayloadClass::blocking) { \
        getMsgQueue()->request(DataMsg<PayloadClass>(PayloadClass::static_payload_class_id(), std::move(tmp))); \
    } else { \
        getMsgQueue()->put(DataMsg<PayloadClass>(PayloadClass::static_payload_class_id(), std::move(tmp))); \
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


    // check the func ptr existance before use it
    inline void checkApi(void *funcptr) {
        if (loader_apis == nullptr) [[unlikely]] {
            throw PluginNotEnabledException(MIRAICP_EXCEPTION_WHERE);
        } else if (funcptr == nullptr) [[unlikely]] {
            throw PluginNotAuthorizedException(MIRAICP_EXCEPTION_WHERE);
        }
    }

    /// interfaces for plugins

    MiraiCPString pluginOperation(MiraiCPStringview s) {
        using PayloadClass = MiraiCP::OperationMessage;
        PayloadClass tmp;
        tmp.msg_string = s;
        auto ret = getMsgQueue()->request(DataMsg<PayloadClass>(PayloadClass::static_payload_class_id(), std::move(tmp)));
        if (ret != nullptr){
            auto ret_msg = static_cast<DataMsg<MiraiCPString>*>(ret.get()); // NOLINT(*-pro-type-static-cast-downcast)
            return std::move(ret_msg->getPayload());
        }
        throw
//        PUT_MSG;
//        checkApi((void *) loader_apis->_pluginOperation);
//        return loader_apis->_pluginOperation(s);
    }

    void loggerInterface(const MiraiCPString &content, const MiraiCPString &name, long long id, int level) {
        using PayloadClass = MiraiCP::LoggerMessage;
        PayloadClass tmp;
        tmp.msg_string = content;
        tmp.name = name;
        tmp.msg_lid = id;
        tmp.msg_level = level;
        PUT_MSG;

        MiraiCP::getMsgQueue()->put(PolyM::DataMsg<PayloadClass>(PayloadClass::static_payload_class_id(), std::move(tmp)));
        checkApi((void *) loader_apis->_loggerInterface);
        loader_apis->_loggerInterface(content, name, id, level);
    }

    MiraiCPString showAllPluginId() {
        using PayloadClass = MiraiCP::PluginIdMessage;
        PayloadClass tmp;
        PUT_MSG;
        checkApi((void *) loader_apis->_showAllPluginId);
        return loader_apis->_showAllPluginId();
    }

    void pushTask(task_func func) {
        using PayloadClass = MiraiCP::PushTaskMessage;
        PayloadClass tmp;
        tmp.task_func = func;
        PUT_MSG;
        checkApi((void *) loader_apis->_pushTask);
        loader_apis->_pushTask(func);
    }

    void pushTaskWithId(task_func_with_id func, size_t id) {
        using PayloadClass = MiraiCP::PushTaskWithIdMessage;
        PayloadClass tmp;
        tmp.task_func_with_id = func;
        tmp.id = id;
        PUT_MSG;
        checkApi((void *) loader_apis->_pushTaskWithId);
        loader_apis->_pushTaskWithId(func, id);
    }

    void timer(const MiraiCPString &id, const MiraiCPString &content, size_t sec) {
        using PayloadClass = MiraiCP::TimerMessage;
        PayloadClass tmp;
        tmp.timer_id = id;
        tmp.content = content;
        tmp.sec = sec;
        PUT_MSG;
        checkApi((void *) loader_apis->_timer);
        loader_apis->_timer(id, content, sec);
    }

    void enablePluginById(const MiraiCPString &id) {
        checkApi((void *) loader_apis->_enablePluginById);
        loader_apis->_enablePluginById(id);
    }

    void disablePluginById(const MiraiCPString &id) {
        checkApi((void *) loader_apis->_disablePluginById);
        loader_apis->_disablePluginById(id);
    }

    void enableAllPlugins() {
        checkApi((void *) loader_apis->_enableAllPlugins);
        loader_apis->_enableAllPlugins();
    }

    void disableAllPlugins() {
        checkApi((void *) loader_apis->_disableAllPlugins);
        loader_apis->_disableAllPlugins();
    }

    void loadNewPlugin(const MiraiCPString &path, bool activateNow) {
        checkApi((void *) loader_apis->_loadNewPlugin);
        loader_apis->_loadNewPlugin(path, activateNow);
    }

    void unloadPluginById(const MiraiCPString &id) {
        checkApi((void *) loader_apis->_unloadPluginById);
        loader_apis->_unloadPluginById(id);
    }

    void reloadPluginById(const MiraiCPString &id) {
        checkApi((void *) loader_apis->_reloadPluginById);
        loader_apis->_reloadPluginById(id);
    }
} // namespace LibLoader::LoaderApi

namespace MiraiCP::LoaderApi {
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
