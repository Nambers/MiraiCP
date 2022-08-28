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

#include "LoaderLogger.h"
#include "MiraiCPMacros.h"
#include "EnvMocker.h"
#include "redirectCout.h"
#include <gtest/gtest.h>
#include <string>

namespace MiraiCP::Redirector {
    void SetCoutRecorder(void (*recorder)(std::string));
    void SetCerrRecorder(void (*recorder)(std::string));
} // namespace MiraiCP::Redirector


TEST(RedirectStandardOutputForLibLoaderTest, COUT) {
    // use local cout and cerr
    MiraiCP::Redirector::setRedirectedObjs(&std::cout, &std::cerr);
    // initialize mocker
    EnvScopeMocker mocker;

    // define string to memorize result
    static std::string cout_result;
    MIRAICP_DEFER(cout_result.clear(););

    // define recorder
    struct inner {
        static void recoder(std::string t) {
            cout_result += t;
        }
    };

    // start redirection
    MiraiCP::Redirector::start();
    MIRAICP_DEFER(MiraiCP::Redirector::reset(););

    // set recorder
    MiraiCP::Redirector::SetCoutRecorder(inner::recoder);
    MIRAICP_DEFER(MiraiCP::Redirector::SetCoutRecorder(nullptr););

    // test
    std::cout << "test";
    std::cout.flush();
    std::cout << "aabb";
    std::cout.flush();
    std::cout << "111" << std::endl;
    ASSERT_EQ("testaabb111\n", cout_result);
}

TEST(RedirectStandardOutputForLibLoaderTest, CERR) {
    // use local cout and cerr
    MiraiCP::Redirector::setRedirectedObjs(&std::cout, &std::cerr);
    // initialize mocker
    EnvScopeMocker mocker;

    // define string to memorize result
    static std::string cerr_result;
    MIRAICP_DEFER(cerr_result.clear(););

    // define recorder
    struct inner {
        static void recoder(std::string t) {
            cerr_result += t;
        }
    };

    // start redirection
    MiraiCP::Redirector::start();
    MIRAICP_DEFER(MiraiCP::Redirector::reset(););

    // set recorder
    MiraiCP::Redirector::SetCerrRecorder(inner::recoder);
    MIRAICP_DEFER(MiraiCP::Redirector::SetCerrRecorder(nullptr););

    // test
    std::cerr << "test";
    std::cerr << "aabb";
    std::cerr.flush();
    std::cerr << "111" << std::endl;
    ASSERT_EQ("testaabb111\n", cerr_result);
}
