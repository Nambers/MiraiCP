# MiraiCP异常库

## 总异常
通常用于检查有没有异常出现
`class MiraiCPException :public exception `
## 初始化异常
通常是因为插件或loader的版本和SDK的不匹配导致的接口对接不上的问题
`class InitException :public MiraiCPException`
## 文件读取异常
通常为发送图片时找不到地址
`class IOException :public MiraiCPException`
## 内部异常
MiraiCP内部出现的预料之外的异常，请汇报给MiraiCP项目
`class APIException :public MiraiCPException`
## 机器人操作异常
目前为无权限
`class BotException : public MiraiCPException`
## 禁言异常
通常为禁言时间不在0s~30d内
`class MuteException :public MiraiCPException`
## 获取群成员错误
按照type来看，1是找不到群，2是找不到群成员
`class MemberException :public MiraiCPException`
## 获取好友错误
通常为找不到好友在bot好友列表
`class FriendException :public MiraiCPException`
## 获取群错误
通常为找不到群
`class GroupException :public MiraiCPException`