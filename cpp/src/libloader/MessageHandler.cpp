//
// Created by Antares on 2023/9/24.
//

#include "MessageHandler.h"
#include "MsgDefine.h"
#include "loaderApiInternal.h"
#include "Plugin.h"
namespace LibLoader::MessageHandler {
    template<typename T>
    T *message_cast(void *msg) {
        return reinterpret_cast<T *>(msg);
    }

    void operation_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin) {
        using PayloadClass = MiraiCP::OperationMessage;
        auto operation = message_cast<PayloadClass>(msg);
        auto ret = LoaderApi::pluginOperation(operation->msg_string);
        if (plugin->isEnabled()) {
            plugin->makeResponse(operation->msg_lid, ret);
        }
    }

    void logger_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin) {
    }

    void plugin_id_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin) {
    }
    void push_task_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin) {
    }
    void push_task_with_id_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin) {
    }
    void timer_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin) {
    }
    void admin_message(MessageProcessor::payload_ptr msg, plugin_ptr plugin) {
    }
} // namespace LibLoader::MessageHandler