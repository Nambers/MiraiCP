# Change Log

## v2.7-RC-dev

+ 优化cmake组合方式(所以推荐一并更新全部的Cmake文件)
+ loader增加jline3命令输入框
 - 支持运行时登录
 - 支持调用onDisable退出
+ loader支持md5作为密码(32位md5 大小写都可以)
+ 更新mirai版本到v2.7-RC-dev1 修复管理员识别问题
+ 支持群公告操作
+ 支持上下文
+ 添加ServiceMessage支持(UrlSharer)
+ 修复一些MiraiCode的小问题

> 该版本为预览版的开发版(，包含一些可能出现问题的特性

## v2.7-Beta

+ 重构框架cmake组成方式
 - 代码集中到一个hpp文件，更新更为方便
 - 增加namespace隔离可能冲突的语义
 - cmake包含源码子项目更容易导入sdk
 - 更多插件信息，更多logger

> 该版本为预览版，包含可能出现问题的特性

## v-NONE