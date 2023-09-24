// Copyright (c) 2022 - 2023. Antares, Eritque arcus and contributors.
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

// 本文件的目的是分离出PluginListManager交互的部分函数实现，方便代码的阅读和维护
#include "MiraiCPMacros.h"
// -----------------------
#include "LoaderExceptions.h"
#include "LoaderLogger.h"
#include "Plugin.h"
#include "PluginConfig.h"
#include "PluginManager.h"
#include "ThreadIdentify.h"
#include "ThreadPool.h"
#include "commonTools.h"
#include "libOpen.h"
#include "loaderTools.h"
#if MIRAICP_WINDOWS
#include "WindowsMin.h"
#include <filesystem>
#include <utf8/utf8.h>
#endif

extern "C" {
void plugin_raw_API_try_wake_loader();
}

namespace LibLoader {
    class SymbolResolveException : public LoaderExceptionCRTP<SymbolResolveException> {
    public:
        enum SymbolType : uint8_t {
            Entrance,
            Exit,
            Event,
            Config
        };

    public:
        SymbolResolveException(const string &_pluginPath, SymbolType Type, string _filename, int _lineNum)
            : LoaderExceptionCRTP(_pluginPath + " 寻找符号失败，类型：" + SymbolTypeToString(Type), std::move(_filename), _lineNum) {}

        static string exceptionType() { return "SymbolResolveException"; }

    private:
        static string SymbolTypeToString(SymbolType Type) {
            switch (Type) {
                case Entrance:
                    return "插件入口";
                case Exit:
                    return "插件出口";
                case Event:
                    return "事件函数";
                case Config:
                    return "插件配置函数";
                default:
                    throw LoaderException("无法到达的代码", MIRAICP_EXCEPTION_WHERE);
            }
        }
    };

    class InfoNotCompleteException : public LoaderExceptionCRTP<InfoNotCompleteException> {
    public:
        InfoNotCompleteException(const string &_pluginPath, string _filename, int _lineNum)
            : LoaderExceptionCRTP("无法加载位于" + _pluginPath + "的插件：其插件信息填写不完整，请检查四项必填内容是否已经填写", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "InfoNotCompleteException"; }
    };

    class PluginHandleInvalidException : public LoaderExceptionCRTP<PluginHandleInvalidException> {
    public:
        PluginHandleInvalidException(const string &_pluginPath, string _filename, int _lineNum)
            : LoaderExceptionCRTP("位于" + _pluginPath + "的插件handle无效！请向开发者提供此错误信息", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "PluginHandleInvalidException"; }
    };

    class InvalidPayloadPointerException : public LoaderExceptionCRTP<InvalidPayloadPointerException> {
    public:
        InvalidPayloadPointerException(string _filename, int _lineNum)
            : LoaderExceptionCRTP("无效的payload指针", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "InvalidPayloadPointerException"; }
    };

    class InvalidThreadException : public LoaderExceptionCRTP<InvalidThreadException> {
    public:
        InvalidThreadException(string _filename, int _lineNum)
            : LoaderExceptionCRTP("函数只能在loader线程中调用", std::move(_filename), _lineNum) {}

        static string exceptionType() { return "InvalidThreadException"; }
    };

    ////////////////////////////////////
    /// 这部分是一些工具函数、对象
    MiraiCP::PluginInterface::PluginMessageHandles Plugin::callEntrance() const {
        return MiraiCP::PluginInterface::PluginMessageHandles::fromC(entrance({plugin_raw_API_try_wake_loader}));
    }

    inline bool checkPluginInfoValid(plugin_info_func_ptr info_ptr) {
        return info_ptr()->isValid();
    }

