// Copyright (c) 2022. Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

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
        auto pluginName = LibLoader::ThreadController::getPluginIdFromThreadId(std::this_thread::get_id());
        if (pluginName.empty()) {
            // test the thread name is jvm
            char threadName[80];
            pthread_getname_np(pthread_self(), threadName, 80);
            if (strcmp(threadName, "libLoader") != 0) {
                return EXCEPTION_CONTINUE_EXECUTION;
            }
            LibLoader::logger.error("libLoader线程遇到致命错误，请向MiraiCP仓库提交您的报错信息以及堆栈信息\n");
            LibLoader::logger.error("errCod:" + std::to_string(pExceptionPointers->ExceptionRecord->ExceptionCode));
            exit(1);
        }
        LibLoader::logger.error("插件" + pluginName + "遇到致命错误! 线程终止, errCod:" + std::to_string(pExceptionPointers->ExceptionRecord->ExceptionCode));
        LibLoader::sendPluginException(std::move(pluginName));
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
        LibLoader::logger.error("插件" + pluginName + "遇到致命错误! 线程终止");
        LibLoader::sendPluginException(std::move(pluginName));

        pthread_cancel(pthread_self());
        // 无尽循环
        // for (;;) std::this_thread::sleep_for(std::chrono::hours(1));
    }

    static SignalHandle Handler;
};

SignalHandle SignalHandle::Handler;
#endif
