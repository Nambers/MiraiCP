//
// Created by Antares on 2023-8-6.
//

#ifndef MIRAICP_PRO_MSGQUEUE_H
#define MIRAICP_PRO_MSGQUEUE_H

#include "polym/Queue.hpp"
#include "MsgDefine.h"

namespace MiraiCP {
    PolyM::Queue *getMsgQueue();

    void sendHello();
}

#endif //MIRAICP_PRO_MSGQUEUE_H
