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
    val isMingwX64 = hostOs.startsWith("Windows")
    val nativeTarget = when {
        hostOs == "Mac OS X" -> macosX64("unix")
        hostOs == "Linux" -> linuxX64("unix")
        isMingwX64 -> mingwX64("win")
        else -> throw GradleException("Host OS is not supported in Kotlin/Native.")
    }
    sourceSets {
        val commonMain by getting {
            dependencies {
                implementation("net.mamoe:mirai-core-api:${Version.mirai}")
                implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:${Version.`kotlinx-coroutines-core`}")
//                classpath("org.jetbrains.kotlin:kotlinx-serialization-json:${Version.`kotlinx-serialization-json`}")
                implementation("io.ktor:ktor-utils:2.0.1")
            }
            apply(plugin = "org.jetbrains.kotlin.plugin.serialization")
        }
        val jvmMain by getting
        val unixMain = maybeCreate("unixMain").apply {
            this.dependsOn(commonMain)
        }
        val winMain = maybeCreate("winMain").apply {
            this.dependsOn(commonMain)
        }
    }
}