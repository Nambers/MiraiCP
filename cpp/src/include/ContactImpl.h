// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_CONTACTIMPL_H
#define MIRAICP_PRO_CONTACTIMPL_H


#include "LowLevelAPI.h"
#include "MiraiCPMacros.h"
#include "commonTypes.h"
#include <atomic>
#include <shared_mutex>
#include <unordered_map>


namespace MiraiCP {
    class Contact;

    // todo(Antares): 改为Internal存储数据，外部wrapper仅提供接口，
    //  但不要在此处声明数据成员，除非确实是所有子类都需要且意义明确的成员
    //  不要传入Contact指针！
    template<class ImplClass>
    class IContactInternalImpl {
    private:
        std::shared_mutex _mtx;
        std::atomic_bool _inited = false;

    public:
        using PoolType = std::unordered_map<MiraiCP::QQID, std::shared_ptr<ImplClass>>;

        IContactInternalImpl() = default;

        virtual ~IContactInternalImpl() = default;

        virtual void refreshInfo(MiraiCP::Contact *) = 0;

        // for auto cast
        void request_refresh(Contact *c) {
            if (!MiraiCP::checkSafeCall()) return;
            if (!_inited.exchange(true)) {
                std::unique_lock<std::shared_mutex> _lck(_mtx);
                refreshInfo(c);
            }
        }

        void force_refresh() { _inited = false; }
    };
} // namespace MiraiCP


#endif //MIRAICP_PRO_CONTACTIMPL_H
