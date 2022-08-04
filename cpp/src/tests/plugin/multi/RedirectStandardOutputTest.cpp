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

#include "Logger.h"
#include "MockLibLoaderApi.h"
#include <gtest/gtest.h>
using namespace MiraiCP;

// TEST(RedirectStandardOutputForPluginTest, COUT) {
//     LoaderApiMock();
//     std::string re;
//     MiraiCP::Logger::logger.registerHandle([&re](const std::string &str, int level) {
//         if (level == 0) re += str;
//     });
//     MiraiCP::Logger::logger.info("test");
//     std::cout << "aabb";
//     std::cout.flush();
//     std::cout << "111" << std::endl;
//     ASSERT_EQ("testaabb111\n", re);
// }
//
// TEST(RedirectStandardOutputForPluginTest, CERR) {
//     LoaderApiMock();
//     std::string re;
//     MiraiCP::Logger::logger.registerHandle([&re](const std::string &str, int level) {
//         if (level == 2) re += str;
//     });
//     MiraiCP::Logger::logger.error("test");
//     std::cerr << "aabb";
//     std::cerr.flush();
//     std::cerr << "111" << std::endl;
//     ASSERT_EQ("testaabb111\n", re);
// }
