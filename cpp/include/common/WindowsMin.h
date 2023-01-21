//
// Created by 60168 on 2023-1-21.
//

#ifndef MIRAICP_PRO_WINDOWSMIN_H
#define MIRAICP_PRO_WINDOWSMIN_H

// 定义下面这些宏以去掉Windows中那些没用的组件

#ifndef FULL_WINTARD
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>


#endif //MIRAICP_PRO_WINDOWSMIN_H
