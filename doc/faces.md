# Mirai支持的qq表情及编号
本文档编写于2021/2/8,mirai版本2.3.2(如果图片加载不出来请到doc/face_pic下查看,是raw.github被墙了)

使用以下代码自动生成:
```kotlin
		for (a in 0..6) {
                var text = ""
                for (c in 1..50 step 4) {
                    var b = c
                    text +=  "${b + 50 * a} | [mirai:face:${b + 50 * a}] "
                    b++
                    text +=  "${b + 50 * a} | [mirai:face:${b + 50 * a}] "
                    b++
                    text +=  "${b + 50 * a} | [mirai:face:${b + 50 * a}] "
                    b++
                    text +=  "${b + 50 * a} | [mirai:face:${b + 50 * a}]\n"
                }
                sender.sendMessage(MiraiCode.deserializeMiraiCode(text))
            }
```

MiraiCode表情格式:`[mirai:face:表情id]`

以下为具体对应:

## 1~50
![1~50](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/1-50.jpg?raw=true)

## 50~100
![50~100](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/50-100.jpg?raw=true)

## 100~150
![100~150](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/100-150.jpg?raw=true)

## 150~200
![150~200](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/150-200.jpg?raw=true)

## 200~250
![200~250](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/200-250.jpg?raw=true)

## 250~300
![250~300](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/250-300.jpg?raw=true)

## 关于emoji
如需发送emoji，请查看[这里](https://apps.timwhitlock.info/emoji/tables/unicode)，然后把emoji黏贴到字符串里或直接用utf8编码

在miraicp的C++部分中:
```C++
	procession->registerEvent<GroupMessageEvent>([=](GroupMessageEvent e) {
        e.group.SendMsg("☺");
	e.group.SendMsg("\xF0\x9F\x98\x81");
	});
```
