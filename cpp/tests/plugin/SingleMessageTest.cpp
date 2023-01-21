// Copyright (c) 2022 - 2023. Eritque arcus and contributors.
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

//
// Created by antares on 11/25/22.
//

#include <gtest/gtest.h>
#include "SingleMessage.h"
#include "MessageChain.h"
#include "MessageSource.h"

using namespace MiraiCP;


TEST(MessageTests, PlainText) {
    auto textNum = PlainText(123);
    auto textStr = PlainText("123");
    auto textStr2 = PlainText(std::string("123"));
    ASSERT_EQ(textNum.content, textStr.content);
    ASSERT_EQ(textNum.content, textStr2.content);
}

// overall test
TEST(MessageTests, MessageDeserialization) {
    auto jsonText = R"([{"type":"MessageSource","kind":"GROUP","botId":7390207025106941417,"ids":[-1591912080],"internalIds":[-1756799623],"time":1671208337,"fromId":222,"targetId":111,"originalMessage":[{"type":"QuoteReply","source":{"kind":"GROUP","botId":7390207025106941417,"ids":[-1591912081],"internalIds":[-1756799622],"time":1671208337,"fromId":222,"targetId":111,"originalMessage":[{"type":"PlainText","content":"aaa"}]}},{"type":"PlainText","content":"IAmPlainText"},{"type":"Image","imageId":"{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg","size":123,"imageType":"PNG","width":0,"height":0,"isEmoji":false},{"type":"AtAll"},{"type":"At","target":222},{"type":"Face","id":1},{"type":"UnsupportedMessage","struct":""}]},{"type":"QuoteReply","source":{"kind":"GROUP","botId":7390207025106941417,"ids":[-1591912081],"internalIds":[-1756799622],"time":1671208337,"fromId":222,"targetId":111,"originalMessage":[{"type":"PlainText","content":"aaa"}]}},{"type":"PlainText","content":"IAmPlainText"},{"type":"Image","imageId":"{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg","size":123,"imageType":"PNG","width":0,"height":0,"isEmoji":false},{"type":"AtAll"},{"type":"At","target":222},{"type":"Face","id":1},{"type":"UnsupportedMessage","struct":""}])";
    auto mc = MessageChain::deserializationFromMessageJson(nlohmann::json::parse(jsonText));
    ASSERT_EQ(8 - 1, mc.size()); // messageSource doesn't count
    ASSERT_EQ(std::nullopt, mc.source);
    mc.add(MessageSource::deserializeFromString(
            R"({"kind":"GROUP","botId":2067911492835845661,"ids":[1042067660],"internalIds":[1509903364],"time":1674322522,"fromId":222,"targetId":111,"originalMessage":[{"type":"PlainText","content":"x"}]})"));
    ASSERT_NE(std::nullopt, mc.source);
    ASSERT_EQ(SingleMessageType::QuoteReply_t, mc[0].getType());
    ASSERT_EQ("[-1591912081]", mc[0].getVal<QuoteReply>().source.ids);
    ASSERT_EQ(SingleMessageType::PlainText_t, mc[1].getType());
    ASSERT_EQ("IAmPlainText", mc[1].getVal<PlainText>().content);
    ASSERT_EQ(SingleMessageType::Image_t, mc[2].getType());
    ASSERT_EQ("{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg", mc[2].getVal<Image>().id);
    ASSERT_EQ(SingleMessageType::AtAll_t, mc[3].getType());
    ASSERT_EQ(SingleMessageType::At_t, mc[4].getType());
    ASSERT_EQ(222, mc[4].getVal<At>().target);
    ASSERT_EQ(SingleMessageType::Face_t, mc[5].getType());
    ASSERT_EQ(1, mc[5].getVal<Face>().id);
    ASSERT_EQ(SingleMessageType::UnsupportedMessage_t, mc[6].getType());
}
