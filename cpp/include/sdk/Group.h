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

#ifndef MIRAICP_PRO_GROUP_H
#define MIRAICP_PRO_GROUP_H

#include "Contact.h"
#include <json_fwd.hpp>
#include <utility>


namespace MiraiCP {
    class Member; // forward declaration


    struct GroupData : public GroupRelatedData {
        struct GroupSetting {
            /// 群名称
            std::string name;
            /// 禁言全部
            bool isMuteAll{};
            /// 允许群成员邀请
            bool isAllowMemberInvite{};
            /// 自动同意进群
            bool isAutoApproveEnabled{};
            /// 允许匿名聊天
            bool isAnonymousChatEnabled{};
        };
        /**
         * @brief 群设置
         * @details 使用 Group::updateSetting 上传设置，后面两项由于 https://github.com/mamoe/mirai/issues/1307 还不能改
         */
        GroupSetting _setting;

        explicit GroupData(QQID in_groupid) : GroupRelatedData(in_groupid) {}

        void deserialize(nlohmann::json in_json) override;
        void refreshInfo() override;
    };

    /*!
     * @brief 群聊类
     */
    class Group : public Contact, public ContactDataHelper<Group, GroupData> {
    private:
        friend class Contact;

    public: // nested classes and structs
        /// 群公告参数
        struct AnnouncementParams {
            /// 发送给新成员
            bool send2new;
            /// 需要确认
            bool requireConfirm;
            /// 置顶
            bool pinned;
            /// 引导群成员修改群名片
            bool showEditCard;
            /// 显示弹窗
            bool showPopup;

            /// 序列化到文本
            nlohmann::json serializeToJson();

            //            explicit AnnouncementParams(bool send2New = false, bool requireConfirm = false, bool pinned = false,
            //                                        bool showEditCard = false, bool showPopup = false) : send2new(send2New),
            //                                                                                             requireConfirm(
            //                                                                                                     requireConfirm),
            //                                                                                             pinned(pinned),
            //                                                                                             showEditCard(showEditCard),
            //                                                                                             showPopup(showPopup) {}
        };

        /// 在线群公告
        struct OnlineAnnouncement {
            /// 内容
            std::string content;
            /// 所属bot
            QQID botId;
            /// 所在群id
            QQID groupId;
            /// 发送者id
            QQID senderId;
            /// 发送时间戳
            long long publicationTime;
            /// 唯一识别属性
            std::string fid;
            /// 图片id, 如果不存在即为空
            std::string imageId;
            /// 如果需要确认，即为确认的人数
            int confirmNum;
            /// 公告属性
            AnnouncementParams params;
            /// 删除当前群公告
            /// @throw BotException
            void deleteThis();

            /// 反序列化
            static OnlineAnnouncement deserializeFromJson(const nlohmann::json &);

            //            OnlineAnnouncement(std::string content, AnnouncementParams params,
            //                               QQID groupId, QQID senderId, QQID botId,
            //                               long long int publicationTime, std::string fid, int confirmNum,
            //                               std::string imageId) : content(std::move(content)), botId(botId), params(std::move(params)),
            //                                                             groupId(groupId), senderId(senderId),
            //                                                             publicationTime(publicationTime),
            //                                                             fid(std::move(fid)), confirmNum(confirmNum), imageId(std::move(imageId)) {}
        };

        /// 本地(未发送)群公告
        struct OfflineAnnouncement {
            /// 内容
            std::string content;
            /// 公告属性
            AnnouncementParams params;

            /// 发布群公告
            Group::OnlineAnnouncement publishTo(const Group &);

            //            OfflineAnnouncement(const std::string &content, AnnouncementParams params) : content(content),
            //                                                                                         params(params) {}
        };

        DECL_GETTER(setting)

        /// 群文件的简短描述
        struct file_short_info {
            // 路径带文件名
            std::string path;
            // 唯一id
            std::string id;
        };

        /// 群荣耀类型
        /// @note 与这里的值对应: <https://github.com/mamoe/mirai/blob/v2.13.4/mirai-core-api/src/commonMain/kotlin/data/GroupHonorType.kt#L52>
        enum HonorMemberType{
            /// 龙王
            TALKATIVE = 1,
        };

    public: // constructors
        ///  @brief 构建以群号构建群对象
        /// @param groupId 群号
        /// @param botId 机器人id
        /// @doxygenEg{1007, group.cpp, 从群号构建群对象}
        Group(QQID groupId, QQID botId);

        /// @brief 构建群对象，一般为内部调用
        /// @param in_json 内容至少如下：{"id":123, "botId":456}
        /// @throw IllegalArgumentException
        explicit Group(nlohmann::json in_json);

        /// @note dev: avoid unintentional implicit conversion to nlohmann::json
        Group(bool) = delete;

    public: // methods
        /// @brief 获取groupId
        /// @note 同 id()
        INLINE_GETTER(groupId)

        /**
         * @brief 更新群设置, 即覆盖服务器上的群设置
         * @details 从服务器拉去群设置用refreshInfo
         * @param newSetting 新的设置
         * @see Group::refreshInfo()
         */
        void updateSetting(GroupData::GroupSetting newSetting);

        /// 取群成员列表
        /// @return vector<long>
        std::vector<unsigned long long> getMemberList();

        /*!
         * 以string格式取群成员列表
         * @note 格式:
         *  每个群成员id间用逗号分隔
        */
        std::string MemberListToString();

        /// 取群主
        Member getOwner();

        /// 取群成员
        Member getMember(QQID memberid);

        Member operator[](QQID id);

        /// 取群公告列表
        std::vector<OnlineAnnouncement> getAnnouncementsList();

        /// 刷新群聊信息
        //void refreshInfo();

        void quit();

        /*!
        @brief 上传并发送远程(群)文件
        @param path-群文件路径(带文件名),根目录为/
        @param filepath-本地文件路径
        @attention 路径分隔符是 `/`
        @doxygenEg{1008, group.cpp, 发送群文件}
        */
        RemoteFile sendFile(const std::string &path, const std::string &filepath);

        /// 发送语音
        MessageSource sendVoice( std::string path) {
            return sendVoiceImpl(std::move(path));
        }

        /*!
        取群文件信息,会自动搜索子目录
        @param path-群文件路径(不带文件名)
        @param id-文件id,可空，空则为用路径查找(此时路径要带文件名)
        @attention 因为群文件允许重名文件存在的特性，如果没有id该查找并不可靠，只能返回重名文件中的其中一个文件
        @see RemoteFile
        @doxygenEg{1009, group.cpp, 获取群文件}
        */
        RemoteFile getFile(const std::string &path, const std::string &id = "");

        /*!
         * @brief 取文件信息(根据id)
         * @param id 文件id
         * @return 文件
         * @detail 相当于从根目录开始遍历查找文件, 相当于getFile("/", id);
         */
        RemoteFile getFileById(const std::string &id);

        RemoteFile getFileByFile(const RemoteFile &file) {
            return getFileById(file.id);
        }

        /*!
         * 获取path路径下全部文件信息
         * @param path - 远程路径
         * @return 返回值为一个vector容器, 每一项为short_info
         * @doxygenEg{1010, group.cpp, 获取群文件列表}
        */
        std::vector<file_short_info> getFileList(const std::string &path);

        /// 取文件列表以字符串形式返回
        /// @param path 文件夹路径
        std::string getFileListString(const std::string &path);

        /// 查询目前拥有对应群荣耀的成员
        std::optional<Member> queryCurrentHonorMember(HonorMemberType);
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_GROUP_H
