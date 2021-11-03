#include "Event.h"
#include "Exception.h"

namespace MiraiCP {
    class Event;

    MiraiCPException::ExceptionBroadcasting::~ExceptionBroadcasting() {
        Event::processor.broadcast<MiraiCPExceptionEvent>(MiraiCPExceptionEvent(e));
    }
    // 在 MiraiCPException 被构造之后执行，实现于析构函数

} // namespace MiraiCP
