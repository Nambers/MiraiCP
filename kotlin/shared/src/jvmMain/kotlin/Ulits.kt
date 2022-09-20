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

import net.mamoe.mirai.Bot
import net.mamoe.mirai.contact.*
import java.io.File
import java.util.concurrent.SynchronousQueue
import java.util.concurrent.ThreadPoolExecutor
import java.util.concurrent.TimeUnit

// 执行线程池, 以免阻塞mirai协程的线程池
private val queue = SynchronousQueue<Runnable>()
private val cc by lazy {
    ThreadPoolExecutor(
        0, PublicShared.maxThread, 60L, TimeUnit.SECONDS, queue
    )
}

actual fun event(obj: Any) {
    cc.submit {
        CPPLib.Event(if (obj is String) obj else PublicShared.gson.toJson(obj))
    }
}

actual fun getLibLoader(pathsInput: List<String>): String {
    val suffix =
        File.separator + if (System.getProperty("os.name").contains("Windows")) "libLoader.dll" else "libLoader.so"
    val paths = pathsInput.plus(System.getProperty("user.dir")).map { it + suffix }
    paths.forEach { path ->
        File(path).let {
            if (it.exists()) return it.absolutePath
        }
    }
    PublicShared.logger.error(
        "找不到 libLoader 组件于下列位置:\n" +
                paths.joinToString(prefix = "\t", postfix = "\n") +
                "请到 MiraiCP release 下载 libLoader 组件并放在以上目录其中一个的位置"
    )
    throw IllegalStateException("找不到 libLoader 组件")
}

actual fun Contact.toContact(): Config.Contact? = when (this) {
    is Group -> this.toContact()
    is Friend -> this.toContact()
    is Member -> this.toContact()
    else -> {
        PublicShared.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和黏贴本信息以修复此问题, 位置:Contact.toContact(), info:${this.javaClass.name}")
        null
    }
}

actual fun ContactOrBot.toContact(): Config.Contact? = when (this) {
    is Contact -> this.toContact()
    is Bot -> this.asFriend.toContact()
    else -> {
        PublicShared.logger.error("MiraiCP遇到意料之中的问题, 请到github仓库发送issue和本信息以修复此问题, 位置:ContactOrBot.toContact(), info:${this.javaClass.name}")
        null
    }
}