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

#ifndef MIRAICP_PRO_COMMONTYPES_H
#define MIRAICP_PRO_COMMONTYPES_H
// don't create cpp for this header


#include "MiraiCPMacros.h"
// -----------------------
#include "PluginConfig.h"
#include "loaderApiInternal.h"

namespace MiraiCP::PluginInterface {
    struct PayLoadInfo {
        void *payload;
        int payload_id;
    };
    typedef MiraiCP::PluginInterface::PayLoadInfo (*message_queue_handle)();
    typedef void (*message_delete_handle)();

    struct PluginMessageHandles{
        message_queue_handle try_get_payload{};
        message_delete_handle delete_one_msg{};

        void clear(){
            try_get_payload = nullptr;
            delete_one_msg = nullptr;
        }
    };
} // namespace MiraiCP::PluginInterface

namespace LibLoader {
    typedef void *plugin_handle;

    /// @see @macro FUNC_ENTRANCE
    typedef MiraiCP::PluginInterface::PluginMessageHandles (*plugin_entrance_func_ptr)(const LoaderApi::interface_funcs &);
    /// @see @macro FUNC_EVENT
    typedef int (*plugin_event_func_ptr)(const MiraiCP::MiraiCPString &);
    /// @see @macro FUNC_EXIT
    typedef int (*plugin_func_ptr)();
    /// @see @macro PLUGIN_INFO
    typedef const MiraiCP::PluginConfig *(*plugin_info_func_ptr)();
} // namespace LibLoader


#endif //MIRAICP_PRO_COMMONTYPES_H
