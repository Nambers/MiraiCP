//
// Created by Antares on 2023/9/24.
//

#include "MessageHandler.h"
#include "MsgDefine.h"
#include "Plugin.h"
#include "loaderApiInternal.h"
namespace LibLoader::MessageHandler {
    template<typename T>
    T *message_cast(void *msg) {
        return reinterpret_cast<T *>(msg);
    }

    void operation_message(MessageProcessor::payload_ptr msg, const plugin_ptr &plugin) {
        if (!plugin->isEnabled()) return;
        using PayloadClass = MiraiCP::OperationMessage;
        auto operation = message_cast<PayloadClass>(msg);
        auto ret = LoaderApi::pluginOperation(operation->msg_string);
        plugin->makeResponse(operation->uid, ret);
    }

    void logger_message(MessageProcessor::payload_ptr msg, const plugin_ptr &plugin) {
        if (!plugin->isEnabled()) return;
        using PayloadClass = MiraiCP::LoggerMessage;
        auto log = message_cast<PayloadClass>(msg);
        LoaderApi::loggerInterface(log->msg_string, log->name, log->msg_lid, log->msg_level);
    }

    void plugin_id_message(MessageProcessor::payload_ptr msg, const plugin_ptr &plugin) {
        if (!plugin->isEnabled()) return;
        using PayloadClass = MiraiCP::PluginIdMessage;
        auto plugin_id_msg = message_cast<PayloadClass>(msg);
        plugin->makeResponse(plugin_id_msg->uid, LoaderApi::showAllPluginId());
    }

    void push_task_message(MessageProcessor::payload_ptr msg, const plugin_ptr &plugin) {
        if (!plugin->isEnabled()) return;
        using PayloadClass = MiraiCP::PushTaskMessage;
        auto push_task_msg = message_cast<PayloadClass>(msg);
        LoaderApi::pushTask(push_task_msg->task_func);
    }
    void push_task_with_id_message(MessageProcessor::payload_ptr msg, const plugin_ptr &plugin) {
        if (!plugin->isEnabled()) return;
        using PayloadClass = MiraiCP::PushTaskWithIdMessage;
        auto push_task_with_id_msg = message_cast<PayloadClass>(msg);
        LoaderApi::pushTaskWithId(push_task_with_id_msg->task_func_with_id, push_task_with_id_msg->id);
    }

    void timer_message(MessageProcessor::payload_ptr msg, const plugin_ptr &plugin) {
        if (!plugin->isEnabled()) return;
        using PayloadClass = MiraiCP::TimerMessage;
        auto timer_msg = message_cast<PayloadClass>(msg);
        LoaderApi::timer(timer_msg->timer_id, timer_msg->content, timer_msg->sec);
    }

    void admin_message(MessageProcessor::payload_ptr msg, const plugin_ptr &plugin) {
        if (!plugin->isEnabled()) return;
        using PayloadClass = MiraiCP::AdminMessage;
        auto admin_msg = message_cast<PayloadClass>(msg);
        switch (admin_msg->cmd_id) {
            case MiraiCP::AdminMessage::ENABLE_ONE:
                LoaderApi::enablePluginById(admin_msg->cmd_arg);
                break;
            case MiraiCP::AdminMessage::DISABLE_ONE:
                LoaderApi::disablePluginById(admin_msg->cmd_arg);
                break;
            case MiraiCP::AdminMessage::ENABLE_ALL:
                LoaderApi::enableAllPlugins();
                break;
            case MiraiCP::AdminMessage::DISABLE_ALL:
                LoaderApi::disableAllPlugins();
                break;
            case MiraiCP::AdminMessage::RELOAD_ONE:
                LoaderApi::reloadPluginById(admin_msg->cmd_arg);
                break;
            case MiraiCP::AdminMessage::UNLOAD_ONE:
                LoaderApi::unloadPluginById(admin_msg->cmd_arg);
                break;
            case MiraiCP::AdminMessage::LOAD_NEW:
                LoaderApi::loadNewPlugin(admin_msg->cmd_arg, admin_msg->immediate);
                break;
            default:
                break;
        }
    }
} // namespace LibLoader::MessageHandler