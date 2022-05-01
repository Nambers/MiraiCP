// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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
#include <MiraiCP.hpp>
using namespace MiraiCP;
using namespace std;

class Main : public CPPPlugin {
public:
    Main() : CPPPlugin(PluginConfig(
                     "id",
                     "test",
                     "v1.0",
                     "a")) {}
    void onEnable() override {
        Event::registerEvent<GroupMessageEvent>([](GroupMessageEvent e) {
            // 从文本构造
            //风格1，适合文字展示，不能交互,无大图
            //图标地址，应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
            string icon = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
            //小标题
            string titles = "{\"title\":\"hi\", \"value\":\"test3\"}";
            //下面的按钮，但是不能按
            string buttons = "{\"name\":\"Test4\",\"action\":\"plugin\",\"actionData\":\"https://baidu.com\"}";
            string x = "{\"app\":\"com.tencent.miniapp\",\"desc\":\"\",\"view\":\"notification\",\"ver\":\"0.0.0.1\",\"prompt\":\"[应用]\",\"appID\":\"\",\"sourceName\":\"\",\"actionData\":\"\",\"actionData_A\":\"\",\"sourceUrl\":\"\",\"meta\":{\"notification\":{\"appInfo\":"
                       "{\"appName\":\"Test1\",\"appType\":4,\"appid\":1109659848,"
                       "\"iconUrl\":\"" +
                       icon + "\"},"
                              "\"data\":[" +
                       titles + "],"
                                "\"title\":\"Test2\",\"button\":"
                                "[" +
                       buttons + "],"
                                 "\"emphasis_keyword\":\"\"}},\"text\":\"\",\"sourceAd\":\"\"}";
            e.group.sendMessage(LightApp(x));
            //风格2，不能交互，有预览图
            //icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
            string icon1 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
            //预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
            string preview1 = icon1;
            string a = "{\"config\":"
                       "{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
                       "\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"1.0.0.103\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
                       "\"meta\":{\"detail_1\":{\"appid\":\"1110081493\",\"preview\":\"" +
                       preview1 + "\",\"shareTemplateData\":{},"
                                  "\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test5\",\"title\":\"Test6\","
                                  "\"host\":{\"uin\":1000000,\"nick\":\"应用消息\"},"
                                  "\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" +
                       icon1 + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
            e.group.sendMessage(LightApp(a));
            //风格3，可以跳转到网址，有预览图
            //跳转链接
            string url = "https://www.baidu.com";
            //icon图标,应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
            string icon2 = "http://gchat.qpic.cn/gchatpic_new/1924306130/1044565129-2580521429-8ECE44863FC01DBD17FB8A177B355356/0";
            //预览图(大图),应该是要qq的服务器里有的图片，也就是说先上传(发送)图片然后取下载链接
            string preview = icon2;
            string b = "{\"config\":{\"height\":0,\"forward\":1,\"ctime\":0,\"width\":0,\"type\":\"normal\",\"token\":\"\",\"autoSize\":0},"
                       "\"prompt\":\"[QQ小程序]\",\"app\":\"com.tencent.miniapp_01\",\"ver\":\"0.0.0.1\",\"view\":\"view_8C8E89B49BE609866298ADDFF2DBABA4\","
                       "\"meta\":{\"detail_1\":{\"appid\":\"1109937557\",\"preview\":\"" +
                       preview + "\",\"shareTemplateData\":{},\"gamePointsUrl\":\"\",\"gamePoints\":\"\",\"url\":\"m.q.qq.com\",\"scene\":0,\"desc\":\"Test1\",\"title\":\"Test3\",\"host\":{\"uin\":0,\"nick\":\"\"},\"shareTemplateId\":\"8C8E89B49BE609866298ADDFF2DBABA4\",\"icon\":\"" + icon + "\",\"qqdocurl\":\"" + url + "\",\"showLittleTail\":\"\"}},\"desc\":\"\"}";
            e.group.sendMessage(LightApp(b));
        });
    }
};

void MiraiCP::enrollPlugin() {
    MiraiCP::enrollPlugin0(new Main());
}
