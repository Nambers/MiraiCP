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
// -----------------------
#include "LoaderTaskQueue.h"
#include "PlatformThreading.h"
#include "PluginListManager.h"
#include "commonTools.h"

// TODO(Antares): 处理线程池异常
#if MIRAICP_WINDOWS
#include "LoaderLogger.h"
#include "ThreadController.h"
#include <windows.h>

thread_local bool alreadyInHandler;

// 禁止从其他地方构造
class EventHandlerPitch {
public:
    static long __stdcall eventHandler(PEXCEPTION_POINTERS pExceptionPointers) {
        if (alreadyInHandler) {
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        auto pluginName = LibLoader::PluginListManager::getThreadRunningPluginId();

        if (pluginName.empty()) {
            // test the thread is from jvm
            char threadName[80];
            platform_get_thread_name(platform_thread_self(), threadName, 80);
            if (strcmp(threadName, "libLoader") == 0) {
                LibLoader::logger.error("libLoader线程遇到致命错误，请向MiraiCP仓库提交您的报错信息以及堆栈信息");
                exit(1);
            } else if (strcmp(threadName, "LoaderWorker") == 0) {
                // 非插件导致的工作线程致命错误
                LibLoader::logger.error("libLoader工作线程遇到致命错误，请向MiraiCP仓库提交您的报错信息以及堆栈信息");
                exit(1);
            }
            // todo(Antares): 判断是不是插件调new thread弄出来的线程

            {
                return EXCEPTION_CONTINUE_EXECUTION;
            }
        }
        // 插件导致的崩溃，卸载该插件
        alreadyInHandler = true;
        MIRAICP_DEFER(alreadyInHandler = false;);
        LibLoader::PluginListManager::disableByIdVanilla(pluginName);
        LibLoader::logger.error("插件" + pluginName + "遇到致命错误! 线程终止, errCod:" +
                                std::to_string(pExceptionPointers->ExceptionRecord->ExceptionCode));
        LibLoader::sendPluginException(std::move(pluginName));

        // todo(Antares): 重启被杀死的工作线程，思路：通过给worker线程加入thread_local标记确定线程对应的index，让thread pool重置该位置的线程
        TerminateThread(GetCurrentThread(), 1);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

private:
    EventHandlerPitch() {
        preHandler = SetUnhandledExceptionFilter(EventHandlerPitch::eventHandler);
    }

    ~EventHandlerPitch() {
        SetUnhandledExceptionFilter(preHandler);
    }

    static EventHandlerPitch pitch;

private:
    LPTOP_LEVEL_EXCEPTION_FILTER preHandler;
};

[[maybe_unused]] EventHandlerPitch EventHandlerPitch::pitch = EventHandlerPitch();

#else

#include "LoaderLogger.h"
#include <csignal>
#include <thread>


thread_local bool alreadyInHandler = false;
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
        if (alreadyInHandler) {
            return;
        }
        auto pluginName = LibLoader::PluginListManager::getThreadRunningPluginId();
        if (pluginName.empty()) {
            // test the thread is from jvm
            char threadName[80];
            platform_get_thread_name(platform_thread_self(), threadName, 80);
            if (strcmp(threadName, "libLoader") == 0) {
                LibLoader::logger.error("libLoader线程遇到致命错误，请向MiraiCP仓库提交您的报错信息以及堆栈信息");
                exit(1);
            } else if (strcmp(threadName, "LoaderWorker") == 0) {
                // 非插件导致的工作线程致命错误
                LibLoader::logger.error("libLoader工作线程遇到致命错误，请向MiraiCP仓库提交您的报错信息以及堆栈信息");
                exit(1);
            }
            // todo(Antares): 判断是不是插件调new thread弄出来的线程

            {
                getOact().sa_sigaction(a, si, unused);
                return;
            }
        }
        // 插件导致的崩溃，卸载该插件
        alreadyInHandler = true;
        MIRAICP_DEFER(alreadyInHandler = false;);
        LibLoader::PluginListManager::disableByIdVanilla(pluginName);
        LibLoader::logger.error("插件" + pluginName + "遇到致命错误! 插件运行终止");
        LibLoader::sendPluginException(std::move(pluginName));

        // todo(Antares): 重启被杀死的工作线程，思路：通过给worker线程加入thread_local标记确定线程对应的index，让thread pool重置该位置的线程
        pthread_cancel(pthread_self());
    }

private:
    static SignalHandle Handler;
};

SignalHandle SignalHandle::Handler;

#endif