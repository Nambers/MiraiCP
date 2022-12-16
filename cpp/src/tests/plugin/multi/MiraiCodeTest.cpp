// Copyright (c) 2022. Eritque arcus and contributors.
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

#include "MessageChain.h"
#include <gtest/gtest.h>
using namespace MiraiCP;

TEST(MiraiCodeTest, Image) {
    auto target = MessageChain::deserializationFromMiraiCode("[mirai:image:{4D120D1B-F461-3A12-C731-57D974CE28EB}.jpg]");
    ASSERT_EQ(1, target.size());
    ASSERT_EQ(true, target.first<Image>().has_value());
    EXPECT_EQ("{4D120D1B-F461-3A12-C731-57D974CE28EB}.jpg", target.first<Image>().value().id);
    EXPECT_EQ(MessageChain(Image("{4D120D1B-F461-3A12-C731-57D974CE28EB}.jpg")), target);
}

TEST(MiraiCodeTest, PlainText) {
    auto target = MessageChain::deserializationFromMiraiCode("4D120D1B-F461-3A12-C731-57D974CE28EB");
    ASSERT_EQ(1, target.size());
    ASSERT_EQ(true, target.first<PlainText>().has_value());
    EXPECT_EQ("4D120D1B-F461-3A12-C731-57D974CE28EB", target.first<PlainText>().value().content);
    EXPECT_EQ(MessageChain(PlainText("4D120D1B-F461-3A12-C731-57D974CE28EB")), target);
}

TEST(MiraiCodeTest, LightApp) {
    auto target = MessageChain::deserializationFromMiraiCode(
            R"([mirai:app:{"config"\:{"height"\:0\,"forward"\:1\,"ctime"\:1658542999\,"width"\:0\,"type"\:"normal"\,"token"\:"5bb9b573252b20d1ab9__e8516ff785a"\,"autoSize"\:0}\,"prompt"\:"\[\]"\,"app"\:"com._____.miniapp_01"\,"ver"\:"1.0.0.19"\,"view"\:"view_8C8E89B49BE6098__298ADDFF2DBABA4"\,"meta"\:{"detail_1"\:{"appid"\:"11099__557"\,"preview"\:"https\:\\/\\/pubminishare-30161.picsz.____.cn\\/B79E2482-3B36-4BB9-8F15-301B55CDD6A6"\,"shareTemplateData"\:{}\,"gamePointsUrl"\:""\,"gamePoints"\:""\,"url"\:"m.q.__.com\\/a\\/s\\/8734f8611101c272b7fd7a561cbd9b02"\,"appType"\:0\,"desc"\:"____"\,"title"\:"____"\,"scene"\:0\,"host"\:{"uin"\:0\,"nick"\:"___"}\,"icon"\:"https\:\\/\\/miniapp._____.cn\\/public\\/appicon\\/51f90239b78a2e4994c11215f4c4ba15_200.jpg"\,"shareTemplateId"\:"8C8E89B49BE609866298ADDFF2DBABA4"\,"showLittleTail"\:""}}\,"desc"\:""}])");
    ASSERT_EQ(1, target.size());
    ASSERT_EQ(true, target.first<LightApp>().has_value());
    EXPECT_EQ(
            R"({"config"\:{"height"\:0\,"forward"\:1\,"ctime"\:1658542999\,"width"\:0\,"type"\:"normal"\,"token"\:"5bb9b573252b20d1ab9__e8516ff785a"\,"autoSize"\:0}\,"prompt"\:"\[\]"\,"app"\:"com._____.miniapp_01"\,"ver"\:"1.0.0.19"\,"view"\:"view_8C8E89B49BE6098__298ADDFF2DBABA4"\,"meta"\:{"detail_1"\:{"appid"\:"11099__557"\,"preview"\:"https\:\\/\\/pubminishare-30161.picsz.____.cn\\/B79E2482-3B36-4BB9-8F15-301B55CDD6A6"\,"shareTemplateData"\:{}\,"gamePointsUrl"\:""\,"gamePoints"\:""\,"url"\:"m.q.__.com\\/a\\/s\\/8734f8611101c272b7fd7a561cbd9b02"\,"appType"\:0\,"desc"\:"____"\,"title"\:"____"\,"scene"\:0\,"host"\:{"uin"\:0\,"nick"\:"___"}\,"icon"\:"https\:\\/\\/miniapp._____.cn\\/public\\/appicon\\/51f90239b78a2e4994c11215f4c4ba15_200.jpg"\,"shareTemplateId"\:"8C8E89B49BE609866298ADDFF2DBABA4"\,"showLittleTail"\:""}}\,"desc"\:""})",
            target.first<LightApp>().value().content);
}

TEST(MiraiCodeTest, MessageChain) {
    auto target = MessageChain::deserializationFromMiraiCode(
            "IAmPlainText[mirai:image:{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg][mirai:atall][mirai:at:222][mirai:face:1]");
    ASSERT_EQ(5, target.size());
    ASSERT_EQ(SingleMessageType::PlainText_t, target[0].getType());
    ASSERT_EQ("IAmPlainText", target[0].getVal<PlainText>().content);
    ASSERT_EQ(SingleMessageType::Image_t, target[1].getType());
    ASSERT_EQ("{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg", target[1].getVal<Image>().id);
    ASSERT_EQ(SingleMessageType::AtAll_t, target[2].getType());
    ASSERT_EQ(SingleMessageType::At_t, target[3].getType());
    ASSERT_EQ(222, target[3].getVal<At>().target);
    ASSERT_EQ(SingleMessageType::Face_t, target[4].getType());
    ASSERT_EQ(1, target[4].getVal<Face>().id);
}

TEST(MiraiCodeTest, serviceMessage) {
    auto target = MessageChain::deserializationFromMiraiCode(
            "[mirai:service:1,<?xml version='1.0' encoding='UTF-8' standalone='yes' ?><msg templateID='1' serviceID='1' action='plugin' actionData='' brief='' flag='3' url=''><source name='' icon=''/></msg>]");
    ASSERT_EQ(1, target.size());
    ASSERT_EQ(SingleMessageType::ServiceMessage_t, target[0].getType());
    ASSERT_EQ(1, target[0].getVal<ServiceMessage>().id);
}

TEST(MiraiCodeTest, flashImg) {
    auto target = MessageChain::deserializationFromMiraiCode(
            "[mirai:flash:{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg]");
    ASSERT_EQ(1, target.size());
    ASSERT_EQ(SingleMessageType::FlashImage_t, target[0].getType());
    ASSERT_EQ("{01E9451B-70ED-EAE3-B37C-101F1EEBF5B5}.jpg", target[0].getVal<FlashImage>().id);
}
