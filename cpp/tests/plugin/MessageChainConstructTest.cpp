// Copyright (c) 2023. Eritque arcus and contributors.
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
#define ASSERT_INSTANCE(expect, actual) ASSERT_EQ(expect::type(), actual->internalType)

#include "MiraiCode.h"
#include "MessageChain.h"
#include <gtest/gtest.h>

using namespace MiraiCP;

TEST(MessageChainTest, String) {
    MessageChain chain = MessageChain("test");
    MessageChain chain2 = MessageChain("test", "test2");
    ASSERT_EQ(1, chain.size());
    ASSERT_INSTANCE(PlainText, chain[0]);
    ASSERT_EQ("test", chain[0]->content);
    ASSERT_EQ(2, chain2.size());
    ASSERT_EQ("test", chain2[0]->content);
    ASSERT_EQ("test2", chain2[1]->content);
}

TEST(MessageChainTest, SingleMessage) {
    MessageChain chain = MessageChain(PlainText("test"));
    ASSERT_EQ(1, chain.size());
    ASSERT_INSTANCE(PlainText, chain[0]);
    ASSERT_EQ("test", chain[0]->content);

    MessageChain chain2 = MessageChain(PlainText("test"), At(1));
    ASSERT_EQ(2, chain2.size());
    ASSERT_INSTANCE(PlainText, chain2[0]);
    ASSERT_EQ("test", chain2[0]->content);
    ASSERT_INSTANCE(At, chain2[1]);
    ASSERT_EQ(1, chain2[1].getVal<At>().target);
}

TEST(MessageChainTest, add) {
    MessageChain chain = MessageChain("TEST");
    chain.add(PlainText("TEST2"));
    ASSERT_EQ(2, chain.size());
    ASSERT_INSTANCE(PlainText, chain[0]);
    ASSERT_EQ("TEST", chain[0]->content);
    ASSERT_INSTANCE(PlainText, chain[1]);
    ASSERT_EQ("TEST2", chain[1]->content);
}

TEST(MessageChainTest, plus) {
    auto expect = MessageChain("TEST");
    auto actual = MessageChain("TEST");
    auto chain = actual.plus(PlainText("x"));
    ASSERT_EQ(expect, actual);
    ASSERT_EQ(2, chain.size());
}