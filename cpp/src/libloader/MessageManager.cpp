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
#include "PluginManager.h"
#include <polym/Queue.hpp>

namespace LibLoader {
    MessageManager &MessageManager::get() {
        static MessageManager manager;
        return manager;
    }

    void MessageManager::tick() { // NOLINT(*-convert-member-functions-to-static)
        std::vector<MessageProxy> messages;
        PluginManager::Get().runOverPlugins([&messages](const Plugin &plugin) {
            auto msg = plugin.popMessage();
            if (msg) messages.emplace_back(std::move(msg));
        });
        for (auto &msg: messages) {
            processor->processMessage(msg);
        }
    }

    void MessageManager::init(MessageProcessor *in_processor) {
        processor = in_processor;
        in_processor->registerDefaultHandlers();
    }
} // namespace LibLoader
