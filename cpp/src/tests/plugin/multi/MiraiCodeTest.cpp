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
    EXPECT_EQ(1, target.size());
    ASSERT_EQ(true, target.first<Image>().has_value());
    EXPECT_EQ("{4D120D1B-F461-3A12-C731-57D974CE28EB}.jpg", target.first<Image>().value().id);
}

TEST(MiraiCodeTest, PlainText) {
    auto target = MessageChain::deserializationFromMiraiCode("4D120D1B-F461-3A12-C731-57D974CE28EB");
    EXPECT_EQ(1, target.size());
    ASSERT_EQ(true, target.first<PlainText>().has_value());
    EXPECT_EQ("4D120D1B-F461-3A12-C731-57D974CE28EB", target.first<PlainText>().value().content);
}
