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


namespace MiraiCP {
    namespace MessageType{
        enum Type {
            OperationMessage,
            LoggerMessage,
            PluginIdMessage,
            PushTaskMessage,
            PushTaskWithIdMessage,
            TimerMessage,
            MESSAGE_TYPE_COUNT,
        };
    }


    template<bool t_blocking = false>
    class MessageTraits {
        static inline bool blocking = t_blocking;
    };

    //    struct OperationMessage: public MessageTraits<true>{
    //        MiraiCPStringview msg_string_view;
    //        inline static int id = 0;
    //    };
    //
    //    struct LoggerMessage{
    //        MiraiCPStringview msg_string;
    //        MiraiCPStringview name;
    //        long long msg_lid;
    //        int msg_level;
    //        inline static int id = 1;
    //    };
} // namespace MiraiCP


#endif //MIRAICP_PRO_MSGDEFINE_H
