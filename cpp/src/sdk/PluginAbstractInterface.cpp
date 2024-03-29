// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#include "Event.h"
#include "Events/MiraiCPExceptionEvent.h"
#include "Exception.h"
#include "Logger.h"
#include "MsgDefine.h"
#include "commonTypes.h"
#include "utils.h"
#include <deque>
#include <iostream>
#include <memory>
#include <polym/Msg.hpp>
#include <polym/Queue.hpp>


namespace MiraiCP {
    PolyM::Queue *getMsgQueue() {
        static PolyM::Queue queue;
        return &queue;
    }
} // namespace MiraiCP

namespace LibLoader{
    static void (*wake_loader_func_ptr)() = nullptr;

    void try_wake_loader(){
        if(wake_loader_func_ptr != nullptr){
            wake_loader_func_ptr();
        }
    }
}

namespace MiraiCP::PluginInterface {
    static auto &deleteQueue() {
        static std::deque<std::unique_ptr<PolyM::Msg>> queue;
        return queue;
    }

    PayLoadInfo _try_get_payload() {
        PayLoadInfo ret{nullptr, -1};
        auto queue_ptr = MiraiCP::getMsgQueue();
        auto msg = queue_ptr->tryGet();
        if (!msg) return ret;
        ret.payload = msg->getRawPayLoad();
        ret.payload_id = msg->getMsgId();
        deleteQueue().emplace_back(std::move(msg));
        return ret;
    }

    void _delete_one_msg() {
        deleteQueue().pop_front();
    }

    void _make_response(PolyM::MsgUID msg_uid, MiraiCPString msg) {
        auto queue_ptr = MiraiCP::getMsgQueue();
        queue_ptr->respondTo(msg_uid, PolyM::DataMsg<MiraiCPString>(MessageType::RET, std::move(msg)));
    }
} // namespace MiraiCP::PluginInterface


namespace MiraiCP {
    void register_plugin_argv(PluginArgv_C argv) {
        LibLoader::wake_loader_func_ptr = argv.wake_loader;
    }
} // namespace MiraiCP

