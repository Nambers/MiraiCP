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

import net.mamoe.mirai.Bot
import net.mamoe.mirai.network.RetryLaterException
import net.mamoe.mirai.utils.DeviceVerificationRequests
import net.mamoe.mirai.utils.DeviceVerificationResult
import net.mamoe.mirai.utils.LoginSolver
import tech.eritquearcus.miraicp.loader.console.Console
import tech.eritquearcus.miraicp.uilts.MiraiCPFiles
import kotlin.system.exitProcess

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

class MiraiCPLoginSolver : LoginSolver() {
    override val isSliderCaptchaSupported: Boolean
        get() = true

    override suspend fun onSolveDeviceVerification(
        bot: Bot,
        requests: DeviceVerificationRequests
    ): DeviceVerificationResult {
        return if (requests.sms != null) {
            bot.logger.info("需要短信验证码, 输入: resendMySMS 重新发送验证码")
            var code = "resendMySMS"
            while (code == "resendMySMS") {
                try {
                    requests.sms!!.requestSms()
                } catch (e: RetryLaterException) {
                    bot.logger.error("请求过于频繁, 请稍后再试")
                    bot.logger.error(e)
                    exitProcess(-1)
                }
                code = Console.console.prompt("短信验证码:")!!
                bot.logger.info("已发送短信验证码")
            }
            requests.sms!!.solved(code)
        } else {
            Console.console.println("url" + requests.fallback!!.url)
            bot.logger.info("正在处理fallBack验证")
            requests.fallback!!.solved()
        }
    }

    override suspend fun onSolvePicCaptcha(bot: Bot, data: ByteArray): String {
        MiraiCPFiles.create("verify.jpg").writeByteArray(data)
        bot.logger.info("需要图形验证码")
        return Console.console.prompt("图形验证码(图片在verify.jpg):")!!
    }

    override suspend fun onSolveSliderCaptcha(bot: Bot, url: String): String {
        Console.console.println("Url: $url")
        bot.logger.info("需要滑块验证码")
        return Console.console.prompt("滑块验证码(ticket):")!!
    }

    @Deprecated(
        "Please use onSolveDeviceVerification instead",
        replaceWith = ReplaceWith("onSolveDeviceVerification(bot, url, null)"),
        level = DeprecationLevel.WARNING
    )
    override suspend fun onSolveUnsafeDeviceLoginVerify(bot: Bot, url: String): String {
        Console.console.println("Url: $url")
        bot.logger.info("需要UnsafeDeviceLoginVerify验证")
        return Console.console.prompt("Unsafe验证码:")!!
    }
}