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

#ifndef MIRAICP_PRO_LOADERTASKQUEUE_H
#define MIRAICP_PRO_LOADERTASKQUEUE_H


#include <queue>
#include <string>


namespace LibLoader {
    enum struct LOADER_TASKS {
        ADD_THREAD,
        END_THREAD,
        ENABLE_ALL,
        DISABLE_ALL,
        LOAD_NEW_ACTIVATENOW,
        LOAD_NEW_DONTACTIVATE,
        UNLOAD,
        RELOAD,
        EXCEPTION_PLUGINEND,
        TASK_TYPES_COUNT [[maybe_unused]],
    };

    typedef std::pair<LOADER_TASKS, std::string> loadertask;

    extern std::queue<loadertask> loader_thread_task_queue;
    extern std::recursive_mutex task_mtx;
} // namespace LibLoader


#endif //MIRAICP_PRO_LOADERTASKQUEUE_H
