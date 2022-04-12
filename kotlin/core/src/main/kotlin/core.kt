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

package tech.eritquearcus.miraicp.core

import net.mamoe.mirai.Mirai
import net.mamoe.mirai.utils.MiraiLogger
import tech.eritquearcus.miraicp.loader.KotlinMain
import tech.eritquearcus.miraicp.loader.console.Console
import tech.eritquearcus.miraicp.loader.console.LoaderCommandHandlerImpl
import tech.eritquearcus.miraicp.loader.login
import tech.eritquearcus.miraicp.shared.*
import tech.eritquearcus.miraicp.shared.PublicShared.gson
import tech.eritquearcus.miraicp.shared.PublicShared.now_tag
import java.io.File

object Core {
    @JvmStatic
    fun main(args: Array<String>) {
        println("请勿直接打开miraicp-core组件, 请使用loader组件")
    }
    @JvmStatic
    fun login(source: String){
        gson.fromJson(source, CPPConfig.loaderConfig.Account::class.java).login()
    }
    @JvmStatic
    fun load() {
        println("load")
        KotlinMain.job.start()
        Console
        val logger = MiraiLogger.Factory.create(this::class, "MiraiCP")
        PublicShared.init(logger)
        PublicShared.cachePath = File("cache")
        if (PublicShared.cachePath.exists()) PublicShared.cachePath.deleteRecursively()
        PublicShared.cachePath.mkdir()
        logger.info("⭐MiraiCP启动中⭐")
        logger.info("⭐github存储库:https://github.com/Nambers/MiraiCP")
        logger.info("⭐当前MiraiCP版本: $now_tag, 构建时间: ${BuiltInConstants.date}, mirai版本: ${BuiltInConstants.miraiVersion}")
        PublicShared.commandReg = LoaderCommandHandlerImpl()
        CPPLib(null, null){
            enroll()
        }
        logger.info("⭐已成功加载MiraiCP⭐")
        Console.listen()
        while (KotlinMain.alive) {
        }
    }
    private external fun enroll(): Void
}