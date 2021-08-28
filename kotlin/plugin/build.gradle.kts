import kotlinx.serialization.decodeFromHexString

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
    id("net.mamoe.mirai-console")
    id("net.mamoe.maven-central-publish")
    `maven-publish`
}
version = libs.versions.miraiCPVersion.get()

tasks {
    afterEvaluate {
        named<net.mamoe.mirai.console.gradle.BuildMiraiPluginTask>("buildPlugin") {
            archiveBaseName.set("MiraiCP-plugin")
            archiveClassifier.set("")
            archiveVersion.set(libs.versions.miraiCPVersion.get())
            manifest {
                attributes["Description"] = "MiraiCP-Plugin"
                attributes["Built-By"] = "Eritque arcus"
                attributes["Implementation-Version"] = libs.versions.miraiCPVersion.get()
                attributes["Created-By"] = "Gradle " + gradle.gradleVersion
                attributes["Build-Kotlin"] = libs.versions.kotlinVersion.get()
            }
        }
    }
}
mavenCentralPublish {
    credentials = kotlinx.serialization.protobuf.ProtoBuf.decodeFromHexString(
        net.mamoe.him188.maven.central.publish.protocol.PublicationCredentials.serializer(),
        rootProject.file("plugin\\c.txt").readText()
    )
    this.useCentralS01()
    singleDevGithubProject("Nambers", "MiraiCP", "Eritque arcus")
    licenseFromGitHubProject("AGPL-3.0", "master")
    publicationConfigurators.add {
        groupId = "io.github.nambers"
        artifactId = "MiraiCP-plugin"
        description = "C++ SDK of Mirai"
        artifact(tasks["buildPlugin"])
        this.pom {
            name.set("MiraiCP-plugin")
            version = libs.versions.miraiCPVersion.get()
        }
    }
}
dependencies {
    implementation(project(":shared"))
}