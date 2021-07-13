#pragma once
#ifndef CPP_MIRAICP_HPP
#define CPP_MIRAICP_HPP
#pragma execution_character_set("utf-8")

// 在include文件夹里的依赖
#include "tech_eritquearcus_miraicp_shared_CPP_lib.h"
#include <json.hpp>
#include <utf8.h>

//C++ 标准库
#include <utility>
#include <vector>
#include <windows.h>
#include <string>
#include <functional>
#include <sstream>
#include <regex>
#include <thread>
#include <map>
#include <mutex>

namespace MiraiCP {
    using json = nlohmann::json;
    // 开始声明MiraiCP常量声明代码
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
        PluginConfig(std::string name, std::string version, std::string author,
                     std::string description = "", std::string time = "") : name(std::move(name)), version(std::move(version)),
                                                                                          author(std::move(author)), description(std::move(description)),
                                                                                          time(std::move(time)) {}
        json serialize() const;
        std::string serialize2string() const{
            return serialize().dump();
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

    // 接受声明MiraiCP常量声明代码

    // 开始MiraiCP类声明代码
///@}
/// @brief 全局JavaVM对象，用于多线程管理中新建线程的JNIEnv.
    JavaVM *gvm = nullptr;
/// @brief JNI 版本.
    int JNIVersion = 0;


/*!
 * @class threadManager
 * @brief 多线程管理.
 */
    class ThreadManager {
    private:
        /// @brief 每个线程实例.
        struct t {
            JNIEnv *e;
            bool attach;
        };
        std::map<std::string, t> _threads;/// < 线程池(线程id:env).
        std::recursive_mutex mtx; ///< 线程池读写锁.
        void newEnv(const char *threadName = nullptr);///< 新建一个env，于getEnv中没取到env时调用.
        /// 判断该线程id是否包含在线程池里
        bool included(const std::string &id);

    public:
        /// @brief 获取线程id.
        static std::string getThreadId() {
            auto myid = std::this_thread::get_id();
            std::stringstream ss;
            ss << myid;
            return ss.str();
        }

        ThreadManager() {
            this->_threads = std::map<std::string, t>();
        };

        /// @brief 设置env给当前线程.
        void setEnv(JNIEnv *e);

        /*!
         * 	@brief 结束当前线程的env，也就是释放当前线程缓存的env.
         *  @note 不过继续调用getEnv()将再次获取，所以本方法调用后线程也可以通过getEnv重新获取一个env，本方法的作用就是在结束后释放空间
         * @example 多线程管理
         * @code
         * void func(){
         *      //do some things
         *      // 一个完整的线程应该在结束时调用detach来释放env的空间
         *      manager->detch();
         * }
         * @endcode
         */
        void detach();

        /// @brief 取env,如果不存在重新获取
        /// @internal 一般为miraicp内部调用jni接口时调用
        JNIEnv *getEnv();
    };

    /// 线程管理实例
    ThreadManager *manager = new ThreadManager();

/// @brief 工具类声明, 常用的一些转换工具, 如需转码使用std::filesystem
/// @class Tools
    class Tools {
    public:
        /*!
         * @name jstring2str
         * @brief string类型转码转换到jstring类型, UTF16 -> UTF8
         * @note 来源https://blog.csdn.net/chunleixiahe/article/details/51394116
         * @param jstr 转换内容,jstring类型
         * @param env 可选，JNIEnv
         * @return 内容转换成jstring类型
         */
        static std::string jstring2str(jstring jstr, JNIEnv * = manager->getEnv());

        /*!
         * @name str2jstring
         * @brief string类型到jsting类型 UTF8 -> UTF16
         * @note 来源https://blog.csdn.net/chunleixiahe/article/details/51394116
         * @param pat const char*(string.c_str())转换的内容
         * @param env 可选JNIEnv
         * @return 转换后jstring类型
         */
        static jstring str2jstring(const char *pat, JNIEnv * = manager->getEnv());

        /*!
         * @name JLongToString
         * @brief jlong类型到string类型
         * @param val 需要转换的数值
         * @return 转换后string类型
         */
        static std::string JLongToString(jlong val);

        /*!
         * @brief 替换全部在一个字符串中.
         * @param str 原字符串.
         * @param from 需要被替换的字符.
         * @param to 替换到的字符.
         * @return 返回替换后的字符串.
         * @note 来源:https://stackoverflow.com/a/24315631/14646226
         */
        static std::string replace(std::string str, const std::string &from, const std::string &to) {
            size_t start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length(); // Handles case where 'to' is a substd::string of 'from'
            }
            return str;
        }

        /// @brief long long 类型的vector格式化输出
        /// @param v vector
        /// @return string
        static std::string VectorToString(std::vector<unsigned long long> v);

        /// @brief 从string格式化到vector
        /// @param s string
        /// @return vector
        static std::vector<unsigned long long> StringToVector(std::string s);

        /// @brief 从miraicode转义到正常
        /// @param s 经过miraicode转义的字符串
        /// @return 原字符串
        static std::string escapeFromMiraiCode(const std::string &s);

        /// @brief 转义miraicode格式
        /// @param s
        /// @return
        static std::string escapeToMiraiCode(const std::string &s);
    };

/// @brief 配置类声明, 主要存放各种jmethodid, MiraiCP内部使用, 不需要更改或其他操作
/// @internal 一般为MiraiCP内部调用jni接口使用
/// @class Config
    class Config {
    public:
        /// kt中JNI接口类
        jclass CPP_lib = nullptr;
        /// 调用mirai方法
        jmethodID KOperation = nullptr;

        /// 操作id
        enum operation_set {
            /// 撤回信息
            Recall,
            /// 发送信息
            Send,
            /// 查询信息接口
            RefreshInfo,
            /// 上传图片
            UploadImg,
            /// 取好友列表
            QueryBFL,
            /// 取群组列表
            QueryBGL,
            /// 上传文件
            SendFile,
            /// 查询文件信息
            RemoteFileInfo,
            /// 查询图片下载地址
            QueryImgUrl,
            /// 禁言
            MuteM,
            /// 查询权限
            QueryM,
            /// 踢出
            KickM,
            /// 取群主
            QueryOwner,
            /// 语音
            Voice,
            /// 查询群成员列表
            QueryML,
            /// 群设置
            GroupSetting,
            /// 构建转发信息
            Buildforward,
            /// 好友申请事件
            Nfroperation,
            /// 群聊邀请事件
            Gioperation,
            /// 回复(引用并发送)
            SendWithQuote
        };

        /**
         * @brief 调用mirai操作
         * @param code 操作id
         * @param json 传入数据
         * @return 返回数据
         */
        std::string koperation(int code, const nlohmann::json &json, JNIEnv * = manager->getEnv()) const;

        Config() {};

        void Init(JNIEnv * = manager->getEnv());

        ~Config();
    };

/// 全局Config对象
    Config *config = new Config();

/// MiraiCode父类, 指可以被转换成miraicode的类型
    class MiraiCodeable {
    public:
        virtual std::string toMiraiCode() = 0;
    };

/// @brief miraicode字符串
/// @attention MiraiCode会把非miraicode组成部分(非[mirai:])转码, 输出转码前的文本用toString, 参考: https://github.com/mamoe/mirai/blob/dev/docs/Messages.md#%E8%BD%AC%E4%B9%89%E8%A7%84%E5%88%99
    class MiraiCode {
    private:
        std::string content;
    public:
        /// 输出当前内容, 会自动转码
        std::string toString() {
            return Tools::escapeFromMiraiCode(this->content);
        }

        /// 和toString作用一样, 不过不会自动转码
        std::string toMiraiCode() {
            return content;
        }

        /// 从MiraiCodeable类型初始化一个miraicode字符串
        MiraiCode(MiraiCodeable *a) {
            content = a->toMiraiCode();
        }

        /// 从文本初始化一个miraicode字符串
        MiraiCode(std::string a) {
            content = a;
        }

        MiraiCode operator+(MiraiCodeable *a) {
            return MiraiCode(content + a->toMiraiCode());
        }

        MiraiCode operator+(std::string a) {
            return MiraiCode(content + a);
        }

        MiraiCode operator+=(MiraiCodeable *a) {
            return MiraiCode(this->content + a->toMiraiCode());
        }

        MiraiCode operator+=(std::string a) {
            return MiraiCode(this->content + a);
        }

        MiraiCode operator=(MiraiCodeable *a) {
            return MiraiCode(a->toMiraiCode());
        }

        MiraiCode operator=(std::string a) {
            return MiraiCode(a);
        }

        MiraiCode plus(MiraiCodeable *a) {
            return MiraiCode(content + a->toMiraiCode());
        }

        MiraiCode plus(std::string a) {
            return MiraiCode(content + a);
        }
    };

/*!
 * @class Logger
 * @brief 以MiraiCP的名义发送日志, 日志表现格式是: 2021-06-28 09:37:22 [log level]/MiraiCP: [log content], 为最底层的logger
 * @example 发送日志
	 *  发送消息级日志
	 *  @code logger->Info(string) @endcode
	 *  发送警告级日志
	 *  @code logger->Warning(string) @endcode
	 *  发送错误级日志
	 *  @code logger->Error(string) @endcode
 */
    class Logger_interface {
    protected:
        /// @brief 日志底层实现封装
        /// @param log 日志内容
        /// @param botid botid
        /// @param level 日志等级
        /// @param env jnienv
        virtual void log0(const std::string &log, int level, JNIEnv *env) = 0;

        jmethodID log = nullptr;
    public:
        jmethodID getjmethod() {
            return this->log;
        }

        /// @brief 封装lambda类型
        /// @param string 日志内容
        /// @param 日志级别
        ///     - 0 info
        ///     - 1 warning
        ///     - 2 error
        typedef std::function<void(std::string, int)> action;
        /// @brief loggerhandler会在每次log执行前执行一遍，可用于执行自定义的保存操作等
        struct handler {
            /// @brief 是否启用
            bool enable = true;
            /// @brief 执行的操作，格式为lambda
            action action = [](const std::string& content, int level) {};
        };
        handler loggerhandler;

        // 初始化 获取methodid
        void init(JNIEnv * = manager->getEnv());

        ///发送普通(info级日志)
        void Info(const std::string &, JNIEnv * = manager->getEnv());

        ///发送普通(info级日志)
        void Info(MiraiCode msg, JNIEnv *env = manager->getEnv()) {
            Info(msg.toString(), env);
        }

        ///发送警告(warning级日志)
        void Warning(const std::string &, JNIEnv * = manager->getEnv());

        ///发送警告(warning级日志)
        void Warning(MiraiCode msg, JNIEnv *env = manager->getEnv()) {
            Warning(msg.toString(), env);
        }

        ///发送错误(error级日志)
        void Error(const std::string &, JNIEnv * = manager->getEnv());

        ///发送错误(error级日志)
        void Error(MiraiCode msg, JNIEnv *env = manager->getEnv()) {
            Error(msg.toString(), env);
        }

        /// @brief 设置loggerhandler的action
        /// @param action 执行的操作
        /// @see Logger::handler
        /// @example 设置loggerhandler的action
        /// @code logger->registerHandle([](std::string content, int level){
        ///     \\do some things
        /// });@endcode
        inline void registerHandle(action action);

