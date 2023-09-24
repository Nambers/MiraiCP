//
// Created by Antares on 2023/9/24.
//

#ifndef MIRAICP_PRO_MESSAGEHANDLER_H
#define MIRAICP_PRO_MESSAGEHANDLER_H

#include "MessageProcessor.h"
namespace LibLoader::MessageHandler {
    using plugin_ptr = std::shared_ptr<const Plugin>;

    void operation_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin);

    void logger_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin);

    void plugin_id_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin);

    void push_task_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin);

    void push_task_with_id_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin);

    void timer_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin);

    void admin_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin);
} // namespace LibLoader::MessageHandler


#endif //MIRAICP_PRO_MESSAGEHANDLER_H
