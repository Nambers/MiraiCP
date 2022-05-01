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
#ifndef MIRAICP_HPP_H
#define MIRAICP_HPP_H


#include "Bot.h"
#include "CPPPlugin.h"
#include "Command.h"
#include "Config.h"
#include "Contact.h"
#include "Event.h"
#include "Exception.h"
#include "ForwardedMessage.h"
#include "Friend.h"
#include "Group.h"
#include "Logger.h"
#include "LowLevelAPI.h"
#include "Member.h"
#include "MessageChain.h"
#include "MessageSource.h"
#include "MiraiCode.h"
#include "MiraiDefs.h"
#include "PluginConfig.h"
#include "SingleMessage.h"
#include "ThreadManager.h"
#include "Tools.h"
#include "utils.h"
/*!
 * 所有加进doxygen doc的example
 * @example command.cpp
 * 监听指令
 * @example forwardedMessage.cpp
 * 构造转发信息
 * @example group.cpp
 * 群操作
 * @example lightApp.cpp
 * 构建并发送lightApp
 * @example logger.cpp
 * 自定义logger handle以及启用禁用logger
 * @example member.cpp
 * 群成员操作
 * @example message.cpp
 * 消息操作
 * @example nextMessage.cpp
 * 取下一条消息
 * @example schedule.cpp
 * 定时事件
 * @example thread.cpp
 * 多线程
 */
#endif //MIRAICP_HPP_H