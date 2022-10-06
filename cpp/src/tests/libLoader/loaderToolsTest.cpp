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

#include "common/loaderTools.h"
#include <gtest/gtest.h>


using namespace LibLoader;
using namespace MiraiCP;


TEST(loaderToolsTest, readJsonFile) {
    // todo(ea): 其他配置项
    std::ofstream file("test.tmp");
    file << "{\n"
            "  \"pluginConfig\":[\n"
            "    {\n"
            "      \"path\":\"libMiraiCP_multi.dll\"\n"
            "    }\n"
            "  ]\n"
            "}";
    file.close();
    auto target = readJsonFile("test.tmp");
    EXPECT_EQ("libMiraiCP_multi.dll", target["pluginConfig"][0]["path"]);
    file.open("test.tmp");
    file.clear();
    file << "{\n"
            "  \"accounts\": [{\n"
            "      \"id\": 111,\n"
            "      \"passwords\": \"a\"\n"
            "  }],\n"
            "  \"cppPaths\": [\n"
            "    {\"path\":\"libMiraiCP_multi.dll\"}\n"
            "  ]\n"
            "}";
    file.close();
    target = readJsonFile("test.tmp");
    EXPECT_EQ(111, target["accounts"][0]["id"]);
    EXPECT_EQ("a", target["accounts"][0]["passwords"]);
    EXPECT_EQ("libMiraiCP_multi.dll", target["cppPaths"][0]["path"]);
    remove("test.tmp");
}


TEST(loaderToolsTest, compareAndSwapStrong) {
    int a = 0;
    int b = 1;
    CASStrong(a, b);
    ASSERT_EQ(1, a);
}
