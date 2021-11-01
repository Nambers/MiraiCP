#include "ExceptionBroadcasting.h"
#include "Event.h"

namespace MiraiCP {
    class Event;

    ExceptionBroadcasting::ExceptionBroadcasting(MiraiCPException *ex) : e(ex) {}

    // 在 MiraiCPException 被构造之后执行，实现于析构函数
    ExceptionBroadcasting::~ExceptionBroadcasting() {
        Event::processor.broadcast<MiraiCPExceptionEvent>(MiraiCPExceptionEvent(e));
    }
} // namespace MiraiCP
