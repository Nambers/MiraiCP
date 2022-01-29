# loader 和 plugin 配置文件详解

## 1. loader 端
loader 的配置文件位于`miraicp-loader-version.jar`同目录或传入的参数目录, 应遵守json格式并最少包含以下元素
```json
{
  "accounts": [{
	"id": ,
	"passwords": ""
  }],
  "cppPaths":[
	{
	  "path":"\\cmake-build-debug\\MiraiCP.dll"
	}
  ]
}
```
详细模板代码见[Config.kt#L146](https://github.com/Nambers/MiraiCP/blob/master/kotlin/shared/src/main/kotlin/Config.kt#L146)
### 1.1 accounts
`accounts` 元素类型为列表，里面包含你想要登录的单个或多个账号, 如
```json
{
  "accounts": [{
	"id": 111,
	"passwords": ""
  },
  {
	"id": 222,
	"passwords": ""
  }],
  "cppPaths":[
	{
	  "path":"\\cmake-build-debug\\MiraiCP.dll"
	}
  ]
}
```
而每个账号应包含以下配置
#### account
最小配置:
```json
{
	"id": <qq id>,
	"passwords": "<明文密码>"
  }
```
或:
```json
{
	"id": <qq id>,
	"passwords": "<32位md5 密码 大小写都可以>",
  	"md5": true
  }
```
除了基本的 `id` 和 `passwords` 之外，`autoLogin` 配置项也需特别注意
如果 `autoLogin` 为 true, loader 端就会在加载时自动登录, 否则需要手动在loader端控制台输入 `login <qqid>` 来登录, 然后全部 `login` 登录的账号都需要实现在配置文件里写明
如:
```json
{
	"id":,
	"passwords": "",
  	"autoLogin": true
  }
```
模板代码见[config.kt#L151](https://github.com/Nambers/MiraiCP/blob/master/kotlin/shared/src/main/kotlin/Config.kt#L151)
就会自动在loader端启动的时候登录
##### 进阶账号配置
所有的可选账号配置:
- `heatBeat` 账号心跳策略, 可选: STAT_HB/REGISTER/NONE (默认为state_hb), 详细查看[mirai文档](https://github.com/mamoe/mirai/blob/dev/docs/Bots.md#%E5%88%87%E6%8D%A2%E5%BF%83%E8%B7%B3%E7%AD%96%E7%95%A5)
- `protocol` 登录协议选择, 可选: pad/phone/watch/ipad (默认为phone), 详细查看[mirai文档](https://github.com/mamoe/mirai/blob/dev/docs/Bots.md#%E5%88%87%E6%8D%A2%E7%99%BB%E5%BD%95%E5%8D%8F%E8%AE%AE)

### 1.2 cppPaths
`cppPaths` 该元素类型为列表, 里面包含你全部要加载的插件(会自动在启动时加载全部), 如:
```json
{
  "accounts": [{
	"id": ,
	"passwords": ""
  }],
  "cppPaths":[
	{
	  "path":"<dll1 路径>"
	},
  {
    "path": "<dll2 路径>"
  }
  ]
}
```
#### cppPath
最小配置:
```json
{
  "path": "<dll 路径>"
}
```
除了最基本的路径, 也可以设置依赖dll的路径(在报错 `cannot find dependences` 的时候可以试一下把依赖加进来):
```json
{
  "path": "<dll 路径>",
  "dependencies": [
    "<依赖的dll1>",
    "<依赖的dll2>"
  ]
}
```
模板代码见[Config.kt#L132](https://github.com/Nambers/MiraiCP/blob/master/kotlin/shared/src/main/kotlin/Config.kt#L132)
### 1.3 进阶配置
配置文件中还可以加入 `config` 配置进阶配置:
```json
{
  "config":{
    "threadNum": 10
  },
  "accounts": [{
	"id": ,
	"passwords": ""
  }],
  "cppPaths":[
	{
	  "path":"\\cmake-build-debug\\MiraiCP.dll"
	}
  ]
}
```
可配置项:
- `threadNum` 配置bot用线程池, 默认为5, 作用代码于[Ulits.kt#L28](https://github.com/Nambers/MiraiCP/blob/master/kotlin/shared/src/main/kotlin/Ulits.kt#L28)
模板代码见[Config.kt#L137](https://github.com/Nambers/MiraiCP/blob/master/kotlin/shared/src/main/kotlin/Config.kt#L137)
## 2. plugin 端
`plugin` 端配置文件位于 `/data/miraiCP/config.json` 应遵守json格式并至少拥有以下元素
```json
{
	"pluginConfig":[{
		"path":"\\cmake-build-debug\\MiraiCP.dll"
	}]
}
```
模板代码见[Config#L141](https://github.com/Nambers/MiraiCP/blob/master/kotlin/shared/src/main/kotlin/Config.kt#L141)
### pluginConfig
该元素类型为列表, 包含要加载的全部dll的路径, 与loader端的配置文件相同, 也可以加载多个dll文件和加载依赖的dll, 如
```json
{
	"pluginConfig":[{
		"path":"<dll路径1>",
    "dependences": [
    "<依赖的dll1>",
    "<依赖的dll2>",
    ]
	},
  {
		"path":"<dll路径2>",
    "dependences": [
    "<依赖的dll1>",
    "<依赖的dll2>",
    ]
	}]
}
```
### 进阶配置
与loader端相同, 可以加入config块配置进阶选项
```json
{
  "config": {},
	"pluginConfig":[{
		"path":"\\cmake-build-debug\\MiraiCP.dll"
	}]
}
```

[返回根目录](.)
