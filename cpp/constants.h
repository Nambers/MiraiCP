#pragma once
#include "pch.h"

/// MiraiCP当前版本
const std::string MiraiCPVersion = "v2.6.5";

/// @brief 插件信息
class PluginConfig{
public:
    /// @brief 插件名称
    const std::string name;
    /// @brief 插件版本
    const std::string version;
    /// @brief 插件作者(及联系方式)
    const std::string author;
    /// @brief [optional]插件描述
    const std::string description;
    /// @brief [optional]构建时间
    const std::string time;
    PluginConfig(const std::string &name, const std::string &version, const std::string &author,
                 const std::string &description = "", const std::string &time = "") : name(name), version(version),
                                                                            author(author), description(description),
                                                                            time(time) {}
    std::string serialize2string(){
        nlohmann::json j;
        j["name"] = name;
        j["version"] = version;
        j["author"] = author;
        j["description"] = description;
        j["time"] = time;
        return j.dump();
    }
};

/*! @addtogroup 需要表态的值
 * @brief 如加好友事件
 * @{*/
///拒绝
static const bool REJECT = false;
/// 同意
static const bool ACCEPT = true;
/// @}

/*! @addtogroup 群成员退出事件
 * @{*/

///被踢出
static const int T_KICK = 1;
///主动退出
static const int T_QUIT = 2;
/// @}

/*!
 * @addtogroup 新群成员事件
 * @{
 * */
/*!被邀请进入*/
static const int T_INVITE = 1;
/*!主动加入*/
static const int T_ACTIVE = 2;
/*!旧群主恢复群主位置*/
static const int T_Retrieve = 3;
/// @}

/*!@addtogroup 群成员权限
 * @{*/
/// 群主
static const int P_Owner = 2;
/// 管理员
static const int P_Administer = 1;
/// 群成员
static const int P_Member = 0;
///@}

/*!
 * @addtogroup LightApp风格
 * @{
 */
/*!
LightApp风格1
*/
struct LightAppStyle1{
    ///小图标地址
    std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
    ///小标题，用逗号分割多个
    std::string titles = "{\"title\":\"1\", \"value\":\"t1\"},"
                         "{\"title\":\"2\", \"value\":\"t2\"}";
    ///下面的按钮，但是不能按，用逗号分割多个
    std::string buttons = "{\"name\":\"T1\",\"action\":\"\"}"
                          "{\"name\":\"T2\",\"action\":\"\"}";
    ///大标题
    std::string appName = "test1";
    ///小标题的标题
    std::string title = "test2";
};

/// 风格2
struct LightAppStyle2 {
    ///小图标
    std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
    ///预览图大图
    std::string preview = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
    ///小图片右侧小标题
    std::string title = "Test1";
    ///图片上标题
    std::string title2 = "Test2";
};

/// 风格3
struct LightAppStyle3 {
    /// 点击跳转链接
    std::string url = "https://www.baidu.com";
    /// 图标
    std::string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
    ///预览图
    std::string preview = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
    ///大标题
    std::string title = "Test1";
    ///图片旁描述
    std::string description = "Test2";
};
///@}
