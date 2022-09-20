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
    /*!
     * @brief 群聊类
     */
    class Group : public ContactWithSendSupport {
    private:
        friend class Contact;

    public: // nested classes and structs
        /// 群公告参数
        class AnnouncementParams {
        public:
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
        class OnlineAnnouncement {
        public:
            /// 内容
            std::string content;
            /// 所属bot
            QQID botid;
            /// 所在群id
            QQID groupid;
            /// 发送者id
            QQID senderid;
            /// 发送时间戳
            long long publicationTime;
            /// 唯一识别属性
            std::string fid;
            /// 图片id, 如果不存在即为空
            std::string imageid;
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
//                               QQID groupid, QQID senderid, QQID botid,
//                               long long int publicationTime, std::string fid, int confirmNum,
//                               std::string imageid) : content(std::move(content)), botid(botid), params(std::move(params)),
//                                                             groupid(groupid), senderid(senderid),
//                                                             publicationTime(publicationTime),
//                                                             fid(std::move(fid)), confirmNum(confirmNum), imageid(std::move(imageid)) {}
        };

        /// 本地(未发送)群公告
        class OfflineAnnouncement {
        public:
            /// 内容
            std::string content;
            /// 公告属性
            AnnouncementParams params;

            /// 发布群公告
            Group::OnlineAnnouncement publishTo(const Group &);

            OfflineAnnouncement(const std::string &content, AnnouncementParams params) : content(content),
                                                                                         params(params) {}
        };

        /**
         * @brief 群设置
         * @details 使用uploadSetting上传设置，使用refreshInfo同步服务器设定，后面两项由于https://github.com/mamoe/mirai/issues/1307 还不能改
         */
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

        /// 群文件的简短描述
        struct file_short_info {
            // 路径带文件名
            std::string path;
            // 唯一id
            std::string id;
        };

    public: // attrs
        /// 群设置
        GroupSetting setting;

    public: // constructors
        /*!
         * @brief 群聊类的内部构造函数，不可使用
         */
        explicit Group() = default;

        ///  @brief 构建以群号构建群对象
        /// @param groupid 群号
        /// @param botid 机器人id
        /// @doxygenEg{1007, group.cpp, 从群号构建群对象}
        Group(QQID groupid, QQID botid);

        //        explicit Group(const Contact &c) : Contact(c) {
        //            if (c.type() != 2)
        //                throw IllegalArgumentException("无法从 type==" + std::to_string(c.type()) + " 转为 type == 2(group)", MIRAICP_EXCEPTION_WHERE);
        //            refreshInfo();
        //        }


    public: // methods
        /**
         * @brief 更新群设置, 即覆盖服务器上的群设置
         * @details 从服务器拉去群设置用refreshInfo
         * @see Group::refreshInfo()
         */
        void updateSetting();

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
        void refreshInfo();

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
        MessageSource sendVoice(const std::string &path) {
            return sendVoice0(path);
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
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_GROUP_H
