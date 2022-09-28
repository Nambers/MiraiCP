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
import Version.`mirai-core`
import Version.`mirai-core-api`

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
    val nativeTarget = when {
        hostOs == "Mac OS X" -> macosX64("native")
        hostOs == "Linux" -> linuxX64("native")
        hostOs.startsWith("Windows") -> mingwX64("native")
        else -> throw GradleException("Host OS is not supported in Kotlin/Native.")
    }
    nativeTarget.binaries {
        sharedLib {

        }
        executable {
//            this.entryPoint = "tech.eritquearcus.miraicp.loader.KotlinMainEntry.main"
        }
    }
    sourceSets {
        val commonMain by getting {
            apply(plugin = "org.jetbrains.kotlin.plugin.serialization")
            dependencies {
                implementation(project(":shared"))
                implementation(project(":utils"))
                implementation(`mirai-core-api`)
                implementation(`kotlinx-coroutines-core`)
                implementation(`ktor-ulits`)
                implementation("net.mamoe:mirai-logging-log4j2:${Version.mirai}")
            }
        }

        val jvmMain by getting {
            apply(plugin = "com.github.johnrengelman.shadow")
            apply(plugin = "application")
            dependencies {
                implementation(`mirai-core`)
                implementation("org.fusesource.jansi:jansi:${Version.jansi}")
                implementation("org.jline:jline:${Version.jline}")
            }
            project.setProperty("mainClassName", "tech.eritquearcus.miraicp.loader.KotlinMainEntry")
        }
        val jvmTest by getting {
            dependencies {
                implementation(kotlin("test"))
            }
        }
        val nativeMain by getting {
            dependencies {
                implementation(`mirai-core`)
                implementation("com.github.ajalt.mordant:mordant:2.0.0-beta7")
            }
        }
    }
}
tasks.withType<com.github.jengelman.gradle.plugins.shadow.tasks.ShadowJar> {
    dependsOn(":fillingConstants")
    archiveBaseName.set("MiraiCP-loader")
    archiveClassifier.set("")
    archiveVersion.set(Version.miraiCP)
    manifest {
        attributes["Description"] = "MiraiCP-Loader"
        attributes["Built-By"] = "Eritque arcus"
        attributes["Implementation-Version"] = Version.miraiCP
        attributes["Created-By"] = "Gradle " + gradle.gradleVersion
        attributes["Build-Kotlin"] = Version.kotlin
    }
}

version = Version.miraiCP
description = "Loader version for MiraiCP"