        /// @brief 设置handler的启用状态
        /// @param state 状态，启用或者关闭
        /// @example 设置handler的启用状态
        /// @code logger->setHandleState(ture); @endcode
        inline void setHandleState(bool state);
    };
    class Logger: public Logger_interface{
    protected:
        /// @brief 日志底层实现封装
        /// @param log 日志内容
        /// @param botid botid
        /// @param level 日志等级
        /// @param env jnienv
        void log0(const std::string &log, int level, JNIEnv *env);
    };

/// 带id(一般为bot账号)的logger
    class IdLogger : public Logger_interface {
    protected:
        void log0(const std::string &content, int level, JNIEnv *env) ;
    public:
        const unsigned long long id;

        IdLogger(unsigned long long id, Logger *l) : id(id) {
            this->loggerhandler = l->loggerhandler;
            this->log = l->getjmethod();
        }
    };

    /// 插件logger
    class PluginLogger : public Logger_interface {
    protected:
        void log0(const std::string &content, int level, JNIEnv *env);
    public:
        PluginLogger(Logger *l) {
            this->loggerhandler = l->loggerhandler;
            this->log = l->getjmethod();
        }
    };

/// 声明全局日志对象
    Logger *logger = new Logger();

/// 插件父类
    class CPPPlugin {
    public:
        /// @brief 插件信息
        const PluginConfig config;
        /// @brief 插件级logger
        static PluginLogger *pluginLogger;

        virtual void onEnable() {}

        virtual void onDisable() {}

        explicit CPPPlugin(PluginConfig c) : config(std::move(c)) {}
    };

    PluginLogger *CPPPlugin::pluginLogger = nullptr;
    CPPPlugin *plugin = nullptr;

/// @brief 总异常抽象类
/// @interface MiraiCPException
    class MiraiCPException : public std::exception {
    public:
        /// 异常信息
        virtual std::string what() { return "MiraiCP异常"; };

