// Copyright (c) 2022 - 2022. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_MIRAICPNEWTHREAD_H
#define MIRAICP_PRO_MIRAICPNEWTHREAD_H

#include "CPPPlugin.h"
#include "Event.h"
#include "Exception.h"
#include "Logger.h"
#include "MiraiCPMacros.h"
#include "PlatformThreading.h"
#include <ostream>
#include <thread>


namespace MiraiCP {
    /// MiraiCP 对 std::thread 的封装
    class MiraiCPNewThread : public std::thread {
    public:
        MiraiCPNewThread() noexcept = default;

        template<typename Callable, typename... Args>
        explicit MiraiCPNewThread(Callable &&func, Args &&...args)
            : std::thread(
                      [lambda_func = std::forward<Callable>(func)](auto &&...argss) {
                          try {
                              const char *thread_name = CPPPlugin::config.id;
                              platform_set_thread_name(platform_thread_self(), thread_name);
                              lambda_func(std::forward<decltype(argss)>(argss)...);
                          } catch (MiraiCPExceptionBase &e) {
                              e.raise();
                              Event::broadcast(MiraiCPExceptionEvent(&e));
                          } catch (const std::exception &e) {
                              MiraiCPThreadException exNew(std::string(e.what()), std::this_thread::get_id(), MIRAICP_EXCEPTION_WHERE);
                              exNew.raise();
                              Event::broadcast(MiraiCPExceptionEvent(&exNew));
                          } catch (...) {
                              MiraiCPThreadException exNew("unknown exception type", std::this_thread::get_id(), MIRAICP_EXCEPTION_WHERE);
                              exNew.raise();
                              Event::broadcast(MiraiCPExceptionEvent(&exNew));
                          }
                      },
                      std::forward<Args>(args)...) {}

        MiraiCPNewThread &operator=(const std::thread &) = delete;
        MiraiCPNewThread &operator=(const MiraiCPNewThread &) = delete;

        MiraiCPNewThread &operator=(std::thread &&other) {
            static_cast<std::thread &>(*this) = std::move(other);
            return *this;
        }

        MiraiCPNewThread &operator=(MiraiCPNewThread &&other) noexcept {
            static_cast<std::thread &>(*this) = std::move(static_cast<std::thread &>(other));
            return *this;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_MIRAICPNEWTHREAD_H
