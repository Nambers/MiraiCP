//
// Created by Antares on 2023-8-19.
//

#ifndef MIRAICP_PRO_MESSAGEPROCESSOR_H
#define MIRAICP_PRO_MESSAGEPROCESSOR_H


#include "MsgDefine.h"
#include "commonTypes.h"
#include <memory>
#include <polym/Msg.hpp>
#include <utility>

namespace LibLoader {
    class Plugin;
    class MessageProxy {
        MiraiCP::PluginInterface::PayLoadInfo payload;
        std::shared_ptr<const Plugin> plugin;

    public:
        MessageProxy(MiraiCP::PluginInterface::PayLoadInfo payload, std::shared_ptr<const Plugin> plugin);

        ~MessageProxy();

        [[nodiscard]] auto getPayload() const { return payload; }
    };

    class MessageProcessor {
        using MsgType = MiraiCP::MessageType::Type;
        typedef void (*message_handler)(std::unique_ptr<PolyM::Msg> msg);

        message_handler handlers[MsgType::MESSAGE_TYPE_COUNT];

    public:
        static MessageProcessor &get();

        void processMessage(MsgType type, std::unique_ptr<PolyM::Msg> msg);

        void registerHandler(MsgType type, message_handler handler);

        void registerDefaultHandlers();
    };
} // namespace LibLoader

#endif //MIRAICP_PRO_MESSAGEPROCESSOR_H
