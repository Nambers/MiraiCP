//
// Created by Antares on 2023-8-6.
//

#ifndef MIRAICP_PRO_MSGDEFINE_H
#define MIRAICP_PRO_MSGDEFINE_H

#include "MiraiCPStringInternal.h"


namespace MiraiCP {
    enum class MessageType {
        OperationMessage,
        LoggerMessage,
        PluginIdMessage,
        PushTaskMessage,
        PushTaskWithIdMessage,
        TimerMessage,
    };

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
