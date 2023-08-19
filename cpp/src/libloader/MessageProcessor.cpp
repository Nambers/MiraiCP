//
// Created by Antares on 2023-8-19.
//

#include "MessageProcessor.h"
#include "LoaderLogger.h"
MiraiCP::MessageProcessor &MiraiCP::MessageProcessor::get() {
    static MessageProcessor processor;
    return processor;
}

void MiraiCP::MessageProcessor::registerDefaultHandlers() {
}


void MiraiCP::MessageProcessor::registerHandler(MiraiCP::MessageProcessor::MsgType type, MiraiCP::MessageProcessor::message_handler handler) {
}
void MiraiCP::MessageProcessor::processMessage(MiraiCP::MessageProcessor::MsgType type, std::unique_ptr<PolyM::Msg> msg) {
    if(type < 0 || type >= MessageType::MESSAGE_TYPE_COUNT){
        LoaderLogger::get().error("MessageProcessor::processMessage: invalid message type: %d", type);
        return;
    }
}
