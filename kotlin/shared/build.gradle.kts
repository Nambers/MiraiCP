plugins{
    kotlin("plugin.serialization")
}
dependencies{
    implementation("net.mamoe:mirai-core-api:2.6.5") // mirai-core 的 API
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.5.0")
    implementation("org.jetbrains.kotlinx:kotlinx-serialization-json:1.2.1")
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