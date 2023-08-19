//
// Created by Antares on 2023-8-19.
//

#ifndef MIRAICP_PRO_MESSAGEPROCESSOR_H
#define MIRAICP_PRO_MESSAGEPROCESSOR_H


#include "MsgDefine.h"
#include <polym/Msg.hpp>
#include <memory>
namespace LibLoader{
    class MessageProcessor {
        using MsgType = MiraiCP::MessageType::Type;
        typedef void (*message_handler)(std::unique_ptr<PolyM::Msg> msg);

        message_handler handlers[MsgType::MESSAGE_TYPE_COUNT];

    public:
        static MessageProcessor& get();

        void processMessage(MsgType type, std::unique_ptr<PolyM::Msg> msg);

        void registerHandler(MsgType type, message_handler handler);

        void registerDefaultHandlers();
    };
}

#endif //MIRAICP_PRO_MESSAGEPROCESSOR_H
