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
#include <utility>
#include <variant>
#include <optional>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <regex>
#include <thread>
#include <map>
#include <mutex>
#include <stack>

namespace MiraiCP {
    using json = nlohmann::json;
    using QQID = unsigned long long;
    // 开始声明MiraiCP常量声明代码
    /// MiraiCP当前版本
    const std::string MiraiCPVersion = "v2.8.0-M1";

    /// @brief 插件信息
    class PluginConfig{
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
        PluginConfig(std::string id, std::string name, std::string version, std::string author,
                     std::string description = "", std::string time = __DATE__) : id(std::move(id)), name(std::move(name)), version(std::move(version)),
                                                                                          author(std::move(author)), description(std::move(description)),
                                                                                          time(std::move(time)) {}
        json serialize();
        std::string serialize2string(){
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
    struct LightAppStyle4{
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
    struct URLSharer{
        /// 简介 没点进来看见的样子
        std::string brief="简介 没点进来看见的样子";
        /// 目标url
        std::string url="目标url";
        /// 图标地址
        std::string cover="图标地址";
        /// 标题
        std::string title = "标题";
        /// 描述文字
        std::string summary="描述文字";
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
    public:
        class StackTracer{
            std::vector<std::string> stackTrace = std::vector<std::string>();
        public:

            /// print all
            std::string print(){
                std::string re = "StackTrace:";
                for(const auto& a:stackTrace)
                    re += "\n" + a;
                return re;
            }

            /// push stack
            void push(const std::string& file = __FILE__, int loc = __LINE__, const std::string& func = "", const std::string& commit = ""){
                stackTrace.push_back(func + (!commit.empty() ?"(" + commit + ")":"")+"[" +file+":"+std::to_string(loc)+ "]");
            }

            /// 清空
            void clear(){
                stackTrace.clear();
            }
        };
        /// @brief 每个线程实例.
        struct ThreadInfo {
            JNIEnv *e{};
            bool attach{};
            StackTracer stack;
        };
        static std::map<std::string, ThreadInfo> threads;/// < 线程池(线程id:env).
        static std::recursive_mutex mtx; ///< 线程池读写锁.
    private:
        static void newEnv(const char *threadName = nullptr);///< 新建一个env，于getEnv中没取到env时调用.
        /// 判断该线程id是否包含在线程池里
        static bool included(const std::string &id);

    public:
        /// 获取线程
        static ThreadInfo* getThread(){
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
        static JNIEnv *getEnv(const std::string& file, int loc, const std::string& func = "");
    };

    std::map<std::string, ThreadManager::ThreadInfo> ThreadManager::threads = std::map<std::string, ThreadInfo>();
    std::recursive_mutex ThreadManager::mtx = std::recursive_mutex();

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
        static std::string VectorToString(std::vector<T> a, const std::string& = ",");

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
        static bool starts_with(const std::string& f, const std::string& s){return f.rfind(s, 0) == 0;}
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
            ModifyAdmin
        };

        /**
         * @brief 调用mirai操作
         * @param type 操作id
         * @param data 传入数据
         * @return 返回数据
         */
        static std::string koperation(operation_set type, nlohmann::json &data, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__), bool catchErr = true, const std::string& errorInfo = "");

        static void construct(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        static void destruct();
    };

    jclass Config::CPP_lib = nullptr;
    jmethodID Config::KOperation = nullptr;

/// MiraiCode父类, 指可以被转换成miraicode的类型
    class MiraiCodeable {
    public:
        virtual std::string toMiraiCode() = 0;
    };

/// @brief miraicode字符串
/// @attention MiraiCode会把非miraicode组成部分(非[mirai:])转码, 输出转码前的文本用toString, 参考: https://github.com/mamoe/mirai/blob/dev/docs/Messages.md#%E8%BD%AC%E4%B9%89%E8%A7%84%E5%88%99
/// @detail 为了便捷使用，构造函数不以explicit注释
    class MiraiCode:MiraiCodeable {
    private:
        std::string content;
    public:
        /// 输出当前内容, 会自动转码
        std::string toString() {
            return Tools::escapeFromMiraiCode(this->content);
        }

        /// 和toString作用一样, 不过不会自动转码
        std::string toMiraiCode() override {
            return content;
        }

        /// 从MiraiCodeable类型初始化一个miraicode字符串
        MiraiCode(MiraiCodeable *a) {  // NOLINT(google-explicit-constructor)
            content = a->toMiraiCode();
        }

        /// 从文本初始化一个miraicode字符串, 根据第二个参数决定是否转码, 默认不转码
        /// @attention 如果是传入文本MiraiCode，请勿转码，转码只是为了[mirai:xxx:<应该转码的部分>], 如果<应该转码>的部分里面含有'[]:,'内容，请调用Tools::escapeToMiraiCode转码
        MiraiCode(const std::string& a, bool convert = false){ // NOLINT(google-explicit-constructor)
            if(!convert)
                content = a;
            else
                content = Tools::escapeToMiraiCode(a);
        }

        MiraiCode operator+(MiraiCodeable *a) {
            return {content + a->toMiraiCode()};
        }

        MiraiCode operator+(const std::string& a) {
            return {content + a};
        }

        MiraiCode operator+(const MiraiCode& a){
            return {content + a.content};
        }

        MiraiCode operator+(MiraiCode* a){
            return {content + a->content};
        }

        MiraiCode operator=(const std::string& a) {
            return {a};
        }

        MiraiCode plus(MiraiCodeable *a) {
            return {content + a->toMiraiCode()};
        }

        MiraiCode plus(const std::string& a) {
            return MiraiCode(a)+this;
        }

        /// 不执行转义，适用于已经被MiraiCode转义过的字符串
        static MiraiCode MiraiCodeWithoutEscape(const std::string& a){
            return {a, false};
        }
        /// 不执行转义，因为MiraiCodeable的toMiraiCode已经转义过了
        static MiraiCode MiraiCodeWithoutEscape(MiraiCodeable* a){
            return {a->toMiraiCode(), false};
        }
    };

/*!
 * @class Logger
 * @brief 以MiraiCP的名义发送日志, 日志表现格式是: 2021-06-28 09:37:22 [log level]/MiraiCP: [log content], 为最底层的logger
 * @example 发送日志
	 *  发送消息级日志
	 *  @code Logger::logger.info(string) @endcode
	 *  发送警告级日志
	 *  @code Logger::logger.warning(string) @endcode
	 *  发送错误级日志
	 *  @code Logger::logger.error(string) @endcode
 */
    class Logger_interface {
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
            Action action = [](const std::string& content, int level) {};
        };
        Handler loggerhandler;

        // 初始化 获取methodid
        void init(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        ///发送普通(info级日志)
        void info(const std::string &, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        template<typename T>
        void info(std::vector<T> content, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
            info(Tools::VectorToString(content), env);
        }

        template<typename T>
        void info(T content, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
            std::stringstream sst;
            sst << content;
            info(sst.str(), env);
        }

        ///发送普通(info级日志)
        void info(MiraiCode msg, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            info(msg.toString(), env);
        }

        ///发送警告(warning级日志)
        void warning(const std::string &, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        ///发送警告(warning级日志)
        void warning(MiraiCode msg, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            warning(msg.toString(), env);
        }

        ///发送错误(error级日志)
        void error(const std::string &, bool printStack = true, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        ///发送错误(error级日志)
        void error(MiraiCode msg, bool printStack = true, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            error(msg.toString(), printStack, env);
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
    class Logger: public Logger_interface{
    protected:
        /// @brief 日志底层实现封装
        /// @param content 日志内容
        /// @param level 日志等级
        /// @param env jnienv
        void log0(const std::string &content, int level, JNIEnv *env) override;
    private:
        Logger()= default;
    public:
        static Logger logger;
    };
    Logger Logger::logger = Logger();

    [[deprecated("Use Logger::logger instead")]]
    Logger* const logger = &Logger::logger;

/// 带id(一般为bot账号)的logger
    class IdLogger : public Logger_interface {
    protected:
        void log0(const std::string &content, int level, JNIEnv *env) override ;
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
            Logger::logger.error(what());
        };
    };

/// 文件读取异常.
/// @see MiraiCPException
    class UploadException : public MiraiCPException {
    public:
        explicit UploadException(const std::string& text) {
            this->description = "上传(图片/文件)异常" + text;
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
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
        explicit IllegalStateException(const std::string& text) {
            this->description = "状态异常:" + text;
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
            Logger::logger.error(this->description);
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

/// 内部异常, 通常为json读写问题
/// @see MiraiCPException
    class APIException : public MiraiCPException {
    public:
        explicit APIException(const std::string& text) {
            this->description = "MiraiCP内部无法预料的错误:" + text;
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
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
        explicit BotException(std::string d = "没有权限执行该操作") {
            this->description = std::move(d);
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
        }

    private:
        std::string description;
    };

    /// 被禁言异常, 通常发生于发送信息
    class BotIsBeingMutedException: public BotException{
    public:
        /// 剩余禁言时间, 单位秒
        int timeRemain;
        explicit BotIsBeingMutedException(int t):BotException("发送信息失败, bot已被禁言, 剩余时间"+std::to_string(t)){
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
        MuteException() {
            this->description = "禁言时长不在0s~30d中间";
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
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
        explicit MemberException(int type) {
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
            return this->description;
        }

        void raise() override {
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
        FriendException() {
            this->description = "找不到好友";
        }

        //返回错误信息
        std::string what() override {
            return this->description;
        }

        void raise() override {
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
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
        std::string what() override {
            return this->description;
        }

        void raise() override {
            //ThreadManager::getEnv(__FILE__, __LINE__)->ThrowNew(Config::initexception, (this->description).c_str());
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

        std::string what() override {
            return this->description;
        }

    private:
        std::string description;
    };

/// 远程资源出现问题
/// @see MiraiCPException
    class RemoteAssetException : public MiraiCPException {
    public:
        explicit RemoteAssetException(std::string e) {
            this->description = std::move(e);
        }

        std::string what() override {
            return this->description;
        }

    private:
        std::string description;
    };

/// 参数错误
    class IllegalArgumentException : public MiraiCPException {
    private:
        std::string description;
    public:
        explicit IllegalArgumentException(std::string e) {
            this->description = std::move(e);
        }

        std::string what() override {
            return this->description;
        }

        void raise() override {
            Logger::logger.error(this->what());
        }
    };

    /// 超时
    class TimeOutException : public MiraiCPException {
    private:
        std::string description;
    public:
        explicit TimeOutException(std::string e) {
            this->description = std::move(e);
        }

        std::string what() override {
            return this->description;
        }

        void raise() override {
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
        if(re == "EB")
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
        MessageSource quoteAndSendMiraiCode(MiraiCodeable *msg, QQID groupid = 0, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) const {
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
        MessageSource quoteAndSendMsg(const std::string &c, QQID groupid = 0, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /**
         * @brief 回复(引用并发送)
         * @param c 引用后发送的内容, 为MiraiCode形式
         * @param groupid 如果为发送给群成员需把该群成员的groupid传入以帮助获取到该成员
         * @return MessageSource
         */
        MessageSource quoteAndSendMiraiCode(const std::string &c, QQID groupid = 0, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__)) const;

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
    class SingleMessage:public MiraiCodeable{
    public:
        static std::map<int, std::string> messageType;
        /// @brief 找对应类型的index key
        /// @param value 类型名
        /// @return 如果没找到返回-1
        static int getKey(const std::string& value){
            for(const auto& a : messageType){
                if(a.second == value) return a.first;
            }
            return -1;
        }
        /// MiraiCode类别
        /// @see SingleMessage::messageType
        int type;
        std::string content;
        std::string prefix;
        std::string toMiraiCode() override{
            if(type > 0)
                if(type == 1)
                    return "[mirai:at:" + content + "] ";
                else
                    return "[mirai:" + messageType[type] + this->prefix + Tools::escapeToMiraiCode(content) + "]";
            else
                return content;
        }

        /// @brief 构建单条
        /// @param type 消息类型 @see messageType
        /// @param content 内容
        /// @param prefix 前缀, 默认为`:`, 第二个冒号部分的内容, 目前在serviceMesage有使用
        SingleMessage(int type, std::string content, std::string prefix = ":") : type(type), content(std::move(content)), prefix(std::move(prefix)){}
    };
    std::map<int, std::string> SingleMessage::messageType = {
            {-1, "unSupportMessage"},
            {0, "plainText"},
            {1, "at"},
            {2, "image"},
            {3, "app"},
            {4, "service"}
    };

    /// 纯文本信息
    class PlainText: public SingleMessage{
    public:
        std::string content;
        std::string toMiraiCode() override{
            return content;
        }
        explicit PlainText(const SingleMessage& sg):SingleMessage(sg){
            if(sg.type != 0) throw IllegalArgumentException("Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to PlainText");
            this->content = sg.content;
        }
        template<typename T>
        explicit PlainText(const T& a):SingleMessage(0, ([&a]() ->std::string{
            std::stringstream sst;
            sst << a;
            return sst.str();
        })()){}
    };

    /// @
    class At: public SingleMessage{
    public:
        QQID target;
        explicit At(const SingleMessage& sg):SingleMessage(sg){
            if(sg.type != 1) throw IllegalArgumentException("Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to At");
            this->target = std::stol(sg.content);
        }
        explicit At(QQID a):SingleMessage(1, std::to_string(a)), target(a){};
        std::string toMiraiCode() override{
            return "[mirai:at:"+ std::to_string(this->target) +"] ";// 后面有个空格
        }
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
        explicit Image(std::string imageId);

        explicit Image(const SingleMessage& sg):SingleMessage(sg){
            if(sg.type != 2) assert(sg.type != 2);
            this->id = sg.content;
        }

        /*
        * 获取图片下载url
        */
        std::string queryURL(JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 取图片Mirai码
        std::string toMiraiCode() override;
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
        explicit LightApp(std::string content) : SingleMessage(3, std::move(content)) {}

        explicit LightApp(const SingleMessage& sg):SingleMessage(sg){
            if(sg.type != 3) throw IllegalArgumentException("Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to LighApp");
        }

        /// 使用样式1,适合文字展示，无大图，不能交互
        /// @param c 结构体，用于自定义里面的数据
        /// @see LightAppStyle1 in pch.h
        explicit LightApp(const LightAppStyle1 &c) : SingleMessage(3,"") {
            this->content =
                    "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":{\"appName\":\"" +
                    c.appName + "\",\"appType\":4,\"appid\":1109659848,\"iconUrl\":\"" + c.icon +
                    "\"},\"data\":[" + c.titles + "],\"title\":\"" + c.title + "\",\"button\":[" + c.buttons +
                    "],\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
        }

        /// 使用样式2，有大图，不能交互
        /// @param c 结构体，用于自定义里面的数据
        /// @see LightAppStyle1 in pch.h
        explicit LightApp(const LightAppStyle2 &c) : SingleMessage(3,"") {
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
        explicit LightApp(const LightAppStyle3 &c) : SingleMessage(3, "") {
            this->content ="{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
                    "\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
                    "\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" +
                    c.preview +
                    "\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"" +
                    c.title + "\",\"title\":\"" + c.description +
                    "\",\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" +
                    c.icon + "\",\"qqdocurl\":\"" + c.url +
                    "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
        }

        explicit LightApp(const LightAppStyle4 &c) : SingleMessage(3,
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
        std::string toMiraiCode() override {
            return "[mirai:app:" + Tools::escapeToMiraiCode(content) + "]";
        }
    };

    /// xml格式的超文本信息
    /// @attention 自带的模板不稳定，可能发出现没有效果
    class ServiceMessage : public SingleMessage {
    public:
        std::string toMiraiCode() override {
            return "[mirai:service:" + this->prefix + Tools::escapeToMiraiCode(content) + "]";
        }

        /// @brief ServiceMessage
        /// @param id 在xml内容前面的id (不包括逗号)
        /// @param a xml内容 (不需要事先转码到miraiCode)
        explicit ServiceMessage(int id, std::string a) : SingleMessage(4, std::move(a), ":" +  std::to_string(id) + ',') {}

        explicit ServiceMessage(const SingleMessage& sg):SingleMessage(sg){
            if(sg.type != 4)throw IllegalArgumentException("Cannot convert(" + MiraiCP::SingleMessage::messageType[sg.type] + ") to ServiceMessage");
        }

        explicit ServiceMessage(const URLSharer &a) : SingleMessage(4,
                                                                    "<?xml version=\"1.0\" encoding=\"utf-8\"?><msg templateID=\"12345\" action=\"web\" brief=\"" +
                                                                    a.brief + "\" serviceID=\"1\" url=\"" + a.url +
                                                                    "\"><item layout=\"2\"><picture cover=\"" +
                                                                    a.cover + "\"/><title>" + a.title +
                                                                    "</title><summary>" + a.summary +
                                                                    "</summary></item><source/></msg>",
                                                                    ":1,") {}
    };

    class UnSupportMessage:public SingleMessage{
    public:
        std::string content;
        std::string toMiraiCode() override{
            return content;
        }
        explicit UnSupportMessage(const SingleMessage& s):SingleMessage(s){};
        explicit UnSupportMessage(const std::string& content):SingleMessage(-1, content){}
    };

    /// 消息链, 一般由SingleMessage组成
    class MessageChain:public MiraiCodeable{
    private:
        void p(std::vector<std::shared_ptr<SingleMessage>>* v){}
        template<class T1, class... T2>
        void p(std::vector<std::shared_ptr<SingleMessage>>* v, T1 h, T2 ... args){
            static_assert(std::is_base_of_v<SingleMessage, T1>, "只支持SingleMessage子类");
            v->push_back(std::make_shared<SingleMessage>(h));
            Logger::logger.info(((SingleMessage)h).toMiraiCode());
            p(v,args...);
        }
        std::vector<std::shared_ptr<SingleMessage>> content;

    public:
        size_t size(){
            return this->content.size();
        }

        const std::vector<std::shared_ptr<SingleMessage>>& vector(){
            return this->content;
        }

        /// 如果由MiraiCP构造(incoming)就会存在，否则则不存在
        std::optional<MessageSource> source = std::nullopt;
        /// @brief 找到miraiCode结尾的`]`
        /// @param s 文本
        /// @param start 开始位置
        /// @return 如果不存在返回-1, 存在则返回index
        static size_t findEnd(const std::string& s, size_t start){
            size_t pos = start;
            while(pos < s.length()) {
                switch(s[pos]){
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
        std::string toMiraiCode() override{
            return Tools::VectorToString(this->toMiraiCodeVector(), "");
        }
        std::vector<std::string> toMiraiCodeVector(){
            std::vector <std::string> tmp;
            for(const auto& a : this->content){
                tmp.emplace_back(a->toMiraiCode());
            }
            return tmp;
        }
        /// @brief 添加元素
        /// @tparam T 任意的SingleMessage的子类
        /// @param a 添加的值
        template<class T>
        void add(const T& a){
            static_assert(std::is_base_of_v<SingleMessage, T>, "只接受SingleMessage的子类");
            this->content.push_back(std::make_shared<SingleMessage>(a));
        }
        void add(const MessageSource& val){
            this->source = val;
        }
        /// 筛选出某种特点type的信息
        template<class T>
        std::vector<T> filter(int type){
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for(const auto& a:this->content){
                if(a->type == type)
                    re.push_back(std::static_pointer_cast<T>(a));
            }
            return re;
        }
        /// 自定义筛选器
        template<class T>
        std::vector<T> filter(std::function<bool(std::shared_ptr<SingleMessage>)> func){
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            std::vector<T> re;
            for(const auto& a:this->content){
                if(func(a))
                    re.push_back(std::static_pointer_cast<T>(a));
            }
            return re;
        }
        /// 找出第一个指定的type的信息
        template<class T>
        T first(int type){
            for(const auto& a:this->content)
                if(a->type == type)
                    return std::static_pointer_cast<T>(a);
        }
        /// incoming构造器
        template<class... T>
        explicit MessageChain(MessageSource ms, T ... args): source(std::move(ms)){
            this->p(&this->content, args...);
        };
        explicit MessageChain(const std::vector<std::shared_ptr<SingleMessage>>& v){
            this->content = v;
        }
        /// outcoming构造器
        template<class... T>
        explicit MessageChain(T ... args){
            p(&this->content, args...);
        };
        /// outcoming 构造器
        template<class T>
        explicit MessageChain(const T& msg){
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage子类");
            this->content.push_back(std::make_shared<SingleMessage>(msg));
        };
        template<class T>
        [[nodiscard]] MessageChain plus(const T& a){
            static_assert(std::is_base_of_v<SingleMessage, T>, "只支持SingleMessage的子类");
            MessageChain tmp(*this);
            tmp.content.push_back(std::make_shared<SingleMessage>(a));
            return tmp;
        }
        MessageChain plus(const MessageSource& ms){
            MessageChain tmp(*this);
            tmp.source = ms;
            return tmp;
        }
        template<class T>
        MessageChain operator+ (const T& msg){
            return this->plus(msg);
        }

        // TODO
        std::variant<PlainText, At, Image, LightApp, ServiceMessage, UnSupportMessage> operator[](size_t i){
            auto tmp = this->content[i];
            switch(tmp->type){
                case -1:
                    return (UnSupportMessage)(*tmp);
                case 0:
                    return (PlainText)(*tmp);
                case 1:
                    return (At)(*tmp);
                case 2:
                    return (Image)(*tmp);
                case 3:
                    return (LightApp)(*tmp);
                case 4:
                    return (ServiceMessage)(*tmp);
                default:
                    throw APIException("位置MessageChain::get");
            }
        }

        /// 从string构建MessageChain, 常用于Incoming message
        static MessageChain deserializationFromString(const std::string& m){
            size_t pos = 0;
            size_t lastPos = -1;
            MessageChain mc;
            if(m.length() <= 7){
                return MessageChain(PlainText(m));
            }
            do{
                if(m.length() - 7 - pos > 0 && m.substr(pos, 7) == "[mirai:"){
                    if(pos - lastPos > 1)
                        mc.add(PlainText(m.substr(lastPos + 1, pos - lastPos - 1)));// plain text
                    size_t back = MessageChain::findEnd(m, pos);
                    if(back == -1) throw IllegalStateException("");
                    std::string tmp = m.substr(pos, back - pos);
                    tmp = Tools::replace(tmp, "[mirai:", "");
                    size_t i = tmp.find(':');// first :
                    int t = SingleMessage::getKey(tmp.substr(0, i));
                    switch(t) {
                        case 0:
                            // no miraiCode key is PlainText
                            break;
                        case 1:
                            mc.add(At(std::stoll(tmp.substr(i + 1, tmp.length() - i - 1))));
                            break;
                        case 2:
                            mc.add(Image(tmp.substr(i + 1, tmp.length() - i - 1)));
                            break;
                        case 3:
                            mc.add(LightApp(tmp.substr(i + 1, tmp.length() - i - 1)));
                            break;
                        case 4: {
                            size_t comma = tmp.find(',');
                            mc.add(ServiceMessage(std::stoi(tmp.substr(i + 1, comma - i - 1)),
                                                  tmp.substr(comma + 1, tmp.length() - comma - 1)));
                            break;
                        }
                        default:
                            mc.add(UnSupportMessage("[mirai:" + tmp));
                            break;
                    }
                    pos = back;
                    lastPos = pos;
                    if(t == 1)
                        lastPos++;
                }
                pos++;
            }while(pos < m.length());
            if(lastPos + 1 < m.length())
                mc.add(PlainText(m.substr(lastPos + 1, m.length() - lastPos - 1)));// plain text
            return mc;
        }
    };

    /*!
 * @brief group, friend, member的父类
 */
    class Contact {
    private:
        /// 发送纯文本信息
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        MessageSource sendMsg0(const std::string& msg, int retryTime, bool miraicode = false, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

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
        explicit Contact(int type, QQID id, QQID gid, const std::string& name, QQID botid, bool anonymous = false) {
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
        [[deprecated("Use sendMessage")]]
        MessageSource sendMiraiCode(MiraiCode msg, int retryTime = 3, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }
        /// @brief 发送一条MessageChain
        /// @param msg MessageChain
        /// @param retryTime 重试次数
        /// @return MessageSource
        MessageSource sendMessage(MessageChain msg, int retryTime = 3, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }
        /// @brief 发送一条MiraiCode信息
        /// @param msg MiraiCode
        /// @param retryTime 重试次数
        /// @return MessageSource
        MessageSource sendMessage(MiraiCode msg, int retryTime = 3, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }
        /// @brief 发送一条文本信息(如果发送纯文本形式的MiraiCode)要传入`miraiCode` = true
        /// @param msg 文本
        /// @param miraiCode 是否是MiraiCode
        /// @param retryTime 重试次数
        /// @return MessageSource
        MessageSource sendMessage(const std::string& msg, bool miraiCode = false, int retryTime = 3, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
            return sendMsg0(msg, retryTime, miraiCode, env);
        }
        /// @brief 发送一条singleMessage
        /// @param msg SingleMessage
        /// @param retryTime 重试次数
        /// @return MessageSource
        MessageSource sendMessage(SingleMessage msg, int retryTime = 3, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
            return sendMsg0(msg.toMiraiCode(), retryTime, true, env);
        }

        /// @brief 发送纯文本信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        [[deprecated("Use sendMessage")]]
        MessageSource sendMsg(const std::string& msg, int retryTime = 3, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            return sendMsg0(msg, retryTime, false, env);
        }

        /// @brief 以纯文本发送MiraiCode信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        [[deprecated("Use sendMessage")]]
        MessageSource sendMsg(MiraiCode msg, int retryTime = 3, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
            return sendMsg0(msg.toMiraiCode(), retryTime, false, env);
        }

        /// @brief 发送纯文本信息
        /// @param msg 发送的信息
        /// @param retryTime 当服务器无应答(通常由于发送消息频率太快导致)时的重试次数，每次重试相隔1s，-1为无限制，如果在重试次数用完后还是没能成功发送就会抛出TimeOutException
        /// @return MessageSource
        /// @throw IllegalArgumentException, TimeOutException, BotIsBeingMutedException
        [[deprecated("Use sendMessage")]]
        MessageSource sendMsg(std::vector<std::string> msg, int retryTime = 3, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
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
        virtual void refreshInfo(JNIEnv *) {};
    };

// 群文件

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

/// @brief 远程(群)文件类型
    class RemoteFile : public MiraiCodeable {
    public:
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
        explicit RemoteFile(const std::string &i, unsigned int ii, std::string n, long long s, const std::string &p,
                   struct Dinfo d, struct Finfo f) : id(i),
                                                     internalid(ii),
                                                     name(std::move(n)),
                                                     size(s),
                                                     path(p),
                                                     dinfo(std::move(d)),
                                                     finfo(f) {};

        /// 仅在上传后构建的有效, 即获取到internalid时(internalid != 0) 否则重新上传并重新获取internalid再转换
        std::string toMiraiCode() override {
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
        static std::string send0(const std::string& content, Contact *c, int retryTime, bool miraicode, JNIEnv *env, std::string errorInfo = "") {
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
        static inline std::string uploadImg0(const std::string& path, Contact *c, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
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
        static info info0(const std::string& source) {
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
        ForwardNode(Contact *c, const std::string &message, int t) : id(c->id()), name(c->nickOrNameCard()), message(message),
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

        void refreshInfo(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) override{
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
        void sendNudge(){
            json j;
            j["contactSource"] = this->serializationToString();
            std::string re = Config::koperation(Config::SendNudge, j);
            if(re == "E1")
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
        void modifyAdmin(bool admin, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__));

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

        explicit Member(Contact c) : Contact(std::move(c)) { this->isAnonymous = this->_anonymous; refreshInfo(); };

        /// 是否是匿名群成员, 如果是匿名群成员一些功能会受限
        bool isAnonymous = false;

        /// 重新获取(刷新)群成员信息
        void refreshInfo(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) override {
            if(isAnonymous)
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
        MessageSource sendVoice(const std::string &path, JNIEnv * env= ThreadManager::getEnv(__FILE__, __LINE__)){
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
        void unMute(JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
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
        void sendNudge(){
            if(isAnonymous) return;
            json j;
            j["contactSource"] = this->serializationToString();
            std::string re = Config::koperation(Config::SendNudge, j);
            if(re == "E1")
                throw IllegalStateException("发送戳一戳失败，登录协议不为phone");
        }
    };

/// 群聊类声明
    class Group : public Contact {
    public:
        /// 群公告参数
        class AnnouncementParams{
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
            explicit AnnouncementParams(bool send2New = false, bool requireConfirm = false, bool pinned = false, bool showEditCard = false, bool showPopup = false) :
            send2new(send2New), requireConfirm(requireConfirm),
                                                               pinned(pinned), showEditCard(showEditCard),
                                                               showPopup(showPopup){}
        };
        /// 在线群公告
        class OnlineAnnouncement{
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
                               QQID groupid, QQID senderid,QQID botid,
                               long long int publicationTime, const std::string &fid, int confirmNum,
                               const std::string &imageid) : content(content), params(params), groupid(groupid),
                                                             senderid(senderid), botid(botid), publicationTime(publicationTime),
                                                             fid(fid), confirmNum(confirmNum), imageid(imageid) {}
        };
        /// 本地(未发送)群公告
        class OfflineAnnouncement{
        public:
            /// 内容
            std::string content;
            /// 公告属性
            AnnouncementParams params;
            /// 发布群公告
            Group::OnlineAnnouncement publishTo(const Group&);

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
        Member getMember(QQID memberid, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
            return Member(memberid, this->id(), this->groupid(), env);
        }

        /// 取群公告列表
        std::vector<OnlineAnnouncement> getAnnouncementsList(JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)){
            json j;
            j["source"] = this->serializationToString();
            j["announcement"] = true;
            std::string re = Config::koperation(Config::RefreshInfo, j, env);
            std::vector<OnlineAnnouncement> oa;
            for(const json& e : json::parse(re)){
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
        RemoteFile sendFile(const std::string &path, const std::string &filepath, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

        /// 发送语音
        MessageSource sendVoice(const std::string &path, JNIEnv * env= ThreadManager::getEnv(__FILE__, __LINE__)){
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
         //根据qq群远程路径(带文件名)找, 不过由于qq群文件允许同名文件这一特性, 返回的文件为群文件中同名文件中随机一个
         Event::processor.registerEvent([](GroupMessageEvent e) {
            e.group.SendMsg(e.group.getFile("/", id).name());
            e.group.SendMsg(e.group.getFile("/test.txt").name());
            e.group.SendMsg(e.group.getFileListString("/"));
            });
         @endcode
        */
        RemoteFile getFile(const std::string &path, const std::string &id = "", JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

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
        std::vector<file_short_info> getFileList(const std::string &path, JNIEnv * = ThreadManager::getEnv(__FILE__, __LINE__));

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
        Friend getFriend(QQID i, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)) const{
            return Friend(i, this->id, env);
        }

        /// 取群聊
        Group getGroup(QQID groupid, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)) const{
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
    };

/// 所以事件处理timeoutevent都是机器人事件，指都有机器人实例
    class BotEvent {
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
                          sender(sender), message(mc) {};
        /*!
         * @brief 取群聊下一个消息(群聊与本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain nextMessage(long time = -1, bool halt = true, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__));

        /*!
         * @brief 取群聊中同群成员的下一个消息(发送人和群与本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain senderNextMessage(long time = -1, bool halt = true, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__));

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
        PrivateMessageEvent(QQID botid, Friend sender, MessageChain mc) : BotEvent(botid), sender(std::move(sender)), message(mc) {};
        /*!
         * @brief 取下一个消息(发送人和接收人和本事件一样)
         * @warning 如果两次发送信息间隔过短可能会漏过信息
         * @param time 超时时间限制
         * @param halt 是否拦截该事件(不被注册的监听器收到处理)
         * @return MiraiCP::Message
         */
        MessageChain nextMessage(long time = -1, bool halt = true, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__));
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

        static void operation0(const std::string& source, QQID botid, bool accept, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            nlohmann::json j;
            j["text"] = source;
            j["accept"] = accept;
            j["botid"] = botid;
            std::string re = Config::koperation(Config::Gioperation, j, env);
            if (re == "E")Logger::logger.error("群聊邀请事件同意失败(可能因为重复处理),id:" + source);
        }

        void reject(JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            GroupInviteEvent::operation0(this->source, this->bot.id,false, env);
        }

        void accept(JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)) {
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
        static void operation0(const std::string& source, QQID botid, bool accept, JNIEnv *env = ThreadManager::getEnv(__FILE__, __LINE__), bool ban=false) {
            nlohmann::json j;
            j["text"] = source;
            j["accept"] = accept;
            j["botid"] = botid;
            j["ban"] = ban;
            std::string re = Config::koperation(Config::Nfroperation, j, env);
            if (re == "E")Logger::logger.error("好友申请事件同意失败(可能因为重复处理),id:" + source);
        }

        /// @brief 拒绝好友申请
        void reject(bool ban = false, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)) {
            NewFriendRequestEvent::operation0(this->source, this->bot.id, false, env);
        }

        /// @brief 接受申请
        /// @param ban - 是否加入黑名单
        void accept(JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)) {
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
    class TimeOutEvent{
    public:
        /// 事件所附信息
        std::string msg;

        explicit TimeOutEvent(std::string msg) : msg(std::move(msg)) {}
    };

    /// @brief 开启一个新的定时任务
    /// @param time 延迟多久，毫秒为单位
    /// @param msg string类型附带信息，推荐使用json格式方便解析
    void schedule(long time, const std::string& msg, JNIEnv* env = ThreadManager::getEnv(__FILE__, __LINE__)){
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
    class NudgeEvent: public BotEvent{
    public:
        /// 谁发送的
        Contact from;
        NudgeEvent(const Contact& c, QQID botid):BotEvent(botid){}
    };

/**监听类声明*/
    class Event {
    private:
        Event() = default;
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
        Node<TimeOutEvent> * TOHead = new Node<TimeOutEvent>();
        Node<NudgeEvent>* NHead = new Node<NudgeEvent>();

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
            } else if constexpr(std::is_same_v<T, TimeOutEvent>){
                return TOHead;
            }else if constexpr(std::is_same_v<T, TimeOutEvent>){
                return NHead;
            }
            Logger::logger.error("内部错误, 位置:C-Head");
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
        Node<TimeOutEvent> * TOTail = TOHead;
        Node<NudgeEvent> * NTail = NHead;

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
            } else if constexpr(std::is_same_v<T, TimeOutEvent>) {
                TOTail->next = temp;
                TOTail->nextNode = temp;
                TOTail = temp;
            } else if constexpr(std::is_same_v<T, NudgeEvent>){
                NTail->next = temp;
                NTail->nextNode = temp;
                NTail = temp;
            }else {
                Logger::logger.error("内部错误, 位置:C-Tail");
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

        /*!
        * 广播函数重载
        */
        template<typename T>
        void broadcast(T e) {
            /// 清空stack中内容, 不然可能保留上一次Event的操作
            ThreadManager::getThread()->stack.clear();
            ThreadManager::getThread()->stack.push(__FILE__, __LINE__, __func__, typeid(T).name());
            Node<T> *now = Event::head<T>();
            while (now) {
                if (now->enable) { now->f(e); }
                now = now->next;
            }
        }

        /*!
        * @brief 监听函数
        * @note 在极其少见(MiraiCP内部出问题的时候)会抛出异常
        */
        template<typename T>
        NodeHandle registerEvent(std::function<void(T)> f) {
            bool *e = Event::tail(f);
            if (e != nullptr)
                return NodeHandle(e);
            throw APIException("位置:registerEvent");
        }

        ~Event();

        static Event processor;
    };

    Event Event::processor = Event();

    [[deprecated("Use Event::processor instead")]]
    Event * const procession = &Event::processor;

/*线程管理*/

// 结束MiraiCP类声明代码
// 开始MiraiCP实现代码

    void ThreadManager::setEnv(JNIEnv *e) {
        mtx.lock();
        if (!ThreadManager::included(ThreadManager::getThreadId())) {
            ThreadInfo tmp{
                    e,
                    false
            };
            ThreadManager::threads.insert(std::pair<std::string, ThreadInfo>(ThreadManager::getThreadId(), tmp));
        } else {
            ThreadManager::threads[ThreadManager::getThreadId()].e = e;
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
        ThreadInfo tmp{env, true};
        ThreadManager::threads.insert(std::pair<std::string, ThreadInfo>(ThreadManager::getThreadId(), tmp));
        Logger::logger.info("refresh env");
    };

    void ThreadManager::detach() {
        mtx.lock();
        if (ThreadManager::included(ThreadManager::getThreadId())) {
            bool att = ThreadManager::threads[ThreadManager::getThreadId()].attach;
            ThreadManager::threads.erase(ThreadManager::getThreadId());
            if (att)
                gvm->DetachCurrentThread();
        }
        mtx.unlock();
    }

    bool ThreadManager::included(const std::string &id) {
        if (ThreadManager::threads.empty() || ThreadManager::threads.count(id) == 0)
            return false;
        return true;
    }

    JNIEnv *ThreadManager::getEnv(const std::string& file, int loc, const std::string& func) {
        mtx.lock();
        if (!ThreadManager::included(getThreadId())) {
            ThreadManager::newEnv();
        }
        JNIEnv *tmp = ThreadManager::threads[ThreadManager::getThreadId()].e;
        ThreadManager::threads[ThreadManager::getThreadId()].stack.push(file, loc, func);
        mtx.unlock();
        return tmp;
    }

/*
日志类实现
throw: InitException 即找不到签名
*/

    void Logger_interface::init(JNIEnv *env) {
        this->log = env->GetStaticMethodID(Config::CPP_lib, "KSendLog", "(Ljava/lang/String;I)V");
    }

    void Logger_interface::registerHandle(Logger_interface::Action action) {
        this->loggerhandler.action = std::move(action);
    }

    void Logger_interface::setHandleState(bool state) {
        this->loggerhandler.enable = state;
    }

    void Logger_interface::warning(const std::string &content, JNIEnv *env) {
        this->log0(content, 1, env);
    }

    void Logger_interface::error(const std::string &content, bool printStack, JNIEnv *env) {
        ThreadManager::StackTracer a = ThreadManager::getThread()->stack;
        if(printStack)
            this->log0(content + "\n" + a.print(), 2, env);
        else
            this->log0(content, 2, env);
    }

    void Logger_interface::info(const std::string &content, JNIEnv *env) {
        this->log0(content, 0, env);
    }

    void Logger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = -2;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

    void IdLogger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = id;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }
    void PluginLogger::log0(const std::string &content, int level, JNIEnv *env) {
        if (this->loggerhandler.enable)
            this->loggerhandler.action(content, level);
        json j;
        j["id"] = -1;
        j["name"] = plugin->config.id;
        j["log"] = content;
        env->CallStaticVoidMethod(Config::CPP_lib, log, Tools::str2jstring(j.dump().c_str()), (jint) level);
    }

/*
配置类实现
throw: InitxException 即找不到对应签名
*/

    void Config::construct(JNIEnv *env) {
        Config::CPP_lib = reinterpret_cast<jclass>(env->NewGlobalRef(
                env->FindClass("tech/eritquearcus/miraicp/shared/CPP_lib")));
        if (Config::CPP_lib == nullptr) {
            throw APIException("初始化错误，找不到CPP_lib类");
        }
        Config::KOperation = env->GetStaticMethodID(CPP_lib, "KOperation", "(Ljava/lang/String;)Ljava/lang/String;");
    }

    void Config::destruct() {
        ThreadManager::getEnv(__FILE__, __LINE__)->DeleteGlobalRef(Config::CPP_lib);
    }

    std::string Config::koperation(operation_set type, json &data, JNIEnv *env, bool catchErr, const std::string& errorInfo) {
        json j;
        j["type"] = type;
        j["data"] = data;
        std::string re = Tools::jstring2str((jstring) env->CallStaticObjectMethod(Config::CPP_lib, Config::KOperation,
                                                           Tools::str2jstring(j.dump().c_str(), env)), env);
        if(catchErr) ErrorHandle(re, errorInfo);
        return re;
    }

    Event::~Event() {
        Node0 *temp[] = {GMHead, PMHead, GHead, NFHead, MJHead, MLHead, RHead, BHead, TOHead};
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

    void MessageSource::recall(JNIEnv *env) const {
        json j;
        j["source"] = this->serializeToString();
        std::string re = Config::koperation(Config::Recall, j, env);
        if (re == "Y") return;
        if (re == "E1") throw BotException();
        if (re == "E2") throw RecallException();
    }

    MessageSource::MessageSource(std::string ids, std::string internalids, std::string source) : ids(std::move(
            ids)), internalids(std::move(internalids)), source(std::move(source)) {}

    std::string MessageSource::serializeToString() const {
        return source;
    }

    MessageSource MessageSource::deserializeFromString(const std::string &source) {
        json j = json::parse(source);
        try {
            return {j["ids"].dump(), j["internalIds"].dump(), source};
        }
        catch (json::type_error &e) {
            Logger::logger.error("消息源序列化出错，格式不符合(MessageSource::deserializeFromString)");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
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
            Logger::logger.error("格式化json失败，RemoteFile::deserializeFromString");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
            throw e;
        }
        try {
            struct Dinfo d{
                    j["dinfo"]["url"],
                    j["dinfo"]["md5"],
                    j["dinfo"]["sha1"]
            };
            struct Finfo f{
                    j["finfo"]["size"],
                    j["finfo"]["uploaderid"],
                    j["finfo"]["downloadtime"],
                    j["finfo"]["uploadtime"],
                    j["finfo"]["lastmodifytime"]
            };
            return RemoteFile(j["id"], j["internalid"], j["name"], j["finfo"]["size"], j["path"], d, f);
        }
        catch (json::type_error &e) {
            Logger::logger.error("json格式化失败，位置:RemoteFile");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
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
    MessageSource ForwardMessage::sendTo(Contact *c, JNIEnv *env) {
        json temp;
        json text;
        text["id"] = c->id();
        text["groupid"] = c->groupid();
        text["type"] = c->type();
        text["content"] = sendmsg;
        temp["text"] = text.dump();
        temp["botid"] = c->botid();
        std::string re = Config::koperation(Config::Buildforward, temp, env);
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
    Image::Image(std::string imageId) : SingleMessage(2, std::move(imageId)) {
        this->id = std::move(imageId);
    }

    std::string Image::queryURL(JNIEnv *env) {
        json j;
        j["id"] = this->id;
        std::string re = Config::koperation(Config::QueryImgUrl, j, env);
        if (re == "E1")
            throw RemoteAssetException("图片id格式错误");
        return re;
    }

    std::string Image::toMiraiCode() {
        return "[mirai:image:" + Tools::escapeToMiraiCode(this->id) + "]";
    }

    MessageSource MessageSource::quoteAndSendMiraiCode(const std::string &content, QQID groupid, JNIEnv *env) const {
        json obj;
        json sign;
        obj["messageSource"] = this->serializeToString();
        obj["msg"] = content;
        sign["MiraiCode"] = true;
        sign["groupid"] = groupid;
        obj["sign"] = sign.dump();
        std::string re = Config::koperation(Config::SendWithQuote, obj, env);
        return MessageSource::deserializeFromString(re);
    }

    MessageSource MessageSource::quoteAndSendMsg(const std::string &content, QQID groupid, JNIEnv *env) {
        json obj;
        json sign;
        obj["messageSource"] = this->serializeToString();
        obj["msg"] = content;
        sign["MiraiCode"] = false;
        sign["groupid"] = groupid;
        obj["sign"] = sign.dump();
        std::string re = Config::koperation(Config::SendWithQuote, obj, env);
        return MessageSource::deserializeFromString(re);
    }

    MessageSource Contact::sendMsg0(const std::string& msg, int retryTime, bool miraicode, JNIEnv *env) {
        if (msg.empty()) {
            Logger::logger.warning("警告:发送空信息, 位置: Contact::SendMsg");
            throw IllegalArgumentException("参数不能为空, 位置: Contact::SendMsg");
        }
        std::string re = LowLevelAPI::send0(msg, this, retryTime, miraicode, env, "reach a error area, Contact::SendMiraiCode");
        if(re == "ET")
            throw TimeOutException("发送消息过于频繁导致的tx服务器未能即使响应, 位置: Contact::SendMsg");
        if(Tools::starts_with(re, "EBM"))
            throw BotIsBeingMutedException(std::stoi(re.substr(3)));
        return MessageSource::deserializeFromString(re);
    }

    Image Contact::uploadImg(const std::string &path, JNIEnv *env) {
        std::string re = LowLevelAPI::uploadImg0(path, this, env);
        if (re == "E2")
            throw UploadException("上传图片大小超过30MB,路径:" + path);
        return Image(re);
    }

/*好友类实现*/
    Friend::Friend(QQID id, QQID botid, JNIEnv *env) : Contact() {
        this->_type = 1;
        this->_id = id;
        this->_botid = botid;
        refreshInfo(env);
    }

/*成员类实现*/
    Member::Member(QQID id, QQID groupid, QQID botid, JNIEnv *env)
            : Contact() {
        this->_type = 3;
        this->_id = id;
        this->_groupid = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }

    unsigned int Member::getPermission(JNIEnv *env) {
        if(isAnonymous) return 0;
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::QueryM, j, env);
        return stoi(re);
    }

    void Member::mute(int time, JNIEnv *env) {
        json j;
        j["time"] = time;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::MuteM, j, env);
        if (re == "E3") {
            throw BotException();
        }
        if (re == "E4") {
            throw MuteException();
        }
    }

    void Member::kick(const std::string &reason, JNIEnv *env) {
        json j;
        j["message"] = reason;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::KickM, j, env);
        if (re == "E3") {
            throw BotException();
        }
    }

    void Member::modifyAdmin(bool admin, JNIEnv* env){
        if(isAnonymous)return;
        json j;
        j["admin"] = admin;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::ModifyAdmin, j, env);
        if(re == "E1"){
            throw BotException();
        }
    }

/*群聊类实现*/
    Group::Group(QQID groupid, QQID botid, JNIEnv *env) : Contact() {
        this->_type = 2;
        this->_id = groupid;
        this->_botid = botid;
        refreshInfo(env);
    }

    void Group::OnlineAnnouncement::deleteThis(){
        json j, i;
        i["botid"] = this->botid;
        i["groupid"] = this->groupid;
        i["fid"] = this->fid;
        i["type"] = 1;
        j["identify"] = i.dump();
        std::string re = Config::koperation(Config::Announcement, j);
        if(re == "E1")
            throw IllegalArgumentException("无法根据fid找到群公告(群公告不存在)");
        if(re == "E2")
            throw BotException();
        if(re == "E3")
            throw IllegalStateException("群公告状态异常");
    }

    json Group::AnnouncementParams::serializeToJson() {
        json j;
        j["sendToNewMember"] = this->send2new;
        j["isPinned"] = this->pinned;
        j["showEditCard"] = this->showEditCard;
        j["showPopup"] = this->showPopup;
        j["requireConfirmation"] = this->requireConfirm;
        return j;
    }

    Group::OnlineAnnouncement Group::OfflineAnnouncement::publishTo(const Group& g) {
        json j, i, s;
        i["botid"] = g.botid();
        i["groupid"] = g.id();
        i["type"] = 2;
        j["identify"] = i.dump();
        s["content"] = this->content;
        s["params"] = this->params.serializeToJson();
        j["source"] = s.dump();
        std::string re = Config::koperation(Config::Announcement, j);
        if(re == "E1")
            throw BotException();
        return Group::OnlineAnnouncement::deserializeFromJson(json::parse(re));
    }

    Group::OnlineAnnouncement Group::OnlineAnnouncement::deserializeFromJson(json j) {
        Group::AnnouncementParams ap(
                j["params"]["sendToNewMember"],
                j["params"]["requireConfirmation"],
                j["params"]["isPinned"],
                j["params"]["showEditCard"],
                j["params"]["showPopup"]
        );
        return Group::OnlineAnnouncement(
            j["content"],
            ap,
            j["groupid"],
            j["senderid"],
            j["botid"],
            j["time"],
            j["fid"],
            j["confirmationNum"],
            j["imageid"]
        );
    }

    void Group::updateSetting(JNIEnv *env) {
        json j;
        json tmp;
        j["name"] = this->setting.name;
        j["isMuteAll"] = this->setting.isMuteAll;
        j["isAllowMemberInvite"] = this->setting.isAllowMemberInvite;
        j["isAutoApproveEnabled"] = this->setting.isAutoApproveEnabled;
        j["isAnonymousChatEnabled"] = this->setting.isAnonymousChatEnabled;
        tmp["source"] = j.dump();
        tmp["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::GroupSetting, tmp, env);
        if (re == "E1")
            throw BotException();
        refreshInfo(env);
    }

    RemoteFile Group::sendFile(const std::string &path, const std::string &filepath, JNIEnv *env) {
        json tmp;
        json source;
        source["path"] = path;
        source["filepath"] = filepath;
        tmp["source"] = source.dump();
        tmp["contactSource"] = this->serializationToString();
        std::string callback = Config::koperation(Config::SendFile, tmp, env);
        if (callback == "E2") throw UploadException("找不到" + filepath + "位置:C-uploadfile");
        if (callback == "E3") throw UploadException("Upload error:路径格式异常，应为'/xxx.xxx'或'/xx/xxx.xxx'目前只支持群文件和单层路径, path:" + path);
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
        std::string re = Config::koperation(Config::RemoteFileInfo, j, env);
        if (re == "E2") throw RemoteAssetException("Get error: 文件路径不存在, path:" + path);
        return RemoteFile::deserializeFromString(re);
    }

    Member Group::getOwner(JNIEnv *env) {
        json j;
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::QueryOwner, j, env);
        return Member(stoi(re), this->id(), this->botid());
    }

    std::string Group::getFileListString(const std::string &path, JNIEnv *env) {
        json temp;
        json j;
        temp["id"] = "-1";
        temp["path"] = path;
        j["source"] = temp.dump();
        j["contactSource"] = this->serializationToString();
        return Config::koperation(Config::RemoteFileInfo, j, env);
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

    MessageChain PrivateMessageEvent::nextMessage(long time, bool halt, JNIEnv* env) {
        json j;
        j["contactSource"] = this->sender.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if(r == "-1")
            throw TimeOutException("取下一条信息超时");
        json re = json::parse(r);
        return MessageChain::deserializationFromString(re["message"]).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::nextMessage(long time, bool halt, JNIEnv *env) {
        json j;
        j["contactSource"] = this->group.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if(r == "-1")
            throw TimeOutException("取下一条信息超时");
        json re = json::parse(r);
        return MessageChain::deserializationFromString(re["message"]).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

    MessageChain GroupMessageEvent::senderNextMessage(long time, bool halt, JNIEnv *env) {
        json j;
        j["contactSource"] = this->sender.serializationToString();
        j["time"] = time;
        j["halt"] = halt;
        std::string r = Config::koperation(Config::NextMsg, j, env);
        if(r == "-1")
            throw TimeOutException("取下一条信息超时");
        json re = json::parse(r);
        return MessageChain::deserializationFromString(re["message"]).plus(MessageSource::deserializeFromString(re["messageSource"]));
    }

/*工具类实现*/
    std::string Tools::jstring2str(jstring jStr, JNIEnv *env) {
        if (!jStr) {
            Logger::logger.error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
            return "";
        }
        std::u16string s = reinterpret_cast<const char16_t *>(env->GetStringChars(jStr, nullptr));
        if (s.length() == 0) {
            Logger::logger.error("警告:kotlin部分返回空字符串, 位置:Tools::jstring2str");
            return "";
        }
        std::string x;
        utf8::utf16to8(s.begin(), s.end(), std::back_inserter(x));
        return x;
    }

    jstring Tools::str2jstring(const char *stra, JNIEnv *env) {
        if (!stra) {
            Logger::logger.error("警告:C++部分传入空字符串，位置:Tools::str2jstring");
        }
        std::string str(stra);
        std::vector<unsigned short> utf16line;
        utf8::utf8to16(str.begin(), str.end(), std::back_inserter(utf16line));
        auto *c = new jchar[utf16line.size()];
        for (int i = 0; i < utf16line.size(); i++) {
            c[i] = utf16line[i];
        }
        return env->NewString((jchar *) c, (jsize) utf16line.size());
    }

    template<typename T>
    std::string Tools::VectorToString(std::vector<T> a, const std::string& separator) {
        std::stringstream ss;
        for (size_t i = 0; i < a.size(); ++i) {
            if (i != 0)
                ss << separator;
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
        result.reserve(v.size());
        for (auto &&s : v)
            result.push_back(std::stoull(s));
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
    }

    Contact Contact::deserializationFromString(const std::string &source) {
        json j;
        try {
            j = json::parse(source);
        }
        catch (json::parse_error &e) {
            Logger::logger.error("json序列化错误 Contact::deserializationFromString");
            Logger::logger.error(source);
            Logger::logger.error(e.what());
        }
        return Contact::deserializationFromJson(j);
    }

    Contact Contact::deserializationFromJson(nlohmann::json j) {
        return Contact(j["type"],
                j["id"],
                j["groupid"],
                j["nickornamecard"],
                j["botid"],
                    j["anonymous"]);
    }

    MessageSource Contact::sendVoice0(const std::string &path, JNIEnv *env) {
        json j;
        json source;
        source["path"] = path;
        j["source"] = source.dump();
        j["contactSource"] = this->serializationToString();
        std::string re = Config::koperation(Config::Voice, j, env);
        if (re == "E1")
            throw UploadException("上传语音文件格式不对(必须为.amr/.silk)或文件不存在");
        else if(re == "E2")
            throw UploadException("上传语音文件大小超过服务器限制，一般限制在1MB上下");
        return MessageSource::deserializeFromString(re);
    }

    void enrollPlugin();

    void enrollPlugin0(CPPPlugin *p) {
        plugin = p;
    }

    json PluginConfig::serialize() {
        json j;
        j["name"] = name;
        j["version"] = version;
        j["author"] = author;
        j["description"] = description;
        j["time"] = time;
        j["id"] = id;
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
jstring Verify(JNIEnv *env, jobject) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    env->GetJavaVM(&gvm);
    JNIVersion = (int) env->GetVersion();
    try {
        //初始化日志模块
        Config::construct();
        Logger::logger.init();
        enrollPlugin();
        if (plugin == nullptr) {
            Logger::logger.error("无插件实例加载");
        } else {
            CPPPlugin::pluginLogger = new PluginLogger(&Logger::logger);
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
jobject PluginDisable(JNIEnv *env, jobject job) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    plugin->onDisable();
    plugin = nullptr;
    return job;
}
/*返回空值*/
jstring returnNull() {
    return MiraiCP::Tools::str2jstring("MIRAICP_NULL");
}
/*
* 消息解析分流
*/
jstring Event(JNIEnv *env, jobject, jstring content) {
    using namespace MiraiCP;
    ThreadManager::setEnv(env);
    std::string tmp = Tools::jstring2str(content, env);
    json j;
    try {
        j = json::parse(tmp);
    }
    catch (json::parse_error &e) {
        APIException("格式化json错误").raise();
        Logger::logger.error("For debug:" + j.dump());
        Logger::logger.error(e.what(), false);
        return returnNull();
    }
    ThreadManager::getThread()->stack.push(__FILE__, __LINE__, "source: " + tmp);
    try {
        switch ((int) j["type"]) {
            case 1: {
                //GroupMessage
                Event::processor.broadcast<GroupMessageEvent>(
                        GroupMessageEvent(j["group"]["botid"],
                                          Group(Group::deserializationFromJson(j["group"])),
                                          Member(Member::deserializationFromJson(j["member"])),
                                          MessageChain::deserializationFromString(j["message"].get<std::string>())
                                          .plus(MessageSource::deserializeFromString(j["source"]))
                        )
                );
                break;
            }
            case 2: {
                //私聊消息
                Event::processor.broadcast<PrivateMessageEvent>(
                        PrivateMessageEvent(j["friend"]["botid"],
                                            Friend(Friend::deserializationFromJson(j["friend"])),
                                            MessageChain::deserializationFromString(j["message"])
                                                    .plus(MessageSource::deserializeFromString(j["source"]))
                        ));
                break;
            }
            case 3:
                //群聊邀请
                Event::processor.broadcast<GroupInviteEvent>(
                        GroupInviteEvent(
                                j["source"]["botid"],
                                j["request"],
                                j["source"]["inviternick"],
                                j["source"]["inviterid"],
                                j["source"]["groupname"],
                                j["source"]["groupid"]
                        ));
                break;
            case 4:
                //好友
                Event::processor.broadcast<NewFriendRequestEvent>(
                        NewFriendRequestEvent(
                                j["source"]["botid"],
                                j["request"],
                                j["source"]["fromid"],
                                j["source"]["fromgroupid"],
                                j["source"]["fromnick"],
                                j["source"]["message"]
                        ));
                break;
            case 5:
                //新成员加入
                Event::processor.broadcast<MemberJoinEvent>(
                        MemberJoinEvent(
                                j["group"]["botid"],
                                j["jointype"],
                                Member(Member::deserializationFromJson(j["member"])),
                                Group(Group::deserializationFromJson(j["group"])),
                                j["inviterid"]
                        ));
                break;
            case 6:
                //群成员退出
                Event::processor.broadcast<MemberLeaveEvent>(MemberLeaveEvent(
                        j["group"]["botid"],
                        j["leavetype"],
                        j["memberid"],
                        Group(Group::deserializationFromJson(j["group"])),
                        j["operatorid"]
                ));
                break;
            case 7:
                Event::processor.broadcast<RecallEvent>(RecallEvent(
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
                Event::processor.broadcast<BotJoinGroupEvent>(BotJoinGroupEvent(
                        j["group"]["botid"],
                        j["etype"],
                        Group(Group::deserializationFromJson(j["group"])),
                        j["inviterid"]
                ));
                break;
            case 10:
                Event::processor.broadcast<GroupTempMessageEvent>(GroupTempMessageEvent(
                        j["group"]["botid"],
                        Group(Group::deserializationFromJson(j["group"])),
                        Member(Member::deserializationFromJson(j["member"])),
                        MessageChain::deserializationFromString(j["message"])
                                .plus(MessageSource::deserializeFromString(j["source"]))
                ));
                break;
            case 11:
                Event::processor.broadcast<BotOnlineEvent>(BotOnlineEvent(j["botid"]));
                break;
            case 12:
                Event::processor.broadcast<TimeOutEvent>(TimeOutEvent(j["msg"]));
                break;
            case 13:
                Event::processor.broadcast<NudgeEvent>(NudgeEvent(Contact::deserializationFromString(j["from"]), j["botid"]));
                break;
            default:
                throw APIException("Unreachable code");
        }
    }
    catch (json::type_error &e) {
        Logger::logger.error("json格式化异常,位置C-Handle");
        Logger::logger.error(e.what(), false);
        return Tools::str2jstring("ERROR");
    } catch (MiraiCPException &e) {
        Logger::logger.error("MiraiCP error:" + e.what());
        return Tools::str2jstring("ERROR");
    }
    return returnNull();
}

static int registerMethods(JNIEnv *env, const char *className,
                           JNINativeMethod *gMethods, int numMethods) {
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        return JNI_FALSE;
    }
    //注册native方法
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static JNINativeMethod method_table[] = {
        {(char*)"Verify", (char*)"()Ljava/lang/String;", (jstring*)Verify },
        {(char*)"Event", (char*)"(Ljava/lang/String;)Ljava/lang/String;", (jstring *) Event},
        {(char*)"PluginDisable", (char*)"()Ljava/lang/Void;", (jobject*) PluginDisable}
        };

extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    assert(env != nullptr);

    // 注册native方法
    if (!registerMethods(env, "tech/eritquearcus/miraicp/shared/CPP_lib", method_table, 3)) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

//结束对接JNI接口代码
#endif
#pragma clang diagnostic pop
