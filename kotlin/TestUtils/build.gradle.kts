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

import Version.`junit-jupiter`
import Version.`kotlin-test`
import Version.`mirai-core-mock`

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
    }
    sourceSets {
        val commonMain by getting

        val jvmMain by getting {
            dependencies {
                // pass junit and other dependencies
                api(`junit-jupiter`)
                api(`kotlin-test`)
                api(`mirai-core-mock`)
                implementation(project(":shared"))
            }
        }
    }
}