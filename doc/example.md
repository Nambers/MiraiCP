# 欢迎来到MiraiCP案例库

## 1. 取当前消息里全部照片的下载链接
```C++
procession->registerEvent([](PrivateMessageEvent param)->void {
	/*示例: 取消息里全部图片的下载地址*/
	vector<string> temp = Image::GetImgIdFromMiraiCode(param.message);
	for (int i = 0; i < temp.size(); i++) {
		logger->Info(temp[i]);
		logger->Info("图片下载地址:" + Image(param.env, temp[i]).queryURL());
	}
	});
```