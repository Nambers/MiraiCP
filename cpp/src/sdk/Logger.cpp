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

#include "Logger.h"
#include "CPPPlugin.h"
#include "MiraiCode.h"
#include "loaderApiInternal.h"


namespace MiraiCP {
    Logger Logger::logger;

    void Logger::log_interface(const std::string &content, int level) {
        handler_trigger(content, level);
        LibLoader::LoaderApi::loggerInterface(content, ("plugin/" + MiraiCP::CPPPlugin::config.getName()), -1, level);
    }

    void IdLogger::log_interface(const std::string &content, int level) {
        handler_trigger(content, level);
        LibLoader::LoaderApi::loggerInterface(content, "", static_cast<long long>(id), level);
    }

    IdLogger::IdLogger(QQID inId, Logger *l) : id(inId) {
        this->loggerhandler = l->loggerhandler;
    }

    void Logger_interface::handler_trigger(Logger_interface::string log, int level) {
        if (!loggerhandler) create_loggerhandler();
        if (loggerhandler->enable && loggerhandler->action) loggerhandler->action(std::move(log), level);
    }

    void Logger_interface::registerHandle(Logger_interface::Action action) {
        if (!this->loggerhandler) create_loggerhandler();
        this->loggerhandler->action = std::move(action);
    }

    void Logger_interface::setHandleState(bool state) {
        if (!this->loggerhandler) create_loggerhandler();
        this->loggerhandler->enable = state;
    }
} // namespace MiraiCP
