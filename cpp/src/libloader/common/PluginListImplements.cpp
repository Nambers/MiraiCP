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

/// 本文件的目的是分离出PluginListManager交互的部分函数实现，方便代码的阅读和维护

#include "PluginListImplements.h"
#include "LoaderExceptions.h"
#include "LoaderLogger.h"
#include "MiraiCPMacros.h"
#include "PluginConfig.h"
#include "PluginListManager.h"
#include "ThreadController.h"
#include "libOpen.h"
#include "loaderTools.h"


#if MIRAICP_WINDOWS
#include "utf8.h"
#include <filesystem>
#include <windows.h>
// https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString() {
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return {}; //No error message has been recorded
    }

    LPWSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &messageBuffer, 0, nullptr);

    //Copy the error message into a std::string.
    std::wstring message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    // Translate to utf8 encoded std::string
    std::string ans;
    utf8::utf16to8(message.begin(), message.end(), std::back_inserter(ans));

    return ans;
}
#endif

namespace LibLoader {
    struct PluginAddrInfo {
        plugin_event_func_ptr event_func;
        plugin_info_func_ptr pluginAddr;
    };

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

    ////////////////////////////////////
    /// 这部分是一些工具函数、对象

    /// 该对象的地址将被用于传递给MiraiCP插件
    constexpr static LoaderApi::interface_funcs interfaces = LoaderApi::collect_interface_functions(true);

    constexpr static LoaderApi::interface_funcs normal_interfaces = LoaderApi::collect_interface_functions(false);

    void callEntranceFuncAdmin(plugin_entrance_func_ptr func) {
        func(interfaces);
    }

    void callEntranceFuncNormal(plugin_entrance_func_ptr func) {
        func(normal_interfaces);
    }

    inline bool checkPluginInfoValid(plugin_info_func_ptr info_ptr) {
        return info_ptr().isValid();
    }

