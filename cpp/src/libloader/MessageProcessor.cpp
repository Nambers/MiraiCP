//
// Created by Antares on 2023-8-19.
//

#include "MessageProcessor.h"
#include "LoaderLogger.h"
#include "commonTools.h"

namespace LibLoader{

    MessageProcessor &MessageProcessor::get() {
        static MessageProcessor processor;
        return processor;
    }

    void MessageProcessor::processMessage(MsgType type, std::unique_ptr<PolyM::Msg> msg) {
        bool invalid = true;
        MIRAICP_DEFER(if (invalid) logger.error("MessageProcessor::processMessage: invalid message type:" + std::to_string(type)););

        if (type < 0 || type >= MsgType::MESSAGE_TYPE_COUNT) return;

        auto handler = handlers[type];
        if (!handler) return;

        invalid = false;
        handler(std::move(msg));
    }

    void MessageProcessor::registerHandler(MsgType type, message_handler handler) {
        if (type < 0 || type >= MsgType::MESSAGE_TYPE_COUNT) {
            logger.error("MessageProcessor::processMessage: invalid message type:" + std::to_string(type));
            return;
        }

        auto old_handler = handlers[type];
        if (old_handler) {
            logger.warning("MessageProcessor::processMessage: message type:" + std::to_string(type) + " has been registered");
            return;
        }

        handlers[type] = handler;
    }

    void MessageProcessor::registerDefaultHandlers() {
        // todo
    }
}