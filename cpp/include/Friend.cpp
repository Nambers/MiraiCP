#include "Friend.h"

namespace MiraiCP {
    /*好友类实现*/
    Friend::Friend(QQID id, QQID botid, JNIEnv *env) : Contact() {
        this->_type = 1;
        this->_id = id;
        this->_botid = botid;
        refreshInfo(env);
    }
} // namespace MiraiCP
