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

class Main : public CPPPlugin {
public:
    Main() : CPPPlugin(PluginConfig(
                     "id",
                     "test",
                     "v1.0",
                     "a")) {}
    void onEnable() override {
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent e) {
            auto style1 = ForwardedMessageDisplayStrategy();
            style1.summary = "Summarya";
            style1.preview.emplace_back("preview1");
            style1.source = "SourceA";
            style1.brief = "BriefA";
            e.group.sendMessage("Origin:");
            ForwardedMessage(
                    {ForwardedNode(12, "a", MessageChain(PlainText("test")), 1),
                     ForwardedNode(12, "b", ForwardedMessage({ForwardedNode(12, "a", MessageChain(PlainText("test")), 1)}), 1)},
                    ForwardedMessageDisplayStrategy::defaultStrategy())
                    .sendTo(&e.group);

            if (e.message[0].type() == OnlineForwardedMessage::type())
                e.message[0].get<OnlineForwardedMessage>().toForwardedMessage(ForwardedMessageDisplayStrategy::defaultStrategy()).sendTo(&e.group);
            e.group.sendMessage("style1");
            ForwardedMessage(
                    {ForwardedNode(12, "a", MessageChain(PlainText("test")), 1),
                     ForwardedNode(12, "b", ForwardedMessage({ForwardedNode(12, "a", MessageChain(PlainText("test")), 1)}), 1)},
                    style1)
                    .sendTo(&e.group);

            if (e.message[0].type() == OnlineForwardedMessage::type())
                e.message[0].get<OnlineForwardedMessage>().toForwardedMessage(style1).sendTo(&e.group);
        });
    }
};

void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin0(new Main());
}
