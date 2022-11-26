/*
 * Copyright (c) 2020 - 2022. Eritque arcus and contributors.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version(in your opinion).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

package tech.eritquearcus.miraicp.shared

interface CommandHandler {
    fun register(c:Command): String
}

data class Command(
    // 主要指令名
    val primaryName: String,
    // 次要指令名
    val secondName: List<String>,
    // 描述
    val description: String?,
    // 用法
    val usage: String?,
    // cpp端对应的监听id
    val bindId: Int,
    // kt端插件编号
    val pluginId: Int,
    // 覆盖已有指令
    val override: Boolean,
    // 前缀`/`可选
    val preFixOption: Boolean
)

data class CommandWrap(
    val botId: Long,
    val eventData: Command2C,
) {
    data class Command2C(
        val contact: Config.Contact?,
        val botid: Long,
        val message: String,
        val bindId: Int,
        val type: Int = 17,
    )
}