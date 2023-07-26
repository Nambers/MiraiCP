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
import Version.jansi
import Version.jline
import Version.`kotlinx-coroutines-core`
import Version.`mirai-core`
import Version.`mirai-logging`
import Version.miraiCP

plugins {
    kotlin("jvm")
    application
    kotlin("plugin.serialization")
    id("com.github.johnrengelman.shadow")
}

project.setProperty("mainClassName", "tech.eritquearcus.miraicp.loader.KotlinMainEntry")

dependencies {
    implementation(`mirai-core`)
    implementation(`kotlinx-coroutines-core`)
    implementation(jansi)
    implementation(`mirai-logging`)
    implementation(jline)

    implementation(project(":shared"))

    testImplementation(kotlin("test"))
    testImplementation(project(":TestUtils"))
}

tasks.withType<com.github.jengelman.gradle.plugins.shadow.tasks.ShadowJar> {
    dependsOn(":fillingConstants")
    archiveBaseName.set("MiraiCP-loader")
    archiveClassifier.set("")
    archiveVersion.set(miraiCP)
    manifest {
        attributes["Description"] = "MiraiCP-Loader"
        attributes["Built-By"] = "Eritque arcus"
        attributes["Implementation-Version"] = miraiCP
        attributes["Created-By"] = "Gradle " + gradle.gradleVersion
        attributes["Build-Kotlin"] = Version.kotlin
    }
}
tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinNativeLink>() {
    dependsOn(":fillingConstants")
}


version = miraiCP
description = "Loader version for MiraiCP"