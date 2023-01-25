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

import Version.`kotlinx-coroutines-core`
import Version.`mirai-core-api`
import kotlinx.serialization.decodeFromHexString


plugins {
    kotlin("jvm")
    id("net.mamoe.mirai-console")
    id("me.him188.maven-central-publish")
    `maven-publish`
}
version = Version.miraiCP
description = "Plugin version for MiraiCP"

mirai {
    jvmTarget = JavaVersion.VERSION_15
}

tasks {
    afterEvaluate {
        named<me.him188.maven.central.publish.gradle.tasks.CheckMavenCentralPublication>(me.him188.maven.central.publish.gradle.tasks.CheckMavenCentralPublication.TASK_NAME) {
            dependsOn(getByPath(":fillingConstants"))
        }
        named<net.mamoe.mirai.console.gradle.BuildMiraiPluginV2>("buildPlugin") {
            dependsOn(getByPath(":fillingConstants"))
            archiveBaseName.set("MiraiCP-plugin")
            archiveClassifier.set("")
            archiveVersion.set(Version.miraiCP)
            manifest {
                attributes["Description"] = "MiraiCP-Plugin"
                attributes["Built-By"] = "Eritque arcus"
                attributes["Implementation-Version"] = Version.miraiCP
                attributes["Created-By"] = "Gradle " + gradle.gradleVersion
                attributes["Build-Kotlin"] = Version.kotlin
            }
        }
    }
}
mavenCentralPublish {
    credentials = rootProject.file("plugin\\c.txt").let {
        if (it.exists()) kotlinx.serialization.protobuf.ProtoBuf.decodeFromHexString(
            me.him188.maven.central.publish.protocol.PublicationCredentials.serializer(),
            rootProject.file("plugin\\c.txt").readText()
        )
        else null
    }
    this.useCentralS01()
    singleDevGithubProject("Nambers", "MiraiCP", "Eritque arcus")
    licenseFromGitHubProject("AGPL-3.0", "master")
    publicationConfigurators.add {
        groupId = "io.github.nambers"
        artifactId = "MiraiCP-plugin"
        description = "C++ SDK of Mirai"
        artifact(tasks["buildPlugin"])
        artifact(tasks["buildPluginLegacy"])
        this.pom {
            name.set("MiraiCP-plugin")
            version = Version.miraiCP
        }
    }
}
dependencies {
    implementation(project(":utils"))
    compileOnly(`mirai-core-api`)
    implementation(`kotlinx-coroutines-core`)
    implementation(project(":shared"))
    testImplementation(project(":TestUtils"))
}