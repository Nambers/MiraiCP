// Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_ICONTACTDATA_H
#define MIRAICP_PRO_ICONTACTDATA_H

#include "IMiraiData.h"
#include "SdkType.h"


namespace MiraiCP {
    /// Contact 内部储存数据抽象类
    struct IContactData : public IMiraiData {
        /// 昵称或者群名片
        std::string _nickOrNameCard;
        /// 头像 url
        std::string _avatarUrl;
        /// id
        QQID _id{};
        /// 所属的 bot 的 id
        QQID _botId{};
        /// Contact 类型
        ContactType _type = MIRAI_CONTACT;

        void deserialize(nlohmann::json inJson) override;

        nlohmann::json internalToJson() const override;

        /// 刷新数据
        void refreshInfo() override;

        /**
         * 更新传入 json 中的数据, 在调用后, 原 json 数据会被更新
         * @param jsonToUpdate 需更新的 json 数据
         * @see nlohmann::update
         */
        void updateJson(nlohmann::json &jsonToUpdate) const;
    };

    /**
     * @brief 获取指向的数据的裸指针，实际取内容时必须通过该函数，否则可能会取不到。
     * @note dev: 为了让getter正常地拥有const语义, 该函数不可以由外部调用.
     *  该函数使用const qualifier, 但返回非const的指针类型. 若允许外部调用可能造成数据问题。
     * @see IMPL_GETTER
     */
    template<typename DataType>
    DataType *GetDataInternal(std::shared_ptr<IContactData> &Data) {
        assert(Data != nullptr);
        return static_cast<DataType *>(Data.get());
    }
} // namespace MiraiCP

#endif //MIRAICP_PRO_ICONTACTDATA_H
