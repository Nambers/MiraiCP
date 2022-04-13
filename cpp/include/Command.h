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
#include "Config.h"
#include "Exception.h"
#include "Logger.h"
#include <optional>
namespace MiraiCP {
    class MessageChain;
    class Bot;
    class Contact;
    /*!
     * @brief 指令 Interface
     * @example 注册一条指令
     * @code
     * class a: public IRawCommand{
        public:
            IRawCommand::Config config() override{
                return IRawCommand::Config("test");
            }
            void onCommand(std::optional<Contact> c, const Bot& b, const MessageChain& a) override{
                Logger::logger.info(a.toMiraiCode());
            }
            a() = default;
        };
     * @endcode
     * 然后在`onEnable()`里
     * @code
     * CommandManager::commandManager.registerCommand(a());
     * @endcode
     * @attention loader端的命令只支持从console传入, plugin端是对接mirai的RawCommand
     */
    class IRawCommand {
        using string = std::string;

    public:
        struct Config {
        public:
            /// 指令名不能为空
            string primaryName;
            /// 可以为空
            std::vector<string> secondNames;
            /// 用法
            string usage;
            /// 描述
            string description;
            /// 覆盖已有命令
            bool override;
            /// 前缀`/`可省略
            bool preFixOption;
            explicit Config(string primaryName,
                            std::vector<string> secondNames = std::vector<std::string>(),
                            string usage = "null",
                            string description = "null",
                            bool override = false,
                            bool preFixOption = false)
                : primaryName(std::move(primaryName)),
                  secondNames(std::move(secondNames)),
                  usage(std::move(usage)),
                  description(std::move(description)),
                  override(override),
                  preFixOption(preFixOption) {}
        };

        virtual IRawCommand::Config config() = 0;
        virtual void onCommand(std::optional<Contact>, const Bot &, const MessageChain &) = 0;
        IRawCommand() = default;
        virtual ~IRawCommand() = default;
    };

    class CommandManager {
    private:
        CommandManager() = default;
        std::vector<std::shared_ptr<IRawCommand>> commandList;

    public:
        std::shared_ptr<IRawCommand> &operator[](const int &index) { return commandList[index]; }

        /*!
         * @brief 注册一条指令
         * @param command 指令
         * @return 是否注册成功
         */
        template<class T>
        bool registerCommand(T command) {
            static_assert(std::is_base_of_v<IRawCommand, T>, "只支持IRawCommand的派生类");
            nlohmann::json j;
            j["pluginId"] = Config::pluginId;
            j["usage"] = command.config().usage;
            j["primaryName"] = command.config().primaryName;
            j["secondName"] = command.config().secondNames;
            j["description"] = command.config().description;
            j["override"] = command.config().override;
            j["preFixOption"] = command.config().preFixOption;
            size_t before = commandList.size();
            std::shared_ptr<IRawCommand> c;
            c.reset(new T(command));
            commandList.push_back(c);
            size_t now = commandList.size();
            if (now - before == 1)
                j["bindId"] = now - 1;
            else {
                auto i = std::find(commandList.begin(), commandList.end(), c);
                if (i != commandList.end())
                    j["bindId"] = i - commandList.begin();
                else
                    throw IllegalArgumentException("找不到合适的bindId", MIRAICP_EXCEPTION_WHERE);
            }
            nlohmann::json rej;
            rej["command"] = j.dump();
            std::string re = Config::koperation(Config::CommandReg, rej);
            return re == "true";
        }
        static CommandManager commandManager;
    };
} // namespace MiraiCP
#endif //MIRAICP_PRO_COMMAND_H
