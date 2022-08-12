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

#include "MiraiCPMacros.h"


// TODO(Antares): 1. 调用插件出口函数；2. 防止sigsegv重入
#if MIRAICP_WINDOWS

#include "LoaderLogger.h"
#include "ThreadController.h"
#include "redirectCout.h"
#include <windows.h>
// 禁止从其他地方构造
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
// TODO(Antares): 改为类静态成员，删掉maybe_unused
[[maybe_unused]] EventHandlerPitch pitch = EventHandlerPitch();

#else

#include "LoaderLogger.h"
#include "ThreadController.h"
#include <csignal>
#include <thread>
// 禁止从其他地方构造
class [[maybe_unused]] SignalHandle {
    SignalHandle() noexcept {
        auto &act = getAct();
        act.sa_flags = SA_SIGINFO;
        sigemptyset(&act.sa_mask);
        act.sa_sigaction = &SignalHandle::sigsegv_handler;
        sigaction(SIGSEGV, &act, &getOact());
    }

    ~SignalHandle() {
        sigaction(SIGSEGV, &getOact(), &getAct());
    }

private:
    static struct sigaction &getAct() {
        static struct sigaction act;
        return act;
    }

    static struct sigaction &getOact() {
        static struct sigaction oact;
        return oact;
    }

    static void sigsegv_handler(int a, siginfo_t *si, void *unused) {
        auto pluginName = LibLoader::ThreadController::getPluginIdFromThreadId(std::this_thread::get_id());
        if (pluginName.empty()) {
            // test the thread name is jvm
            char threadName[80];
            pthread_getname_np(pthread_self(), threadName, 80);
            if (strcmp(threadName, "libLoader") != 0) {
                getOact().sa_sigaction(a, si, unused);
                return;
            }
            LibLoader::logger.error("libLoader线程遇到致命错误，请向MiraiCP仓库提交您的报错信息以及堆栈信息");
            exit(1);
        }
        LibLoader::logger.error("插件" + pluginName + "遇到致命错误! 线程终止");
        LibLoader::sendPluginException(std::move(pluginName));

        pthread_cancel(pthread_self());
    }

private:
    static SignalHandle Handler;
};

SignalHandle SignalHandle::Handler;

#endif