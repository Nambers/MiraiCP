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
#include <MiraiCP.hpp>
using namespace MiraiCP;

const PluginConfig CPPPlugin::config{
        "id",   // 插件id
        "test", // 插件名称
        "v1.0", // 插件版本
        "a",    // 插件作者
                // 可选：插件描述
                // 可选：日期
};

// 目前需要把 Exception 复制出来用, 将在下面版本修复.
// 或者直接捕获基类 (在不需要内部数据的情况下)
class MIRAICP_EXPORT BotException : public MiraiCPExceptionCRTP<BotException> {
public:
    explicit BotException(string _filename, int _lineNum) : MiraiCPExceptionCRTP("没有权限执行该操作", std::move(_filename), _lineNum) {}
    explicit BotException(const string &d, string _filename, int _lineNum) : MiraiCPExceptionCRTP(d, std::move(_filename), _lineNum) {}
    static string exceptionType() { return "BotException"; }
};
class MIRAICP_EXPORT MemberException : public MiraiCPExceptionCRTP<MemberException> {
public:
    enum MemberExceptionType : int {
        OtherType,
        NoSuchGroup,
        NoSuchMember
    };
    MemberExceptionType type = OtherType;
    /*
        *   "1" - 找不到群
        *	"2" - 找不到群成员
        */
    explicit MemberException(int _type, string _filename, int _lineNum) : MiraiCPExceptionCRTP(
                                                                                  [&]() -> string {
                                                                                      type = MemberExceptionType(_type);
                                                                                      switch (type) {
                                                                                          case NoSuchGroup:
                                                                                              return "找不到群";
                                                                                          case NoSuchMember:
                                                                                              return "找不到群成员";
                                                                                          default:
                                                                                              return "";
                                                                                      }
                                                                                  }(),
                                                                                  std::move(_filename), _lineNum) {}
    static string exceptionType() { return "MemberException"; }
};


class Main : public CPPPlugin {
public:
    Main() : CPPPlugin() {}
    void onEnable() override {
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent e) {
            // 从id构造群成员
            Member a(e.sender.id(), e.group.id(), e.bot.id());
            // 踢出群成员
            try {
                Member m = Member(e.sender.id(), e.group.id(), e.bot.id());
                m.kick("this_is_reason");
            } catch (BotException &err) {
                //权限不足
                Logger::logger.error(err.what());
            } catch (MemberException &err) {
                if (err.type == 1) {
                    //找不到群
                }
                if (err.type == 2) {
                    //找不到群成员
                }
            }
            // 或者
//            catch(MiraiCPExceptionBase &err){
//                if(err.getExceptionType() == "BotException"){
//                    // 权限不足
//                }
//            }
        });
    }
};

void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin<Main>();
}
