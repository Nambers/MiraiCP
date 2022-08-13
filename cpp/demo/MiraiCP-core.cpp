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
#include "core.h"
#include <iostream>

using namespace MiraiCP;

bool alive = true;

int main() {
    {
#ifdef _WIN32
        int sig = Core::loadCore(R"(C:\path\to\MiraiCP-core-<version>.jar)");
#else
        int sig = Core::loadCore("/path/to/MiraiCP-core-<version>.jar");
#endif
        if (sig != 0) exit(sig);
    }

    // begin MiraiCP code
    Bot tmp(123456789);

    try {
        // PAD/WATCH/PHONE/IPAD/MACOS
        tmp = Core::login(123456789, "password", false, "PHONE");
    } catch (...) {
        std::cerr << "Login failed\n";
        exit(1);
    }

    try {
        Logger::logger.info(tmp.nick());
    } catch (std::exception &e) {
        Logger::logger.error(e.what());
    }

    Event::registerEvent<PrivateMessageEvent>([&](const PrivateMessageEvent &a) {
        alive = false;
        Logger::logger.info("program end");
    });

    while (alive)
        ;

    Core::exitCore();
}
