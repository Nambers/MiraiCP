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
import java.util.*

plugins {
    kotlin("jvm") version Version.kotlin apply false
    kotlin("plugin.serialization") version Version.kotlin apply false
    id("com.github.johnrengelman.shadow") version Version.shadowJar apply false
    id("net.mamoe.mirai-console") version Version.mirai apply false
//    id("me.him188.maven-central-publish") version Version.mavenCenterPublish apply false
}
allprojects {
    repositories {
        mavenLocal()
        mavenCentral()
        gradlePluginPortal()
        maven("https://maven.aliyun.com/repository/central") // 阿里云国内代理仓库
        // mirai snapshot
        maven("https://repo.mirai.mamoe.net/snapshots")
    }
    tasks {
        withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile>().configureEach {
            kotlinOptions {
//                freeCompilerArgs += "-Xjvm-default=enable"
                freeCompilerArgs += "-opt-in=kotlin.RequiresOptIn"
                kotlinOptions.jvmTarget = JavaVersion.VERSION_11.toString()
            }
        }
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
                            .replace(
                                Regex("""val miraiVersion = "(.*)""""),
                                "val miraiVersion = \"${Version.mirai}\""
                            )
                    )
                }
        }
    }
}