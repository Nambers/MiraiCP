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

#include "utils.h"
#include "Event.h"
#include "Exception.h"
#include "KtOperation.h"
#include "Logger.h"
#include "loaderApi.h"


// 开始对接libloader接口代码

using json = nlohmann::json;

namespace LibLoader::LoaderApi {
    const interface_funcs *get_loader_apis();

    void set_loader_apis(const LibLoader::LoaderApi::interface_funcs *apis);

    void reset_loader_apis();
} // namespace LibLoader::LoaderApi

extern "C" {
/// 插件开启入口
void FUNC_ENTRANCE(const LibLoader::LoaderApi::interface_funcs &funcs) {
    static_assert(std::is_same_v<decltype(&FUNC_ENTRANCE), LibLoader::plugin_entrance_func_ptr>);
    using namespace MiraiCP;
    MiraiCP::Event::clear();

    LibLoader::LoaderApi::set_loader_apis(&funcs);

    assert(LibLoader::LoaderApi::get_loader_apis() != nullptr);

    try {
        enrollPlugin();
        // plugin == nullptr 无插件实例加载
        if (CPPPlugin::plugin != nullptr)
            CPPPlugin::plugin->onEnable();
    } catch (const MiraiCPExceptionBase &e) {
        e.raise();
        Logger::logger.info("插件" + MiraiCP::CPPPlugin::config.id + "加载失败");
        FUNC_EXIT();
    } catch (...) {
    }
}


/// 插件结束(也可能是暂时的disable)
void FUNC_EXIT() {
    static_assert(std::is_same_v<decltype(&FUNC_EXIT), LibLoader::plugin_func_ptr>);

    using namespace MiraiCP;

    if (CPPPlugin::plugin == nullptr) return;

    CPPPlugin::plugin->onDisable();
    LibLoader::LoaderApi::reset_loader_apis();
    CPPPlugin::plugin.reset();
}


/// 消息解析分流
/// env != null, call from jni
void FUNC_EVENT(std::string content) {
    static_assert(std::is_same_v<decltype(&FUNC_EVENT), LibLoader::plugin_event_func_ptr>);

    using namespace MiraiCP;
    json j;
    try {
        j = json::parse(content);
    } catch (json::parse_error &e) {
        Logger::logger.error("消息解析分流：格式化json错误");
        Logger::logger.error("For debug: " + content);
        Logger::logger.error(e.what());
        return;
    }
    int type = j["type"].get<int>();

    if (eventTypes(type) != eventTypes::Command && Event::noRegistered(type)) return;
    try {
        Event::incomingEvent(std::move(j), type);
    } catch (json::type_error &e) {
        Logger::logger.error("json格式化异常,位置C-Handle");
        Logger::logger.error(e.what());
        Logger::logger.error("info:", content);
    } catch (MiraiCPExceptionBase &e) {
        Event::broadcast<MiraiCPExceptionEvent>(MiraiCPExceptionEvent(&e));
        e.raise();
    } catch (const std::exception &e) {
        // 这里如果不catch全部exception就会带崩jvm
        Logger::logger.error(e.what());
        Logger::logger.error("info:", content);
    }
}

/// 获取 Plugin Info
/// 如果未正确定义，插件无法正确加载
/// 该函数不可调用loader api；因为会在入口函数调用前先调用，loader api未初始化
const MiraiCP::PluginConfig &PLUGIN_INFO() {
    static_assert(std::is_same_v<decltype(&PLUGIN_INFO), LibLoader::plugin_info_func_ptr>);

    if (MiraiCP::CPPPlugin::config.id.empty())
        throw std::exception();

    return MiraiCP::CPPPlugin::config;
}
}
//结束对接JNI接口代码
