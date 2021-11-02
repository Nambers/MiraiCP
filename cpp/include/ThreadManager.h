#ifndef MIRAICP_PRO_THREADMANAGER_H
#define MIRAICP_PRO_THREADMANAGER_H
#define MiraiCPThrow(x) throw((x).append(__FILE__, __LINE__))
#include <jni.h>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace MiraiCP {
    /*!
    * @class threadManager
    * @brief 多线程管理.
    */
    class ThreadManager {
    public:

        /// @brief 每个线程实例.
        struct ThreadInfo {
            JNIEnv *e{};
            bool attach{};
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
        static JNIEnv *getEnv();
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_THREADMANAGER_H
