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

#include "miraicpString.h"
#include <gtest/gtest.h>
using namespace MiraiCP;

TEST(MiraiCPStringTest, emptyStr) {
    auto target = MiraiCPString("");
    auto chars = target.copyToCharPtr();
    EXPECT_EQ(true, target.isEmpty());
    EXPECT_STRCASEEQ("", chars);
    try {
        delete[] chars;
    } catch (...) {
        FAIL() << "Cannot delete chars";
    }
}

TEST(MiraiCPStringTest, NullPtr) {
    auto target = MiraiCPString(nullptr);
    auto chars = target.copyToCharPtr();
    EXPECT_NE(nullptr, chars);
    EXPECT_EQ("", std::string(chars));
    try {
        delete[] chars;
    } catch (...) {
        FAIL() << "Cannot delete chars";
    }
}

TEST(MiraiCPStringTest, Convert) {
    std::string origin("aabbcc--2333//\\");
    auto charPointer = origin.c_str();
    auto target = MiraiCPString(charPointer);
    EXPECT_STRCASEEQ(origin.c_str(), target.toString().c_str());
    EXPECT_STRCASEEQ(origin.c_str(), target.copyToCharPtr());
    EXPECT_EQ(origin, target.toString());
    auto target2 = MiraiCPString(origin);
    EXPECT_STRCASEEQ(origin.c_str(), target2.toString().c_str());
    EXPECT_STRCASEEQ(origin.c_str(), target2.copyToCharPtr());
    EXPECT_EQ(origin, target2.toString());
    EXPECT_EQ(target, target2);
}