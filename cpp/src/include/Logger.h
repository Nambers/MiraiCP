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

#ifndef MIRAICP_PRO_LOGGER_H
#define MIRAICP_PRO_LOGGER_H


#include "MiraiCode.h"
#include "commonTypes.h"
#include <functional>
#include <sstream>


namespace MiraiCP {
    class MiraiCodeable; // forward declaration

    /*!
    * @class Logger
    * @brief 以MiraiCP的名义发送日志, 日志表现格式是: 2021-06-28 09:37:22 [log level]/MiraiCP: [log content], 为最底层的logger
	* 发送消息级日志
	* @code Logger::logger.info(string) @endcode
	* 发送警告级日志
	* @code Logger::logger.warning(string) @endcode
	* 发送错误级日志
	* @code Logger::logger.error(string) @endcode
    * @doxygenEg{1011, logger.cpp, 自定义日志handle}
    */
    class Logger_interface {
        using string = std::string;

    public:
        /// @brief 封装lambda类型
        /// @param string 日志内容
        /// @param 日志级别
        ///     - 0 info
        ///     - 1 warning
        ///     - 2 error
        typedef std::function<void(string, int)> Action;

        /// @brief loggerhandler会在每次log执行前执行一遍，可用于执行自定义的保存操作等
        struct Handler {
            /// @brief 是否启用
            bool enable = true;
            /// @brief 执行的操作，格式为lambda
            Action action = [](const string &content, int level) {};
        };

        Handler loggerhandler;

    private:
        static std::string constructString() {
            return "";
        }

        template<class T, class... T1>
        static std::string constructString(T val, T1... val1) {
            std::stringstream sstream;
            sstream << val;
            return sstream.str() + constructString(val1...);
        }

        template<class... T>
        static std::string constructString(std::string a, T... val1) {
            return a + constructString(val1...);
        }

        template<class... T>
        static std::string constructString(MiraiCodeable &val, T... val1) {
            return val.toMiraiCode() + constructString(val1...);
        }

    protected:
        /// @brief 日志底层实现封装
        /// @param log 日志内容
        /// @param level 日志等级
        virtual void log_interface(const string &log, int level) = 0;

    public:
        ///发送普通(info级日志)
        template<class... T>
        void info(T... val) {
            this->log_interface(constructString(val...), 0);
        }

        ///发送警告(warning级日志)
        template<class... T>
        void warning(T... val) {
            this->log_interface(constructString(val...), 1);
        }

        ///发送错误(error级日志)
        template<class... T>
        void error(T... val) {
            this->log_interface(constructString(val...), 2);
        }

        /// @brief 设置loggerhandler的action
        /// @param action 执行的操作
        /// @see Logger::handler
        void registerHandle(Action action) {
            this->loggerhandler.action = std::move(action);
        }

        /// @brief 设置handler的启用状态
        /// @param state 状态，启用或者关闭
        /// @doxygenEg{1012, logger.cpp, 启用或关闭日志}
        void setHandleState(bool state) {
            this->loggerhandler.enable = state;
        }
    };

    class MIRAICP_EXPORT Logger : public Logger_interface {
    private:
        Logger() = default;

    protected:
        /// @brief 日志底层实现封装
        /// @param content 日志内容
        /// @param level 日志等级
        void log_interface(const std::string &content, int level) override;

    public:
        static Logger logger;
    };

    /// 带id(一般为bot账号)的logger
    class IdLogger : public Logger_interface {
    public:
        QQID id;

    protected:
        void log_interface(const std::string &content, int level) override;

    public:
        IdLogger(QQID id, Logger *l) : id(id) {
            this->loggerhandler = l->loggerhandler;
        }
    };
} // namespace MiraiCP

#endif //MIRAICP_PRO_LOGGER_H
