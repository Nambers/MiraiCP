// Copyright (C) 2020-2021 Eritque arcus and contributors.
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

#pragma once
// 针对clion
#pragma clang diagnostic push
// 禁止检查unused
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "cert-err58-cpp"
#ifndef CPP_MIRAICP_HPP
#define CPP_MIRAICP_HPP
#pragma execution_character_set("utf-8")
// 在include文件夹里的依赖
#include <jni.h>
#include <json.hpp>
#include <utf8.h>
//C++ 标准库
#include <array>
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

namespace MiraiCP {
    using json = nlohmann::json;
    using QQID = unsigned long long;

    // 开始声明MiraiCP常量声明代码
    /// MiraiCP当前版本
    const std::string MiraiCPVersion = "v2.8.0-M2";

    /// @brief 插件信息
    class PluginConfig {
    public:
        /// @brief 插件id, 要与别人不一样否则报错无法加载(建议用类包格式，如: io.github.nambers)
        std::string id;
        /// @brief 插件名称
        std::string name;
        /// @brief 插件版本
        std::string version;
        /// @brief 插件作者(及联系方式)
        std::string author;
        /// @brief [optional]插件描述
        std::string description;
        /// @brief [optional]构建时间, 默认为__DATE__宏
        std::string time;

        PluginConfig(std::string id,
                     std::string name,
                     std::string version,
                     std::string author,
                     std::string description = "",
                     std::string time = __DATE__)
            : id(std::move(id)),
              name(std::move(name)),
              version(std::move(version)),
              author(std::move(author)),
              description(std::move(description)),
              time(std::move(time)) {}

        json serialize();

        std::string serialize2string() {
            return serialize().dump();
        }
    };

    //region 需要表态的值，如加好友事件
    /*! @addtogroup 需要表态的值
     * @brief 如加好友事件
     * @{
     */
    /// 拒绝
    static const bool REJECT = false;
    /// 同意
    static const bool ACCEPT = true;
    /// @}
    //endregion

    //region 群成员退出事件
    /*! @addtogroup 群成员退出事件
     * @{
     */
    ///被踢出
    static const int T_KICK = 1;
    ///主动退出
    static const int T_QUIT = 2;
    /// @}
    //endregion

    //region 新群成员事件
    /*! @addtogroup 新群成员事件
     * @{
     */
    /*!被邀请进入*/
    static const int T_INVITE = 1;
    /*!主动加入*/
    static const int T_ACTIVE = 2;
    /*!旧群主恢复群主位置*/
    static const int T_Retrieve = 3;
    /// @}
    //endregion

    //region 群成员权限
    /*! @addtogroup 群成员权限
     * @{
     */
    /// 群主
    static const int P_Owner = 2;
    /// 管理员
    static const int P_Administer = 1;
    /// 群成员
    static const int P_Member = 0;
    /// @}
    //endregion

    //region LightApp风格
    /*!
    * @addtogroup LightApp风格
    * @{
    */
    /*!
    LightApp风格1
    */
    struct LightAppStyle1 {
        ///小图标地址
        std::string icon = "https://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
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
        std::string icon = "https://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
        ///预览图大图
        std::string preview = "https://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
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
        std::string icon = "https://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
        ///预览图
        std::string preview = "https://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
        ///大标题
        std::string title = "Test1";
        ///图片旁描述
        std::string description = "Test2";
    };

    /// 风格4
    struct LightAppStyle4 {
        /// 没点进来显示的内容
        std::string prompt = "prompt";
        std::string appName = "appName";
        /// 左上角图片网址
        std::string iconUrl = "https://q.qlogo.cn/headimg_dl?dst_uin=1930893235&spec=100";
        std::string button_action = "button_action";
        std::string button_name = "button_name";
        std::string data_title = "data_title";
        std::string data_value = "data_value";
    };

    /// 用serviceMessage的分享信息
    struct URLSharer {
        /// 简介 没点进来看见的样子
        std::string brief = "简介 没点进来看见的样子";
        /// 目标url
        std::string url = "目标url";
        /// 图标地址
        std::string cover = "图标地址";
        /// 标题
        std::string title = "标题";
        /// 描述文字
        std::string summary = "描述文字";
    };
    /// @}
    // endregion

    // 结束声明MiraiCP常量声明代码

    // 开始MiraiCP类声明代码

    //region MiraiCP类声明
    /*!
    * @class threadManager
    * @brief 多线程管理.
    */
    class ThreadManager {
    public:
        class StackTracer {
        private:
            std::vector<std::string> stackTrace = std::vector<std::string>();

        public:
            /// print all
            std::string print() {
                std::string re = "StackTrace:";
                for (const auto &a: stackTrace)
                    re += "\n" + a;
                return re;
            }

            /// push stack
            void push(const std::string &file = __FILE__, int loc = __LINE__, const std::string &func = "",
                      const std::string &commit = "") {
                stackTrace.push_back(
                        func + (!commit.empty() ? "(" + commit + ")" : "") + "[" + file + ":" + std::to_string(loc) +
                        "]");
            }

            /// 清空
            void clear() {
                stackTrace.clear();
            }
        };

        /// @brief 每个线程实例.
        struct ThreadInfo {
            JNIEnv *e{};
            bool attach{};
            StackTracer stack;
        };
        static std::map<std::string, ThreadInfo> threads; /// < 线程池(线程id:env).
        static std::recursive_mutex mtx;                  ///< 线程池读写锁.
    private:
        static void newEnv(const char *threadName = nullptr); ///< 新建一个env，于getEnv中没取到env时调用.
        /// 判断该线程id是否包含在线程池里
        static bool included(const std::string &id);

        ThreadManager() = default;

    public:
        /// @brief 全局JavaVM对象，用于多线程管理中新建线程的JNIEnv.
        static JavaVM *gvm;
        /// @brief JNI 版本.
        static long JNIVersion;

        /// 获取线程
        static ThreadInfo *getThread() {
            return &threads[getThreadId()];
        }

        /// @brief 获取线程id.
        static std::string getThreadId() {
            auto myid = std::this_thread::get_id();
            std::stringstream ss;
            ss << myid;
            return ss.str();
        }

        /// @brief 设置env给当前线程.
        static void setEnv(JNIEnv *e);


        /*!
         * 	@brief 结束当前线程的env，也就是释放当前线程缓存的env.
         *  @note 不过继续调用getEnv()将再次获取，所以本方法调用后线程也可以通过getEnv重新获取一个env，本方法的作用就是在结束后释放空间
         * @example 多线程管理
         * @code
         * void func(){
         *      //do some things
         *      // 一个完整的线程应该在结束时调用detach来释放env的空间
         *      ThreadManager::detch();
         * }
         * @endcode
         */
        static void detach();

        /// @brief 取env,如果不存在重新获取
        /// @internal 一般为`miraicp`内部调用jni接口时调用
        /// @param file 为支持`StackTracer`而增加, 为`__FILE__`宏(文件名), 在调用处传入因为当__FILE__作为默认参数传入时不准确
        /// @param loc 为`__LINE__`宏(行号), 同上
        /// @param func 为`__FUNC__`宏(方法名)
        static JNIEnv *getEnv(const std::string &file, int loc, const std::string &func = "");
    };

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
        static std::string jstring2str(jstring jstr, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /*!
         * @name str2jstring
         * @brief string类型到jsting类型 UTF8 -> UTF16
         * @note 来源https://blog.csdn.net/chunleixiahe/article/details/51394116
         * @param stra const char*(string.c_str()转换的内容)
         * @param env 可选JNIEnv
         * @return 转换后jstring类型
         */
        static jstring str2jstring(const char *stra, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

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
        /// @param a vector
        /// @return string
        template<typename T>
        static std::string VectorToString(std::vector<T> a, const std::string & = ",");

        /// @brief 从string格式化到vector
        /// @param temp string
        /// @return vector
        static std::vector<unsigned long long> StringToVector(std::string temp);

        /// @brief 从miraicode转义到正常
        /// @param s 经过miraicode转义的字符串
        /// @return 原字符串
        static std::string escapeFromMiraiCode(const std::string &s);

        /// @brief 转义miraicode格式
        /// @param s
        /// @return
        static std::string escapeToMiraiCode(const std::string &s);

        /// starts_with, from <https://stackoverflow.com/questions/1878001/how-do-i-check-if-a-c-stdstring-starts-with-a-certain-string-and-convert-a>
        static bool starts_with(const std::string &f, const std::string &s) { return f.rfind(s, 0) == 0; }

        static bool icompareChar(char &c1, char &c2) {
            return c1 == c2 || std::toupper(c1) == std::toupper(c2);
        }

        /// case insensitive string compare from https://thispointer.com/c-case-insensitive-string-comparison-using-stl-c11-boost-library/
        static bool iequal(std::string str1, std::string str2) {
            return ((str1.size() == str2.size()) &&
                    std::equal(str1.begin(), str1.end(), str2.begin(), &icompareChar));
        }
    };

    /// @brief 配置类声明, 主要存放各种jmethodid, MiraiCP内部使用, 不需要更改或其他操作
    /// @internal 一般为MiraiCP内部调用jni接口使用
    /// @class Config
    class Config {
    public:
        /// kt中JNI接口类
        static jclass CPP_lib;
        /// 调用mirai方法
        static jmethodID KOperation;

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
            SendWithQuote,
            /// 群公告操作
            Announcement,
            /// 定时任务
            TimeOut,
            /// 发送戳一戳
            SendNudge,
            /// 下一条信息
            NextMsg,
            /// 更改权限
            ModifyAdmin,
            /// 群成员申请入群
            MemberJoinRequest
        };

        /**
         * @brief 调用mirai操作
         * @param type 操作id
         * @param data 传入数据
         * @return 返回数据
         */
        static std::string
        koperation(operation_set type, nlohmann::json &data, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__),
                   bool catchErr = true, const std::string &errorInfo = "");

