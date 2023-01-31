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

#ifndef MIRAICP_PRO_COMMAND_H
#define MIRAICP_PRO_COMMAND_H

#include <json.hpp>
#include <optional>


namespace MiraiCP {
    class MessageChain;
    class Bot;
    class Contact;

    /*!
     * @brief 指令 Interface
     * @doxygenEg{1001, command.cpp, 新建自定义命令}
     * @attention loader端的命令只支持从console传入, plugin端是对接 mirai 的RawCommand
     * @note 析构函数必须重写基类，否则会造成内存泄漏
     */
    class IRawCommand {
    public:
        struct Config {
            using string = std::string;
            /// 指令名不能为空
            string primaryName;
            /// 可以为空
            std::vector<string> secondNames;
            /// 用法
            string usage = "null";
            /// 描述
            string description = "null";
            /// 覆盖已有命令
            bool overrideOrigin = false;
            /// 前缀`/`可省略
            bool preFixOption = false;
        };

    public:
        virtual IRawCommand::Config config() = 0;
        virtual void onCommand(std::shared_ptr<Contact>, const Bot &, const MessageChain &) = 0;
        /// 析构函数必须override该函数
        virtual ~IRawCommand() = 0;
    };

    class CommandManager {
        std::vector<IRawCommand *> commandList;

    private:
        CommandManager() = default;
        ~CommandManager();

        bool internalRegister(IRawCommand *inPtr);

    public:
        IRawCommand *&operator[](const int &index) { return commandList[index]; }

        /*!
         * @brief 注册一条指令
         * @tparam T 指令的类，必须继承 IRawCommand 且默认构造函数可访问
         * @return 是否注册成功
         * @note 默认构造函数构造出的对象的config()函数内容会被注册
         */
        template<typename T>
        bool registerCommand() {
            static_assert(std::is_base_of_v<IRawCommand, T>, "只支持IRawCommand的派生类");
            return internalRegister(new T);
        }

        static CommandManager commandManager;
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_COMMAND_H
