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

#include "MiraiCPMacros.h"
// -----------------------
#include <functional>
#include <jni.h>
#include <mutex>
#include <thread>


class JNIEnvManager;


class MIRAICP_EXPORT JNIEnvManager {
public:
    /// @brief 每个线程实例.
    /// @note dev: thread_local对象，线程唯一实例
    class ThreadInfo;

private:
    JNIEnvManager() = default;

public:
    /// @brief 获取全局JavaVM对象.
    static JavaVM *getGvm();

    /// @brief 设置全局JavaVM对象.
    static void setGvm(JavaVM *_gvm);

    /// @brief 尝试给当前线程设置env.
    /// @return true:设置成功，false:该线程已经设置过了.
    static bool setEnv(JNIEnv *e);

    /*!
     * 	@brief 结束当前线程的env，也就是释放当前线程缓存的env.
     *  @note 不过继续调用getEnv()将再次获取，所以本方法调用后线程也可以通过getEnv重新获取一个env，本方法的作用就是在结束后释放空间
     */
    static void detach();


    /// @brief 取env,如果不存在重新获取
    /// @internal 一般为`miraicp`内部调用jni接口时调用
    /// @param file 为支持`StackTracer`而增加, 为`__FILE__`宏(文件名), 在调用处传入因为当__FILE__作为默认参数传入时不准确
    /// @param loc 为`__LINE__`宏(行号), 同上
    /// @param func 为`__FUNC__`宏(方法名)
    static JNIEnv *getEnv();
};

#endif //MIRAICP_PRO_JNIENVMANAGER_H
