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
package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Job
import tech.eritquearcus.miraicp.shared.CPPConfig
import kotlin.native.concurrent.ThreadLocal

@ThreadLocal
object KotlinMainData {
    val job = Job()
    val coroutineScope = CoroutineScope(job)
    lateinit var loginAccount: List<CPPConfig.LoaderConfig.Account>
    var alive = true
}

expect object KotlinMain {
    var exit: () -> Unit
    fun main(j: String, path: String)
}

expect object KotlinMainEntry {
    fun main(args: Array<String>)
}