    /// 测试符号存在性，并返回event func的地址。return {nullptr, nullptr} 代表符号测试未通过，
    /// 不是一个可以使用的MiraiCP插件
    /// 不会调用其中任何一个函数
    PluginFuncAddrData Plugin::testSymbolExistance(plugin_handle handle, const std::string &path) {
        auto enter = (plugin_entrance_func_ptr) LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_ENTRANCE));
        if (!enter) throw SymbolResolveException(path, SymbolResolveException::Entrance, MIRAICP_EXCEPTION_WHERE);

        auto exiting = (plugin_func_ptr) LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_EXIT));
        if (!exiting) throw SymbolResolveException(path, SymbolResolveException::Exit, MIRAICP_EXCEPTION_WHERE);

        auto event_addr = (plugin_event_func_ptr) LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_EVENT));
        if (!event_addr) throw SymbolResolveException(path, SymbolResolveException::Event, MIRAICP_EXCEPTION_WHERE);

        auto pluginInfo = (plugin_info_func_ptr) LoaderApi::libSymbolLookup(handle, STRINGIFY(PLUGIN_INFO));
        if (!pluginInfo) throw SymbolResolveException(path, SymbolResolveException::Config, MIRAICP_EXCEPTION_WHERE);

        if (!checkPluginInfoValid(pluginInfo)) throw InfoNotCompleteException(path, MIRAICP_EXCEPTION_WHERE);

        return {enter, event_addr, exiting, pluginInfo};
    }

    ////////////////////////////////////

    Plugin::~Plugin() {
        if (handle != nullptr) {
            unloadPlugin();
        }
    }

    // 将插件加载进内存
    void Plugin::loadPlugin(bool alsoEnablePlugin) {
        std::unique_lock lk(_mtx);
        loadInternal(alsoEnablePlugin);
    }

    // enable的前提是该插件已经被加载进内存，且满足下列条件之一：
    // 1. 尚未执行entrance
    // 2. 最后一次执行过exit后，未执行过entrance
    void Plugin::enablePlugin() {
        std::unique_lock lk(_mtx);
        enableInternal();
    }

    // enable的前提是该插件已经被加载进内存，之前执行过entrance，且最后一次执行entrance后没有执行过exit
    void Plugin::disablePlugin() {
        std::unique_lock lk(_mtx);
        disableInternal();
    }

    // 释放插件对应的内存
    void Plugin::unloadPlugin() {
        std::unique_lock lk(_mtx);
        unloadInternal();
    }

    // 当发生错误时，强制释放插件对应内存
    void Plugin::unloadWhenException() {
        // cannot lock; the plugin maybe is in a deadlock state
        bool locked = _mtx.try_lock();
        MIRAICP_DEFER(if (locked) _mtx.unlock(););
        unloadWhenExceptionInternal();
    }

    plugin_handle Plugin::loadNewPlugin(const std::string &path) noexcept {
        try {
            auto actualPath = path; // NOLINT(performance-unnecessary-copy-initialization)
#if MIRAICP_WINDOWS
            auto from = std::filesystem::path(path);
            if (!exists(from) || !from.has_extension()) {
                logger.error("path don't exist or invalid " + path);
                return nullptr;
            }
            auto actualFile = std::filesystem::temp_directory_path().append(
                    std::to_string(std::hash<std::string>()(path)) + ".dll");
            actualPath = actualFile.string();
            // try to del it anyway
            // works around with https://github.com/msys2/MSYS2-packages/issues/1937
            std::error_code ec;
            std::filesystem::remove(actualFile, ec);
            ec.clear();
            std::filesystem::copy(path, actualFile, std::filesystem::copy_options::overwrite_existing, ec);
            if (ec) {
                logger.error("无法复制dll(" + path + ")到缓存目录(" + actualPath + "), 原因: " + ec.message() + "，请检查是否有别的进程在占用该缓存文件！");
                return nullptr;
            }
#endif
            return LoaderApi::libOpen(actualPath);
        } catch (...) {}
        return nullptr;
    }

    ////////////////////////////////////

    void Plugin::enableInternal() {
        if (handle == nullptr) {
            throw PluginNotLoadedException(path, MIRAICP_EXCEPTION_WHERE);
        }

        if (enabled) {
            assert(infoFunc);
            throw PluginAlreadyEnabledException(_getId(), MIRAICP_EXCEPTION_WHERE);
        }

        if (infoFunc()->getMVersion() != MiraiCP::MiraiCPVersion) {
            LibLoader::logger.warning("MiraiCP依赖库版本(" + infoFunc()->getMVersion() + ")和libLoader版本(" + MiraiCP::MiraiCPVersion + ")不一致, 建议更新到最新");
        }

        _enable();
        updateTimeStamp();

        Antares::pool->push_task([this] {
            std::shared_lock lk(_mtx);

            if (nullptr == entrance) {
                // 并发过快，插件在启用前被卸载了。直接return
                return;
            }

            MiraiCP::PluginInterface::PluginMessageHandles ret{};

            {
                ThreadIdentify::setThreadWorkingName(_getId());
                MIRAICP_DEFER(ThreadIdentify::unsetThreadWorkingName(););
                try {
                    ret = callEntrance();
                } catch (...) {
                }
            }

            if (!ret.try_get_payload) {
                logger.error("插件：" + _getId() + "启用时出现错误");
            }

            // reinterpret cast to message_queue
            message_queue = ret;
        });
    }

    std::shared_ptr<std::future<void>> Plugin::disableInternal(bool lockedAndWait) {
        if (exit == nullptr) return nullptr;
        _disable();
        updateTimeStamp();

        return std::make_shared<std::future<void>>(Antares::pool->submit([this, lockedAndWait] {
            std::shared_lock lk(_mtx, std::defer_lock);
            if (!lockedAndWait) lk.lock();

            if (!checkValid() || exit == nullptr) {
                // 任务分派过慢
                return;
            }

            message_queue.clear();
            int ret = -1;

            {
                ThreadIdentify::setThreadWorkingName(_getId());
                MIRAICP_DEFER(ThreadIdentify::unsetThreadWorkingName(););
                try {
                    ret = exit();
                } catch (...) {}
            }

            assert(infoFunc);

            if (ret != 0) logger.error("插件：" + _getId() + "退出时出现错误");
        }));
    }

    void Plugin::unloadInternal() {
        if (nullptr == handle) {
            // DON'T CALL getIdSafe() if plugin is not valid!!!
            logger.warning("plugin at path: " + path + " is already unloaded");
            return;
        }

        // first disable it
        if (enabled) {
            auto fu = disableInternal(true);
            if (fu) {
                fu->wait();
            }
        }

        // then unload it
        LoaderApi::libClose(handle);

        _unload();
    }

    void Plugin::loadInternal(bool alsoEnablePlugin) {
        if (handle != nullptr) {
            assert(infoFunc);
            throw PluginAlreadyLoadedException(_getId(), MIRAICP_EXCEPTION_WHERE);
        }

        handle = loadNewPlugin(path); // nothrow

        if (handle == nullptr) {
            logger.error(LoaderApi::libError());
            throw PluginLoadException(path, MIRAICP_EXCEPTION_WHERE);
        }

        auto symbols = testSymbolExistance(handle, path); // throws

        _load(handle, symbols);
        _disable();

        if (alsoEnablePlugin) enableInternal();
    }

    void Plugin::unloadWhenExceptionInternal() {
        if (nullptr == handle) {
            logger.warning("plugin at location " + path + " is already unloaded");
            return;
        }

        // first detach the thread
        // unload it directly since the thread is already down by exception
        LoaderApi::libClose(handle);

        _disable();
        _unload();
    }

    void Plugin::pushEvent_worker(const MiraiCP::MiraiCPString &event) {
        std::shared_lock lk(_mtx);
        if (!enabled) {
            // 线程池任务分派过慢。插件此时已经被卸载，直接return
            return;
        }

        int ret = -1;

        {
            ThreadIdentify::setThreadWorkingName(_getId());
            MIRAICP_DEFER(ThreadIdentify::unsetThreadWorkingName(););
            try {
                ret = eventFunc(event);
            } catch (...) {
            }
        }

        assert(infoFunc);

        if (ret != 0) {
            logger.error("插件：" + _getId() + "出现严重错误");
            logger.error(event);
        }
    }

    ////////////////////////////////////


    /// 仅加载并测试所有plugin的符号是否存在
    /// 仅被registerAllPlugin调用，即，在kt（主）线程Verify中会被调用一次
    /// 作用是将所有plugin加入plugin列表
    /// 这一过程必须是原子的
    void Plugin::loadsAll(const std::vector<std::string> &paths, const std::vector<PluginAuthority> &authorities) noexcept {
        // std::lock_guard lk(PluginManager::getLock());

        for (size_t i = 0; i < paths.size(); ++i) {
            auto &path = paths[i];
            auto authority = authorities[i];

            auto newPlugin = std::make_shared<Plugin>(path);

            auto timestamp = std::chrono::system_clock::now();

            try {
                newPlugin->loadPlugin(false);
            } catch (LoaderBaseException &e) {
                e.raise();
                continue;
            }

            auto timestamp2 = std::chrono::system_clock::now();

            auto timedelta = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp2 - timestamp).count();
            logger.info("loaded plugin " + path + " in " + std::to_string(timedelta) + "ms");

            newPlugin->authority = authority;

            try {
                PluginManager::Get().addNewPlugin(newPlugin);
            } catch (LoaderBaseException &e) {
                e.raise();
            }
        }
    }

    /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用且仅调用一次
    /// 实际的入口，id_plugin_map 必须在这里初始化
    void Plugin::registerAllPlugin(const std::string &cfgPath) noexcept {
        // 获取cfg文件路径并读取
        nlohmann::json j = readJsonFile(cfgPath);

        // 读取全部path
        auto [paths, authorities] = collect_plugins(cfgPath, std::move(j));

        if (paths.empty()) {
            logger.warning("no plugin to load");
            return;
        }

        loadsAll(paths, authorities);
    }

    std::string Plugin::getIdSafe() const {
        std::shared_lock lk(_mtx);
        if (!checkValid() || infoFunc == nullptr) {
            throw PluginHandleInvalidException(path, MIRAICP_EXCEPTION_WHERE);
        }

        return _getId();
    }

    void formatInternal(const MiraiCP::PluginConfig &plugin_config, std::vector<std::string> &out, size_t (&charNum)[4]) {
        CASStrong(charNum[0], strlen(plugin_config.id) + 1);
        CASStrong(charNum[1], strlen(plugin_config.name) + 1);
        CASStrong(charNum[2], strlen(plugin_config.author) + 1);
        CASStrong(charNum[3], strlen(plugin_config.description) + 1);
        out.emplace_back(plugin_config.id);
        out.emplace_back(plugin_config.name);
        out.emplace_back(plugin_config.author);
        out.emplace_back(plugin_config.description);
        out.emplace_back("\n");
    }

    void Plugin::formatTo(std::vector<std::string> &out, size_t (&charNum)[4]) {
        std::shared_lock lk(_mtx);
        if (!checkLoaded()) formatInternal(MiraiCP::PluginConfig{"(unknown)",
                                                                 path.c_str(),
                                                                 "(unknown)",
                                                                 "(unknown)",
                                                                 "(unknown)",
                                                                 "(unknown)",
                                                                 "(unknown)"},
                                           out, charNum);
        formatInternal(*infoFunc(), out, charNum);
    }

    void Plugin::forceCallExit() {
        std::shared_lock lk(_mtx);
        if (!exit) return;
        try {
            exit();
        } catch (...) {} // do not leak any exception
    }

    void Plugin::updateTimeStamp() {
        timestamp = std::chrono::system_clock::now();
    }

    Plugin::timepoint Plugin::getTimeStamp() const {
        std::shared_lock lk(_mtx);
        return timestamp;
    }

    MessageProxy Plugin::popMessage() const {
        if (!message_queue.try_get_payload) return MessageProxy({}, nullptr);
        auto payload = tryGetPayload();
        if (!payload.payload) return MessageProxy({}, nullptr);
        return {payload, shared_from_this()};
    }

    void Plugin::delete_message() const {
        if (message_queue.delete_one_msg) message_queue.delete_one_msg();
    }

    MiraiCP::PluginInterface::PayLoadInfo Plugin::tryGetPayload() const {
        if (!message_queue.try_get_payload) {
            logger.error("tryGetPayload: try_get_payload is nullptr");
            throw InvalidPayloadPointerException(MIRAICP_EXCEPTION_WHERE);
        }
        // check thread
        if (!ThreadIdentify::isMeLoaderThread()) {
            logger.error("tryGetPayload: you should call this function in loader thread!!!");
            throw InvalidThreadException(MIRAICP_EXCEPTION_WHERE);
        }
        return message_queue.try_get_payload();
    }

    void Plugin::makeResponse(PolyM::MsgUID uid, MiraiCP::MiraiCPString s) const {
        if (!message_queue.make_response) {
            logger.error("makeResponse: make_response is nullptr");
            throw InvalidPayloadPointerException(MIRAICP_EXCEPTION_WHERE);
        }
        // check thread
        if (!ThreadIdentify::isMeLoaderThread()) {
            logger.error("makeResponse: you should call this function in loader thread!!!");
            throw InvalidThreadException(MIRAICP_EXCEPTION_WHERE);
        }
        message_queue.make_response(uid, std::move(s));
    }

    void registerAllPlugin(const std::string &cfgPath) noexcept {
        Plugin::registerAllPlugin(cfgPath);
    }
} // namespace LibLoader
