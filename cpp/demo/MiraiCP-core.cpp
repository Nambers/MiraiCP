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
#include "miraicp-core/core.h"
#include "MiraiCP.hpp"
using namespace MiraiCP;

int main() {
#ifdef _WIN32
    Core::loadCore(R"(D:\Git\mirai\MiraiCP\kotlin\core\build\libs\MiraiCP-core-2.11.0-M1-all.jar)");
#else
    Core::loadCore("/mnt/d/git/mirai/miraicp/kotlin/core/build/libs/MiraiCP-core-2.11.0-M1-all.jar");
#endif
    Bot tmp = Core::login(11, "aa", true);
    Logger::logger.info("aa");
    try {
        Logger::logger.info(tmp.nick());
    } catch (std::exception& e) {
        Logger::logger.error(e.what());
    }
    bool alive = true;
    Event::processor.registerEvent<GroupMessageEvent>([&alive](GroupMessageEvent a){
        Logger::logger.info("b");
        auto b = Group(a.group.id(), a.group.botid());
        Logger::logger.info("c");
        auto c = b[a.sender.id()];
        Logger::logger.info("d");
        c.changeNameCard(a.message.toMiraiCode());
        alive = false;
    });
    while (alive){};
    Core::exitCore();
}