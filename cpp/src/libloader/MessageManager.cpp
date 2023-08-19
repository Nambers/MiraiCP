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

#include "MessageManager.h"
#include "MessageProcessor.h"
#include "Plugin.h"
#include "PluginListManager.h"
#include <polym/Queue.hpp>

namespace LibLoader {
    MessageManager &MessageManager::get() {
        static MessageManager manager;
        return manager;
    }

    void MessageManager::tick() {
        for (auto &msg: collectAllMessages()) {
            auto type_id = static_cast<MiraiCP::MessageType::Type>(msg->getMsgId());
            processor->processMessage(type_id, std::move(msg));
        }
    }

    void MessageManager::init(MessageProcessor *in_processor) {
        processor = in_processor;
        in_processor->registerDefaultHandlers();
    }

    std::vector<std::unique_ptr<PolyM::Msg>> MessageManager::collectAllMessages() {
        // run over all plugins to get all messages
        std::vector<std::unique_ptr<PolyM::Msg>> ret;
        PluginListManager::run_over_pluginlist([&ret](const Plugin &plugin) {
            plugin.popMessageTo(ret);
        });
        return ret;
    }
} // namespace LibLoader
