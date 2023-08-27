//
// Created by Antares on 2023-8-19.
//

#include "MessageProcessor.h"
#include "LoaderLogger.h"
#include "Plugin.h"
#include "commonTools.h"

namespace LibLoader {
    /// MessageProxy
    MessageProxy::~MessageProxy() {
        if (payload.payload && plugin != nullptr && plugin->checkValid()) {
            plugin->delete_message();
        }
    }

    MessageProxy::MessageProxy(MiraiCP::PluginInterface::PayLoadInfo payload, std::shared_ptr<const Plugin> plugin) : payload(payload), plugin(std::move(plugin)) {
    }

    MessageProxy::operator bool() const {
        return payload.payload != nullptr && plugin != nullptr;
    }

    /// MessageProcessor
    MessageProcessor &MessageProcessor::get() {
        static MessageProcessor processor;
        return processor;
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

    void MessageProcessor::processMessage(const MessageProxy &msg) {
        bool invalid = true;
        auto payload = msg.getPayload();
        auto type = payload.payload_id;
        MIRAICP_DEFER(if (invalid) logger.error("MessageProcessor::processMessage: invalid message type:" + std::to_string(type)););

        if (type < 0 || type >= MsgType::MESSAGE_TYPE_COUNT) return;

        auto handler = handlers[type];
        if (!handler) return;

        invalid = false;
        handler(payload.payload);
    }
} // namespace LibLoader
