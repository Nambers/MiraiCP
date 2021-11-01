#ifndef MIRAICP_PRO_GROUP_H
#define MIRAICP_PRO_GROUP_H

#include "Member.h"

namespace MiraiCP {
    /// 群聊类声明
    class Group : public Contact {
    public:
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
            json serializeToJson();

            explicit AnnouncementParams(bool send2New = false, bool requireConfirm = false, bool pinned = false,
                                        bool showEditCard = false, bool showPopup = false) : send2new(send2New),
                                                                                             requireConfirm(
                                                                                                     requireConfirm),
                                                                                             pinned(pinned),
                                                                                             showEditCard(showEditCard),
                                                                                             showPopup(showPopup) {}
        };

        /// 在线群公告
        class OnlineAnnouncement {
        public:
            /// 内容
            std::string content;
            /// 所属bot
            QQID botid;
            /// 公告属性
            AnnouncementParams params;
            /// 所在群id
            QQID groupid;
            /// 发送者id
            QQID senderid;
            /// 发送时间戳
            long long publicationTime;
            /// 唯一识别属性
            std::string fid;
            /// 如果需要确认，即为确认的人数
            int confirmNum;
            /// 图片id, 如果不存在即为空
            std::string imageid;

            /// 删除当前群公告
            /// @throw BotException
            void deleteThis();

            /// 反序列化
            static OnlineAnnouncement deserializeFromJson(json);

            OnlineAnnouncement(const std::string &content, AnnouncementParams &params,
                               QQID groupid, QQID senderid, QQID botid,
                               long long int publicationTime, const std::string &fid, int confirmNum,
                               const std::string &imageid) : content(content), params(params), groupid(groupid),
                                                             senderid(senderid), botid(botid),
                                                             publicationTime(publicationTime),
                                                             fid(fid), confirmNum(confirmNum), imageid(imageid) {}
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
        /// 群设置
        GroupSetting setting;

        /**
         * @brief 更新群设置, 即覆盖服务器上的群设置
         * @details 从服务器拉去群设置用refreshInfo
         * @see Group::refreshInfo()
         */
        void updateSetting(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 取群成员列表
        /// @return vector<long>
        std::vector<unsigned long long> getMemberList(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["contactSource"] = this->serializationToString();
            std::string re = Config::koperation(Config::QueryML, j, env);
            if (re == "E1") {
                throw GroupException();
            }
            return Tools::StringToVector(re);
        }

        /*!
         * 以string格式取群成员列表
         * @note 格式:
         *  每个群成员id间用逗号分隔
        */
        std::string MemberListToString() {
            return Tools::VectorToString(getMemberList());
        };

        /// 取群主
        Member getOwner(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        ///  @brief 构建以群号构建群对象
        /// @param groupid 群号
        /// @param botid 机器人id
        /// @example 在事件中构建Group对象
        /// @code Group(this.group.id, this.bot.id) @endcode
        Group(QQID groupid, QQID botid, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        explicit Group(Contact c) : Contact(std::move(c)) { refreshInfo(); };

        /// 取群成员
        Member getMember(QQID memberid, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return Member(memberid, this->id(), this->groupid(), env);
        }

        /// 取群公告列表
        std::vector<OnlineAnnouncement> getAnnouncementsList(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            json j;
            j["source"] = this->serializationToString();
            j["announcement"] = true;
            std::string re = Config::koperation(Config::RefreshInfo, j, env);
            std::vector<OnlineAnnouncement> oa;
            for (const json &e: json::parse(re)) {
                oa.push_back(Group::OnlineAnnouncement::deserializeFromJson(e));
            }
            return oa;
        };

        /// 刷新群聊信息
        void refreshInfo(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) override {
            std::string re = LowLevelAPI::getInfoSource(this, env);
            LowLevelAPI::info tmp = LowLevelAPI::info0(re);
            this->_nickOrNameCard = tmp.nickornamecard;
            this->_avatarUrl = tmp.avatarUrl;
            nlohmann::json j = nlohmann::json::parse(re)["setting"];
            this->setting.name = j["name"];
            this->setting.isMuteAll = j["isMuteAll"];
            this->setting.isAllowMemberInvite = j["isAllowMemberInvite"];
            this->setting.isAutoApproveEnabled = j["isAutoApproveEnabled"];
            this->setting.isAnonymousChatEnabled = j["isAnonymousChatEnabled"];
        }

        void quit(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["source"] = this->serializationToString();
            j["quit"] = true;
            Config::koperation(Config::RefreshInfo, j, env);
        }

        /*!
        @brief 上传并发送远程(群)文件
        @param path-群文件路径(带文件名),根目录为/
        @param filepath-本地文件路径
        @example 上传并发送远程(群)文件
         @code
         Event::processor.registerEvent([](GroupMessageEvent e) {
            // 发送D:\\ValveUnhandledExceptionFilter.txt本地文件到qq群的 /test.txt 路径
            RemoteFile tmp = e.group.sendFile("/test.txt", "D:\\ValveUnhandledExceptionFilter.txt");
            });
         @endcode
        */
        RemoteFile sendFile(const std::string &path, const std::string &filepath,
                            JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 发送语音
        MessageSource sendVoice(const std::string &path, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return Contact::sendVoice0(path, env);
        }

        /*!
        取群文件信息,会自动搜索子目录
        @param path-群文件路径(不带文件名)
        @param id-文件id,可空，空则为用路径查找(此时路径要带文件名)
        @attention 因为群文件允许重名文件存在的特性，该查找并不可靠，只能返回重名文件中的其中一个文件
        @see RemoteFile
         @example 取群文件信息
         @code
         //根据qq群远程路径(不带文件名)和文件id, 文件id可以在上传返回的RemoteFile类型中获得, 会在子目录中查找如果当前目录未找到
         //根据qq群远程路径(带文件名)找, 不过由于qq群文件允许同名文件这一特性, 返回的文件为群文件中同名文件中随机一个(不可靠)
         Event::processor.registerEvent([](GroupMessageEvent e) {
            e.group.SendMsg(e.group.getFile("/", id).name());
            e.group.SendMsg(e.group.getFile("/test.txt").name());
            e.group.SendMsg(e.group.getFileListString("/"));
            });
         @endcode
        */
        RemoteFile getFile(const std::string &path, const std::string &id = "",
                           JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /*!
         * @brief 取文件信息(根据id)
         * @param id 文件id
         * @return 文件
         * @detail 相当于从根目录开始遍历查找文件, 相当于getFile("/", id);
         */
        RemoteFile getFileById(const std::string &id, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 群文件的简短描述
        struct file_short_info {
            // 路径带文件名
            std::string path;
            // 唯一id
            std::string id;
        };

        /*!
         * 获取path路径下全部文件信息
         * @param path - 远程路径
         * @return 返回值为一个vector容器, 每一项为short_info
        */
        std::vector<file_short_info>
        getFileList(const std::string &path, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 取文件列表以字符串形式返回
        ///@example 取string格式群文件列表
        /// @code
        /// e.group.SendMsg(e.group.getFileListString("/"));
        /// @endcode
        std::string getFileListString(const std::string &path, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_GROUP_H