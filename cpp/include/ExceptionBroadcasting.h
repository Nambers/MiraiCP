#ifndef MIRAICP_PRO_EXCEPTIONBROADCASTING_H
#define MIRAICP_PRO_EXCEPTIONBROADCASTING_H

namespace MiraiCP {
    // 仅声明
    class MiraiCPException;

    /// @brief 异常事件广播
    class ExceptionBroadcasting {
    public:
        MiraiCPException *e;
        ExceptionBroadcasting(MiraiCPException *ex);
        ~ExceptionBroadcasting();
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_EXCEPTIONBROADCASTING_H
