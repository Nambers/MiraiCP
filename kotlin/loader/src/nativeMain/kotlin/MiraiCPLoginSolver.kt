/*
 * Copyright (c) 2020 - 2023. Eritque arcus and contributors.
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
import net.mamoe.mirai.network.UnsupportedSmsLoginException
import net.mamoe.mirai.utils.DeviceVerificationRequests
import net.mamoe.mirai.utils.DeviceVerificationResult
import net.mamoe.mirai.utils.LoginSolver
import tech.eritquearcus.miraicp.loader.console.Console
import tech.eritquearcus.miraicp.uilts.MiraiCPFiles

class MiraiCPLoginSolver : LoginSolver() {
    override val isSliderCaptchaSupported: Boolean
        get() = true

    override suspend fun onSolveDeviceVerification(
        bot: Bot,
        requests: DeviceVerificationRequests
    ): DeviceVerificationResult {
        bot.logger.info(
            "需要设备验证:\n" +
                    "(1) FallBack 方式 [" + if (requests.fallback != null) "可用" else "不可用" + "]\n" +
                    "(2) 短信验证码 [" + if (requests.sms != null) "可用" else "不可用" + "]\n" +
                    "优先短信验证码:" + requests.preferSms
        )
        val default = if (requests.fallback != null) 1 else 2
        val chooseText = Console.console.prompt("输入验证方式(1 或 2, 输入非法为 $default):")!!.trim()
        val c = when (chooseText.toIntOrNull() ?: default) {
            1 -> if (requests.fallback == null) default else chooseText.toIntOrNull() ?: default
            2 -> if (requests.sms == null) default else chooseText.toIntOrNull() ?: default
            else -> default
        }
        return when (c) {
            1 -> {
                Console.console.println("url" + requests.fallback!!.url)
                bot.logger.info("正在处理fallBack验证")
                Console.console.prompt("打开上面的url处理后输入任意值继续")
                requests.fallback!!.solved()
            }

            2 -> {
                bot.logger.info("需要短信验证码, 输入: resendMySMS 重新发送验证码")
                var code = "resendMySMS"
                while (code == "resendMySMS") {
                    try {
                        requests.sms!!.requestSms()
                    } catch (e: RetryLaterException) {
                        bot.logger.error("请求过于频繁, 请稍后再试")
                        bot.logger.error(e)
                    }
                    bot.logger.info("已发送短信验证码到:" + (requests.sms!!.phoneNumber ?: "未知"))
                    code = Console.console.prompt("短信验证码(输入: resendMySMS):")!!.trim()
                }
                requests.sms!!.solved(code)
            }

            else -> throw UnsupportedSmsLoginException("未知错误, 应该报告到 MiraiCP")
        }
    }

    override suspend fun onSolvePicCaptcha(bot: Bot, data: ByteArray): String {
        MiraiCPFiles.create("verify.jpg").writeByteArray(data)
        bot.logger.info("需要图形验证码")
        return Console.console.prompt("图形验证码(图片在verify.jpg)")!!.trim()
    }

    override suspend fun onSolveSliderCaptcha(bot: Bot, url: String): String {
        Console.console.println("Url: $url")
        bot.logger.info("需要滑块验证码")
        return Console.console.prompt("滑块验证码(ticket)")!!.trim()
    }

    @Deprecated(
        "Please use onSolveDeviceVerification instead",
        replaceWith = ReplaceWith("onSolveDeviceVerification(bot, url, null)"),
        level = DeprecationLevel.WARNING
    )
    override suspend fun onSolveUnsafeDeviceLoginVerify(bot: Bot, url: String): String? {
        throw UnsupportedSmsLoginException("This login session requires device verification, but current LoginSolver($this) does not support it. Please override `LoginSolver.onSolveDeviceVerification`.")
    }
}