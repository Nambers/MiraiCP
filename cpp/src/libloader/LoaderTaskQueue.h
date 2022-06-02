//
// Created by antares on 6/3/22.
//


#ifndef MIRAICP_PRO_LOADERTASKQUEUE_H
#define MIRAICP_PRO_LOADERTASKQUEUE_H


#include <string>


namespace LibLoader {
    enum struct LOADER_TASKS {
        ADD_THREAD,
        END_THREAD,
        ENABLE_ALL,
        DISABLE_ALL,
        LOAD_NEW_ACTIVATENOW,
        LOAD_NEW_DONTACTIVATE,
        TASK_TYPES_COUNT [[maybe_unused]],
    };

    typedef std::pair<LOADER_TASKS, std::string> loadertask;
} // namespace LibLoader


#endif //MIRAICP_PRO_LOADERTASKQUEUE_H
