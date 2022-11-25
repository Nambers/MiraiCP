//
// Created by antares on 11/25/22.
//

#include <gtest/gtest.h>
#include "SingleMessage.h"
using namespace MiraiCP;


TEST(ToolsTest, PlainText) {
    auto textNum = PlainText(123);
    auto textStr = PlainText("123");
    auto textStr2 = PlainText(std::string("123"));
    ASSERT_EQ(textNum.content, textStr.content);
    ASSERT_EQ(textNum.content, textStr2.content);
}
