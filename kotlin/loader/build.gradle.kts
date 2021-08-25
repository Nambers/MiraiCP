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
    application
    id("com.github.johnrengelman.shadow")
}

application {
    mainClass.set("tech.eritquearcus.miraicp.loader.KotlinMainKt")
    // mainClassName = "tech.eritquearcus.miraicp.loader.KotlinMainKt"
}

tasks {
    withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile>().configureEach {
        kotlinOptions.freeCompilerArgs += "-Xopt-in=kotlin.RequiresOptIn"
        kotlinOptions.jvmTarget = "1.8"
    }
    shadowJar{
        archiveBaseName.set("MiraiCP-loader")
        archiveClassifier.set("")
        archiveVersion.set(libs.versions.miraiCPVersion)
        manifest {
            attributes["Description"] = "MiraiCP-Loader"
            attributes["Built-By"] = "Eritque arcus"
            attributes["Implementation-Version"] = libs.versions.miraiCPVersion.get()
            attributes["Created-By"] = "Gradle " + gradle.gradleVersion
            attributes["Build-Kotlin"] = libs.versions.kotlinVersion.get()
        }
    }
}
version = libs.versions.miraiCPVersion.get()
dependencies {
    testImplementation(kotlin("test"))
    implementation(project(":shared"))
    implementation(libs.mirai.core.self)
    implementation(libs.jansi)
    implementation(libs.jline)
}
tasks.test {
    useJUnitPlatform()
}