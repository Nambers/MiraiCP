// Copyright (c) 2023. Eritque arcus and contributors.
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

//
// Created by antares on 11/10/22.
//

#include "Schedule.h"
#include "CPPPlugin.h"
#include "Event.h"
#include "loaderApiInternal.h"


void MiraiCP::schedule(size_t time, const std::string &msg) {
    LibLoader::LoaderApi::timer(CPPPlugin::config.id, msg, time);
}

void MiraiCP::schedule(std::chrono::seconds sec, const std::string &msg) {
    schedule(sec.count(), msg);
}
