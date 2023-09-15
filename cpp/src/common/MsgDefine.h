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

#ifndef MIRAICP_PRO_MSGDEFINE_H
#define MIRAICP_PRO_MSGDEFINE_H

#include "MiraiCPStringInternal.h"

#define GEN_MSG_BODY(_class_name)                                     \
    constexpr static int class_payload_id = MessageType::_class_name; \
    ~_class_name() override = default;
namespace MiraiCP {


    namespace MessageType {
        enum Type {
            OperationMessage,
            LoggerMessage,
            PluginIdMessage,
            PushTaskMessage,
            PushTaskWithIdMessage,
            TimerMessage,
            AdminMessage,
            MESSAGE_TYPE_COUNT,
            RET = MESSAGE_TYPE_COUNT,
        };
    }

    template<typename T, bool t_blocking = false>
    struct MessageTraits {
        constexpr static bool blocking = t_blocking;
        virtual int get_payload_class_id() {
            return static_payload_class_id();
        }
        virtual ~MessageTraits() = default;

        constexpr static int static_payload_class_id() {
            return T::class_payload_id;
        }
    };

    struct OperationMessage : public MessageTraits<OperationMessage, true> {
        MiraiCPStringview msg_string;
        GEN_MSG_BODY(OperationMessage)
    };

    struct LoggerMessage : public MessageTraits<LoggerMessage, false> {
        MiraiCPString msg_string;
        MiraiCPString name;
        long long msg_lid;
        int msg_level;
        GEN_MSG_BODY(LoggerMessage)
    };

    struct PluginIdMessage : public MessageTraits<PluginIdMessage, true> {
        GEN_MSG_BODY(PluginIdMessage)
    };

    struct PushTaskMessage : public MessageTraits<PushTaskMessage, false> {
        void (*task_func)();
        GEN_MSG_BODY(PushTaskMessage)
    };

    struct PushTaskWithIdMessage : public MessageTraits<PushTaskWithIdMessage, false> {
        void (*task_func_with_id)(size_t);
        size_t id;
        GEN_MSG_BODY(PushTaskWithIdMessage)
    };

    struct TimerMessage : public MessageTraits<TimerMessage, false> {
        MiraiCPString timer_id;
        MiraiCPString content;
        size_t sec;
        GEN_MSG_BODY(TimerMessage)
    };

    struct AdminMessage : public MessageTraits<AdminMessage, false> {
        enum CmdId{
            ENABLE_ONE,         // arg: plugin id
            DISABLE_ONE,        // arg: plugin id
            ENABLE_ALL,         // arg: none
            DISABLE_ALL,        // arg: none
            RELOAD_ONE,         // arg: plugin id
            UNLOAD_ONE,         // arg: plugin id
            LOAD_NEW,           // arg: path, bool immediate
        };
        int cmd_id;
        MiraiCPString cmd_arg;
        bool immediate;
        GEN_MSG_BODY(AdminMessage)
    };
} // namespace MiraiCP


#endif //MIRAICP_PRO_MSGDEFINE_H
