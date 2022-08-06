//
// Created by antares on 8/6/22.
//

#include "LoaderLogger.h"
#include "redirectCout.h"


#ifdef _WIN64
#include <windows.h>
class EventHandlerPitch {
public:
    static long __stdcall eventHandler(PEXCEPTION_POINTERS pExceptionPointers) {
        // todo(ea): add loader task and handle exceptionCode
        pExceptionPointers->ExceptionRecord->ExceptionCode;
        LibLoader::OString::errTarget.out();
        TerminateThread(GetCurrentThread(), 1);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    EventHandlerPitch() {
        preHandler = SetUnhandledExceptionFilter(EventHandlerPitch::eventHandler);
    }
    ~EventHandlerPitch() {
        SetUnhandledExceptionFilter(preHandler);
    }

private:
    LPTOP_LEVEL_EXCEPTION_FILTER preHandler;
};
[[maybe_unused]] EventHandlerPitch pitch = EventHandlerPitch();
#else
#include <signal.h>
#include <thread>
// 禁止从其他地方构造
class [[maybe_unused]] SignalHandle {
    SignalHandle() noexcept {
        static struct sigaction act;
        act.sa_handler = &SignalHandle::sigsegv_handler;
        act.sa_flags = SA_SIGINFO;
        sigemptyset(&act.sa_mask);
        act.sa_restorer = nullptr;
        sigaction(SIGSEGV, &act, nullptr);
    }

    ~SignalHandle() = default;

    static void sigsegv_handler(int) {
        LibLoader::logger.error("插件遇到致命错误！线程终止");
        pthread_kill(_GLIBCXX_NATIVE_THREAD_ID, SIGKILL);
    }

    static SignalHandle Handler;
};

SignalHandle SignalHandle::Handler;
#endif