        /// raise 抛出方法
        virtual void raise() {
            if (logger != nullptr)
                logger->Error(what());
        };
    };

/// 文件读取异常.
/// @see MiraiCPException
    class UploadException : public MiraiCPException {
    public:
        explicit UploadException(std::string text) {
            this->description = "上传(图片/文件)异常" + text;
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
            logger->Error(this->description);
            //manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

/// 内部异常, 通常为json读写问题
/// @see MiraiCPException
    class APIException : public MiraiCPException {
    public:
        explicit APIException(std::string text) {
            this->description = "MiraiCP内部无法预料的错误:" + text;
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
            logger->Error(this->description);
            //manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

/// 机器人操作异常
/// @see MiraiCPException
    class BotException : public MiraiCPException {
    public:
        explicit BotException(std::string d = "没有权限执行该操作") {
            this->description = std::move(d);
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
            //manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

/// 禁言异常
/// @see MiraiCPException
    class MuteException : public MiraiCPException {
    public:
        /*
        *	 禁言时间超出0s~30d
        */
        MuteException() {
            this->description = "禁言时长不在0s~30d中间";
        }

        //返回错误信息
        std::string what() {
            return this->description;
        }

        void raise() {
            //manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

/// 获取群成员错误
/// @see MiraiCPException
    class MemberException : public MiraiCPException {
    public:
        /*
        *   "1" - 找不到群
        *	"2" - 找不到群成员
        */
        int type = 0;

        MemberException(int type) {
            this->type = type;
            switch (type) {
                case 1:
                    this->description = "找不到群";
                    break;
                case 2:
                    this->description = "找不到群成员";
                    break;
            }
        }

        //返回错误信息
        std::string what() {
            return this->description;
        }

        void raise() {
            //manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

/// 获取群成员错误
/// @see MiraiCPException
    class FriendException : public MiraiCPException {
    public:
        /*
        *   找不到好友
        */
        FriendException() {
            this->description = "找不到好友";
        }

        //返回错误信息
        std::string what() {
            return this->description;
        }

        void raise() {
            //manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

/// 获取群错误
/// @see MiraiCPException
    class GroupException : public MiraiCPException {
    public:
        GroupException() {
            this->description = "C++:找不到群";
        }

        //返回错误信息
        std::string what() {
            return this->description;
        }

        void raise() {
            //manager->getEnv()->ThrowNew(config->initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

/// 撤回异常
/// @see MiraiCPException
    class RecallException : public MiraiCPException {
    public:
        RecallException() {
            this->description = "该消息已经被撤回";
        }

        std::string what() {
            return this->description;
        }

    private:
        std::string description = "";
    };

/// 远程资源出现问题
/// @see MiraiCPException
    class RemoteAssetException : public MiraiCPException {
    public:
        RemoteAssetException(std::string e) {
            this->description = e;
        }

        std::string what() {
            return this->description;
        }

    private:
        std::string description = "";
    };

/// 参数错误
    class IllegalArgumentException : public MiraiCPException {
    private:
        std::string description = "";
    public:
        IllegalArgumentException(std::string e) {
            this->description = e;
        }

        std::string what() {
            return this->description;
        }

        void raise() {
            logger->Error(this->what());
        }
    };

    inline void ErrorHandle(const std::string &re, const std::string &ErrorMsg = "") {
        if (re == "EF")
            throw FriendException();
        if (re == "EG")
            throw GroupException();
        if (re == "EM")
            throw MemberException(1);
        if (re == "EMM")
            throw MemberException(2);
        if (re == "EA")
            throw APIException(ErrorMsg);
    }

/// 消息源声明
    class MessageSource {
    public:
        /// 消息的ids
        const std::string ids;
        /// 消息的internalids
        const std::string internalids;
        /// 消息源序列化
        const std::string source;

        MessageSource() {};

        /**
         * @brief 回复(引用并发送miraicode)
         * @param msg - MiraiCodeable类型指针 - 内容
         * @see MessageSource::quoteAndSendMiraiCode
        */
        MessageSource
        quoteAndSendMiraiCode(MiraiCodeable *msg, unsigned long long groupid = 0, JNIEnv *env = manager->getEnv()) {
            return quoteAndSendMiraiCode(msg->toMiraiCode(), groupid, env);
        }

        /// 回复(引用并发送miraicode)
        /// @see MessageSource::quoteAndSendMiraiCode
        MessageSource
        quoteAndSendMiraiCode(MiraiCode msg, unsigned long long groupid = 0, JNIEnv *env = manager->getEnv()) {
            return quoteAndSendMiraiCode(msg.toMiraiCode(), groupid, env);
        }

        /**
         * @brief 回复(引用并发送)
         * @param c 引用后发送的内容, 为纯文本
         * @param groupid 如果为发送给群成员需把该群成员的groupid传入以帮助获取到该成员
         * @return MessageSource
         * @example 回复(引用并发送)
         * @code
         *  e.messageSource.quoteAndSendMsg("HI");
         * @endcode
         */
        MessageSource
        quoteAndSendMsg(const std::string &c, unsigned long long groupid = 0, JNIEnv * = manager->getEnv());

        /**
         * @brief 回复(引用并发送)
         * @param c 引用后发送的内容, 为MiraiCode形式
         * @param groupid 如果为发送给群成员需把该群成员的groupid传入以帮助获取到该成员
         * @return MessageSource
         */
        MessageSource
        quoteAndSendMiraiCode(const std::string &c, unsigned long long groupid = 0, JNIEnv * = manager->getEnv());

        /*!
         * @brief 构建消息源
         * @param ids
         * @param internalids
         * @param source
         */
        MessageSource(std::string ids, std::string internalids, std::string source);

        /*!
         * @brief 从json字符串反序列化到MessageSource对象
         * @note json应该为以下格式
         * @code
         * {"ids":"", "internalids":""}
         * @endcode
         */
        static MessageSource deserializeFromString(const std::string &source);

        std::string serializeToString();

        /*!
         * @brief 撤回该信息
         * @example 撤回信息
         * @code
         * procession->registerEvent([](GroupMessageEvent e) {
            try {
                e.messageSource.recall();
                e.group.SendMsg("hi").recall();
            }
            catch (MiraiCPException &e) {
                logger->Error("错误");
            }
            });
         * @endcode
        */
        void recall(JNIEnv * = manager->getEnv());
    };

/// 图像类声明
    class Image : public MiraiCodeable {
    public:
        //图片id，样式:` {xxx}.xx `
        std::string id = "";

        /*!
        * @brief 从图片id构造，适用于服务器上已经有的图片，即接收到的
        * @example 图片miraiCode格式例子
         * @code [mirai:image:{图片id}.jpg] @endcode
        * @note 可以用这个正则表达式找出id ` \\[mirai:image:(.*?)\\] `
        */
        Image(std::string);

        /*
        * 获取图片下载url
        */
        std::string queryURL(JNIEnv * = manager->getEnv()) const;

        /*!
         * @brief 取一个miraicode字符串中全部的图片id，详情见Image
         * @see Image
         *  @param  miraicode的字符串
         *  @return vector容器，每一项为一个图片id
         *  @example 取一个miraicode字符串中全部的图片id
         *		@code
         *          vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
         *	        for (int i = 0; i < temp.size(); i++) {
         *	    	    logger->Info(temp[i]);
         *	    	    logger->Info("图片下载地址:" + Image(param.env, temp[i]).queryURL());
         *	         }
         *		@endcode
         */
        static std::vector<std::string> GetImgIdsFromMiraiCode(std::string);

        static std::vector<std::string> GetImgIdsFromMiraiCode(MiraiCode msg) {
            return GetImgIdsFromMiraiCode(msg.toMiraiCode());
        }

        /// 取图片Mirai码
        std::string toMiraiCode();
    };

/*!
 * @brief group, friend, member的父类
 */
    class Contact {
    protected:
        int _type = 0;
        unsigned long long _id;
        unsigned long long _groupid;
        std::string _nickOrNameCard;
        std::string _avatarUrl;
        unsigned long long _botid;
    public:
        /*!
         * @brief 无参初始化Contact类型
         * @internal 一般在MiraiCp内部构造
         */
        Contact() {
            this->_type = 0;
            this->_id = 0;
            this->_groupid = 0;
            this->_nickOrNameCard = "";
            this->_botid = 0;
        }

        /*!
         * @brief 构造contact类型
         * @param type 类型
         *  @see Contact::type()
         * @param id ID
         *  @see Contact::id()
         * @param gid 是member的时候是群id，否则为0
         *  @see Contact::groupid
         * @param name 群名片或昵称或群名
         *  @see Contact::name()
         * @param botid 对应的botid
         */
        Contact(int type, unsigned long long id, unsigned long long gid, std::string name, unsigned long long botid) {
            this->_type = type;
            this->_id = id;
            this->_groupid = gid;
            this->_nickOrNameCard = name;
            this->_botid = botid;
        };

        /// @brief 当前对象类型
        ///     - 1 Friend 好友
        ///     - 2 Group 群聊
        ///     - 3 Member 群成员
        int type() { return this->_type; }

        /// @brief id在全部情况存在
        ///     - 当当前type为1(Friend)时，为好友id
        ///     - 当当前type为2(Group)时，为群id
        ///     - 当当前type为3(Member)时，为群成员id
        unsigned long long id() { return this->_id; }

        /// @brief 当type为3的时候存在，否则为0，可以看作补充id
        ///     - 当当前type为1(Friend)时，为0
        ///     - 当当前type为2(Group)时，为0
        ///     - 当当前type为3(Member)时，为群号
        /// @attention 当当前type为2(Group)时，为0，不为群号，id才是群号
        unsigned long long groupid() { return this->_groupid; }

        /// 群名称，群成员群名片，或好友昵称
        std::string nickOrNameCard() { return this->_nickOrNameCard; };

        /// 头像url地址
        std::string avatarUrl() { return this->_avatarUrl; };

        /// 所属bot
        unsigned long long botid() { return this->_botid; };

        /// 序列化到json对象
        nlohmann::json serialization() {
            nlohmann::json j;
            j["type"] = type();
            j["id"] = id();
            j["groupid"] = groupid();
            j["nickornamecard"] = nickOrNameCard();
            j["botid"] = botid();
            return j;
        }

        /// 序列化成文本，可以通过deserializationFromString反序列化，利于保存
        /// @see Contact::fromString()
        std::string serializationToString() {
            return this->serialization().dump();
        }

        /*!
         * @brief 从json节点反序列化
         * @param root json节点
         * @return Contact
         */
        static Contact deserializationFromJson(nlohmann::json root);

        /// 反序列化成bot，可以通过serializationToString序列化，利于保存
        /// @see Contact::serializationToString()
        /// @param source 序列化后的文本
        /// @throw APIException
        static Contact deserializationFromString(const std::string &source);

        /**
         * @brief 发送miraicode
         * @param msg - MiraiCodeable类型指针 - 内容
        */
        MessageSource sendMiraiCode(MiraiCodeable *msg) {
            return sendMiraiCode(msg->toMiraiCode());
        }

        /// 发送miraicode
        MessageSource sendMiraiCode(MiraiCode msg) {
            return sendMiraiCode(msg.toMiraiCode());
        }

        /// 发送miraicode
        MessageSource sendMiraiCode(std::string msg, JNIEnv * = manager->getEnv());

        /// 发送纯文本信息
        MessageSource sendMsg(std::string msg, JNIEnv * = manager->getEnv());

        /// 发送纯文本信息
        MessageSource sendMsg(MiraiCode msg) {
            return sendMsg(msg.toString());
        }

        /// 发送语音
        MessageSource sendVoice(const std::string &path, JNIEnv * = manager->getEnv());

        /*!
        * @brief上传本地图片，务必要用绝对路径
        * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
        * @attention 最大支持图片大小为30MB
        * @throws
        * -可能抛出UploadException异常代表路径无效或大小大于30MB
        * -可能抛出MemberException找不到群或群成员
        */
        Image uploadImg(const std::string &filename, JNIEnv * = manager->getEnv());

        /// 刷新当前对象信息
        virtual void refreshInfo(JNIEnv * = manager->getEnv()) {};
    };

/*!
 * @brief 小程序卡片
 * @see LightAppStyle1, LightAppStyle2, LightAppStyle3
 * @example 通过常量构建并发送小程序卡片
 * @code
 * procession->registerEvent([](GroupMessageEvent e) {
		//修改里面的属性从而自定义
		LightAppStyle1 a = LightAppStyle1();
		LightAppStyle2 b = LightAppStyle2();
		LightAppStyle3 c = LightAppStyle3();
		e.group.SendMiraiCode(LightApp(a).toMiraiCode());
		e.group.SendMiraiCode(LightApp(b).toMiraiCode());
		e.group.SendMiraiCode(LightApp(c).toMiraiCode());
	});
 * @endcode
 * @example 通过文本构建并发送小程序卡片
 * @code
 * procession->registerEvent([](GroupMessageEvent e) {
		//风格1，适合文字展示，不能交互,无大图
		//图标地址，应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//小标题
		string titles = "{\"title\":\"hi\", \"value\":\"test3\"}";
		//下面的按钮，但是不能按
		string buttons = "{\"name\":\"Test4\",\"action\":\"plugin\",\"actionData\":\"https://baidu.com\"}";
		string x = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
			"{\"appName\":\"Test1\",\"appType\":4,\"appid\":1109659848,"
			"\"iconUrl\":\""+icon+"\"},"
			"\"data\":["+titles+"],"
			"\"title\":\"Test2\",\"button\":"
			"["+buttons+"],"
			"\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
		e.group.SendMiraiCode(LightApp(x).toString());
		//风格2，不能交互，有预览图
		//icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon1 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string preview1 = icon1;
		string a = "{\"config\":"
			"{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\""+preview1+"\",\"shareTemplateData\":{},"
			"\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test5\",\"title\":\"Test6\","
			"\"host\":{\"uin\":1000000,\"nick\":\"应用消息\"},"
			"\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\""+icon1+"\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
		e.group.SendMiraiCode(LightApp(a).toString());
		//风格3，可以跳转到网址，有预览图
		//跳转链接
		string url = "https://www.baidu.com";
		//icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string icon2 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
		//预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
		string preview = icon2;
		string b = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
			"\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
			"\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\""+preview+"\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test1\",\"title\":\"Test3\",\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\""+icon+"\",\"qqdocurl\":\""+url+"\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
		e.group.SendMiraiCode(LightApp(b).toString());
		});
 * @endcode
 */
    class LightApp : public MiraiCodeable {
    public:
        std::string content = "";

        /// @brief 使用纯文本构造，推荐使用其他结构体方法构造
        /// @param content 构造文本
        LightApp(std::string content) {
            this->content = content;
        }

        /// 使用样式1,适合文字展示，无大图，不能交互
        /// @param c 结构体，用于自定义里面的数据
        /// @see LightAppStyle1 in pch.h
        LightApp(LightAppStyle1 c) {
            this->content =
                    "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
                    "{\"appName\":\"" + c.appName + "\",\"appType\":4,\"appid\":1109659848,"
                                                    "\"iconUrl\":\"" + c.icon + "\"},"
                                                                                "\"data\":[" + c.titles + "],"
                                                                                                          "\"title\":\"" +
                    c.title + "\",\"button\":"
                              "[" + c.buttons + "],"
                                                "\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
        }

        /// 使用样式2，有大图，不能交互
        /// @param c 结构体，用于自定义里面的数据
        /// @see LightAppStyle1 in pch.h
        LightApp(LightAppStyle2 c) {
            this->content = "{\"config\":"
                            "{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
                            "\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
                            "\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\"" + c.preview +
                            "\",\"shareTemplateData\":{},"
                            "\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" +
                            c.title2 + "\",\"title\":\"" + c.title + "\","
                                                                     "\"host\":{\"uin\":0,\"nick\":\"\"},"
                                                                     "\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" +
                            c.icon + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
        }

        /// 样式3，有大图，可以在电脑qq显示，并在电脑上点击的链接会跳转
        /// @param c 结构体，用于自定义里面的数据
        /// @see LightAppStyle1 in pch.h
        LightApp(LightAppStyle3 c) {
            this->content =
                    "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
                    "\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
                    "\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" + c.preview +
                    "\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" +
                    c.title + "\",\"title\":\"" + c.description + "\","
                                                                  "\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" +
                    c.icon + "\",\"qqdocurl\":\"" + c.url + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
        }

        /// 返回miraicode
        std::string toMiraiCode() {
            return "[mirai:app:" + Tools::escapeToMiraiCode(content) + "]";
        }

        MessageSource sendTo(Contact c) {
            return c.sendMiraiCode(toMiraiCode());
        }
    };

// 群文件

/// @brief 下载信息
/// @see RemoteFile
    struct dinfo {
        /// 下载地址
        std::string url;
        /// md5 可用于校验
        std::string md5;
        /// sha1 可用于校验
        std::string sha1;
    };
/// @brief 文件信息
/// @see RemoteFile
    struct finfo {
        /// 文件大小
        unsigned long long size;
        /// 上传者id
        unsigned long long uploaderid;
        /// 下载次数
        unsigned int downloadtime;
        /// 上传时间, 时间戳格式
        unsigned long long uploadtime;
        /// 上次更改时间, 时间戳格式
        unsigned long long lastmodifytime;
    };

/// @brief 远程(群)文件类型
    class RemoteFile : public MiraiCodeable {
    public:
        /// 文件唯一id, 用于识别
        const std::string id;
        /// 文件内部id, 用于构造miraiCode发送
        const unsigned int internalid;
        /// 文件名
        const std::string name;
        /// 文件大小
        const long long size;
        /// 文件在群文件的路径
        const std::string path;
        /// 文件下载信息
        /// @see dinfo
        const dinfo dinfo;
        /// 文件信息
        /// @see finfo
        const finfo finfo;

        std::string serializeToString();

        static RemoteFile deserializeFromString(const std::string &source);

        /*!
         * @brief 构造远程(群)文件
         * @param i ids
         * @param ii internalids
         * @param n name
         * @param s size
         * @param p path
         * @param d dinfo
         * @param f finfo
         */
        RemoteFile(const std::string &i, unsigned int ii, const std::string &n, long long s, const std::string &p,
                   struct dinfo d, struct finfo f) : id(i),
                                                     internalid(
                                                             ii),
                                                     name(n),
                                                     size(s),
                                                     path(p),
                                                     dinfo(d),
                                                     finfo(f) {};

        /// 仅在上传后构建的有效, 即获取到internalid时(internalid != 0) 否则重新上传并重新获取internalid再转换
        std::string toMiraiCode() {
            if (internalid == 0) {
                // 重新上传
                throw RemoteAssetException("toMiraiCode error: internalid错误，重新上传");
            }
            return "[mirai:file:" + id + "," + std::to_string(internalid) + "," + Tools::escapeToMiraiCode(name) + "," +
                   std::to_string(size) + "]";
        }
    };

/// 较底层api
    class LowLevelAPI {
    public:
        /// @brief 抽象封装底层发送信息接口
        /// @param content 信息字符串
        /// @param c 目标Contact指针
        /// @param miraicode 是否为miraicode格式
        /// @param env JNIEnv
        /// @return
        static std::string send0(std::string content, Contact *c, bool miraicode, JNIEnv *env) {
            nlohmann::json j;
            nlohmann::json tmp;
            tmp["content"] = content;
            tmp["contact"] = c->serialization();
            j["source"] = tmp.dump();
            j["miraiCode"] = miraicode;
            return config->koperation(config->Send, j, env);
//		Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KSend,
//			Tools::str2jstring(j.dump().c_str(), env),
//			(jboolean)miraicode), env);
        }

        /// @brief 取该联系人的一些信息
        /// @param c 该联系人Contact指针
        /// @return json格式字符串，待解析
        static inline std::string getInfoSource(Contact *c, JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["source"] = c->serializationToString();
            return config->koperation(config->RefreshInfo, j, env);
//		Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KRefreshInfo,
//			Tools::str2jstring(c->serializationToString().c_str(),
//				env)));
        }

        /*!
         * @brief 上传图片
         * @param path 本地地址
         * @param c 上传的对象
         * @param env JNIEnv
         * @return string 待解析json
         */
        static inline std::string uploadImg0(std::string path, Contact *c, JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["fileName"] = path;
            j["source"] = c->serializationToString();
            return config->koperation(config->UploadImg, j, env);
//		Tools::jstring2str((jstring)env->CallObjectMethod(config->CPP_lib, config->KUploadImg,
//			Tools::str2jstring(path.c_str(), env),
//			Tools::str2jstring(c->serializationToString().c_str(),
//				env)));
        }

        /// 每个对象的必有信息
        struct info {
            std::string nickornamecard;
            std::string avatarUrl;
        };

        /// 获取每个对象必有信息
        /// @see LowLevelAPI::info
        static info info0(std::string source) {
            info re;
            nlohmann::json j = nlohmann::json::parse(source);
            re.avatarUrl = j["avatarUrl"];
            re.nickornamecard = j["nickornamecard"];
            return re;
        }
    };

///聊天记录里每个消息
    class ForwardNode {
    public:
        ///发送者id
        const unsigned long long id = 0;
        ///发送者昵称
        const std::string name = "";
        ///发送信息
        const std::string message = "";
        ///发送时间
        const int time = 0;

        /// @brief 聊天记录里的每条信息
        /// @param id - 发送者id
        /// @param name - 发送者昵称
        /// @param message - 发送的信息
        /// @param time - 发送时间，以时间戳记
        ForwardNode(const unsigned long long int id, const std::string &name, const std::string &message,
                    const int time)
                : id(id), name(name), message(message), time(time) {}

        /// @brief 构造聊天记录里每条信息
        /// @param c - 发送者的contact指针
        /// @param message - 发送的信息
        /// @param t - 发送时间，时间戳格式
        ForwardNode(Contact *c, std::string &message, int t) : id(c->id()), name(c->nickOrNameCard()), message(message),
                                                               time(t) {}
    };

///聊天记录, 由ForwardNode组成
/// @see class ForwardNode
    class ForwardMessage {
    public:
        /// json节点
        nlohmann::json sendmsg;

        /*!
        *@brief 构建一条聊天记录
        *@details 第一个参数是聊天记录发生的地方
        * 然后是每条信息
        *@example 构建聊天记录
         * @code
        *ForwardMessage(&e.group,
        *{
        *	ForwardNode(1930893235, "Eritque arcus", "hahaha", 1),
        *	ForwardNode(1930893235, "Eritque arcus", "hahaha", -1)
        *}).sendTo(&e.group);
        * @endcode
        */
        ForwardMessage(Contact *c, std::initializer_list<ForwardNode> nodes);

        /// 发送给群或好友或群成员
        MessageSource sendTo(Contact *c, JNIEnv * = manager->getEnv()) const;
    };

/// 当前bot账号信息
    class Bot {
    private:
        bool inited = false;
        std::string _nick;
        std::string _avatarUrl;

        void check() {
            if (!this->inited) {
                refreshInfo();
                this->inited = true;
            }
        }

    public:
        /// 该botid
        const unsigned long long id;

        /*!
         * @brief 刷新bot信息
         * @param env
         */
        void refreshInfo(JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["source"] = Contact(4, 0, 0, "", this->id).serializationToString();
            LowLevelAPI::info tmp = LowLevelAPI::info0(config->koperation(config->RefreshInfo, j, env));
//        Tools::jstring2str(
//                (jstring)env->CallObjectMethod(config->CPP_lib, config->KRefreshInfo, Tools::str2jstring(
//                        Contact(4, 0, 0, "", this->id).serializationToString().c_str(), env)))
            this->_avatarUrl = tmp.avatarUrl;
            this->_nick = tmp.nickornamecard;
        }

        /// 用id构建机器人
        Bot(unsigned long long i) : id(i) {}

        /// 昵称
        std::string nick() {
            check();
            return this->_nick;
        }

        /// 头像下载链接
        std::string avatarUrl() {
            check();
            return this->_avatarUrl;
        }

        /// 取好友列表
        std::vector<unsigned long long> getFriendList(JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["botid"] = this->id;
            std::string temp = config->koperation(config->QueryBFL, j, env);
//		        Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
//			config->KQueryBFL,
//			(jlong)this->id));
            return Tools::StringToVector(temp);
        }

        /// 好友列表string形式返回，利于保存
        std::string FriendListToString() {
            return Tools::VectorToString(getFriendList());
        }

        /// 取群列表
        std::vector<unsigned long long> getGroupList(JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["botid"] = this->id;
            std::string temp = config->koperation(config->QueryBGL, j, env);
//		        Tools::jstring2str((jstring)env->CallStaticObjectMethod(config->CPP_lib,
//			config->KQueryBGL,
//			(jlong)this->id));
            return Tools::StringToVector(temp);
        }

        /// 群列表string形式返回，利于保存
        std::string GroupListToString() {
            return Tools::VectorToString(getGroupList());
        }
    };

/// 好友类声明
    class Friend : public Contact {
    public:
        /*!
         * @brief 构建好友对象
         * @param friendid q号
         * @param botid 对应机器人id
         */
        Friend(unsigned long long friendid, unsigned long long botid, JNIEnv * = manager->getEnv());

        Friend(Contact c) : Contact(c) { refreshInfo(); };

        /// 删除好友(delete是C++关键字
        void deleteFriend(JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["contactSource"] = this->serializationToString();
            j["quit"] = true;
            ErrorHandle(config->koperation(config->RefreshInfo, j, env));
        }

        void refreshInfo(JNIEnv *env = manager->getEnv()) {
            std::string temp = LowLevelAPI::getInfoSource(this, env);
            if (temp == "E1") {
                throw FriendException();
            }
            LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
            this->_nickOrNameCard = tmp.nickornamecard;
            this->_avatarUrl = tmp.avatarUrl;
        }
    };

/// 群成员类声明
    class Member : public Contact {
    public:
        /// @brief 权限等级
        ///     - OWNER群主 为 2
        ///     - ADMINISTRATOR管理员 为 1
        ///     - MEMBER群成员 为 0
        /// @note 上面那些变量在constants.h中有定义
        unsigned int permission = 0;

        /// @brief 构建群成员对象
        /// @param qqid 该成员q号
        /// @param groupid 所在群号
        /// @param botid 机器人id
        /// @example 在事件中构建Member对象
        /// @code
        ///  Member(this.sender.id, this.group.id, this.bot.id)
        /// @endcode
        Member(unsigned long long qqid, unsigned long long groupid, unsigned long long botid,
               JNIEnv * = manager->getEnv());

        Member(Contact c) : Contact(c) { refreshInfo(); };

        void refreshInfo(JNIEnv *env = manager->getEnv()) override {
            std::string temp = LowLevelAPI::getInfoSource(this, env);
            if (temp == "E1")
                throw MemberException(1);
            if (temp == "E2")
                throw MemberException(2);
            LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
            this->_nickOrNameCard = tmp.nickornamecard;
            this->_avatarUrl = tmp.avatarUrl;
            this->permission = getPermission();
            if (temp == "E1") {
                throw MemberException(1);
            }
            if (temp == "E2") {
                throw MemberException(2);
            }
        }

        /// 获取权限，会在构造时调用，请使用permission缓存变量
        /// @see Member::permission
        unsigned int getPermission(JNIEnv * = manager->getEnv());

        /*!
         * 禁言当前对象，单位是秒，最少0秒最大30天
         * @throws 可能抛出不同错误
        */
        void Mute(int time, JNIEnv * = manager->getEnv());

        /*! 踢出这个群成员
        * @param reason - 原因
         * @example 踢出群成员
         * @code
         * procession->registerEvent([](GroupMessageEvent e) {
            try {
                Member m = Member(qqid, e.group.id(), e.bot.id());
                m.Kick("this_is_reason");
            }
            catch (BotException err) {
                //权限不足
                logger->Error(err.what());
            }
            catch (MemberException err) {
                if (err.type == 1) {
                    //找不到群
                }
                if (err.type == 2) {
                    //找不到群成员
                }
            }
            });
         * @endcode
        */
        void Kick(const std::string &reason, JNIEnv * = manager->getEnv());
    };

/// 群聊类声明
    class Group : public Contact {
    public:
        /**
         * @brief 群设置
         * @details 使用uploadSetting上传设置，使用refreshInfo同步服务器设定，后面两项由于https://github.com/mamoe/mirai/issues/1307 还不能改
         */
        struct GroupSetting {
            /// 群名称
            std::string name;
            /// 入群显示公告 **目前暂时不能用**
            /// @see https://github.com/Nambers/MiraiCP/issues/61
            std::string entranceAnnouncement;
            /// 禁言全部
            bool isMuteAll;
            /// 允许群成员邀请
            bool isAllowMemberInvite;
            /// 自动同意进群
            bool isAutoApproveEnabled;
            /// 允许匿名聊天
            bool isAnonymousChatEnabled;
        };
        /// 群设置
        GroupSetting setting;

        /**
         * @brief 更新群设置, 即覆盖服务器上的群设置
         * @details 从服务器拉去群设置用refreshInfo
         * @see Group::refreshInfo()
         */
        void updateSetting(JNIEnv * = manager->getEnv());

        /// 取群成员列表
        /// @return vector<long>
        std::vector<unsigned long long> getMemberList(JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["contactSource"] = this->serializationToString();
            std::string re = config->koperation(config->QueryML, j, env);
//		        Tools::jstring2str((jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib,
//			config->KQueryML,
//			Tools::str2jstring(
//				this->serializationToString().c_str())));
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
        Member getOwner(JNIEnv * = manager->getEnv());

        ///  @brief 构建以群号构建群对象
        /// @param groupid 群号
        /// @param botid 机器人id
        /// @example 在事件中构建Group对象
        /// @code Group(this.group.id, this.bot.id) @endcode
        Group(unsigned long long groupid, unsigned long long botid, JNIEnv * = manager->getEnv());

        Group(Contact c) : Contact(std::move(c)) { refreshInfo(); };

        /// 刷新群聊信息
        void refreshInfo(JNIEnv *env = manager->getEnv()) override {
            std::string re = LowLevelAPI::getInfoSource(this, env);
            ErrorHandle(re);
            LowLevelAPI::info tmp = LowLevelAPI::info0(re);
            this->_nickOrNameCard = tmp.nickornamecard;
            this->_avatarUrl = tmp.avatarUrl;
            nlohmann::json j = nlohmann::json::parse(re)["setting"];
            this->setting.name = j["name"];
            this->setting.entranceAnnouncement = j["entranceAnnouncement"];
            this->setting.isMuteAll = j["isMuteAll"];
            this->setting.isAllowMemberInvite = j["isAllowMemberInvite"];
            this->setting.isAutoApproveEnabled = j["isAutoApproveEnabled"];
            this->setting.isAnonymousChatEnabled = j["isAnonymousChatEnabled"];
        }

        void quit(JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["contactSource"] = this->serializationToString();
            j["quit"] = true;
            ErrorHandle(config->koperation(config->RefreshInfo, j, env));
        }

        /*!
        @brief 上传并发送远程(群)文件
        @param path-群文件路径(带文件名),根目录为/
        @param filepath-本地文件路径
        @example 上传并发送远程(群)文件
         @code
         procession->registerEvent([](GroupMessageEvent e) {
            // 发送D:\\ValveUnhandledExceptionFilter.txt本地文件到qq群的 /test.txt 路径
            RemoteFile tmp = e.group.sendFile("/test.txt", "D:\\ValveUnhandledExceptionFilter.txt");
            });
         @endcode
        */
        RemoteFile sendFile(const std::string &path, const std::string &filepath, JNIEnv * = manager->getEnv());

        /*!
        取群文件信息,会自动搜索子目录
        @param path-群文件路径(不带文件名)
        @param id-文件id,可空，空则为用路径查找(此时路径要带文件名)
        @attention 因为群文件允许重名文件存在的特性，该查找并不可靠，只能返回重名文件中的其中一个文件
        @see RemoteFile
         @example 取群文件信息
         @code
         //根据qq群远程路径(不带文件名)和文件id, 文件id可以在上传返回的RemoteFile类型中获得, 会在子目录中查找如果当前目录未找到
         //根据qq群远程路径(带文件名)找, 不过由于qq群文件允许同名文件这一特性, 返回的文件为群文件中同名文件中随机一个
         procession->registerEvent([](GroupMessageEvent e) {
            e.group.SendMsg(e.group.getFile("/", id).name());
            e.group.SendMsg(e.group.getFile("/test.txt").name());
            e.group.SendMsg(e.group.getFileListString("/"));
            });
         @endcode
        */
        RemoteFile getFile(const std::string &path, const std::string &id = "", JNIEnv * = manager->getEnv());

        /// 群文件的简短描述
        struct file_short_info {
            // 路径带文件名
            std::string path = "";
            // 唯一id
            std::string id = "";
        };

        /*!
         * 获取path路径下全部文件信息
         * @param path - 远程路径
         * @return 返回值为一个vector容器, 每一项为short_info
        */
        std::vector<file_short_info> getFileList(const std::string &path, JNIEnv * = manager->getEnv());

        /// 取文件列表以字符串形式返回
        ///@example 取string格式群文件列表
        /// @code
        /// e.group.SendMsg(e.group.getFileListString("/"));
        /// @endcode
        std::string getFileListString(const std::string &path, JNIEnv * = manager->getEnv());
    };

/// At一个群成员
    inline MiraiCode At(Member a) {
        /*返回at这个人的miraicode*/
        return MiraiCode("[mirai:at:" + std::to_string(a.id()) + "]");
    }

/// At一个群成员
    inline MiraiCode At(unsigned long long a) {
        /*返回at这个人的miraicode*/
        return MiraiCode("[mirai:at:" + std::to_string(a) + "]");
    }

/// 所以事件处理timeoutevent都是机器人事件，指都有机器人实例
    class BotEvent {
    public:
        /// 该事件接受的机器人
        Bot bot;
        /// 以该机器人的名义发送日志
        /// @see BotLogger
        IdLogger botlogger;

        BotEvent(unsigned long long botid) : bot(botid), botlogger(botid, logger) {
        }
    };

///群消息事件声明
    class GroupMessageEvent : public BotEvent {
    public:
        ///来源群
        Group group;
        ///发送人
        Member sender;
        ///信息本体
        MiraiCode message;
        ///消息源
        MessageSource messageSource;

        GroupMessageEvent(unsigned long long int botid, const Group &group, const Member &sender,
                          const std::string &message, const MessageSource &messageSource) : BotEvent(botid),
                                                                                            group(group),
                                                                                            sender(sender),
                                                                                            message(message),
                                                                                            messageSource(
                                                                                                    messageSource) {}
    };

/// 私聊消息事件类声明
    class PrivateMessageEvent : public BotEvent {
    public:
        /// 发起人
        Friend sender;
        /// 附带消息
        MiraiCode message;
        /// 信息源
        MessageSource messageSource;

        /*!
         * @brief 构建私聊信息
         * @param botid 对应botid
         * @param sender 发送者
         * @param message 消息
         * @param messageSource 消息源
         */
        PrivateMessageEvent(unsigned long long int botid, const Friend sender, const std::string &message,
                            const MessageSource &messageSource) : BotEvent(botid), sender(sender), message(message),
                                                                  messageSource(messageSource) {}
    };

/// 群聊邀请事件类声明
    class GroupInviteEvent : public BotEvent {
    public:
        /// 事件序列化文本
        const std::string source;
        /// 发起人昵称
        const std::string inviterNick = "";
        /// 发起人id
        const unsigned long long inviterid = 0;
        /// 被邀请进的组
        const std::string groupName = "";
        /// 群号
        const unsigned long long groupid = 0;

        static void reject(std::string source, JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["text"] = source;
            j["sign"] = false;
            std::string re = config->koperation(config->Gioperation, j, env);
            if (re == "Y") return;
            if (re == "E")logger->Error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
        }

        void reject() {
            this->reject(this->source);
        }

        std::string getSource() {
            return this->source;
        }

        static void accept(std::string source, JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["text"] = source;
            j["sign"] = true;
            std::string re = config->koperation(config->Gioperation, j, env);
//		        Tools::jstring2str(
//			(jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KGioperation,
//				Tools::str2jstring(source.c_str()),
//				(jboolean)true));
            if (re == "Y") return;
            if (re == "E")logger->Error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
        }

        void accept() {
            this->accept(this->source);
        }

        /*!
         * @brief 群邀请事件
         * @param botid 当前botid
         * @param source 序列化后字符串
         * @param inviterNick 邀请人昵称
         * @param inviterid 邀请人id
         * @param groupName 群聊名称
         * @param groupid 群号
         */
        GroupInviteEvent(unsigned long long int botid, const std::string &source, const std::string &inviterNick,
                         unsigned long long int inviterid, const std::string &groupName, unsigned long long int groupid)
                : BotEvent(botid), source(source), inviterNick(inviterNick), inviterid(inviterid), groupName(groupName),
                  groupid(groupid) {}
    };

/// 好友申请事件声明
    class NewFriendRequestEvent : public BotEvent {
    public:
        /// @brief 序列化的事件信息
        const std::string source;
        /// @brief 对方id
        const unsigned long long fromid;
        const unsigned long long fromgroupid;
        /// @brief 对方昵称
        const std::string nick;
        /// @brief 申请理由
        const std::string message;

        /// @brief 拒绝好友申请
        /// @param source 事件序列化信息
        static void reject(std::string source, JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["text"] = source;
            j["sign"] = false;
            std::string re = config->koperation(config->Nfroperation, j, env);
//		        Tools::jstring2str(
//			(jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KNfroperation,
//				Tools::str2jstring(source.c_str()),
//				(jboolean)true));
            if (re == "Y") return;
            if (re == "E")logger->Error("好友申请事件拒绝失败(可能因为重复处理),id:" + source);
        }

        /// @brief 拒绝好友申请
        void reject() {
            this->reject(this->source);
        }

        /// @brief 接受好友申请
        /// @param source 事件序列化信息
        static void accept(std::string source, JNIEnv *env = manager->getEnv()) {
            nlohmann::json j;
            j["text"] = source;
            j["sign"] = true;
            std::string re = config->koperation(config->Nfroperation, j, env);
//		        Tools::jstring2str(
//			(jstring)manager->getEnv()->CallStaticObjectMethod(config->CPP_lib, config->KNfroperation,
//				Tools::str2jstring(source.c_str()),
//				(jboolean)true));
            if (re == "Y") return;
            if (re == "E")logger->Error("好友申请事件同意失败(可能因为重复处理),id:" + source);
        }

        /// @brief 接受申请
        void accept() {
            this->accept(this->source);
        }

        /*!
         * @brief 好友申请事件
         * @param botid 对应botid
         * @param source 序列化后信息
         * @param fromid 对方id
         * @param fromgroupid 从哪个群申请的，否则为0
         * @param nick 对方昵称
         * @param message 申请理由
         */
        NewFriendRequestEvent(unsigned long long int botid, const std::string &source,
                              const unsigned long long int fromid,
                              const unsigned long long int fromgroupid, const std::string &nick,
                              const std::string &message)
                : BotEvent(botid), source(source), fromid(fromid), fromgroupid(fromgroupid), nick(nick),
                  message(message) {}
    };

/// 新群成员加入
    class MemberJoinEvent : public BotEvent {
    public:
        /*!
        * @brief 事件类型
        *   1 - 被邀请进来
        *   2 - 主动加入
        *   3 - 原群主通过 https://huifu.qq.com/ 恢复原来群主身份并入群
        */
        const int type = 0;
        ///新进入的成员
        Member member;
        ///目标群
        Group group;
        ///邀请人, 当type = 1时存在，否则则和member变量相同
        const unsigned long long inviterid;

        /*!
         * @brief 新群成员入群事件
         * @param botid botid
         * @param type 类别 @see MemberJoinEvent::type
         * @param member 入群群成员
         * @param group 群组
         * @param inviterid 邀请群成员id，如果不存在和member id参数一致
         */
        MemberJoinEvent(unsigned long long int botid, const int type, const Member &member, const Group &group,
                        const unsigned long long &inviterid) : BotEvent(botid), type(type), member(member),
                                                               group(group),
                                                               inviterid(inviterid) {}
    };

/// 群成员离开
    class MemberLeaveEvent : public BotEvent {
    public:
        /*!
        * @brief 事件类型
        *           1 - 被踢出
        *           2 - 主动退出
        */
        const int type = 0;
        /// 退出的成员q号
        const unsigned long long memberid;
        /// 目标群
        Group group;
        /// 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
        const unsigned long long operaterid;

        /*!
         * @brief 群成员离开
         * @param botid
         * @param type
         * @param memberid 退出的群成员
         * @param group 群
         * @param operaterid 操作人id, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
         */
        MemberLeaveEvent(unsigned long long int botid, const int type, const unsigned long long memberid,
                         const Group &group,
                         const unsigned long long &operaterid) : BotEvent(botid), type(type), memberid(memberid),
                                                                 group(group),
                                                                 operaterid(operaterid) {}
    };

/// 撤回信息
    class RecallEvent : public BotEvent {
    public:
        /// 为1时是好友私聊中撤回，为2时为群聊内撤回
        const int type = 0;
        /// 时间戳
        const int time = 0;
        /// 原发送者
        const unsigned long long authorid = 0;
        /// 撤回者
        const unsigned long long operatorid = 0;
        /// 信息id
        const std::string ids;
        //内部ids
        const std::string internalids;
        //当type是2的时候存在，否则为0
        const unsigned long long groupid = 0;

        /*!
         * @brief 撤回事件
         * @param botid 对应bot
         * @param type 类型
         * @param time 时间
         * @param authorid 发送者id
         * @param operatorid 撤回者id
         * @param ids 消息源ids
         * @param internalids 消息源internalids
         * @param groupid
         */
        RecallEvent(unsigned long long int botid, const int type, const int time, const unsigned long long int authorid,
                    const unsigned long long int operatorid, std::string ids, const std::string &internalids,
                    const unsigned long long int groupid) : BotEvent(botid), type(type), time(time), authorid(authorid),
                                                            operatorid(operatorid), ids(std::move(ids)),
                                                            internalids(internalids),
                                                            groupid(groupid) {}
    };

/// 机器人进入某群
    class BotJoinGroupEvent : public BotEvent {
    public:
        /// 1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
        const int type;
        /// 进入的群
        Group group;
        /// 当type=2时存在，为邀请人，否则为空，调用可能会报错
        const unsigned long long inviterid;

        /*!
         * @brief bot加入群
         * @param botid 对应bot
         * @param type 类别
         * @param group 加入的群
         * @param inviter 邀请人
         */
        BotJoinGroupEvent(unsigned long long int botid, const int type, Group group,
                          const unsigned long long inviter)
                : BotEvent(botid), type(type), group(std::move(group)), inviterid(inviterid) {}
    };

/// 群临时会话
    class GroupTempMessageEvent : public BotEvent {
    public:
        /// 来源群
        Group group;
        /// 发送人
        Member sender;
        /// 信息本体
        const MiraiCode message;
        /// 消息源
        MessageSource messageSource;

        /*!
         * @brief 群临时会话消息事件
         * @param botid 对应bot
         * @param group 发起的群
         * @param sender 发送消息对象
         * @param message 消息
         * @param messageSource 消息源
         */
        GroupTempMessageEvent(unsigned long long int botid, Group group, const Member &sender,
                              const std::string &message, MessageSource messageSource) : BotEvent(botid),
                                                                                                group(std::move(group)),
                                                                                                sender(sender),
                                                                                                message(message),
                                                                                                messageSource(std::move(
                                                                                                        messageSource)) {}
    };

    class BotOnlineEvent : public BotEvent {
    public:
        BotOnlineEvent(unsigned long long botid) : BotEvent(botid) {}
    };

/**监听类声明*/
    class Event {
    private:
        class Node0 {
        public:
            Node0 *nextNode = nullptr;
            bool enable = true;
        };

        template<class T>
        class Node : public Node0 {
        public:
            std::function<void(T)> f = [](T) -> void {};
            Node *next = nullptr;
        };

        Node<GroupMessageEvent> *GMHead = new Node<GroupMessageEvent>();
        Node<PrivateMessageEvent> *PMHead = new Node<PrivateMessageEvent>();
        Node<GroupInviteEvent> *GHead = new Node<GroupInviteEvent>;
        Node<NewFriendRequestEvent> *NFHead = new Node<NewFriendRequestEvent>();
        Node<MemberJoinEvent> *MJHead = new Node<MemberJoinEvent>();
        Node<MemberLeaveEvent> *MLHead = new Node<MemberLeaveEvent>();
        Node<RecallEvent> *RHead = new Node<RecallEvent>();
        Node<BotJoinGroupEvent> *BHead = new Node<BotJoinGroupEvent>();
        Node<GroupTempMessageEvent> *GTMHead = new Node<GroupTempMessageEvent>();
        Node<BotOnlineEvent> *BOHead = new Node<BotOnlineEvent>();

        /// 取链表首节点
        template<class T>
        Node<T> *head() {
            if constexpr(std::is_same_v<T, GroupMessageEvent>) {
                return GMHead;
            } else if constexpr(std::is_same_v<T, PrivateMessageEvent>) {
                return PMHead;
            } else if constexpr(std::is_same_v<T, GroupInviteEvent>) {
                return GHead;
            } else if constexpr(std::is_same_v<T, NewFriendRequestEvent>) {
                return NFHead;
            } else if constexpr(std::is_same_v<T, MemberJoinEvent>) {
                return MJHead;
            } else if constexpr(std::is_same_v<T, MemberLeaveEvent>) {
                return MLHead;
            } else if constexpr(std::is_same_v<T, RecallEvent>) {
                return RHead;
            } else if constexpr(std::is_same_v<T, BotJoinGroupEvent>) {
                return BHead;
            } else if constexpr(std::is_same_v<T, GroupTempMessageEvent>) {
                return GTMHead;
            } else if constexpr(std::is_same_v<T, BotOnlineEvent>) {
                return BOHead;
            }
            logger->Error("内部错误, 位置:C-Head");
            return nullptr;
        }

        Node<GroupMessageEvent> *GMTail = GMHead;
        Node<PrivateMessageEvent> *PMTail = PMHead;
        Node<GroupInviteEvent> *GTail = GHead;
        Node<NewFriendRequestEvent> *NFTail = NFHead;
        Node<MemberJoinEvent> *MJTail = MJHead;
        Node<MemberLeaveEvent> *MLTail = MLHead;
        Node<RecallEvent> *RTail = RHead;
        Node<BotJoinGroupEvent> *BTail = BHead;
        Node<GroupTempMessageEvent> *GTMTail = GTMHead;
        Node<BotOnlineEvent> *BOTail = BOHead;

        /// 取链表尾节点
        template<class T>
        bool *tail(std::function<void(T)> f) {
            Node<T> *temp = new Node<T>();
            temp->f = f;
            if constexpr(std::is_same_v<T, GroupMessageEvent>) {
                GMTail->next = temp;
                GMTail->nextNode = temp;
                GMTail = temp;
            } else if constexpr(std::is_same_v<T, PrivateMessageEvent>) {
                PMTail->next = temp;
                PMTail->nextNode = temp;
                PMTail = temp;
            } else if constexpr(std::is_same_v<T, GroupInviteEvent>) {
                GTail->next = temp;
                GTail->nextNode = temp;
                GTail = temp;
            } else if constexpr(std::is_same_v<T, NewFriendRequestEvent>) {
                NFTail->next = temp;
                NFTail->nextNode = temp;
                NFTail = temp;
            } else if constexpr(std::is_same_v<T, MemberJoinEvent>) {
                MJTail->next = temp;
                MJTail->nextNode = temp;
                MJTail = temp;
            } else if constexpr(std::is_same_v<T, MemberLeaveEvent>) {
                MLTail->next = temp;
                MLTail->nextNode = temp;
                MLTail = temp;
            } else if constexpr(std::is_same_v<T, RecallEvent>) {
                RTail->next = temp;
                RTail->nextNode = temp;
                RTail = temp;
            } else if constexpr(std::is_same_v<T, BotJoinGroupEvent>) {
                BTail->next = temp;
                BTail->nextNode = temp;
                BTail = temp;
            } else if constexpr(std::is_same_v<T, GroupTempMessageEvent>) {
                GTMTail->next = temp;
                GTMTail->nextNode = temp;
                GTMTail = temp;
            } else if constexpr(std::is_same_v<T, BotOnlineEvent>) {
                BOTail->next = temp;
                BOTail->nextNode = temp;
                BOTail = temp;
            } else {
                logger->Error("内部错误, 位置:C-Tail");
                return nullptr;
            }
            return &temp->enable;
        }

    public:
        /// 事件监听操控, 可用于stop停止监听和resume继续监听
        class NodeHandle {
        private:
            bool *enable;
        public:
            NodeHandle(bool *a) {
                this->enable = a;
            }

            void stop() const {
                *enable = false;
            }

            void resume() const {
                *enable = true;
            }
        };

        /*!
        * 广播函数重载
        */
        template<class T>
        void broadcast(T e) {
            Node<T> *now = Event::head<T>();
            while (now) {
                if (now->enable) { now->f(e); }
                now = now->next;
            }
        }

        /*!
        * @brief 监听函数
        * @note 在极其少见(MiraiCP内部出问题的时候)会返回nullptr
        */
        template<class T>
        NodeHandle registerEvent(std::function<void(T)> f) {
            bool *e = Event::tail(f);
            if (e != nullptr)
                return NodeHandle(e);
            return nullptr;
        }

        ~Event();
    };

/*声明全局监听对象(广播源)*/
    Event *procession = new Event();

/*线程管理*/

// 结束MiraiCP类声明代码
// 开始MiraiCP实现代码

    void ThreadManager::setEnv(JNIEnv *e) {
        mtx.lock();
        if (!this->included(ThreadManager::getThreadId())) {
            t tmp{
                    e,
                    false
            };
            this->_threads.insert(std::pair<std::string, t>(ThreadManager::getThreadId(), tmp));
        } else {
            this->_threads[ThreadManager::getThreadId()].e = e;
        }
        mtx.unlock();
    }

    void ThreadManager::newEnv(const char *threadName) {
        JNIEnv *env = nullptr;
        JavaVMAttachArgs args{
                JNIVersion,
                const_cast<char *>(threadName),
                nullptr
        };
        gvm->AttachCurrentThread((void **) &env, &args);
        t tmp{env, true};
        this->_threads.insert(std::pair<std::string, t>(ThreadManager::getThreadId(), tmp));
        logger->Info("refresh env");
    };

    void ThreadManager::detach() {
        mtx.lock();
        if (this->included(ThreadManager::getThreadId())) {
            bool att = this->_threads[ThreadManager::getThreadId()].attach;
            this->_threads.erase(ThreadManager::getThreadId());
            if (att)
                gvm->DetachCurrentThread();
        }
        mtx.unlock();
    }

    bool ThreadManager::included(const std::string &id) {
        if (this->_threads.empty() || this->_threads.count(id) == 0)
            return false;
        return true;
    }

    JNIEnv *ThreadManager::getEnv() {
        mtx.lock();
        if (!this->included(getThreadId())) {
            this->newEnv();
        }
        JNIEnv *tmp = this->_threads[ThreadManager::getThreadId()].e;
        mtx.unlock();
        return tmp;
    }

/*
日志类实现
throw: InitException 即找不到签名
*/

    void Logger_interface::init(JNIEnv *env) {
        this->log = env->GetStaticMethodID(config->CPP_lib, "KSendLog", "(Ljava/lang/String;I)V");
    }

    void Logger_interface::registerHandle(Logger_interface::action action) {
        this->loggerhandler.action = std::move(action);
    }

    void Logger_interface::setHandleState(bool state) {
        this->loggerhandler.enable = state;
    }

    void Logger_interface::Warning(const std::string &content, JNIEnv *env) {
        this->log0(content, 1, env);
    }

    void Logger_interface::Error(const std::string &content, JNIEnv *env) {
        this->log0(content, 2, env);
    }

    void Logger_interface::Info(const std::string &content, JNIEnv *env) {
        this->log0(content, 0, env);
    }

    void Logger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = -2;
        j["log"] = content;
        env->CallStaticVoidMethod(config->CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

    void IdLogger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = id;
        j["log"] = content;
        env->CallStaticVoidMethod(config->CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }
    void PluginLogger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = -1;
        j["name"] = plugin->config.name;
        j["log"] = content;
        env->CallStaticVoidMethod(config->CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

/*
配置类实现
throw: InitxException 即找不到对应签名
*/

    void Config::Init(JNIEnv *env) {
        this->CPP_lib = reinterpret_cast<jclass>(env->NewGlobalRef(
                env->FindClass("tech/eritquearcus/miraicp/shared/CPP_lib")));
        if (this->CPP_lib == nullptr) {
            throw APIException("初始化错误，找不到CPP_lib类");
        }
        this->KOperation = env->GetStaticMethodID(CPP_lib, "KOperation", "(Ljava/lang/String;)Ljava/lang/String;");
    }

    Config::~Config() {
        manager->getEnv()->DeleteGlobalRef(this->CPP_lib);
    }

    std::string Config::koperation(int type, const json &data, JNIEnv *env) const {
        json j;
        j["type"] = type;
        j["data"] = data;
        jstring re = (jstring) env->CallStaticObjectMethod(this->CPP_lib, this->KOperation,
                                                           Tools::str2jstring(j.dump().c_str(), env));
        return Tools::jstring2str(re, env);
    }

    Event::~Event() {
        Node0 *temp[] = {GMHead, PMHead, GHead, NFHead, MJHead, MLHead, RHead, BHead};
        for (Node0 *ptr : temp) {
            Node0 *now = ptr;
            Node0 *t = nullptr;
            while (true) {
                t = now;
                if (now->nextNode == nullptr) {
                    delete now;
                    break;
                }
                now = now->nextNode;
                delete t;
            }
        }
    }

    void MessageSource::recall(JNIEnv *env) {
        json j;
        j["source"] = this->serializeToString();
        std::string re = config->koperation(config->Recall, j, env);
        if (re == "Y") return;
        if (re == "E1") throw BotException();
        if (re == "E2") throw RecallException();
    }

    MessageSource::MessageSource(std::string ids, std::string internalids, std::string source) : ids(std::move(
            ids)), internalids(std::move(internalids)), source(std::move(source)) {}

    std::string MessageSource::serializeToString() {
        return source;
    }

    MessageSource MessageSource::deserializeFromString(const std::string &source) {
        json j = json::parse(source);
        try {
            return MessageSource(j["ids"].dump(), j["internalIds"].dump(), source);
        }
        catch (json::type_error &e) {
            logger->Error("消息源序列化出错，格式不符合(MessageSource::deserializeFromString)");
            logger->Error(source);
            logger->Error(e.what());
            throw e;
        }
    }

//远程文件(群文件)
    RemoteFile RemoteFile::deserializeFromString(const std::string &source) {
        json j;
        try {
            j = json::parse(source);
        }
        catch (json::parse_error &e) {
            logger->Error("格式化json失败，RemoteFile::deserializeFromString");
            logger->Error(source);
            logger->Error(e.what());
            throw e;
        }
        try {
            struct dinfo d{
                    j["dinfo"]["url"],
                    j["dinfo"]["md5"],
                    j["dinfo"]["sha1"]
            };
            struct finfo f{
                    j["finfo"]["size"],
                    j["finfo"]["uploaderid"],
                    j["finfo"]["downloadtime"],
                    j["finfo"]["uploadtime"],
                    j["finfo"]["lastmodifytime"]
            };
            return RemoteFile(j["id"], j["internalid"], j["name"], j["finfo"]["size"], j["path"], d, f);
        }
        catch (json::type_error &e) {
            logger->Error("json格式化失败，位置:RemoteFile");
            logger->Error(source);
            logger->Error(e.what());
            throw e;
        }
    }

    std::string RemoteFile::serializeToString() {
        json j;
        j["dinfo"]["url"] = this->dinfo.url;
        j["dinfo"]["md5"] = this->dinfo.md5;
        j["dinfo"]["shar1"] = this->dinfo.sha1;
        j["finfo"]["size"] = this->finfo.size;
        j["finfo"]["uploaderid"] = this->finfo.uploaderid;
        j["finfo"]["downloadtime"] = this->finfo.downloadtime;
        j["finfo"]["uploadtime"] = this->finfo.uploadtime;
        j["finfo"]["lastmodifytime"] = this->finfo.lastmodifytime;
        j["id"] = this->id;
        j["internalid"] = this->internalid;
        j["name"] = this->name;
        j["size"] = this->size;
        return j.dump();
    }

//发送这个聊天记录
    MessageSource ForwardMessage::sendTo(Contact *c, JNIEnv *env) const {
        json temp;
        json text;
        text["id"] = c->id();
        text["groupid"] = c->groupid();
        text["type"] = c->type();
        text["content"] = sendmsg;
        temp["text"] = text.dump();
        temp["botid"] = c->botid();
        std::string re = config->koperation(config->Buildforward, temp, env);
        ErrorHandle(re);
        //TODO:https://github.com/mamoe/mirai/issues/1371
        return MessageSource::deserializeFromString(re);
    }

    ForwardMessage::ForwardMessage(Contact *c, std::initializer_list<ForwardNode> nodes) {
        json root;
        json value;
        root["type"] = c->type();
        root["id"] = c->id();
        root["id2"] = c->groupid();
        for (const ForwardNode &node : nodes) {
            json temp;
            temp["id"] = node.id;
            temp["time"] = node.time;
            temp["message"] = node.message;
            temp["name"] = node.name;
            value.push_back(temp);
        }
        root["value"] = value;
        sendmsg = root;
    }

/*图片类实现*/
    Image::Image(std::string imageId) {
        this->id = std::move(imageId);
    }

    std::string Image::queryURL(JNIEnv *env) const {
        json j;
        j["id"] = this->id;
        std::string re = config->koperation(config->QueryImgUrl, j, env);
        if (re == "E1")
            throw RemoteAssetException("图片id格式错误");
        return re;
    }

    std::vector<std::string> Image::GetImgIdsFromMiraiCode(std::string MiraiCode) {
        std::vector<std::string> result = std::vector<std::string>();
        std::string temp = std::move(MiraiCode);
        std::smatch m;
        std::regex re("\\[mirai:image:(.*?)\\]");
        while (std::regex_search(temp, m, re)) {
            result.push_back(m[1]);
            temp = m.suffix().str();
        }
        return result;
    }

    std::string Image::toMiraiCode() {
        return "[mirai:image:" + Tools::escapeToMiraiCode(this->id) + "]";
    }

    MessageSource
    MessageSource::quoteAndSendMiraiCode(const std::string &content, unsigned long long groupid, JNIEnv *env) {
        json obj;
        json sign;
        obj["messageSource"] = this->serializeToString();
        obj["msg"] = content;
        sign["MiraiCode"] = true;
        sign["groupid"] = groupid;
        obj["sign"] = sign.dump();
        std::string re = config->koperation(config->SendWithQuote, obj, env);
        ErrorHandle(re);
        return MessageSource::deserializeFromString(re);
    }

    MessageSource MessageSource::quoteAndSendMsg(const std::string &content, unsigned long long groupid, JNIEnv *env) {
        json obj;
        json sign;
        obj["messageSource"] = this->serializeToString();
        obj["msg"] = content;
        sign["MiraiCode"] = false;
        sign["groupid"] = groupid;
        obj["sign"] = sign.dump();
        std::string re = config->koperation(config->SendWithQuote, obj, env);
        ErrorHandle(re);
        return MessageSource::deserializeFromString(re);
    }

    MessageSource Contact::sendMiraiCode(std::string msg, JNIEnv *env) {
        if (msg.empty())
            throw IllegalArgumentException("参数不能为空, 位置: Contact::SendMiraiCode");
        std::string re = LowLevelAPI::send0(std::move(msg), this, true, env);
        ErrorHandle(re, "reach a error area, Contact::SendMiraiCode");
        return MessageSource::deserializeFromString(re);
    }

    MessageSource Contact::sendMsg(std::string msg, JNIEnv *env) {
        if (msg.empty()) {
            logger->Error("警告:发送空信息, 位置: Contact::SendMsg");
            throw IllegalArgumentException("参数不能为空, 位置: Contact::SendMsg");
        }
        std::string re = LowLevelAPI::send0(std::move(msg), this, false, env);
        ErrorHandle(re, "reach a error area, Contact::SendMiraiCode");
        return MessageSource::deserializeFromString(re);
    }

    Image Contact::uploadImg(const std::string &path, JNIEnv *env) {
        std::string re = LowLevelAPI::uploadImg0(path, this, env);
        ErrorHandle(re);
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path);
        return Image(re);
    }

/*好友类实现*/
    Friend::Friend(unsigned long long id, unsigned long long botid, JNIEnv *env) : Contact() {
        this->_type = 1;
        this->_id = id;
        this->_botid = botid;
        refreshInfo(env);
    }

/*成员类实现*/
    Member::Member(unsigned long long id, unsigned long long groupid, unsigned long long botid, JNIEnv *env)
            : Contact() {
        this->_type = 3;
        this->_id = id;
        this->_groupid = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }

    unsigned int Member::getPermission(JNIEnv *env) {
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = config->koperation(config->QueryM, j, env);
        ErrorHandle(re);
        return stoi(re);
    }

    void Member::Mute(int time, JNIEnv *env) {
        json j;
        j["time"] = time;
        j["contactSource"] = this->serializationToString();
        std::string re = config->koperation(config->MuteM, j, env);
        ErrorHandle(re);
        if (re == "E3") {
            throw BotException();
        }
        if (re == "E4") {
            throw MuteException();
        }
    }

    void Member::Kick(const std::string &reason, JNIEnv *env) {
        json j;
        j["message"] = reason;
        j["contactSource"] = this->serializationToString();
        std::string re = config->koperation(config->KickM, j, env);
        ErrorHandle(re);
        if (re == "E3") {
            throw BotException();
        }
    }

/*群聊类实现*/
    Group::Group(unsigned long long i, unsigned long long bi, JNIEnv *env) : Contact() {
        this->_type = 2;
        this->_id = i;
        this->_botid = bi;
        refreshInfo(env);
    }

    void Group::updateSetting(JNIEnv *env) {
        json j;
        json tmp;
        j["name"] = this->setting.name;
        j["entranceAnnouncement"] = this->setting.entranceAnnouncement;
        j["isMuteAll"] = this->setting.isMuteAll;
        j["isAllowMemberInvite"] = this->setting.isAllowMemberInvite;
        j["isAutoApproveEnabled"] = this->setting.isAutoApproveEnabled;
        j["isAnonymousChatEnabled"] = this->setting.isAnonymousChatEnabled;
        tmp["source"] = j.dump();
        tmp["contactSource"] = this->serializationToString();
        std::string re = config->koperation(config->GroupSetting, tmp, env);
        if (re == "E1")
            throw BotException();
        refreshInfo(env);
    }

    RemoteFile Group::sendFile(const std::string &path, const std::string &filename, JNIEnv *env) {
        json tmp;
        json source;
        source["path"] = path;
        source["filename"] = filename;
        tmp["source"] = source.dump();
        tmp["contactSource"] = this->serializationToString();
        std::string callback = config->koperation(config->SendFile, tmp, env);
        ErrorHandle(callback);
        if (callback == "E2") throw UploadException("找不到" + filename + "位置:C-uploadfile");
        if (callback == "E3")
            throw UploadException("Upload Error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path);
        return RemoteFile::deserializeFromString(callback);
    }

    RemoteFile Group::getFile(const std::string &path, const std::string &id, JNIEnv *env) {
        // source 参数
        json tmp;
        json j;
        tmp["id"] = id;
        tmp["path"] = path;
        j["source"] = tmp.dump();
        j["contactSource"] = this->serializationToString();
        std::string re = config->koperation(config->RemoteFileInfo, j, env);
        ErrorHandle(re);
        if (re == "E2") throw RemoteAssetException("Get Error: 文件路径不存在, path:" + path);
        return RemoteFile::deserializeFromString(re);
    }

    Member Group::getOwner(JNIEnv *env) {
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = config->koperation(config->QueryOwner, j, env);
        ErrorHandle(re);
        return Member(stoi(re), this->id(), this->botid());
    }

    std::string Group::getFileListString(const std::string &path, JNIEnv *env) {
        json temp;
        json j;
        temp["id"] = "-1";
        temp["path"] = path;
        j["source"] = temp.dump();
        j["contactSource"] = this->serializationToString();
        std::string re = config->koperation(config->RemoteFileInfo, j, env);
        ErrorHandle(re);
        return re;
    }

    std::vector<Group::file_short_info> Group::getFileList(const std::string &path, JNIEnv *env) {
        std::vector<file_short_info> re = std::vector<file_short_info>();
        std::string tmp = getFileListString(path, env);
        json root = json::parse(tmp);
        for (auto &i : root) {
            file_short_info t;
            t.path = i[0];
            t.id = i[1];
            re.push_back(t);
        }
        return re;
    }

/*工具类实现*/
    std::string Tools::jstring2str(jstring jStr, JNIEnv *env) {
        if (!jStr) {
            logger->Error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
            return "";
        }
        std::u16string s = reinterpret_cast<const char16_t *>(env->GetStringChars(jStr, nullptr));
        if (s.length() == 0) {
            logger->Error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
            return "";
        }
        std::string x;
        utf8::utf16to8(s.begin(), s.end(), std::back_inserter(x));
        return x;
    }

    jstring Tools::str2jstring(const char *stra, JNIEnv *env) {
        if (!stra) {
            logger->Error("警告:C++部分传入空字符串，位置:Tools::str2jstring");
        }
        std::string str(stra);
        std::vector<unsigned short> utf16line;
        utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16line));
        auto *c = new jchar[utf16line.size()];
        for (int i = 0; i < utf16line.size(); i++) {
            c[i] = utf16line[i];
        }
        return env->NewString((const jchar *) c, (jsize) utf16line.size());
    }

    std::string Tools::JLongToString(jlong qqid) {
        auto id = [qqid]() -> std::string {
            std::stringstream stream;
            stream << qqid;
            std::string a;
            stream >> a;
            stream.clear();
            return a;
        };
        return id();
    }

    std::string Tools::VectorToString(std::vector<unsigned long long> a) {
        std::stringstream ss;
        for (size_t i = 0; i < a.size(); ++i) {
            if (i != 0)
                ss << ",";
            ss << a[i];
        }
        std::string s = ss.str();
        return s;
    }

    std::vector<unsigned long long> Tools::StringToVector(std::string temp) {
        std::vector<unsigned long long> result;
        temp.erase(temp.begin());
        temp.pop_back();
        std::regex ws_re("[,]+");
        std::vector<std::string> v(std::sregex_token_iterator(temp.begin(), temp.end(), ws_re, -1),
                                   std::sregex_token_iterator());
        for (auto &&s : v)
            result.push_back(atoi(s.c_str()));
        return result;
    }

    std::string Tools::escapeFromMiraiCode(const std::string &s) {
        //[	\[
        //]	\]
        //:	\:
        //,	\,
        //\	\\ /
        return Tools::replace(
                Tools::replace(
                        Tools::replace(
                                Tools::replace(
                                        Tools::replace(s,
                                                       "\\\\", "\\"),
                                        "\\,", ","),
                                "\\:", ":"),
                        "\\]", "]"), "\\[", "[");
    }

    std::string Tools::escapeToMiraiCode(const std::string &s) {
        //[	\[
        //]	\]
        //:	\:
        //,	\,
        //\	\\ /
        return Tools::replace(Tools::replace(Tools::replace(Tools::replace(Tools::replace(s,
                                                                                          "\\", "\\\\"),
                                                                           ",", "\\,"),
                                                            ":", "\\:"),
                                             "]", "\\]"), "[", "\\[");
        return std::string();
    }

    Contact Contact::deserializationFromString(const std::string &source) {
        json j;
        try {
            j = json::parse(source);
        }
        catch (json::parse_error &e) {
            logger->Error("json序列化错误 Contact::deserializationFromString");
            logger->Error(source);
            logger->Error(e.what());
        }
        return Contact::deserializationFromJson(j);
    }

    Contact Contact::deserializationFromJson(nlohmann::json j) {
        return Contact(j["type"],
                       j["id"],
                       j["groupid"],
                       j["nickornamecard"],
                       j["botid"]);
    }

    MessageSource Contact::sendVoice(const std::string &path, JNIEnv *env) {
        json j;
        json source;
        source["path"] = path;
        j["source"] = source.dump();
        j["contactSource"] = this->serializationToString();
        std::string re = config->koperation(config->Voice, j, env);
        ErrorHandle(re);
        if (re == "E1")
            throw UploadException("文件格式不对(必须为.amr/.silk)或文件不存在");
        return MessageSource::deserializeFromString(re);
    }

    void enrollPlugin();

    void enrollPlugin0(CPPPlugin *p) {
        plugin = p;
    }

    json PluginConfig::serialize() const {
        json j;
        j["name"] = name;
        j["version"] = version;
        j["author"] = author;
        j["description"] = description;
        j["time"] = time;
        return j;
    }
    // 结束MiraiCP实现代码
}
//开始对接JNI接口代码
/*
* 名称:Java_com_example_plugin_CPP_1lib_Verify
* 作用:判断是否连接上本插件，勿改
* 参数:env 必备，job 必备
* 返回值:jstring (用str2jstring把string类型转成jsrting) 发送返回的字符串
*/
JNIEXPORT jstring JNICALL Java_tech_eritquearcus_miraicp_shared_CPP_1lib_Verify(JNIEnv *env, jobject) {
    using namespace MiraiCP;
    manager->setEnv(env);
    env->GetJavaVM(&gvm);
    JNIVersion = (int) env->GetVersion();
    try {
        //初始化日志模块
        config->Init();
        logger->init();
        enrollPlugin();
        if (plugin == nullptr) {
            logger->Error("无插件实例加载");
        } else {
            CPPPlugin::pluginLogger = new PluginLogger(logger);
            plugin->onEnable();
        }
    }
    catch (MiraiCPException &e) {
        e.raise();
    }
    json j = plugin->config.serialize();
    j["MiraiCPversion"] = MiraiCPVersion;
    return Tools::str2jstring(j.dump().c_str());//验证机制，返回当前SDK版本
}
/* 插件结束事件*/
JNIEXPORT jobject JNICALL Java_tech_eritquearcus_miraicp_shared_CPP_1lib_PluginDisable
        (JNIEnv *env, jobject job) {
    using namespace MiraiCP;
    manager->setEnv(env);
    plugin->onDisable();
    delete (logger);
    delete (procession);
    delete (config);
    gvm->DestroyJavaVM();
    return job;
}

jstring returnNull() {
    return MiraiCP::Tools::str2jstring("MIRAICP_NULL");
}
/*
* 消息解析分流
*/
JNIEXPORT jstring JNICALL Java_tech_eritquearcus_miraicp_shared_CPP_1lib_Event
        (JNIEnv *env, jobject, jstring content) {
    using namespace MiraiCP;
    manager->setEnv(env);
    std::string tmp = Tools::jstring2str(content, env);
    json j;
    try {
        j = json::parse(tmp);
    }
    catch (json::parse_error &e) {
        APIException("格式化json错误, JNIEXPORT jstring JNICALL Java_tech_eritquearcus_miraicp_CPP_1lib_Event").raise();
        logger->Error("For debug:" + j.dump());
        logger->Error(e.what());
        return returnNull();
    }
    try {
        switch ((int) j["type"]) {
            case 1: {
                //GroupMessage
                procession->broadcast<GroupMessageEvent>(
                        GroupMessageEvent(j["group"]["botid"],
                                          Group::deserializationFromJson(j["group"]),
                                          Member::deserializationFromJson(j["member"]),
                                          j["message"],
                                          MessageSource::deserializeFromString(j["source"])
                        )
                );
                break;
            }
            case 2: {
                //私聊消息
                procession->broadcast<PrivateMessageEvent>(
                        PrivateMessageEvent(j["friend"]["botid"],
                                            Friend::deserializationFromJson(j["friend"]),
                                            j["message"],
                                            MessageSource::deserializeFromString(j["source"])
                        ));
                break;
            }
            case 3:
                //群聊邀请
                procession->broadcast<GroupInviteEvent>(
                        GroupInviteEvent(
                                j["botid"],
                                j["source"],
                                j["source"]["inviternick"],
                                j["source"]["inviterid"],
                                j["source"]["groupname"],
                                j["source"]["groupid"]
                        ));
                break;
            case 4:
                //好友
                procession->broadcast<NewFriendRequestEvent>(
                        NewFriendRequestEvent(
                                j["source"]["botid"],
                                j["source"].dump(),
                                j["source"]["fromid"],
                                j["source"]["fromgroupid"],
                                j["source"]["fromnick"],
                                j["source"]["message"]
                        ));
                break;
            case 5:
                //新成员加入
                procession->broadcast<MemberJoinEvent>(
                        MemberJoinEvent(
                                j["group"]["botid"],
                                j["jointype"],
                                Member::deserializationFromJson(j["member"]),
                                Group::deserializationFromJson(j["group"]),
                                j["inviterid"]
                        ));
                break;
            case 6:
                //群成员退出
                procession->broadcast<MemberLeaveEvent>(MemberLeaveEvent(
                        j["group"]["botid"],
                        j["leavetype"],
                        j["memberid"],
                        Group::deserializationFromJson(j["group"]),
                        j["operatorid"]
                ));
                break;
            case 7:
                procession->broadcast<RecallEvent>(RecallEvent(
                        j["botid"],
                        j["etype"],
                        j["time"],
                        j["authorid"],
                        j["operatorid"],
                        j["ids"],
                        j["internalids"],
                        j["groupid"]
                ));
                break;
            case 9:
                procession->broadcast<BotJoinGroupEvent>(BotJoinGroupEvent(
                        j["group"]["botid"],
                        j["etype"],
                        Group::deserializationFromJson(j["group"]),
                        j["inviterid"]
                ));
                break;
            case 10:
                procession->broadcast<GroupTempMessageEvent>(GroupTempMessageEvent(
                        j["group"]["botid"],
                        Group::deserializationFromJson(j["group"]),
                        Member::deserializationFromJson(j["member"]),
                        j["message"],
                        MessageSource::deserializeFromString(j["source"])
                ));
                break;
            case 11:
                procession->broadcast<BotOnlineEvent>(BotOnlineEvent(j["botid"]));
                break;
            default:
                throw APIException("Unreachable code");
        }
    }
    catch (json::type_error &e) {
        logger->Error("json格式化异常,位置C-Handle");
        logger->Error(j.dump());
        logger->Error(e.what());
        return Tools::str2jstring("ERROR");
    } catch (MiraiCPException &e) {
        logger->Error("MiraiCP Error:");
        logger->Error(e.what());
        return Tools::str2jstring("ERROR");
    }
    return returnNull();
}
//结束对接JNI接口代码
#endif