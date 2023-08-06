//
// Created by Antares on 2023-8-6.
//

#include "MsgSender.h"
using namespace PolyM;

PolyM::Queue *MiraiCP::getMsgQueue() {
    static PolyM::Queue queue;
    return &queue;
}

void MiraiCP::sendHello() {
    Msg msg(0);
    getMsgQueue()->put(Msg(0));
}
