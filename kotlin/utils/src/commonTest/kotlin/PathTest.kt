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
import tech.eritquearcus.miraicp.uilts.MiraiCPFiles
import kotlin.test.Test
import kotlin.test.assertEquals

class PathTest {
    private fun <T, K> List<T>.testExpects(lis: List<K>, func: (K) -> T) {
        assertEquals(this.size, lis.size)
        for (a in this.indices) {
            assertEquals(this[a], func(lis[a]))
        }
    }

    @Test
    fun pathWithoutName() {
        listOf("C:\\", "C:\\")
            .testExpects(listOf("C:/a.a", "C:\\aa")) {
                MiraiCPFiles.create(it).pathWithOutName
            }
    }

    @Test
    fun isFile() {
        listOf(true, false)
            .testExpects(listOf("test.exe", "C:\\")) {
                MiraiCPFiles.create(it).isFile
            }
    }
}