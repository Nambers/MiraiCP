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

#include "Command.h"
#include "CPPPlugin.h"
#include "KtOperation.h"
#include "Logger.h"
#include <mutex>


namespace MiraiCP {
    CommandManager CommandManager::commandManager = CommandManager();

    bool CommandManager::internalRegister(IRawCommand *inPtr) {
        static std::mutex mtx;

        auto cfg = inPtr->config();

        nlohmann::json j;
        j["pluginId"] = CPPPlugin::config.id;
        j["usage"] = cfg.usage;
        j["primaryName"] = cfg.primaryName;
        j["secondName"] = cfg.secondNames;
        j["description"] = cfg.description;
        j["override"] = cfg.overrideOrigin;
        j["preFixOption"] = cfg.preFixOption;
        //
        std::lock_guard lk(mtx);
        j["bindId"] = commandList.size();

        std::string re = KtOperation::ktOperation(KtOperation::CommandReg, j);
        if (re != "true") {
            Logger::logger.error("指令注册失败，返回值：" + re);
            return false;
        }
        commandList.emplace_back(inPtr);
        return true;
    }

    CommandManager::~CommandManager() {
        std::vector<IRawCommand *> arr = std::move(commandList);
        for (auto ptr: arr) {
            delete ptr;
        }
    }
} // namespace MiraiCP