        static void construct(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        static void destruct();
    };

    /// MiraiCode父类, 指可以被转换成miraicode的类型
    class MiraiCodeable {
    public:
        virtual std::string toMiraiCode() const = 0;
    };

    /// @brief miraicode字符串
    /// @attention MiraiCode会把非miraicode组成部分(非[mirai:])转码, 输出转码前的文本用toString, 参考: https://github.com/mamoe/mirai/blob/dev/docs/Messages.md#%E8%BD%AC%E4%B9%89%E8%A7%84%E5%88%99
    /// @detail 为了便捷使用，构造函数不以explicit注释
    class MiraiCode : public MiraiCodeable {
    private:
        std::string content;

    public:
        /// 输出当前内容, 会自动转码
        std::string toString() {
            return Tools::escapeFromMiraiCode(this->content);
        }

        /// 和toString作用一样, 不过不会自动转码
        std::string toMiraiCode() const override {
            return content;
        }

        /// 从MiraiCodeable类型初始化一个miraicode字符串
        MiraiCode(MiraiCodeable *a) { // NOLINT(google-explicit-constructor)
            content = a->toMiraiCode();
        }

        /// 从文本初始化一个miraicode字符串, 根据第二个参数决定是否转码, 默认不转码
        /// @attention 如果是传入文本MiraiCode，请勿转码，转码只是为了[mirai:xxx:<应该转码的部分>], 如果<应该转码>的部分里面含有'[]:,'内容，请调用Tools::escapeToMiraiCode转码
        MiraiCode(const std::string &a, bool convert = false) { // NOLINT(google-explicit-constructor)
            if (!convert)
                content = a;
            else
                content = Tools::escapeToMiraiCode(a);
        }

        MiraiCode operator+(MiraiCodeable *a) {
            return {content + a->toMiraiCode()};
        }

        MiraiCode operator+(const std::string &a) {
            return {content + a};
        }

        MiraiCode operator+(const MiraiCode &a) {
            return {content + a.content};
        }

        MiraiCode operator+(MiraiCode *a) {
            return {content + a->content};
        }

        MiraiCode &operator=(const std::string &a) {
            this->content = a;
            return *this;
        }

        MiraiCode plus(MiraiCodeable *a) {
            return {content + a->toMiraiCode()};
        }

        MiraiCode plus(const std::string &a) {
            return MiraiCode(a) + this;
        }

        /// 不执行转义，适用于已经被MiraiCode转义过的字符串
        static MiraiCode MiraiCodeWithoutEscape(const std::string &a) {
            return {a, false};
        }

        /// 不执行转义，因为MiraiCodeable的toMiraiCode已经转义过了
        static MiraiCode MiraiCodeWithoutEscape(MiraiCodeable *a) {
            return {a->toMiraiCode(), false};
        }
    };

    /*!
    * @class Logger
    * @brief 以MiraiCP的名义发送日志, 日志表现格式是: 2021-06-28 09:37:22 [log level]/MiraiCP: [log content], 为最底层的logger
    * @example 发送日志
	* 发送消息级日志
	* @code Logger::logger.info(string) @endcode
	* 发送警告级日志
	* @code Logger::logger.warning(string) @endcode
	* 发送错误级日志
	* @code Logger::logger.error(string) @endcode
    */
    class Logger_interface {
    private:
        std::string p(const std::string &before) {
            return before;
        }

        template<class T, class... T1>
        std::string p(std::string before, T val, T1... val1) {
            std::stringstream sstream;
            sstream << val;
            return p(before + sstream.str(), val1...);
        }

        template<class... T>
        std::string p(std::string before, MiraiCodeable &val, T... val1) {
            return p(before + val.toMiraiCode(), val1...);
        }

    protected:
        /// @brief 日志底层实现封装
        /// @param log 日志内容
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
        typedef std::function<void(std::string, int)> Action;
        /// @brief loggerhandler会在每次log执行前执行一遍，可用于执行自定义的保存操作等
        struct Handler {
            /// @brief 是否启用
            bool enable = true;
            /// @brief 执行的操作，格式为lambda
            Action action = [](const std::string &content, int level) {};
        };
        Handler loggerhandler;

        // 初始化 获取methodid
        void init(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        ///发送普通(info级日志)
        template<class... T>
        void info(T... val) {
            this->log0(p("", val...), 0, ThreadManager::getEnv(__FILE__, __LINE__));
        }

        ///发送警告(warning级日志)
        template<class... T>
        void warning(T... val) {
            this->log0(p("", val...), 1, ThreadManager::getEnv(__FILE__, __LINE__));
        }

        ///发送警告(warning级日志)
        template<class... T>
        void error(T... val) {
            ThreadManager::StackTracer a = ThreadManager::getThread()->stack;
            this->log0(p("", val...) + "\n" + a.print(), 2, ThreadManager::getEnv(__FILE__, __LINE__));
        }

        /// @brief 设置loggerhandler的action
        /// @param action 执行的操作
        /// @see Logger::handler
        /// @example 设置loggerhandler的action
        /// @code Logger::logger.registerHandle([](std::string content, int level){
        ///     \\do some things
        /// });@endcode
        inline void registerHandle(Action action);

        /// @brief 设置handler的启用状态
        /// @param state 状态，启用或者关闭
        /// @example 设置handler的启用状态
        /// @code Logger::logger.setHandleState(ture); @endcode
        inline void setHandleState(bool state);
    };

    class Logger : public Logger_interface {
    protected:
        /// @brief 日志底层实现封装
        /// @param content 日志内容
        /// @param level 日志等级
        /// @param env jnienv
        void log0(const std::string &content, int level, JNIEnv *env) override;

    private:
        Logger() = default;

    public:
        static Logger logger;
    };

    /// 带id(一般为bot账号)的logger
    class IdLogger : public Logger_interface {
    protected:
        void log0(const std::string &content, int level, JNIEnv *env) override;

    public:
        QQID id;

        IdLogger(QQID id, Logger *l) : id(id) {
            this->loggerhandler = l->loggerhandler;
            this->log = l->getjmethod();
        }
    };

    /// 插件logger
    class PluginLogger : public Logger_interface {
    protected:
        void log0(const std::string &content, int level, JNIEnv *env) override;

    public:
        explicit PluginLogger(Logger *l) {
            this->loggerhandler = l->loggerhandler;
            this->log = l->getjmethod();
        }
    };

    /// 插件父类
    class CPPPlugin {
    public:
        /// @brief 插件信息
        PluginConfig config;
        /// @brief 插件级logger
        static PluginLogger *pluginLogger;

        static CPPPlugin *plugin;

        virtual void onEnable() {}

        virtual void onDisable() {}

        explicit CPPPlugin(PluginConfig c) : config(std::move(c)) {}
    };

    //仅声明
    class MiraiCPException;

    /// @brief 异常事件广播
    class ExceptionBroadcasting {
    public:
        MiraiCPException *e;
        ExceptionBroadcasting(MiraiCPException *ex);
        ~ExceptionBroadcasting();
    };

    /// @brief 总异常抽象类
    /// @interface MiraiCPException
    class MiraiCPException : public std::exception {
    protected:
        /// 异常的类型
        const std::string exceptionType;

    public:
        explicit MiraiCPException(const std::string &&type) : exceptionType(type) { ExceptionBroadcasting(this); }

        /// 获取异常类型
        std::string getExceptionType() { return exceptionType; }

        /// 异常信息
        virtual std::string what() { return getExceptionType() + "MiraiCP异常"; }

        /// basicRaise 基本抛出方法，子类重写该方法
        virtual void basicRaise() {
            Logger::logger.error(what());
        };

        /// 实际抛出方法
        void raise() { basicRaise(); }
    };


