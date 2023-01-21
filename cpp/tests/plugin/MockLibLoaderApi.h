// Copyright (c) 2022. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_MOCKLIBLOADERAPI_H
#define MIRAICP_PRO_MOCKLIBLOADERAPI_H
#include "loaderApiInternal.h"
#include "redirectCout.h"
#include <gtest/gtest.h>
using namespace MiraiCP;
namespace LibLoader::LoaderApi {
    const interface_funcs *get_loader_apis();

    void set_loader_apis(const LibLoader::LoaderApi::interface_funcs *apis) noexcept;

    void reset_loader_apis();
} // namespace LibLoader::LoaderApi
class LoaderApiMock {
public:
    static MiraiCPString pluginOperation(const MiraiCPString &) {
        return "";
    }
    static void loggerInterface(const MiraiCPString &content, const MiraiCPString &name, long long id, int level) {}
    static MiraiCPString showAllPluginId() { return ""; }
    static void enablePluginById(const MiraiCPString &) {}
    static void disablePluginById(const MiraiCPString &) {}
    static void pushTask(void (*)()) {}
    static void pushTaskWithId(void (*)(size_t), size_t) {}
    static void timer(const MiraiCPString &, const MiraiCPString &, size_t) {}
    static void enableAllPlugins() {}
    static void disableAllPlugins() {}
    static void loadNewPlugin(const MiraiCPString &, bool) {}
    static void unloadPluginById(const MiraiCPString &) {}
    static void reloadPluginById(const MiraiCPString &) {}
    LibLoader::LoaderApi::interface_funcs a = {
            pluginOperation,
            loggerInterface,
            showAllPluginId,
            pushTask,
            pushTaskWithId,
            timer,
            enablePluginById,
            disablePluginById,
            enableAllPlugins,
            disableAllPlugins,
            loadNewPlugin,
            unloadPluginById,
            reloadPluginById,
    };
    LoaderApiMock() {
        MiraiCP::Redirector::start();
        LibLoader::LoaderApi::set_loader_apis(&a);
    }
    ~LoaderApiMock() {
        MiraiCP::Redirector::reset();
        LibLoader::LoaderApi::reset_loader_apis();
    }
};
#endif //MIRAICP_PRO_MOCKLIBLOADERAPI_H
