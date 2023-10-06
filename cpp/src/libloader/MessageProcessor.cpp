//
// Created by Antares on 2023-8-19.
//

#include "MessageProcessor.h"
#include "LoaderLogger.h"
#include "MessageHandler.h"
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

    std::shared_ptr<const Plugin> MessageProxy::getPlugin() const {
        return plugin;
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
        handlers[MsgType::OperationMessage] = MessageHandler::operation_message;
        handlers[MsgType::LoggerMessage] = MessageHandler::logger_message;
        handlers[MsgType::PluginIdMessage] = MessageHandler::plugin_id_message;
        handlers[MsgType::PushTaskMessage] = MessageHandler::push_task_message;
        handlers[MsgType::PushTaskWithIdMessage] = MessageHandler::push_task_with_id_message;
        handlers[MsgType::TimerMessage] = MessageHandler::timer_message;
        handlers[MsgType::AdminMessage] = MessageHandler::admin_message;
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
        handler(payload.payload, msg.getPlugin());
    }
} // namespace LibLoader
