/*
 * Copyright (C) 2020-2021 Eritque arcus and contributors.
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
 */

plugins {
    kotlin("plugin.serialization")
}
dependencies {
    implementation(libs.mirai.core.api) // mirai-core 的 API
    implementation(libs.kotlinx.coroutines.core)
    implementation(libs.kotlinx.serialization.json)
}
//val generateJniHeaders: Task by tasks.creating {
//    group = "build"
//    dependsOn(tasks.getByName("compileKotlin"))
//
//    // For caching
//    inputs.dir("src/main/kotlin")
//    outputs.dir("src/main/generated/jni")
//
//    doLast {
//        val javaHome = org.gradle.internal.jvm.Jvm.current().javaHome
//        val javap = javaHome.resolve("bin").walk().firstOrNull { it.name.startsWith("javap") }?.absolutePath ?: error("javap not found")
//        val javac = javaHome.resolve("bin").walk().firstOrNull { it.name.startsWith("javac") }?.absolutePath ?: error("javac not found")
//        val buildDir = file("build/classes/kotlin/main")
//        val tmpDir = file("build/tmp/jvmJni").apply { mkdirs() }
//
//        val bodyExtractingRegex = """^.+\Rpublic \w* ?class ([^\s]+).*\{\R((?s:.+))\}\R$""".toRegex()
//        val nativeMethodExtractingRegex = """.*\bnative\b.*""".toRegex()
//
//        buildDir.walkTopDown()
//            .filter { "META" !in it.absolutePath }
//            .forEach { file ->
//                if (!file.isFile) return@forEach
//
//                val output = com.gradle.publish.plugin.dep.org.apache.commons.io.output.ByteArrayOutputStream().use {
//                    project.exec {
//                        commandLine(javap, "-private", "-cp", buildDir.absolutePath, file.absolutePath)
//                        standardOutput = it
//                    }.assertNormalExitValue()
//                    it.toString()
//                }
//
//                val (qualifiedName, methodInfo) = bodyExtractingRegex.find(output)?.destructured ?: return@forEach
//
//                val lastDot = qualifiedName.lastIndexOf('.')
//                val packageName = qualifiedName.substring(0, lastDot)
//                val className = qualifiedName.substring(lastDot+1, qualifiedName.length)
//
//                val nativeMethods =
//                    nativeMethodExtractingRegex.findAll(methodInfo).mapNotNull { it.groups }.flatMap { it.asSequence().mapNotNull { group -> group?.value } }.toList()
//                if (nativeMethods.isEmpty()) return@forEach
//
//                val source = buildString {
//                    append("package $packageName;")
//                    append("public class $className {")
//                    for (method in nativeMethods) {
//                        if ("()" in method) append(method)
//                        else {
//                            val updatedMethod = StringBuilder(method).apply {
//                                var count = 0
//                                var i = 0
//                                while (i < length) {
//                                    if (this[i] == ',' || this[i] == ')') insert(i, " arg${count++}".also { i += it.length + 1 })
//                                    else i++
//                                }
//                            }
//                            append(updatedMethod)
//                        }
//                    }
//                    append("}")
//                }
//                val outputFile = tmpDir.resolve(packageName.replace(".", "/")).apply { mkdirs() }.resolve("$className.java").apply { delete() }.apply { createNewFile() }
//                outputFile.writeText(source)
//
//                project.exec {
//                    commandLine(javac, "-h", "src/main/generated/jni", outputFile.absolutePath)
//                }.assertNormalExitValue()
//            }
//    }
//}