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


#ifndef MIRAICP_PRO_JNIENVMANAGER_H
#define MIRAICP_PRO_JNIENVMANAGER_H


#include <functional>
#include <jni.h>
#include <mutex>
#include <thread>


class JNIEnvManager {
    typedef std::thread::id threadid;

public:
    /// @brief 每个线程实例.
    struct ThreadInfo {
        JNIEnv *env_ptr{};
        bool attach{};
    };

private:
    // 类静态成员
    static std::unordered_map<threadid, ThreadInfo> threadJNIEnvs; /// < 线程池(线程id:env).
    static std::recursive_mutex mtx;                               ///< 线程池读写锁.
    static JavaVM *gvm;                                            ///< 全局JavaVM对象，用于多线程管理中新建线程的JNIEnv.

private:
    JNIEnvManager() = default;

private:
    static JNIEnv *newEnv(); ///< 新建一个env，于getEnv中没取到env时调用.
    /// @brief 获取线程id.
    static threadid getThreadId() { return std::this_thread::get_id(); }

public:
    static void setGvm(JavaVM *_gvm) { gvm = _gvm; }

    static JavaVM *&getGvm() { return gvm; }
    /// @brief 尝试设置env给当前线程.
    /// @return true:设置成功，false:设置失败.
    static bool setEnv(JNIEnv *e);

    /*!
         * 	@brief 结束当前线程的env，也就是释放当前线程缓存的env.
         *  @note 不过继续调用getEnv()将再次获取，所以本方法调用后线程也可以通过getEnv重新获取一个env，本方法的作用就是在结束后释放空间
         */
    static void detach();

    /// @brief detach所有线程的env
    /// @note 一般在程序完全退出前使用
    static void detachAll();

    /// @brief 取env,如果不存在重新获取
    /// @internal 一般为`miraicp`内部调用jni接口时调用
    /// @param file 为支持`StackTracer`而增加, 为`__FILE__`宏(文件名), 在调用处传入因为当__FILE__作为默认参数传入时不准确
    /// @param loc 为`__LINE__`宏(行号), 同上
    /// @param func 为`__FUNC__`宏(方法名)
    static JNIEnv *getEnv();
};


namespace MiraiCP {
    [[deprecated("ThreadManager更名为JNIEnvManager，并且从MiraiCP命名空间中移出；请使用JNIEnvManager")]] typedef JNIEnvManager ThreadManager;
} // namespace MiraiCP

#endif //MIRAICP_PRO_JNIENVMANAGER_H