    /// 文件读取异常.
    /// @see MiraiCPException
    class UploadException : public MiraiCPException {
    public:
        explicit UploadException(const std::string &text) : MiraiCPException("UploadException") {
            this->description = "上传(图片/文件)异常" + text;
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->description);
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 通常为Mirai返回
    /// @see MiraiCPException
    class IllegalStateException : public MiraiCPException {
    public:
        explicit IllegalStateException(const std::string &text) : MiraiCPException("IllegalStateException") {
            this->description = "状态异常:" + text;
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->description);
            // ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 内部异常, 通常为json读写问题
    /// @see MiraiCPException
    class APIException : public MiraiCPException {
    public:
        explicit APIException(const std::string &text) : MiraiCPException("APIException") {
            this->description = "MiraiCP内部无法预料的错误:" + text;
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->description);
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 机器人操作异常
    /// @see MiraiCPException
    class BotException : public MiraiCPException {
    public:
        explicit BotException(std::string d = "没有权限执行该操作", const std::string &&type = "BotException")
            : MiraiCPException(std::move(type)) {
            this->description = std::move(d);
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 被禁言异常, 通常发生于发送信息
    class BotIsBeingMutedException : public BotException {
    public:
        /// 剩余禁言时间, 单位秒
        int timeRemain;

        explicit BotIsBeingMutedException(int t) : BotException("发送信息失败, bot已被禁言, 剩余时间" + std::to_string(t),
                                                                "BotIsBeingMutedException") {
            timeRemain = t;
        }
    };

    /// 禁言异常
    /// @see MiraiCPException
    class MuteException : public MiraiCPException {
    public:
        /*
        *	 禁言时间超出0s~30d
        */
        MuteException() : MiraiCPException("MuteException") {
            this->description = "禁言时长不在0s~30d中间";
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 获取群成员错误
    /// @see MiraiCPException
    class MemberException : public MiraiCPException {
    public:
        int type = 0;

        /*
        *   "1" - 找不到群
        *	"2" - 找不到群成员
        */
        explicit MemberException(int type) : MiraiCPException("MemberException") {
            this->type = type;
            switch (type) {
                case 1:
                    this->description = "找不到群";
                    break;
                case 2:
                    this->description = "找不到群成员";
                    break;
                default:
                    break;
            }
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
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
        FriendException() : MiraiCPException("FriendException") {
            this->description = "找不到好友";
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 获取群错误
    /// @see MiraiCPException
    class GroupException : public MiraiCPException {
    public:
        GroupException() : MiraiCPException("GroupException") {
            this->description = "C++:找不到群";
        }

        //返回错误信息
        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 撤回异常
    /// @see MiraiCPException
    class RecallException : public MiraiCPException {
    public:
        RecallException() : MiraiCPException("RecallException") {
            this->description = "该消息已经被撤回";
        }

        std::string what() override {
            return exceptionType + this->description;
        }

    private:
        std::string description;
    };

    /// 远程资源出现问题
    /// @see MiraiCPException
    class RemoteAssetException : public MiraiCPException {
    public:
        explicit RemoteAssetException(std::string e) : MiraiCPException("RemoteAssetException") {
            this->description = std::move(e);
        }

        std::string what() override {
            return exceptionType + this->description;
        }

    private:
        std::string description;
    };

    /// 参数错误
    /// @see MiraiCPException
    class IllegalArgumentException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit IllegalArgumentException(std::string e) : MiraiCPException("IllegalArgumentException") {
            this->description = std::move(e);
        }

        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->what());
        }
    };

    /// 超时
    /// @see MiraiCPException
    class TimeOutException : public MiraiCPException {
    private:
        std::string description;

    public:
        explicit TimeOutException(std::string e) : MiraiCPException("TimeOutException") {
            this->description = std::move(e);
        }

        std::string what() override {
            return exceptionType + this->description;
        }

        void basicRaise() override {
            Logger::logger.error(this->what());
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
        if (re == "EB")
            throw BotException("找不到bot:" + re);
        if (re == "EA")
            throw APIException(ErrorMsg);
    }

    /// 消息源声明
    class MessageSource {
    public:
        /// 消息的ids
        std::string ids;
        /// 消息的internalids
        std::string internalids;
        /// 消息源序列化
        std::string source;

        MessageSource() = default;

        /**
         * @brief 回复(引用并发送miraicode)
         * @param msg - MiraiCodeable类型指针 - 内容
         * @see MessageSource::quoteAndSendMiraiCode
        */
        [[deprecated("Use Contact.quoteAndSendMessage")]] MessageSource
        quoteAndSendMiraiCode(MiraiCodeable *msg, QQID groupid = 0,
                              JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) const {
            return quoteAndSendMiraiCode(msg->toMiraiCode(), groupid, env);
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
        [[deprecated("Use Contact.quoteAndSendMessage")]] MessageSource
        quoteAndSendMsg(const std::string &c, QQID groupid = 0,
                        JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__)) const;

        /**
         * @brief 回复(引用并发送)
         * @param c 引用后发送的内容, 为MiraiCode形式
         * @param groupid 如果为发送给群成员需把该群成员的groupid传入以帮助获取到该成员
         * @return MessageSource
         */
        [[deprecated("Use Contact.quoteAndSendMessage")]] MessageSource
        quoteAndSendMiraiCode(const std::string &c, QQID groupid = 0,
                              JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__)) const;

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

        std::string serializeToString() const;

        /*!
         * @brief 撤回该信息
         * @example 撤回信息
         * @code
         * Event::processor.registerEvent([](GroupMessageEvent e) {
            try {
                e.messageSource.recall();
                e.group.SendMsg("hi").recall();
            }
            catch (MiraiCPException &e) {
                Logger::logger.error("错误");
            }
            });
         * @endcode
        */
        void recall(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__)) const;
    };

    /// MessageChain的组成部分
    class SingleMessage : public MiraiCodeable {
    public:
        static std::map<int, std::string> messageType;

        /// @brief 找对应类型的index key
        /// @param value 类型名
        /// @return 如果没找到返回-1
        static int getKey(const std::string &value) {
            for (const auto &a: messageType) {
                if (Tools::iequal(a.second, value)) return a.first;
            }
            return -1;
        }

        /// MiraiCode类别
        /// @see SingleMessage::messageType
        int type;
        std::string content;
        std::string prefix;

        std::string toMiraiCode() const override {
            if (type > 0)
                if (type == 1)
                    return "[mirai:at:" + content + "] ";
                else if (type == 2)
                    return "[mirai:atall] ";
                else
                    return "[mirai:" + messageType[type] + this->prefix + Tools::escapeToMiraiCode(content) + "]";
            else
                return content;
        }

        /// @brief 构建单条
        /// @param type 消息类型 @see messageType
        /// @param content 内容
        /// @param prefix 前缀, 默认为`:`, 第二个冒号部分的内容, 目前在serviceMesage有使用
        SingleMessage(int type, std::string content, std::string prefix = ":") : type(type),
                                                                                 content(std::move(content)),
                                                                                 prefix(std::move(prefix)) {}
    };

    /// 纯文本信息
    class PlainText : public SingleMessage {
    public:
        std::string content;

        std::string toMiraiCode() const override {
            return content;
        }

        explicit PlainText(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 0)
                throw IllegalArgumentException(
                        "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to PlainText");
            this->content = sg.content;
        }

        template<typename T>
        explicit PlainText(const T &a) : SingleMessage(0, ([&a]() -> std::string {
                                                           std::stringstream sst;
                                                           sst << a;
                                                           return sst.str();
                                                       })()) {}
    };

    /// @
    class At : public SingleMessage {
    public:
        QQID target;

        explicit At(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 1)
                throw IllegalArgumentException(
                        "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to At");
            this->target = std::stol(sg.content);
        }

        explicit At(QQID a) : SingleMessage(1, std::to_string(a)), target(a){};

        std::string toMiraiCode() const override {
            return "[mirai:at:" + std::to_string(this->target) + "] "; // 后面有个空格
        }
    };

    /// @brief \@全体
    class AtAll : public SingleMessage {
    public:
        std::string toMiraiCode() const override {
            return "[mirai:atall] ";
        }

        AtAll() : SingleMessage(2, "", "") {}
    };

    /// 图像类声明
    class Image : public SingleMessage {
    public:
        //图片id，样式:` {xxx}.xx `
        std::string id;

        /*!
        * @brief 从图片id构造，适用于服务器上已经有的图片，即接收到的
        * @example 图片miraiCode格式例子
         * @code [mirai:image:{图片id}.jpg] @endcode
        * @note 可以用这个正则表达式找出id ` \\[mirai:image:(.*?)\\] `
        */
        explicit Image(std::string imageId) : SingleMessage(3, std::move(imageId)) {
            this->id = std::move(imageId);
        }

        explicit Image(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 2) assert(sg.type != 2);
            this->id = sg.content;
        }

        /*
        * 获取图片下载url
        */
        std::string queryURL(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 取图片Mirai码
        std::string toMiraiCode() const override;
    };

    /*!
    * @brief 小程序卡片
    * @see LightAppStyle1, LightAppStyle2, LightAppStyle3
    * @attention 自带的模板不稳定，可能发出现没有效果
    * @example 通过常量构建并发送小程序卡片
    * @code
 * Event::processor.registerEvent([](GroupMessageEvent e) {
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
 * Event::processor.registerEvent([](GroupMessageEvent e) {
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
    class LightApp : public SingleMessage {
    public:
        /// @brief 使用纯文本构造，推荐使用其他结构体方法构造
        /// @param content 构造文本
        explicit LightApp(std::string content) : SingleMessage(4, std::move(content)) {}

        explicit LightApp(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 3)
                throw IllegalArgumentException(
                        "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to LighApp");
        }

        /// 使用样式1,适合文字展示，无大图，不能交互
        /// @param c 结构体，用于自定义里面的数据
        /// @see LightAppStyle1 in pch.h
        explicit LightApp(const LightAppStyle1 &c) : SingleMessage(4, "") {
            this->content =
                    "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":{\"appName\":\"" +
                    c.appName + "\",\"appType\":4,\"appid\":1109659848,\"iconUrl\":\"" + c.icon +
                    "\"},\"data\":[" + c.titles + "],\"title\":\"" + c.title + "\",\"button\":[" + c.buttons +
                    "],\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
        }

        /// 使用样式2，有大图，不能交互
        /// @param c 结构体，用于自定义里面的数据
        /// @see LightAppStyle1 in pch.h
        explicit LightApp(const LightAppStyle2 &c) : SingleMessage(4, "") {
            this->content =
                    "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
                    "\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\"" +
                    c.preview + "\",\"shareTemplateData\":{},"
                                "\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" +
                    c.title2 + "\",\"title\":\"" + c.title +
                    "\",\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" +
                    c.icon + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
        }

        /// 样式3，有大图，可以在电脑qq显示，并在电脑上点击的链接会跳转
        /// @param c 结构体，用于自定义里面的数据
        /// @see LightAppStyle1 in pch.h
        explicit LightApp(const LightAppStyle3 &c) : SingleMessage(4, "") {
            this->content =
                    "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
                    "\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
                    "\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" +
                    c.preview +
                    "\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" +
                    c.title + "\",\"title\":\"" + c.description +
                    "\",\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" +
                    c.icon + "\",\"qqdocurl\":\"" + c.url +
                    "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
        }

        explicit LightApp(const LightAppStyle4 &c) : SingleMessage(4,
                                                                   R"({"app":"com.tencent.miniapp","desc":"","view":"notification","ver":"1.0.0.11","prompt":")" +
                                                                           c.prompt +
                                                                           "\",\"meta\":{\"notification\":{\"appInfo\":{\"appName\":\"" +
                                                                           c.appName +
                                                                           "\",\"appType\":4,\"appid\":1109659848,\"iconUrl\":\"" +
                                                                           c.iconUrl + "\"},\"button\":[{\"action\":\"" +
                                                                           c.button_action + "\",\"name\":\"" + c.button_name +
                                                                           "\"}],\"data\":[{\"title\":\"" + c.data_title +
                                                                           "\",\"value\":\"" + c.data_value +
                                                                           "\"}],\"emphasis_keyword\":\"\"}}}") {}

        /// 返回miraicode
        std::string toMiraiCode() const override {
            return "[mirai:app:" + Tools::escapeToMiraiCode(content) + "]";
        }
    };

    /// xml格式的超文本信息
    /// @attention 自带的模板不稳定，可能发出现没有效果
    class ServiceMessage : public SingleMessage {
    public:
        std::string toMiraiCode() const override {
            return "[mirai:service:" + this->prefix + Tools::escapeToMiraiCode(content) + "]";
        }

        /// @brief ServiceMessage
        /// @param id 在xml内容前面的id (不包括逗号)
        /// @param a xml内容 (不需要事先转码到miraiCode)
        explicit ServiceMessage(int id, std::string a) : SingleMessage(5, std::move(a),
                                                                       ":" + std::to_string(id) + ',') {}

        explicit ServiceMessage(const SingleMessage &sg) : SingleMessage(sg) {
            if (sg.type != 4)
                throw IllegalArgumentException(
                        "Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to ServiceMessage");
        }

        explicit ServiceMessage(const URLSharer &a) : SingleMessage(5,
                                                                    "<?xml version=\"1.0\" encoding=\"utf-8\"?><msg templateID=\"12345\" action=\"web\" brief=\"" +
                                                                            a.brief + "\" serviceID=\"1\" url=\"" + a.url +
                                                                            "\"><item layout=\"2\"><picture cover=\"" +
                                                                            a.cover + "\"/><title>" + a.title +
                                                                            "</title><summary>" + a.summary +
                                                                            "</summary></item><source/></msg>",
                                                                    ":1,") {}
    };

    /// 引用信息, 不可直接发送, 发送引用信息用MessageChain.quoteAndSendMessage
    class QuoteReply : public SingleMessage {
    public:
        [[deprecated("cannot use, use MessageChain.quote")]] std::string toMiraiCode() const override {
            return "";
        }

        /// 引用信息的MessageSource
        MessageSource source;

        explicit QuoteReply(MessageSource source) : SingleMessage(-2, ""), source(std::move(source)){};
    };

    /// 接收到的音频文件, 发送用`Contact.sendAudio`
    class OnlineAudio : public SingleMessage {
    public:
        /// 文件名
        std::string filename;
        /// 下载地址
        std::string url;
        /// 文件大小
        int size;
        /// 编码方式
        int codec;
        /// 时长(单位s)
        int length;
        /// 16位md5
        std::array<uint8_t, 16> md5;

        [[deprecated("cannot use, use Contact.sendAudio")]] std::string toMiraiCode() const override {
            return "";
        }

        explicit OnlineAudio(std::string f, std::array<uint8_t, 16> md5, int size, int codec, int length,
                             std::string url) : SingleMessage(-3, ""),
                                                filename(std::move(f)), md5(md5), size(size), codec(codec),
                                                length(length), url(std::move(url)){};
    };

    /// @brief 远程(群)文件类型
    class RemoteFile : public SingleMessage {
    public:
        /// @brief 下载信息
        /// @see RemoteFile
        struct Dinfo {
            /// 下载地址
            std::string url;
            /// md5 可用于校验
            std::string md5;
            /// sha1 可用于校验
            std::string sha1;
        };
        /// @brief 文件信息
        /// @see RemoteFile
        struct Finfo {
            /// 文件大小
            QQID size;
            /// 上传者id
            QQID uploaderid;
            /// 下载次数
            unsigned int downloadtime;
            /// 上传时间, 时间戳格式
            QQID uploadtime;
            /// 上次更改时间, 时间戳格式
            QQID lastmodifytime;
        };
        /// 文件唯一id, 用于识别
        std::string id;
        /// 文件内部id, 用于构造miraiCode发送
        unsigned int internalid;
        /// 文件名
        std::string name;
        /// 文件大小
        long long size;
        /// 文件在群文件的路径
        std::string path;
        /// 文件下载信息
        /// @see dinfo
        Dinfo dinfo;
        /// 文件信息
        /// @see finfo
        Finfo finfo;

        std::string serializeToString();

        RemoteFile plus(unsigned int ii) {
            RemoteFile tmp(*this);
            tmp.internalid = ii;
            tmp.content = id + "," + std::to_string(ii) + "," + Tools::escapeToMiraiCode(std::move(name)) + "," +
                          std::to_string(size);
            return tmp;
        }

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
        explicit RemoteFile(const std::string &i, unsigned int ii, std::string n, long long s, std::string p,
                            struct Dinfo d, struct Finfo f) : SingleMessage(6, i + "," + std::to_string(ii) + "," +
                                                                                       Tools::escapeToMiraiCode(std::move(n)) +
                                                                                       "," +
                                                                                       std::to_string(s)),
                                                              id(i),
                                                              internalid(ii),
                                                              name(std::move(n)),
                                                              size(s),
                                                              path(std::move(p)),
                                                              dinfo(std::move(d)),
                                                              finfo(f){};

        /// 仅在上传后构建的有效, 即获取到internalid时(internalid != 0) 否则重新上传并重新获取internalid再转换
        std::string toMiraiCode() const override {
            if (internalid == 0) {
                // 重新上传
                throw RemoteAssetException("toMiraiCode error: internalid, 错误，重新上传");
            }
            return "[mirai:file:" + id + "," + std::to_string(internalid) + "," + Tools::escapeToMiraiCode(name) + "," +
                   std::to_string(size) + "]";
        }
    };

    /// @brief 目前不支持的消息类型
    class UnSupportMessage : public SingleMessage {
    public:
        std::string content;

        std::string toMiraiCode() const override {
            return content;
        }

        explicit UnSupportMessage(const SingleMessage &s) : SingleMessage(s){};

        explicit UnSupportMessage(const std::string &content) : SingleMessage(-1, content) {}
    };

    /// 消息链, 一般由SingleMessage组成
    class MessageChain : public MiraiCodeable {
    public:
        class Message {
        private:
            std::shared_ptr<SingleMessage> content;

        public:
            /// 代表的子类
            /// @see MessageChain::messageType
            int type() const {
                return this->content->type;
            };

            template<class T>
            explicit Message(T a) {
                static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
                content = std::make_shared<SingleMessage>(a);
            }

            explicit Message(std::shared_ptr<SingleMessage> a) {
                content = std::move(a);
            }

            std::shared_ptr<SingleMessage> getPtr() {
                return this->content;
            }

            std::shared_ptr<SingleMessage> operator->() const {
                return this->content;
            }

            template<class T>
            T get() {
                auto tmp = this->content;
                switch (tmp->type) {
                    case -1:
                        if (std::is_same_v<T, UnSupportMessage>)
                            return (UnSupportMessage) (*tmp);
                        else
                            throw IllegalArgumentException("转换错误");
                    case 0:
                        if (std::is_same_v<T, PlainText>)
                            return (PlainText) (*tmp);
                        else
                            throw IllegalArgumentException("转换错误");
                    case 1:
                        if (std::is_same_v<T, At>)
                            return (At) (*tmp);
                        else
                            throw IllegalArgumentException("转换错误");
                    case 2:
                        if (std::is_same_v<T, Image>)
                            return (Image) (*tmp);
                        else
                            throw IllegalArgumentException("转换错误");
                    case 3:
                        if (std::is_same_v<T, LightApp>)
                            return (LightApp) (*tmp);
                        else
                            throw IllegalArgumentException("转换错误");
                    case 4:
                        if (std::is_same_v<T, ServiceMessage>)
                            return (ServiceMessage) (*tmp);
                        else
                            throw IllegalArgumentException("转换错误");
                    default: // cannot reach
                        throw APIException("");
                }
            }

            std::variant<PlainText, At, Image, LightApp, ServiceMessage, UnSupportMessage> get() const {
                auto tmp = this->content;
                switch (tmp->type) {
                    case -1:
                        return (UnSupportMessage) (*tmp);
                    case 0:
                        return (PlainText) (*tmp);
                    case 1:
                        return (At) (*tmp);
                    case 2:
                        return (Image) (*tmp);
                    case 3:
                        return (LightApp) (*tmp);
                    case 4:
                        return (ServiceMessage) (*tmp);
                    default:
                        throw APIException("位置MessageChain::get");
                }
            }
        };

    private:
        void p(std::vector<Message> *v) {}

        template<class T1, class... T2>
        void p(std::vector<Message> *v, T1 h, T2... args) {
            static_assert(std::is_base_of_v<SingleMessage, T1>, "只支持SingleMessage子类");
            v->push_back(Message(h));
            p(v, args...);
        }

        template<class... T2>
        void p(std::vector<Message> *v, std::string h, T2... args) {
            v->push_back(Message(PlainText(h)));
            p(v, args...);
        }

        template<class... T2>
        void p(std::vector<Message> *v, const char *h, T2... args) {
            v->push_back(Message(PlainText(h)));
            p(v, args...);
        }

        std::vector<Message> content;

        MessageSource quoteAndSend0(const std::string &msg, QQID groupid = -1,
                                    JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            json obj;
            json sign;
            obj["messageSource"] = this->source->serializeToString();
            obj["msg"] = msg;
            sign["MiraiCode"] = true;
            sign["groupid"] = groupid;
            obj["sign"] = sign.dump();
            std::string re = Config::koperation(Config::SendWithQuote, obj, env);
            return MessageSource::deserializeFromString(re);
        }

        template<class T>
        MessageSource quoteAndSend1(T s, QQID groupid = -1, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return this->quoteAndSend0(s.toMiraiCode(), groupid, env);
        }

        MessageSource quoteAndSend1(std::string s, QQID groupid, JNIEnv *env) {
            return this->quoteAndSend0(s, groupid, env);
        }

        MessageSource quoteAndSend1(MessageChain mc, QQID groupid, JNIEnv *env) {
            return this->quoteAndSend0(mc.toMiraiCode(), groupid, env);
        }

    public:
        size_t size() {
            return this->content.size();
        }

        const std::vector<Message> &vector() {
            return this->content;
        }

        /// 如果由MiraiCP构造(incoming)就会存在，否则则不存在
        std::optional<MessageSource> source = std::nullopt;

        /// @brief 找到miraiCode结尾的`]`
        /// @param s 文本
        /// @param start 开始位置
        /// @return 如果不存在返回-1, 存在则返回index
        static size_t findEnd(const std::string &s, size_t start) {
            size_t pos = start;
            while (pos < s.length()) {
                switch (s[pos]) {
                    case '\\':
                        pos += 2;
                        continue;
                    case ']':
                        return pos;
                }
                pos++;
            }
            return -1;
        }

        std::string toMiraiCode() const override {
            return Tools::VectorToString(this->toMiraiCodeVector(), "");
        }

        std::vector<std::string> toMiraiCodeVector() const {
            std::vector<std::string> tmp;
            for (const auto &a: this->content)
                tmp.emplace_back(a->toMiraiCode());
            return tmp;
        }

        /// @brief 添加元素
        /// @tparam T 任意的SingleMessage的子类
        /// @param a 添加的值
        template<class T>
        void add(const T &a) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只接受SingleMessage的子类");
            this->content.push_back(Message(a));
        }

        void add(const MessageSource &val) {
            this->source = val;
        }

        /// 筛选出某种特点type的信息
        template<class T>
        std::vector<T> filter(int type) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto a: this->content) {
                if (a->type == type)
                    re.push_back(std::static_pointer_cast<T>(a));
            }
            return re;
        }

        /// 自定义筛选器
        template<class T>
        std::vector<T> filter(std::function<bool(Message)> func) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for (auto a: this->content) {
                if (func(a))
                    re.push_back(std::static_pointer_cast<T>(a));
            }
            return re;
        }

        /// 找出第一个指定的type的信息
        template<class T>
        T first(int type) {
            for (auto a: this->content)
                if (a->type == type)
                    return std::static_pointer_cast<T>(a);
        }

        /// incoming构造器
        template<class... T>
        explicit MessageChain(MessageSource ms, T... args) : source(std::move(ms)) {
            this->p(&this->content, args...);
        };

        /// outcoming构造器
        template<class... T>
        explicit MessageChain(T... args) {
            p(&this->content, args...);
        };

        /// outcoming 构造器
        template<class T>
        explicit MessageChain(const T &msg) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage子类");
            this->content.push_back(Message(msg));
        };

        template<class T>
        [[nodiscard]] MessageChain plus(const T &a) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            MessageChain tmp(*this);
            tmp.content.push_back(std::make_shared<SingleMessage>(a));
            return tmp;
        }

        MessageChain plus(const MessageSource &ms) {
            MessageChain tmp(*this);
            tmp.source = ms;
            return tmp;
        }

        template<class T>
        MessageChain operator+(const T &msg) {
            return this->plus(msg);
        }

        Message operator[](size_t i) {
            return this->content[i];
        }

        /// @brief 回复并发送
        /// @param s 内容
        /// @param groupid 如果是来源于TempGroupMessage就要提供(因为要找到那个Member)
        /// @note 可以改MessageSource里的内容, 客户端在发送的时候并不会校验MessageSource的内容正确性(比如改originalMessage来改引用的文本的内容, 或者改id来定位到其他信息)
        /// @detail 支持以下类型传入
        /// - std::string / const char* 相当于传入PlainText(str)
        /// - SingleMessage的各种派生类
        /// - MessageChain
        template<class T>
        [[deprecated("use Contact.quoteAndSend")]] MessageSource
        quoteAndSendMessage(T s, QQID groupid = -1, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return this->quoteAndSend1(s, groupid, env);
        }

        /// 从string构建MessageChain, 常用于Incoming message
        static MessageChain deserializationFromMiraiCode(const std::string &m);

        static MessageChain deserializationFromMessageSourceJson(const json &tmp);
    };

    /*!
    * @brief group, friend, member的父类
    */
    class Contact {
    private:
        /// 发送纯文本信息
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        MessageSource sendMsg0(const std::string &msg, int retryTime, bool miraicode = false,
                               JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        template<class T>
        MessageSource send1(T msg, int retryTime, JNIEnv *env) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

        MessageSource send1(MessageChain msg, int retryTime, JNIEnv *env) {
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

        MessageSource send1(MiraiCode msg, int retryTime, JNIEnv *env) {
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

        MessageSource send1(std::string msg, int retryTime, JNIEnv *env) {
            return sendMsg0(msg, retryTime, false, env);
        }

        MessageSource send1(const char *msg, int retryTime, JNIEnv *env) {
            return sendMsg0(std::string(msg), retryTime, false, env);
        }

        MessageSource
        quoteAndSend0(const std::string &msg, MessageSource ms, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            json obj;
            json sign;
            obj["messageSource"] = ms.serializeToString();
            obj["msg"] = msg;
            sign["MiraiCode"] = true;
            sign["groupid"] = this->groupid();
            obj["sign"] = sign.dump();
            std::string re = Config::koperation(Config::SendWithQuote, obj, env);
            return MessageSource::deserializeFromString(re);
        }

        template<class T>
        MessageSource quoteAndSend1(T s, MessageSource ms, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的派生类");
            return this->quoteAndSend0(s.toMiraiCode(), ms, env);
        }

        MessageSource quoteAndSend1(std::string s, MessageSource ms, JNIEnv *env) {
            return this->quoteAndSend0(s, ms, env);
        }

        MessageSource quoteAndSend1(MessageChain mc, MessageSource ms, JNIEnv *env) {
            return this->quoteAndSend0(mc.toMiraiCode(), ms, env);
        }

    protected:
        int _type = 0;
        QQID _id;
        QQID _groupid;
        std::string _nickOrNameCard;
        std::string _avatarUrl;
        QQID _botid;
        bool _anonymous = false;

        /// 发送语音
        MessageSource sendVoice0(const std::string &path, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

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

        bool operator==(const Contact &c) const {
            return this->id() == c.id();
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
        explicit Contact(int type, QQID id, QQID gid, const std::string &name, QQID botid, bool anonymous = false) {
            this->_type = type;
            this->_id = id;
            this->_groupid = gid;
            this->_nickOrNameCard = name;
            this->_botid = botid;
            this->_anonymous = anonymous;
        };

        /// @brief 当前对象类型
        ///     - 1 Friend 好友
        ///     - 2 Group 群聊
        ///     - 3 Member 群成员
        int type() const { return this->_type; }

        /// @brief id在全部情况存在
        ///     - 当当前type为1(Friend)时，为好友id
        ///     - 当当前type为2(Group)时，为群id
        ///     - 当当前type为3(Member)时，为群成员id
        QQID id() const { return this->_id; }

        /// @brief 当type为3的时候存在，否则为0，可以看作补充id
        ///     - 当当前type为1(Friend)时，为0
        ///     - 当当前type为2(Group)时，为0
        ///     - 当当前type为3(Member)时，为群号
        /// @attention 当当前type为2(Group)时，为0，不为群号，id才是群号
        QQID groupid() const { return this->_groupid; }

        /// 群名称，群成员群名片，或好友昵称
        std::string nickOrNameCard() { return this->_nickOrNameCard; };

        /// 头像url地址
        std::string avatarUrl() { return this->_avatarUrl; };

        /// 所属bot
        QQID botid() const { return this->_botid; };

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
         * @param j json节点
         * @return Contact
         */
        static Contact deserializationFromJson(nlohmann::json j);

        /// 反序列化成bot，可以通过serializationToString序列化，利于保存
        /// @see Contact::serializationToString()
        /// @param source 序列化后的文本
        /// @throw APIException
        static Contact deserializationFromString(const std::string &source);

        /// @brief 发送MiraiCode信息
        /// @param msg 发送的MiraiCode
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        [[deprecated("Use sendMessage")]] MessageSource sendMiraiCode(const MiraiCode &msg, int retryTime = 3,
                                                                      JNIEnv *env = ThreadManager::getEnv(__FILE__,
                                                                                                          __LINE__)) {
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

        /// @brief 回复并发送
        /// @param s 内容
        /// @param groupid 如果是来源于TempGroupMessage就要提供(因为要找到那个Member)
        /// @note 可以改MessageSource里的内容, 客户端在发送的时候并不会校验MessageSource的内容正确性(比如改originalMessage来改引用的文本的内容, 或者改id来定位到其他信息)
        /// @detail 支持以下类型传入
        /// - std::string / const char* 相当于传入PlainText(str)
        /// - SingleMessage的各种派生类
        /// - MessageChain
        template<class T>
        MessageSource
        quoteAndSendMessage(T s, MessageSource ms, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return this->quoteAndSend1(s, ms, env);
        }

        template<class... T>
        MessageSource quoteAndSendMessage(MessageSource ms, T... val) {
            return this->quoteAndSendMessage(MessageChain(val...), ms);
        }

        template<class... T>
        MessageSource sendMessage(T... msg) {
            return this->sendMessage(MessageChain(msg...));
        }

        /// @brief 发送一条Message
        /// @detail 支持
        /// - std::string: 相当于发送PlainText(str), 不会反序列化miraicode
        /// - MiraiCode: 相当于发送反序列化后的
        /// - 各种SingleMessage的子类
        /// - MessageChain
        /// @param msg Message
        /// @param retryTime 重试次数
        /// @return MessageSource
        template<class T>
        MessageSource sendMessage(T msg, int retryTime = 3, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return this->send1(msg, retryTime, env);
        }

        /// @brief 发送纯文本信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        [[deprecated("Use sendMessage")]] MessageSource
        sendMsg(const std::string &msg, int retryTime = 3, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return sendMsg0(msg, retryTime, false, env);
        }

        /// @brief 以纯文本发送MiraiCode信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        [[deprecated("Use sendMessage")]] MessageSource
        sendMsg(const MiraiCode &msg, int retryTime = 3, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return sendMsg0(msg.toMiraiCode(), retryTime, false, env);
        }

        /// @brief 发送纯文本信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        [[deprecated("Use sendMessage")]] MessageSource sendMsg(std::vector<std::string> msg, int retryTime = 3,
                                                                JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return sendMsg0(Tools::VectorToString(std::move(msg)), retryTime, false, env);
        }

        /*!
        * @brief上传本地图片，务必要用绝对路径
        * 由于mirai要区分图片发送对象，所以使用本函数上传的图片只能发到群
        * @attention 最大支持图片大小为30MB
        * @throws
        * -可能抛出UploadException异常代表路径无效或大小大于30MB
        * -可能抛出MemberException找不到群或群成员
        */
        Image uploadImg(const std::string &path, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 刷新当前对象信息
        virtual void refreshInfo(JNIEnv *){};
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
        static inline std::string getInfoSource(Contact *c, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
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
        uploadImg0(const std::string &path, Contact *c, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
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

    ///聊天记录里每个消息
    class ForwardNode {
    public:
        ///发送者id
        QQID id = 0;
        ///发送者昵称
        std::string name;
        ///发送信息
        std::string message;
        ///发送时间
        int time = 0;

        /// @brief 聊天记录里的每条信息
        /// @param id - 发送者id
        /// @param name - 发送者昵称
        /// @param message - 发送的信息
        /// @param time - 发送时间，以时间戳记
        ForwardNode(QQID id, const std::string &name, const std::string &message,
                    int time)
            : id(id), name(name), message(message), time(time) {}

        /// @brief 构造聊天记录里每条信息
        /// @param c - 发送者的contact指针
        /// @param message - 发送的信息
        /// @param t - 发送时间，时间戳格式
        ForwardNode(Contact *c, const std::string &message, int t) : id(c->id()), name(c->nickOrNameCard()),
                                                                     message(message),
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
        MessageSource sendTo(Contact *c, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));
    };


    /// 好友类声明
    class Friend : public Contact {
    public:
        /*!
         * @brief 构建好友对象
         * @param friendid q号
         * @param botid 对应机器人id
         */
        explicit Friend(QQID friendid, QQID botid, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        explicit Friend(Contact c) : Contact(std::move(c)) { refreshInfo(); };

        /// 删除好友(delete是C++关键字
        void deleteFriend(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["source"] = this->serializationToString();
            j["quit"] = true;
            Config::koperation(Config::RefreshInfo, j, env);
        }

        void refreshInfo(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) override {
            std::string temp = LowLevelAPI::getInfoSource(this, env);
            if (temp == "E1") {
                throw FriendException();
            }
            LowLevelAPI::info tmp = LowLevelAPI::info0(temp);
            this->_nickOrNameCard = tmp.nickornamecard;
            this->_avatarUrl = tmp.avatarUrl;
        }

        /*!
         * @brief 发送戳一戳
         * @warning 发送戳一戳的前提是登录该bot的协议是phone
         * @throw MiraiCP::BotException, MiraiCP::IllegalStateException
         */
        void sendNudge() {
            json j;
            j["contactSource"] = this->serializationToString();
            std::string re = Config::koperation(Config::SendNudge, j);
            if (re == "E1")
                throw IllegalStateException("发送戳一戳失败，登录协议不为phone");
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

        /// @brief 更改群成员权限
        /// @param admin 如果为true为更改到管理员
        /// @param env
        void modifyAdmin(bool admin, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__));

        /// @brief 构建群成员对象
        /// @param qqid 该成员q号
        /// @param groupid 所在群号
        /// @param botid 机器人id
        /// @example 在事件中构建Member对象
        /// @code
        ///  Member(this.sender.id, this.group.id, this.bot.id)
        /// @endcode
        explicit Member(QQID qqid, QQID groupid, QQID botid,
                        JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        explicit Member(Contact c) : Contact(std::move(c)) {
            this->isAnonymous = this->_anonymous;
            refreshInfo();
        };

        /// 是否是匿名群成员, 如果是匿名群成员一些功能会受限
        bool isAnonymous = false;

        /// 重新获取(刷新)群成员信息
        void refreshInfo(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) override {
            if (isAnonymous)
                return;
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

        /// 发送语音
        MessageSource sendVoice(const std::string &path, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return Contact::sendVoice0(path, env);
        }

        /// 获取权限，会在构造时调用，请使用permission缓存变量
        /// @see Member::permission
        unsigned int getPermission(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /*!
         * 禁言当前对象，单位是秒，最少0秒最大30天，如果为0或者为负则unmute
         * @throws BotException, MuteException
        */
        void mute(int time, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 取消禁言
        /// @throws BotException, MuteException
        void unMute(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            mute(0, env);
        }

        /*! 踢出这个群成员
        * @param reason - 原因
         * @example 踢出群成员
         * @code
         * Event::processor.registerEvent([](GroupMessageEvent e) {
            try {
                Member m = Member(qqid, e.group.id(), e.bot.id());
                m.Kick("this_is_reason");
            }
            catch (BotException err) {
                //权限不足
                Logger::logger.error(err.what());
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
        void kick(const std::string &reason, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// At一个群成员
        At at() {
            /*返回at这个人的miraicode*/
            return At(this->id());
        }

        /*!
         * @brief 发送戳一戳
         * @warning 发送戳一戳的前提是登录该bot的协议是phone
         * @throw MiraiCP::BotException, MiraiCP::IllegalStateException
         */
        void sendNudge() {
            if (isAnonymous) return;
            json j;
            j["contactSource"] = this->serializationToString();
            std::string re = Config::koperation(Config::SendNudge, j);
            if (re == "E1")
                throw IllegalStateException("发送戳一戳失败，登录协议不为phone");
        }
    };

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
        QQID id;

        /*!
         * @brief 刷新bot信息
         * @param env
         */
        void refreshInfo(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["source"] = Contact(4, 0, 0, "", this->id).serializationToString();
            LowLevelAPI::info tmp = LowLevelAPI::info0(Config::koperation(Config::RefreshInfo, j, env));
            this->_avatarUrl = tmp.avatarUrl;
            this->_nick = tmp.nickornamecard;
        }

        /// 用id构建机器人
        explicit Bot(QQID i) : id(i) {}

        /// 取好友
        Friend getFriend(QQID i, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) const {
            return Friend(i, this->id, env);
        }

        /// 取群聊
        Group getGroup(QQID groupid, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) const {
            return Group(groupid, this->id, env);
        }

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
        std::vector<unsigned long long> getFriendList(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["botid"] = this->id;
            std::string temp = Config::koperation(Config::QueryBFL, j, env);
            return Tools::StringToVector(temp);
        }

        /// 好友列表string形式返回，利于保存
        std::string FriendListToString() {
            return Tools::VectorToString(getFriendList());
        }

        /// 取群列表
        std::vector<unsigned long long> getGroupList(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["botid"] = this->id;
            std::string temp = Config::koperation(Config::QueryBGL, j, env);
            return Tools::StringToVector(temp);
        }

        /// 群列表string形式返回，利于保存
        std::string GroupListToString() {
            return Tools::VectorToString(getGroupList());
        }

        bool operator==(const Contact &c) const {
            return this->id == c.id();
        }

        bool operator==(const Bot &b) const {
            return this->id == b.id;
        }
    };

    /// Event 基类
    class MiraiCPEvent {};

    /// 所以事件处理timeoutevent都是机器人事件，指都有机器人实例
    class BotEvent : public MiraiCPEvent {
    public:
        /// 该事件接受的机器人
        Bot bot;
        /// 以该机器人的名义发送日志
        /// @see BotLogger
        IdLogger botlogger;

        explicit BotEvent(QQID botid) : bot(botid), botlogger(botid, &Logger::logger) {
        }
    };

    ///群消息事件声明
    class GroupMessageEvent : public BotEvent {
    public:
        ///来源群
        Group group;
        ///发送人
        Member sender;
        /// 信息
        MessageChain message;

        GroupMessageEvent(QQID botid, const Group &group, const Member &sender,
                          MessageChain mc) : BotEvent(botid), group(group),
                                             sender(sender), message(std::move(mc)){};

        /*!
         * @brief 取群聊下一个消息(群聊与本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain
        nextMessage(long time = -1, bool halt = true, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__));

        /*!
         * @brief 取群聊中同群成员的下一个消息(发送人和群与本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain
        senderNextMessage(long time = -1, bool halt = true, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__));
    };

    /// 私聊消息事件类声明
    class PrivateMessageEvent : public BotEvent {
    public:
        /// 发起人
        Friend sender;
        /// 信息
        MessageChain message{};

        /*!
         * @brief 构建私聊信息
         * @param botid 对应botid
         * @param sender 发送者
         * @param message 消息
         * @param messageSource 消息源
         */
        PrivateMessageEvent(QQID botid, Friend sender, MessageChain mc) : BotEvent(botid), sender(std::move(sender)),
                                                                          message(mc){};

        /*!
         * @brief 取下一个消息(发送人和接收人和本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain
        nextMessage(long time = -1, bool halt = true, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__));
    };

    /// 群聊邀请事件类声明
    class GroupInviteEvent : public BotEvent {
    public:
        /// 事件序列化文本
        std::string source;
        /// 发起人昵称
        std::string inviterNick;
        /// 发起人id
        QQID inviterid = 0;
        /// 被邀请进的组
        std::string groupName;
        /// 群号
        QQID groupid = 0;

        static void operation0(const std::string &source, QQID botid, bool accept,
                               JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["text"] = source;
            j["operate"] = accept;
            j["botid"] = botid;
            std::string re = Config::koperation(Config::Gioperation, j, env);
            if (re == "E") Logger::logger.error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
        }

        void reject(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            GroupInviteEvent::operation0(this->source, this->bot.id, false, env);
        }

        void accept(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            GroupInviteEvent::operation0(this->source, this->bot.id, true, env);
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
        GroupInviteEvent(QQID botid, const std::string &source, const std::string &inviterNick,
                         QQID inviterid, const std::string &groupName, QQID groupid)
            : BotEvent(botid), source(source), inviterNick(inviterNick), inviterid(inviterid), groupName(groupName),
              groupid(groupid) {}
    };

    /// 好友申请事件声明
    class NewFriendRequestEvent : public BotEvent {
    public:
        /// @brief 序列化的事件信息
        std::string source;
        /// @brief 对方id
        QQID fromid;
        QQID fromgroupid;
        /// @brief 对方昵称
        std::string nick;
        /// @brief 申请理由
        std::string message;

        /// @brief 接受好友申请
        /// @param source 事件序列化信息
        static void operation0(const std::string &source, QQID botid, bool accept,
                               JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__), bool ban = false) {
            nlohmann::json j;
            j["text"] = source;
            j["operate"] = accept;
            j["botid"] = botid;
            j["ban"] = ban;
            std::string re = Config::koperation(Config::Nfroperation, j, env);
            if (re == "E") Logger::logger.error("好友申请事件同意失败(可能因为重复处理),id:" + source);
        }

        /// @brief 拒绝好友申请
        void reject(bool ban = false, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            NewFriendRequestEvent::operation0(this->source, this->bot.id, false, env);
        }

        /// @brief 接受申请
        /// @param ban - 是否加入黑名单
        void accept(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            NewFriendRequestEvent::operation0(this->source, this->bot.id, true, env);
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
        NewFriendRequestEvent(QQID botid, const std::string &source,
                              QQID fromid,
                              QQID fromgroupid, const std::string &nick,
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
        int type = 0;
        ///新进入的成员
        Member member;
        ///目标群
        Group group;
        ///邀请人, 当type = 1时存在，否则则和member变量相同
        QQID inviterid;

        /*!
         * @brief 新群成员入群事件
         * @param botid botid
         * @param type 类别 @see MemberJoinEvent::type
         * @param member 入群群成员
         * @param group 群组
         * @param inviterid 邀请群成员id，如果不存在和member id参数一致
         */
        MemberJoinEvent(QQID botid, int type, const Member &member, const Group &group,
                        QQID inviterid) : BotEvent(botid), type(type), member(member),
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
        int type = 0;
        /// 退出的成员q号
        QQID memberid;
        /// 目标群
        Group group;
        /// 操作人, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
        QQID operaterid;

        /*!
         * @brief 群成员离开
         * @param botid
         * @param type
         * @param memberid 退出的群成员
         * @param group 群
         * @param operaterid 操作人id, 主动退出时与member相同，该成员可能是当前bot，名称为operater以与系统operator区分
         */
        MemberLeaveEvent(QQID botid, int type, QQID memberid,
                         Group group,
                         QQID operaterid) : BotEvent(botid), type(type), memberid(memberid),
                                            group(std::move(group)),
                                            operaterid(operaterid) {}
    };

    /// 撤回信息
    class RecallEvent : public BotEvent {
    public:
        /// 为1时是好友私聊中撤回，为2时为群聊内撤回
        int type = 0;
        /// 时间戳
        int time = 0;
        /// 原发送者
        QQID authorid = 0;
        /// 撤回者
        QQID operatorid = 0;
        /// 信息id
        std::string ids;
        //内部ids
        std::string internalids;
        //当type是2的时候存在，否则为0
        QQID groupid = 0;

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
        RecallEvent(QQID botid, int type, int time, QQID authorid,
                    QQID operatorid, std::string ids, std::string internalids,
                    QQID groupid) : BotEvent(botid), type(type), time(time), authorid(authorid),
                                    operatorid(operatorid), ids(std::move(ids)),
                                    internalids(std::move(internalids)),
                                    groupid(groupid) {}
    };

    /// 机器人进入某群
    class BotJoinGroupEvent : public BotEvent {
    public:
        /// 1-主动加入,2-被邀请加入,3-提供恢复群主身份加入
        int type;
        /// 进入的群
        Group group;
        /// 当type=2时存在，为邀请人，否则为空，调用可能会报错
        QQID inviterid;

        /*!
         * @brief bot加入群
         * @param botid 对应bot
         * @param type 类别
         * @param group 加入的群
         * @param inviter 邀请人
         */
        BotJoinGroupEvent(QQID botid, int type, Group group,
                          QQID inviter)
            : BotEvent(botid), type(type), group(std::move(group)), inviterid(inviter) {}
    };

    /// 群临时会话
    class GroupTempMessageEvent : public BotEvent {
    public:
        /// 来源群
        Group group;
        /// 发送人
        Member sender;
        /// 信息
        MessageChain message;

        /*!
         * @brief 群临时会话消息事件
         * @param botid 对应bot
         * @param group 发起的群
         * @param sender 发送消息对象
         * @param message 消息
         * @param messageSource 消息源
         */
        GroupTempMessageEvent(QQID botid, Group group, Member sender,
                              MessageChain message) : BotEvent(botid),
                                                      group(std::move(group)),
                                                      sender(std::move(sender)),
                                                      message(std::move(message)) {}
    };

    /// 定时任务结束
    class TimeOutEvent : public MiraiCPEvent {
    public:
        /// 事件所附信息
        std::string msg;

        explicit TimeOutEvent(std::string msg) : msg(std::move(msg)) {}
    };

    /// @brief 开启一个新的定时任务
    /// @param time 延迟多久，毫秒为单位
    /// @param msg string类型附带信息，推荐使用json格式方便解析
    inline void schedule(long time, const std::string &msg, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
        json j;
        j["time"] = time;
        j["msg"] = msg;
        Config::koperation(Config::TimeOut, j, env);
    }

    /// 机器人上线事件
    class BotOnlineEvent : public BotEvent {
    public:
        explicit BotOnlineEvent(QQID botid) : BotEvent(botid) {}
    };

    /// 戳一戳事件
    class NudgeEvent : public BotEvent {
    public:
        /// 谁发送的
        Contact from;
        Contact target;

        NudgeEvent(Contact c, Contact target, QQID botid) : BotEvent(botid), from(std::move(c)),
                                                            target(std::move(target)) {}
    };

    /// 机器人退群事件
    /// 可能有3种类型, 主动退/被踢/解散
    /// 目前mirai的botLeave事件还不稳定暂时不支持类型
    class BotLeaveEvent : public BotEvent {
    public:
        /// 退出的群
        /// @attension 收到这个事件时已经退出该群, 可能取不到相关信息
        QQID groupid;

        BotLeaveEvent(QQID g, QQID botid) : BotEvent(botid), groupid(g) {}
    };

    /// 申请加群事件, bot需为管理员或者群主
    class MemberJoinRequestEvent : public BotEvent {
    private:
        std::string source;

        void operate(const std::string &s, QQID botid, bool sign, const std::string &msg = "",
                     JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) const {
            json j;
            j["source"] = s;
            j["botid"] = botid;
            j["sign"] = sign;
            j["msg"] = msg;
            Config::koperation(Config::MemberJoinRequest, j, env);
        }

    public:
        /// 申请的群, 如果不存在就表明广播这个事件的时候机器人已经退出该群
        std::optional<Group> group;
        /// 邀请人, 如果不存在表明这个邀请人退出了群或没有邀请人为主动进群
        std::optional<Member> inviter;

        MemberJoinRequestEvent(std::optional<Group> g, std::optional<Member> i, QQID botid, const std::string &source)
            : BotEvent(botid), group(std::move(g)), inviter(std::move(i)), source(source){};

        /// 通过
        void accept() {
            operate(this->source, this->bot.id, true);
        }

        /// 拒绝
        void reject(const std::string &msg) {
            operate(this->source, this->bot.id, false, msg);
        }
    };

    /// @brief 异常抛出事件
    class MiraiCPExceptionEvent : public MiraiCPEvent {
    private:
        MiraiCPException *exceptionPtr;

    public:
        explicit MiraiCPExceptionEvent(MiraiCPException *err) {
            exceptionPtr = err;
        }

        MiraiCPException &getException() {
            return *exceptionPtr;
        }
    };

    class Event {
    private:
        template<typename T>
        int id() const {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持广播继承MiraiCPEvent的事件");
            if constexpr (std::is_same_v<T, GroupMessageEvent>) {
                return 0;
            } else if constexpr (std::is_same_v<T, PrivateMessageEvent>) {
                return 1;
            } else if constexpr (std::is_same_v<T, GroupInviteEvent>) {
                return 2;
            } else if constexpr (std::is_same_v<T, NewFriendRequestEvent>) {
                return 3;
            } else if constexpr (std::is_same_v<T, MemberJoinEvent>) {
                return 4;
            } else if constexpr (std::is_same_v<T, MemberLeaveEvent>) {
                return 5;
            } else if constexpr (std::is_same_v<T, RecallEvent>) {
                return 6;
            } else if constexpr (std::is_same_v<T, BotJoinGroupEvent>) {
                return 7;
            } else if constexpr (std::is_same_v<T, GroupTempMessageEvent>) {
                return 8;
            } else if constexpr (std::is_same_v<T, BotOnlineEvent>) {
                return 9;
            } else if constexpr (std::is_same_v<T, NudgeEvent>) {
                return 10;
            } else if constexpr (std::is_same_v<T, BotLeaveEvent>) {
                return 11;
            } else if constexpr (std::is_same_v<T, MemberJoinRequestEvent>) {
                return 12;
            } else if constexpr (std::is_same_v<T, TimeOutEvent>) {
                return 13;
            } else if constexpr (std::is_same_v<T, MiraiCPExceptionEvent>) {
                return 14;
            }
            Logger::logger.error("内部错误, 位置:C-Head");
            return -1;
        }
        Event() = default;
        using type = std::variant<GroupMessageEvent, PrivateMessageEvent, GroupInviteEvent, NewFriendRequestEvent, MemberJoinEvent, MemberLeaveEvent, RecallEvent, BotJoinGroupEvent, GroupTempMessageEvent, BotOnlineEvent, TimeOutEvent, NudgeEvent, BotLeaveEvent, MemberJoinRequestEvent, MiraiCPExceptionEvent>;
        template<typename T>
        class Node {
        public:
            bool enable = true;
            std::function<void(T)> func;
            explicit Node(std::function<void(T)> f) : func(std::move(f)) {}
            void run(type a) const {
                func(std::get<T>(a));
            }
        };
        using e = std::variant<Node<GroupMessageEvent>, Node<PrivateMessageEvent>, Node<GroupInviteEvent>, Node<NewFriendRequestEvent>, Node<MemberJoinEvent>, Node<MemberLeaveEvent>, Node<RecallEvent>, Node<BotJoinGroupEvent>, Node<GroupTempMessageEvent>, Node<BotOnlineEvent>, Node<NudgeEvent>, Node<BotLeaveEvent>, Node<MemberJoinRequestEvent>, Node<MiraiCPExceptionEvent>, Node<TimeOutEvent>>;
        std::vector<e> vec[15] = {std::vector<e>()};

    public:
        /// 事件监听操控, 可用于stop停止监听和resume继续监听
        class NodeHandle {
        private:
            bool *enable;

        public:
            explicit NodeHandle(bool *a) {
                this->enable = a;
            }
            void stop() {
                *enable = false;
            }
            void resume() {
                *enable = true;
            }
        };
        // singleton mode
        static Event processor;
        /// 广播一个事件, 必须为MiraiCPEvent的派生类
        template<typename T>
        void broadcast(T val) {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持广播MiraiCPEvent的派生类");
            /// 清空stack中内容, 不然可能保留上一次Event的操作
            ThreadManager::getThread()->stack.clear();
            ThreadManager::getThread()->stack.push(__FILE__, __LINE__, __func__, typeid(T).name());
            for (e a: vec[id<T>()]) {
                std::get<Node<T>>(a).run(static_cast<type>(val));
            }
        }
        /// 注册一个事件
        template<typename T>
        NodeHandle registerEvent(std::function<void(T)> a) {
            static_assert(std::is_base_of_v<MiraiCPEvent, T>, "只支持注册MiraiCPEvent的派生类事件");
            auto t = Node<T>(a);
            vec[id<T>()].push_back(static_cast<e>(t));
            return NodeHandle(&t.enable);
        }
    };

    void enrollPlugin();

    inline void enrollPlugin0(CPPPlugin *p) {
        CPPPlugin::plugin = p;
    }

    // endregion
    // 结束MiraiCP类声明代码
} // namespace MiraiCP
#endif //CPP_MIRAICP_HPP
#pragma clang diagnostic pop
