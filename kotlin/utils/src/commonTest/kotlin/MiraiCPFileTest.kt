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
import kotlin.random.Random
import kotlin.test.*

class MiraiCPFileTest {
    private val filesForTests = "./src/commonTest/kotlin/FilesForTest"
    private fun <T, K> List<T>.testExpects(lis: List<K>, func: (K) -> T) {
        assertEquals(this.size, lis.size)
        for (a in this.indices) {
            assertEquals(this[a], func(lis[a]), "No.$a " + this[a].toString() + " != " + lis[a])
        }
    }

    @Test
    fun isExist() {
        listOf(true, true, true, true, false)
            .testExpects(
                listOf(
                    "$filesForTests/isFile",
                    "$filesForTests/isFile.ext",
                    "$filesForTests/dir.dir",
                    "$filesForTests/",
                    "$filesForTests/nullDir"
                )
            ) {
                MiraiCPFiles.create(it).exists()
            }
    }

    @Test
    fun isFile() {
        listOf(true, true, false, false, false, false)
            .testExpects(
                listOf(
                    "$filesForTests/isFile",
                    "$filesForTests/isFile.ext",
                    "$filesForTests/dir.dir",
                    "$filesForTests/",
                    "$filesForTests/null.ext",
                    "$filesForTests/null",
                )
            ) {
                MiraiCPFiles.create(it).isFile
            }
    }

    @Test
    fun canRead() {
        listOf(true, true, true, false)
            .testExpects(
                listOf(
                    "$filesForTests/isFile.ext",
                    "$filesForTests/dir.dir",
                    "$filesForTests/",
                    "$filesForTests/nullFile",
                )
            ) {
                MiraiCPFiles.create(it).canRead()
            }
    }

    @Test
    fun read() {
        listOf("a中文(!“\"；‘、\\\\/\\\"\"")
            .testExpects(
                listOf(
                    "$filesForTests/isFile.ext"
                )
            ) {
                MiraiCPFiles.create(it).readText()
            }
        assertFails {
            MiraiCPFiles.create("$filesForTests/dir.dir").readText()
        }
    }

    @Test
    fun writeAndDelete() {
        listOf(true)
            .testExpects(
                listOf(
                    "$filesForTests/writeFile1"
                )
            ) {
                val tmp = MiraiCPFiles.create(it)
                val text = Random.nextBytes(10).toString()
                tmp.writeText(text)
                // exists, isFile and read should be tested before this test
                tmp.exists() && tmp.isFile && tmp.readText() == text
            }
        // assume delete work well
        val tmp = MiraiCPFiles.create("$filesForTests/writeFile1")
        tmp.delete()
        assertEquals(false, tmp.exists() || tmp.canRead() || tmp.isFile)
    }

    @Test
    fun mkdirAndDeleteRecursively() {
        val dir = MiraiCPFiles.create("$filesForTests/dir4mk")
        assertFalse("1") { dir.exists() || dir.isFile }
        dir.mkdir()
        assertTrue("2") { dir.exists() && !dir.isFile && dir.canRead() }
        val dir2 = MiraiCPFiles.create("${dir.absolutePath}/dir4mk2")
        assertFalse("3") { dir2.exists() || dir2.isFile }
        dir2.mkdir()
        assertTrue("4") { dir2.exists() && !dir2.isFile && dir2.canRead() }
        val file = MiraiCPFiles.create("${dir2.absolutePath}/deleteFile.ext")
        assertFalse("5") { file.exists() || file.isFile }
        file.writeText("")
        assertTrue("6") { file.exists() && file.isFile && file.canRead() }
        assertEquals(true, dir.deleteRecursively(), "7")
        assertFalse("8") { file.exists() || file.isFile || file.canRead() }
        assertFalse("9") { dir2.exists() || dir2.isFile || dir2.canRead() }
        assertFalse("X") { dir.exists() || dir.isFile || dir.canRead() }
    }

    @Test
    fun pathWithoutName() {
        // assume absolute path work as expect
        val tmpAbs = MiraiCPFiles.create(filesForTests).absolutePath
        listOf(tmpAbs, tmpAbs)
            .testExpects(listOf("$filesForTests/isFile", "$filesForTests/dir.dir")) {
                MiraiCPFiles.create(it).parentPath
            }
    }

    @Test
    fun testByteArray() {
        val file1 = MiraiCPFiles.create("$filesForTests/test.png")
        val file2 = MiraiCPFiles.create("$filesForTests/test2.png")
        if (file2.exists()) {
            file2.delete()
        }
        file2.writeByteArray(file1.readByteArray())
        assertTrue(file1.readByteArray().contentEquals(file2.readByteArray()))
    }
}