    /// 测试符号存在性，并返回event func的地址。return {nullptr, nullptr} 代表符号测试未通过，
    /// 不是一个可以使用的MiraiCP插件
    /// 不会调用其中任何一个函数
    PluginAddrInfo testSymbolExistance(plugin_handle handle, const std::string &path) {
        auto symbol = LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_ENTRANCE));
        if (!symbol) throw SymbolResolveException(path, SymbolResolveException::Entrance, MIRAICP_EXCEPTION_WHERE);

        symbol = LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_EXIT));
        if (!symbol) throw SymbolResolveException(path, SymbolResolveException::Exit, MIRAICP_EXCEPTION_WHERE);

        auto event_addr = (plugin_event_func_ptr) LoaderApi::libSymbolLookup(handle, STRINGIFY(FUNC_EVENT));
        if (!event_addr) throw SymbolResolveException(path, SymbolResolveException::Event, MIRAICP_EXCEPTION_WHERE);

        auto pluginInfo = (plugin_info_func_ptr) LoaderApi::libSymbolLookup(handle, STRINGIFY(PLUGIN_INFO));
        if (!pluginInfo) throw SymbolResolveException(path, SymbolResolveException::Config, MIRAICP_EXCEPTION_WHERE);

        if (!checkPluginInfoValid(pluginInfo)) throw InfoNotCompleteException(path, MIRAICP_EXCEPTION_WHERE);

        return {event_addr, pluginInfo};
    }

    ////////////////////////////////////

    // enable的前提是该插件已经被加载进内存，但尚未执行初始化函数，或者执行了Exit
    // 不涉及插件列表的修改；不会修改插件权限
    void enable_plugin(LoaderPluginConfig &plugin) {
        if (plugin.enabled) {
            throw PluginAlreadyEnabledException(plugin.getId(), MIRAICP_EXCEPTION_WHERE);
        }

        if (plugin.handle == nullptr) {
            throw PluginNotLoadedException(plugin.path, MIRAICP_EXCEPTION_WHERE);
        }

        auto func = (plugin_entrance_func_ptr) LoaderApi::libSymbolLookup(plugin.handle, STRINGIFY(FUNC_ENTRANCE));
        if (plugin.config().getMVersion() != MiraiCP::MiraiCPVersion) {
            LibLoader::logger.warning("MiraiCP依赖库版本(" + plugin.config().getMVersion() + ")和libLoader版本(" + MiraiCP::MiraiCPVersion + ")不一致, 建议更新到最新");
        }

        plugin.enable();

        // dev:注意：plugin虽然被分配在一个固定地址（map中的值是shared_ptr，内部的 LoaderPluginConfig 不会被复制），
        // 但这里引用plugin地址的话，当shared_ptr在某个线程中被释放掉，还是可能会产生段错误
        // 因为我们无法保证getController().addThread()一定会把提交的这个函数在plugin被销毁前处理掉，
        // 这里按引用捕获plugin可能导致段错误！
        // 请务必全部按值捕获
        ThreadController::getController().addThread(plugin.getId(), [=, authority = plugin.authority] {
            if (authority == PLUGIN_AUTHORITY_ADMIN)
                callEntranceFuncAdmin(func);
            else
                callEntranceFuncNormal(func);
        });
    }

    // 不涉及插件列表的修改；不会修改插件权限
    void disable_plugin(LoaderPluginConfig &plugin) {
        auto disable_func = get_plugin_disable_ptr(plugin);
        if (disable_func == nullptr) return;
        ThreadController::getController().callThreadEnd(plugin.getId(), disable_func);
        plugin.disable();
    }

    plugin_func_ptr get_plugin_disable_ptr(LoaderPluginConfig &plugin) {
        if (nullptr == plugin.handle) {
            logger.error("plugin at location " + plugin.path + " is not loaded!");
            return nullptr;
        }
        if (!plugin.enabled) {
            logger.warning("plugin " + plugin.getId() + " is already disabled");
            return nullptr;
        }

        auto ptr = (plugin_func_ptr) LoaderApi::libSymbolLookup(plugin.handle, STRINGIFY(FUNC_EXIT));
        if (ptr == nullptr)
            throw IllegalStateException(MIRAICP_EXCEPTION_WHERE);

        return ptr;
    }

    inline plugin_handle loadPluginInternal(LoaderPluginConfig &plugin) noexcept {
        auto actualPath = plugin.path;
#if MIRAICP_WINDOWS
        auto from = std::filesystem::path(plugin.path);
        if (!exists(from) || !from.has_extension()) {
            logger.error("path don't exist or invalid " + plugin.path);
            return nullptr;
        }
        auto actualFile = std::filesystem::temp_directory_path().append(
                std::to_string(std::hash<std::string>()(plugin.path)) + ".dll");
        actualPath = actualFile.string();
        // try to del it anyway
        // works around with https://github.com/msys2/MSYS2-packages/issues/1937
        std::error_code ec;
        std::filesystem::remove(actualFile, ec);
        ec.clear();
        std::filesystem::copy(plugin.path, actualFile, std::filesystem::copy_options::overwrite_existing);
        if (ec) {
            logger.error("无法复制dll(" + plugin.path + ")到缓存目录(" + actualPath + "), 原因: " + ec.message() + "，请检查是否有别的进程在占用该缓存文件！");
            return nullptr;
        }
#endif
        return LoaderApi::libOpen(actualPath);
    }

    // 不涉及插件列表的修改；不会修改插件权限
    void load_plugin(LoaderPluginConfig &plugin, bool alsoEnablePlugin) {
        if (plugin.handle != nullptr) {
            throw PluginAlreadyLoadedException(plugin.getId(), MIRAICP_EXCEPTION_WHERE);
        }

        auto handle = loadPluginInternal(plugin);
        if (handle == nullptr) {
#if MIRAICP_WINDOWS
            logger.error(GetLastErrorAsString());
#endif
            throw PluginLoadException(plugin.path, MIRAICP_EXCEPTION_WHERE);
        }

        auto symbols = testSymbolExistance(handle, plugin.path);

        plugin.load(handle, symbols.event_func, symbols.pluginAddr);
        plugin.disable();

        if (alsoEnablePlugin) enable_plugin(plugin);
    }

    // unload将释放插件的内存
    // 不涉及插件列表的修改；不会修改插件权限
    void unload_plugin(LoaderPluginConfig &plugin) {
        if (nullptr == plugin.handle) {
            // DON'T CALL getId() if plugin is disabled!!!
            logger.warning("plugin at path: " + plugin.path + " is already unloaded");
            return;
        }

        // first disable it
        if (plugin.enabled) {
            disable_plugin(plugin);
        }

        // then unload it
        LoaderApi::libClose(plugin.handle);

        plugin.unload();
    }

    void unload_when_exception(LoaderPluginConfig &plugin) {
        if (nullptr == plugin.handle) {
            logger.warning("plugin at location " + plugin.path + " is already unloaded");
            return;
        }

        // first detach the thread
        ThreadController::getController().endThread(plugin.getId());
        // unload it directly since the thread is already down by exception
        LoaderApi::libClose(plugin.handle);

        plugin.disable();
        plugin.unload();
    }
    ////////////////////////////////////

    void loadNewPluginByPath(const std::string &_path, bool activateNow) {
        LoaderPluginConfig cfg{_path};

        load_plugin(cfg, activateNow);

        PluginListManager::addNewPlugin(std::move(cfg));
    }

    /// 仅加载并测试所有plugin的符号是否存在
    /// 仅被registerAllPlugin调用，即，在kt（主）线程Verify中会被调用一次
    /// 作用是将所有plugin加入plugin列表
    /// 这一过程必须是原子的
    inline void loadsAll(const std::vector<std::string> &paths, const std::vector<PluginAuthority> &authorities) noexcept {
        std::lock_guard lk(PluginListManager::getLock());

        for (int i = 0; i < paths.size(); ++i) {
            auto &path = paths[i];
            auto authority = authorities[i];

            LoaderPluginConfig cfg{path};

            auto timestamp = std::chrono::system_clock::now();

            try {
                load_plugin(cfg, false);
            } catch (LoaderBaseException &e) {
                e.raise();
                continue;
            }

            auto timestamp2 = std::chrono::system_clock::now();

            auto timedelta = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp2 - timestamp).count();
            logger.info("loaded plugin " + path + " in " + std::to_string(timedelta) + "ms");

            cfg.authority = authority;

            try {
                PluginListManager::addNewPlugin(std::move(cfg));
            } catch (LoaderBaseException &e) {
                e.raise();
            }
        }
    }

    /// 激活目前所有存储的插件。在Verify步骤中被kt（主）线程调用且仅调用一次
    /// 实际的入口，id_plugin_list 必须在这里初始化
    void registerAllPlugin(const std::string &cfgPath) noexcept {
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
} // namespace LibLoader
