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

#include "Tools.h"
#include <gtest/gtest.h>
using namespace MiraiCP;

TEST(ToolsTest, replace) {
    auto originPath = R"(D:\a\111\[[[\)";
    auto target = Tools::replace(originPath, "\\", "/");
    ASSERT_EQ("D:/a/111/[[[/", target);
}

TEST(ToolsTest, vectorToString) {
    std::vector<std::string> strVec = {"a", "b", "111", "[]\\"};
    std::vector<int> intVec = {1, 2, 3, 4};
    ASSERT_EQ("a,b,111,[]\\", Tools::VectorToString(strVec));
    ASSERT_EQ("1,2,3,4", Tools::VectorToString(intVec));
}

TEST(ToolsTest, stringToVec) {
    {
        auto str = "[111,222,333,444]";
        auto target = Tools::StringToVector(str);
        std::vector<QQID> expect = {111, 222, 333, 444};
        ASSERT_EQ(expect, target);
    }
    {
        auto str = ",111,222,333,,444,";
        auto target = Tools::StringToVector(str);
        std::vector<QQID> expect = {111, 222, 333, 444};
        ASSERT_EQ(expect, target);
    }
}

TEST(ToolsTest, escapeMiraiCode) {
    auto origin = R"(\[\]\:\,\\//)";
    auto unescaping = Tools::escapeFromMiraiCode(origin);
    auto escaping = Tools::escapeToMiraiCode(unescaping);
    ASSERT_EQ("[]:,\\//", unescaping);
    ASSERT_EQ(origin, escaping);
}

TEST(ToolsTest, startsWith) {
    auto target = R"(dj()()===sa_28129>casda\\fwa)";
    ASSERT_TRUE(Tools::starts_with(target, R"(dj()()===sa_28129>casda\\fwa)"));
    ASSERT_TRUE(Tools::starts_with(target, R"(dj()()===sa_28129>casda\\)"));
    ASSERT_TRUE(Tools::starts_with(target, R"(dj()()===sa_28129>)"));
    ASSERT_TRUE(Tools::starts_with(target, R"(dj()()===sa_2)"));
}

TEST(ToolsTest, icompareChar) {
    ASSERT_TRUE(Tools::icompareChar('a', 'A'));
    ASSERT_TRUE(Tools::icompareChar('a', 'a'));
    ASSERT_FALSE(Tools::icompareChar('a', 'b'));
    ASSERT_TRUE(Tools::icompareChar('1', '1'));
}

TEST(ToolsTest, iequal) {
    ASSERT_TRUE(Tools::iequal("aAaBbB((}}||", "AaAbbB((}}||"));
    ASSERT_TRUE(Tools::iequal("啊a", "啊A"));
    ASSERT_FALSE(Tools::iequal("啊a", "啊"));
}

TEST(ToolsTest, split) {
    auto str = R"(111,222,333,sss,bbb,k扩扩扩,\\")";
    auto target = Tools::split(str, ",");
    auto expect = std::vector<std::string>{"111", "222", "333", "sss", "bbb", "k扩扩扩", R"(\\")"};
    ASSERT_EQ(expect, target);
}
