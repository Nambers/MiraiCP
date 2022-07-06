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
#include "Event.h"
#include "Exception.h"
#include <ostream>
#include <thread>
namespace MiraiCP {
    class MiraiCPNewThread : public std::thread {
    public:
        template<typename... Args>
        explicit MiraiCPNewThread(Args &&...args) {
            MiraiCPNewThread(std::forward<Args>(args)...);
        }
        template<typename Callable, typename... Args>
        explicit MiraiCPNewThread(Callable &&func, Args &&...args) : std::thread([func, this](Args &&...argss) {
                                                                         try {
                                                                             func(argss...);
                                                                         } catch (MiraiCPExceptionBase &e) {
                                                                             Event::broadcast(MiraiCPExceptionEvent(&e));
                                                                         } catch (std::exception &) {
                                                                             // todo 传入 exception
                                                                             NewThreadException e(get_id(), MIRAICP_EXCEPTION_WHERE);
                                                                             Event::broadcast(MiraiCPExceptionEvent(&e));
                                                                         } catch (...) {
                                                                             NewThreadException e(get_id(), MIRAICP_EXCEPTION_WHERE);
                                                                             Event::broadcast(MiraiCPExceptionEvent(&e));
                                                                         }
                                                                     },
                                                                                 std::forward<Args>(args)...) {}
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_MIRAICPNEWTHREAD_H
