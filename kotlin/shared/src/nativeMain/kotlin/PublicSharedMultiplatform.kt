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

import kotlinx.coroutines.*
import kotlin.coroutines.EmptyCoroutineContext

private class Timer(val time: Long, val msg: String) {
    private var timer: Job = CoroutineScope(EmptyCoroutineContext).launch {
        for (i in 0..time / 10) {
            if (!isActive) return@launch
            delay(10)
        }
        UlitsMultiPlatform.event(CPPEvent.TimeOutEvent(msg))
    }

    init {
//        ensureNeverFrozen()
    }

    val isCancelled
        get() = timer.isCancelled

    fun cancel() {
        timer.cancel()
    }
}

actual object PublicSharedMultiplatform {
    actual fun onDisable() {
//        event()(json.encodeToString(CPPEvent.LibLoaderEvent("OnDisable")))
    }

    actual fun scheduling(time: Long, msg: String) {
        Timer(time, msg)
    }
}