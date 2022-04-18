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
    application
    id("com.github.johnrengelman.shadow")
}

application {
    mainClass.set("tech.eritquearcus.miraicp.core.Core")
}

dependencies{
    testImplementation(kotlin("test"))
    implementation(project(":MiraiCP-loader"))
    implementation(project(":shared"))
}

group = "tech.eritquearcus"
version = Version.miraiCP
tasks {
    shadowJar {
        dependsOn(getByPath(":fillingConstants"))
        archiveBaseName.set("MiraiCP-core")
        archiveClassifier.set("")
        archiveVersion.set(Version.miraiCP)
        manifest {
            attributes["Description"] = "MiraiCP-Core"
            attributes["Built-By"] = "Eritque arcus"
            attributes["Implementation-Version"] = Version.miraiCP
            attributes["Created-By"] = "Gradle " + gradle.gradleVersion
            attributes["Build-Kotlin"] = Version.kotlin
        }
    }
}
tasks.test {
    useJUnitPlatform()
}