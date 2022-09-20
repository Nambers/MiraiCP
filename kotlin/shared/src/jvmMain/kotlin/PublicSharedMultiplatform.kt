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

import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import net.mamoe.mirai.contact.Contact
import net.mamoe.mirai.message.data.Image.Key.queryUrl
import net.mamoe.mirai.utils.ExternalResource.Companion.uploadAsImage
import net.mamoe.mirai.utils.OverFileSizeMaxException
import tech.eritquearcus.miraicp.shared.CPPLibMultiplatform.PluginDisable
import tech.eritquearcus.miraicp.shared.PublicSharedData.logger
import java.io.File

actual object PublicSharedMultiplatform {
    actual suspend fun uploadImgAndId(
        file: String, temp: Contact, err1: String, err2: String
    ): String = try {
        val img = File(file).uploadAsImage(temp)
        Json.encodeToString(
            Config.ImgInfo(
                img.size,
                img.width,
                img.height,
                Json.encodeToString(img.md5),
                img.queryUrl(),
                img.imageId,
                img.imageType.ordinal
            )
        )
    } catch (e: OverFileSizeMaxException) {
        logger.error("图片文件过大超过30MB,位置:K-uploadImgGroup(),文件名:$file")
        err1
    } catch (e: NullPointerException) {
        logger.error("上传图片文件名异常,位置:K-uploadImgGroup(),文件名:$file")
        err2
    }

    actual fun onDisable() {
        PluginDisable()
    }

}