/*
 * Copyright (c) 2020 - 2021. Eritque arcus and contributors.
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
import java.util.*

plugins {
    kotlin("jvm") version Version.kotlin
    kotlin("plugin.serialization") version Version.kotlin apply false
    id("com.github.johnrengelman.shadow") version Version.shadowJar apply false
    id("net.mamoe.mirai-console") version Version.mirai apply false
    id("net.mamoe.maven-central-publish") version Version.mavenCenterPublish apply false
}
allprojects {
    repositories {
        mavenLocal()
        mavenCentral()
        maven("https://maven.aliyun.com/repository/public") // 阿里云国内代理仓库
        gradlePluginPortal()
        google()
    }
}
subprojects {
    apply(plugin = "org.jetbrains.kotlin.jvm")
    dependencies {
        implementation("com.google.code.gson", "gson", Version.gson)
        implementation("org.json", "json", Version.json)
        compileOnly("net.mamoe", "mirai-core-api", Version.mirai)
        implementation("org.jetbrains.kotlinx", "kotlinx-coroutines-core", Version.`kotlinx-coroutines-core`)
    }
}
group = "tech.eritquearcus"
version = Version.miraiCP

tasks {
    register("fillingConstants") {
        group = "build"
        doFirst {
            File(projectDir, "shared/src/main/kotlin/BuiltInConstants.kt")
                .let {
                    println("write to ${it.absolutePath}")
                    it.writeText(
                        it.readText()
                            .replace(
                                Regex("""val date = "(.*)""""),
                                "val date = \"${Calendar.getInstance().time}\""
                            )
                            .replace(
                                Regex("""val version = "(.*)""""),
                                "val version = \"${Version.miraiCP}\""
                            )
                    )
                }
        }
    }
}