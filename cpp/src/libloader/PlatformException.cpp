//
// Created by antares on 8/6/22.
//

#include "LoaderLogger.h"
#include "ThreadController.h"
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
        static int ExceptionReturnValue = 1;
        auto pluginName = LibLoader::ThreadController::getPluginIdFromThreadId(std::this_thread::get_id());
        if (pluginName.empty()) {
            // test the thread name is jvm
            char threadName[80];
            pthread_getname_np(pthread_self(), threadName, 80);
            if (strcmp(threadName, "libLoader") != 0) {
                return;
            }
            LibLoader::logger.error("libLoader线程遇到致命错误，请向MiraiCP仓库提交您的报错信息以及堆栈信息");
            exit(1);
        }
        LibLoader::logger.error("插件" + pluginName + "遇到致命错误！线程终止");
        LibLoader::sendPluginException(std::move(pluginName));

        pthread_cancel(pthread_self());
        // 无尽循环
        // for (;;) std::this_thread::sleep_for(std::chrono::hours(1));
    }

    static SignalHandle Handler;
};

SignalHandle SignalHandle::Handler;
#endif
