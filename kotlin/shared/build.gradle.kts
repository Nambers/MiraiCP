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
import Version.`kotlinx-coroutines-core`
import Version.`ktor-ulits`
import Version.`mirai-core-api`

plugins {
    kotlin("multiplatform")
}
group = "tech.eritquearcus"
version = Version.miraiCP
kotlin {
    jvm {
        compilations.all {
            kotlinOptions.jvmTarget = "1.8"
        }
        withJava()
        testRuns["test"].executionTask.configure {
            useJUnitPlatform()
        }
    }
    val hostOs = System.getProperty("os.name")
    when {
        hostOs == "Mac OS X" -> macosX64("native")
        hostOs == "Linux" -> linuxX64("native")
        hostOs.startsWith("Windows") -> mingwX64("native")
        else -> throw GradleException("Host OS is not supported in Kotlin/Native.")
    }.compilations.all {
        cinterops {
            this.maybeCreate("localLibcurl")
            this.maybeCreate("localOpenssl")
            val localMiraiCP by creating {
                headers(project.file("src/nativeInterop/cinterop/MiraiCP.h"))
                packageName("tech.eritquearcus.MiraiCP")
            }
        }
    }
    sourceSets {
        val commonMain by getting {
            dependencies {
                implementation(project(":utils"))
                implementation(`mirai-core-api`)
                implementation(`kotlinx-coroutines-core`)
                implementation(`ktor-ulits`)
            }
            apply(plugin = "org.jetbrains.kotlin.plugin.serialization")
        }

        val jvmMain by getting
        val nativeMain by getting
    }
}