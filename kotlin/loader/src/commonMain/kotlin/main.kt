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
package tech.eritquearcus.miraicp.loader

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.Job
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.newSingleThreadContext
import tech.eritquearcus.miraicp.shared.CPPConfig

object KotlinMainData {
    val job = Job()

    @OptIn(ExperimentalCoroutinesApi::class)
    private val threadPool = newSingleThreadContext("console")

    @OptIn(ExperimentalCoroutinesApi::class)
    val coroutineScope = CoroutineScope(threadPool + job)
    lateinit var loginAccount: List<CPPConfig.LoaderConfig.Account>
    val aliveChan: Channel<Unit> = Channel(0)
}


expect object KotlinMain {
    var exit: () -> Unit
    fun main(j: String, path: String)
}

expect object KotlinMainEntry {
    fun main(args: Array<String>)
}