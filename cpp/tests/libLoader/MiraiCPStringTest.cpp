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

#include "MiraiCPStringInternal.h"
#include <gtest/gtest.h>


using namespace MiraiCP;


TEST(loaderToolsTest, MiraiCPString) {
    {
        auto target1 = MiraiCPString("test");
        EXPECT_EQ("test", target1.toString()); // char* -> MiraiCPString -> std::string
    }

    {
        auto ttarget2 = MiraiCPString(std::string("test"));
        const char *tt2 = ttarget2.copyToCharPtr();
        EXPECT_STRCASEEQ("test", tt2);         // std::string -> MiraiCPString -> char*
        EXPECT_NO_FATAL_FAILURE(delete[] tt2); // delete[] char*
    }

    {
        auto ttarget3 = MiraiCPString("test");
        const char *tt3 = ttarget3.copyToCharPtr();
        EXPECT_STRCASEEQ("test", tt3);         // char* -> MiraiCPString -> char*
        EXPECT_NO_FATAL_FAILURE(delete[] tt3); // delete[] char*
    }

    {
        auto target4 = MiraiCPString(std::string("test"));
        std::string s4 = target4.toString();
        EXPECT_EQ("test", s4); // std::string -> MiraiCPString -> std::string
    }

    {
        auto target5 = MiraiCPString(""); // empty char* construction
        const char *t5 = target5.copyToCharPtr();
        EXPECT_NE(nullptr, t5);   // check not null
        EXPECT_EQ(0, strlen(t5)); // check 0 length
        EXPECT_EQ(0, t5[0]);      // check 0
        std::string s2 = target5.toString();
        EXPECT_EQ(std::string(), s2); // check empty string
        EXPECT_NO_FATAL_FAILURE(delete[] t5); // delete[] char*
    }

    {
        auto target6 = MiraiCPString(std::string()); // empty std::string construction
        const char *t6 = target6.copyToCharPtr();
        EXPECT_NE(nullptr, t6);   // check not null
        EXPECT_EQ(0, strlen(t6)); // check 0 length
        EXPECT_EQ(0, t6[0]);      // check 0
        std::string s6 = target6.toString();
        EXPECT_EQ(std::string(), s6); // check empty string
        EXPECT_NO_FATAL_FAILURE(delete[] t6); // delete[] char*
    }

//    // it will be failed because the freePointer in test(executable) is different with freePointer in MiraiCP(library)
//    {
//        MiraiCPString target7; // default construction
//        EXPECT_EQ(nullptr, *(char **) &target7);
//        EXPECT_EQ(0, *(1 + (size_t *) &target7));
//        typedef decltype(&::std::free) freePointerType;
//        MiraiCPString targetTemp("1");
//        EXPECT_EQ(*(2 + (freePointerType *) &targetTemp), *(2 + (freePointerType *) &target7));
//        MiraiCPString targetTemp2(std::string("x"));
//        EXPECT_EQ(*(2 + (freePointerType *) &targetTemp2), *(2 + (freePointerType *) &target7));
//    }
}
