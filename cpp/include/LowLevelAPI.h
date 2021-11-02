#ifndef MIRAICP_PRO_LOWLEVELAPI_H
#define MIRAICP_PRO_LOWLEVELAPI_H

#include "Config.h"
#include "Contact.h"

namespace MiraiCP {
    /// 较底层api
    class LowLevelAPI {
    public:
        /// @brief 抽象封装底层发送信息接口
        /// @param content 信息字符串
        /// @param c 目标Contact指针
        /// @param miraicode 是否为miraicode格式
        /// @param env JNIEnv
        /// @return
        static std::string send0(const std::string &content, Contact *c, int retryTime, bool miraicode, JNIEnv *env,
                                 std::string errorInfo = "") {
            nlohmann::json j;
            nlohmann::json tmp;
            tmp["content"] = content;
            tmp["contact"] = c->serialization();
            j["source"] = tmp.dump();
            j["miraiCode"] = miraicode;
            j["retryTime"] = retryTime;
            return Config::koperation(Config::Send, j, env, true, std::move(errorInfo));
        }

        /// @brief 取该联系人的一些信息
        /// @param c 该联系人Contact指针
        /// @return json格式字符串，待解析
        static inline std::string getInfoSource(Contact *c, JNIEnv *env = ThreadManager::getEnv()) {
            nlohmann::json j;
            j["source"] = c->serializationToString();
            return Config::koperation(Config::RefreshInfo, j, env);
        }

        /*!
         * @brief 上传图片
         * @param path 本地地址
         * @param c 上传的对象
         * @param env JNIEnv
         * @return string 待解析json
         */
        static inline std::string
        uploadImg0(const std::string &path, Contact *c, JNIEnv *env = ThreadManager::getEnv()) {
            nlohmann::json j;
            j["fileName"] = path;
            j["source"] = c->serializationToString();
            return Config::koperation(Config::UploadImg, j, env);
        }

        /// 每个对象的必有信息
        struct info {
            std::string nickornamecard;
            std::string avatarUrl;
        };

        /// 获取每个对象必有信息
        /// @see LowLevelAPI::info
        static info info0(const std::string &source) {
            info re;
            ErrorHandle(source);
            nlohmann::json j = nlohmann::json::parse(source);
            re.avatarUrl = j["avatarUrl"];
            re.nickornamecard = j["nickornamecard"];
            return re;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_LOWLEVELAPI_H
