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

#include "MiraiCPNewThread.h"
#include "MockLibLoaderApi.h"
#include "Tools.h"
#include "Logger.h"
#include <gtest/gtest.h>
#include <iostream>


enum CoyMoveType : uint8_t {
    CONSTRUCT,
    DESTRUCT,
    MOVE_CONSTRUCT,
    MOVE_ASSIGN,
    COPY_CONSTRUCT,
    COPY_ASSIGN,
};

static const std::map<CoyMoveType, std::string> construct_type_string_map{
        {CONSTRUCT, "CONSTRUCT"},
        {DESTRUCT, "DESTRUCT"},
        {MOVE_CONSTRUCT, "MOVE_CONSTRUCT"},
        {MOVE_ASSIGN, "MOVE_ASSIGN"},
        {COPY_CONSTRUCT, "COPY_CONSTRUCT"},
        {COPY_ASSIGN, "COPY_ASSIGN"},
};

static const std::map<std::string, CoyMoveType> construct_type_string_map_inverse{
        {"CONSTRUCT", CONSTRUCT},
        {"DESTRUCT", DESTRUCT},
        {"MOVE_CONSTRUCT", MOVE_CONSTRUCT},
        {"MOVE_ASSIGN", MOVE_ASSIGN},
        {"COPY_CONSTRUCT", COPY_CONSTRUCT},
        {"COPY_ASSIGN", COPY_ASSIGN},
};
struct ToTestMovable {
    int anyval;

    ToTestMovable() {
        std::cout << construct_type_string_map.find(CONSTRUCT)->second << std::endl;
    }
    ToTestMovable(const ToTestMovable &other) {
        std::cout << construct_type_string_map.find(COPY_CONSTRUCT)->second << std::endl;
    }
    ToTestMovable(ToTestMovable &&other) {
        std::cout << construct_type_string_map.find(MOVE_CONSTRUCT)->second << std::endl;
    }
    ToTestMovable &operator=(const ToTestMovable &other) {
        std::cout << construct_type_string_map.find(COPY_ASSIGN)->second << std::endl;
        return *this;
    }
    ToTestMovable &operator=(ToTestMovable &&other) {
        std::cout << construct_type_string_map.find(MOVE_ASSIGN)->second << std::endl;
        return *this;
    }
    ~ToTestMovable() {
        std::cout << construct_type_string_map.find(DESTRUCT)->second << std::endl;
    }
};

void testingFuncRef(ToTestMovable &) {
    std::cout << "Ref" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void testingFuncCopy(ToTestMovable) {
    std::cout << "Copy" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void testingFuncMove(ToTestMovable &&o) {
    ToTestMovable temp(std::move(o));
    static_cast<void>(temp);
    std::cout << "Move" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// if output log s1 shows that program copied more than s2, return positive value
int CheckNoMoreCopy(std::string &s1, std::string &s2) {
    auto v1 = MiraiCP::Tools::split(s1, "\n");
    auto v2 = MiraiCP::Tools::split(s2, "\n");
    int construct = 0;
    int copy = 0;
    for (auto &&v: v1) {
        auto target = construct_type_string_map_inverse.find(v);
        if(target == construct_type_string_map_inverse.end()) {
            continue;
        }
        switch (target->second) {
            case CONSTRUCT:
                construct++;
                break;
            case COPY_CONSTRUCT:
            case COPY_ASSIGN:
                copy++;
                break;
            default:
                break;
        }
    }
    for (auto &&v: v2) {
        auto target = construct_type_string_map_inverse.find(v);
        if(target == construct_type_string_map_inverse.end()) {
            continue;
        }
        switch (target->second) {
            case CONSTRUCT:
                construct--;
                break;
            case COPY_CONSTRUCT:
            case COPY_ASSIGN:
                copy--;
                break;
            default:
                break;
        }
    }
    return construct + copy;
}

TEST(MiraiCPNewThreadTest, test) {
    auto mock = LoaderApiMock();
    static std::string buf;
    static std::mutex bufmtx;

    {
        std::lock_guard<std::mutex> lk(bufmtx);
        buf.clear();
    }

    MiraiCP::Logger::logger.registerHandle([](const std::string &str, int level) {
        std::lock_guard<std::mutex> lck(bufmtx);
        if (level == 0) buf += str;
    });

    ToTestMovable t;

    {
        std::lock_guard<std::mutex> lk(bufmtx);
        buf.clear();
    }

    MiraiCP::MiraiCPNewThread th0(&testingFuncCopy, t);
    th0.join();
    std::string result;
    {
        std::lock_guard<std::mutex> lk(bufmtx);
        std::swap(result, buf);
    }

    std::thread stdth0(&testingFuncCopy, t);
    stdth0.join();
    std::string stdresult;
    {
        std::lock_guard<std::mutex> lk(bufmtx);
        std::swap(stdresult, buf);
    }

    ASSERT_EQ(0, CheckNoMoreCopy(result, stdresult));

    {
        std::lock_guard<std::mutex> lk(bufmtx);
        buf.clear();
    }
    result.clear();
    stdresult.clear();

    t = ToTestMovable();
    MiraiCP::MiraiCPNewThread th1(&testingFuncMove, std::move(t));
    th1.join();
    {
        std::lock_guard<std::mutex> lk(bufmtx);
        std::swap(result, buf);
    }

    t = ToTestMovable();
    std::thread stdth1(&testingFuncMove, std::move(t));
    stdth1.join();
    {
        std::lock_guard<std::mutex> lk(bufmtx);
        std::swap(stdresult, buf);
    }

    ASSERT_EQ(0, CheckNoMoreCopy(result, stdresult)); // fail, 0 != 1
    ASSERT_FALSE(result.empty());
}
