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
    print(isMingwX64)
    val nativeTarget = when {
        hostOs == "Mac OS X" -> macosX64("native")
        hostOs == "Linux" -> linuxX64("native")
        isMingwX64 -> mingwX64("native")
        else -> throw GradleException("Host OS is not supported in Kotlin/Native.")
    }
    nativeTarget.apply {
        binaries {
            compilations.all {
                cinterops {
                    this.maybeCreate("libcurl")
                    this.maybeCreate("openssl")
                }
            }
            sharedLib {
                baseName = "foo"
            }
            staticLib {
                baseName = "foo"
            }
        }
    }


    sourceSets {
        val commonMain by getting {
            dependencies {
                implementation(project(":shared"))
                implementation("net.mamoe:mirai-core:${Version.mirai}")
                implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:${Version.`kotlinx-coroutines-core`}")
            }
        }
        val commonTest by getting {
            dependencies {
                implementation(kotlin("test"))
            }
        }
        val jvmMain by getting {
            dependencies {
                implementation(project(":MiraiCP-loader"))
            }
        }
        val jvmTest by getting
        val nativeMain by getting
        val nativeTest by getting
    }
}
