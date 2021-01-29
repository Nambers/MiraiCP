# Mirai支持的qq表情及编号
本文档编写于2021/1/29,mirai版本2.0.0

使用以下代码自动生成:
```c++
for (int a = 0; a < 7; a++) {
			string text = "";
			for (int b = 1; b <= 50; b++) {
				text.append("[mirai:face:"+to_string(b + 50 * a)+"]");
			}
			param.sender.SendMsg(text);
		}
```

MiraiCode表情格式:`[mirai:face:表情id]`

以下为具体对应:

## 1~50
![1~50](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/1-50.jpg?raw=true)

## 50~100
![50~100](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/50-100.png?raw=true)

## 100~150
![100~150](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/100-150.png?raw=true)

## 150~200
![150~200](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/150-200.png?raw=true)

## 200~250
![200~250](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/200-250.png?raw=true)

## 250~300
![250~300](https://raw.githubusercontent.com/Nambers/MiraiCP/master/doc/face_pic/250-300.png?raw=true)