extern "C" {
/// 插件开启入口
MIRAICP_EXPORT PluginMessageHandles_C FUNC_ENTRANCE(PluginArgv_C argv) {
    static_assert(std::is_same_v<decltype(&FUNC_ENTRANCE), LibLoader::plugin_entrance_func_ptr>);
    using namespace MiraiCP;
    PluginInterface::PluginMessageHandles ret{};

    Event::clear();
    getMsgQueue()->clear();
    register_plugin_argv(argv);
    // logger api is set

    try {
        Logger::logger.info("开始启动插件: " + MiraiCP::CPPPlugin::config.getId());
        enrollPlugin();
        // plugin == nullptr 无插件实例加载
        if (CPPPlugin::plugin != nullptr) {
            CPPPlugin::plugin->onEnable();
        }
    } catch (const MiraiCPExceptionBase &e) {
        try {
            std::cerr.flush();
            e.raise();
            Logger::logger.error("插件(id=" + CPPPlugin::config.getId() + ", name=" + CPPPlugin::config.name + ")启动失败");
        } catch (...) {}
        return ret.toC();
    } catch (const std::exception &e) {
        try {
            std::cerr.flush();
            Logger::logger.error(e.what());
            Logger::logger.error("插件(id=" + CPPPlugin::config.getId() + ", name=" + CPPPlugin::config.name + ")启动失败");
        } catch (...) {}
        return ret.toC();
    } catch (...) {
        try {
            std::cerr.flush();
            Logger::logger.error("插件(id=" + CPPPlugin::config.getId() + ", name=" + CPPPlugin::config.name + ")启动失败");
        } catch (...) {}
        return ret.toC();
    }

    ret.try_get_payload = PluginInterface::_try_get_payload;
    ret.delete_one_msg = PluginInterface::_delete_one_msg;
    ret.make_response = MiraiCP::PluginInterface::_make_response;
    return ret.toC();
}


/// 插件结束(也可能是暂时的disable)
MIRAICP_EXPORT int FUNC_EXIT() {
    static_assert(std::is_same_v<decltype(&FUNC_EXIT), LibLoader::plugin_func_ptr>);

    using namespace MiraiCP;

    MIRAICP_CRITICAL_NOEXCEPT_BLOCK(Logger::logger.info("开始禁用插件：" + MiraiCP::CPPPlugin::config.getId());)
    Event::clear();

    MIRAICP_CRITICAL_NOEXCEPT_BLOCK(if (CPPPlugin::plugin != nullptr) CPPPlugin::plugin->onDisable();)
    CPPPlugin::plugin.reset();

    return PLUGIN_NORMAL;
}


/// 消息解析分流
/// env != null, call from jni
/// 除了致命问题，该函数不会返回 ERROR
MIRAICP_EXPORT int FUNC_EVENT(const MiraiCP::MiraiCPString &c) {
    static_assert(std::is_same_v<decltype(&FUNC_EVENT), LibLoader::plugin_event_func_ptr>);
    using json = nlohmann::json;
    using namespace MiraiCP;
    std::string content = c;
    json j;

    try {
        j = json::parse(content);
    } catch (json::parse_error &e) {
        MIRAICP_CRITICAL_NOEXCEPT_BLOCK(Logger::logger.error("消息解析分流：格式化json错误");
                                        Logger::logger.error("For debug: " + content);
                                        Logger::logger.error(e.what());)
        return PLUGIN_NORMAL;
    }

    int type;

    try {
        if (!j.is_object() || !j.contains("eventId") || !j["eventId"].is_number()) {
            MIRAICP_CRITICAL_NOEXCEPT_BLOCK(Logger::logger.error("Json格式错误：没有eventId这一field或这一field不是数字类型");)
            return PLUGIN_NORMAL;
        }
        type = j["eventId"].get<int>();
        if (type != eventTypes::Command && Event::noRegistered(type)) return PLUGIN_NORMAL;
    } catch (...) {
        MIRAICP_CRITICAL_NOEXCEPT_BLOCK(Logger::logger.error("Json格式错误：解析json时遇到无法预料的异常");)
        return PLUGIN_NORMAL;
    }

    try {
        // core logic
        Event::incomingEvent(BaseEventData(std::move(j)), type);
    } catch (json::type_error &e) {
        MIRAICP_CRITICAL_NOEXCEPT_BLOCK(Logger::logger.error("json格式化异常,位置：FUNC_EVENT");
                                        Logger::logger.error(e.what());
                                        Logger::logger.error(content);)
        return PLUGIN_NORMAL;
    } catch (MiraiCPExceptionBase &e) {
        MIRAICP_CRITICAL_NOEXCEPT_BLOCK(Event::broadcast<MiraiCPExceptionEvent>(MiraiCPExceptionEvent(&e));
                                        e.raise();)
        return PLUGIN_NORMAL;
    } catch (const std::exception &e) {
        MIRAICP_CRITICAL_NOEXCEPT_BLOCK(Logger::logger.error(e.what());
                                        Logger::logger.error("info:", content);)
        return PLUGIN_NORMAL;
    } catch (...) {
        // 如果产生了无法处理的异常，直接退出插件，return 非0值
        // loader端将处理这个异常并直接unload绑定的插件
        // 如果存在其他线程，可能导致段错误
        return PLUGIN_ERROR;
    }
    return PLUGIN_NORMAL;
}

/// 获取 Plugin Info
/// 如果未正确定义，插件无法正确加载
/// dev: 该函数不可调用loader api；因为会在入口函数调用前先调用，loader api未初始化
MIRAICP_EXPORT const MiraiCP::PluginConfig *PLUGIN_INFO() {
    static_assert(std::is_same_v<decltype(&PLUGIN_INFO), LibLoader::plugin_info_func_ptr>);

    return &MiraiCP::CPPPlugin::config; // never nullptr
}
}
//结束对接JNI接口代码
