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
    kotlin("jvm") version V.kotlinVersion apply false
    kotlin("plugin.serialization") version V.kotlinVersion apply false
    id("com.github.johnrengelman.shadow") version "7.0.0" apply false
    id("net.mamoe.mirai-console") version V.miraiVersion apply false
    id("net.mamoe.maven-central-publish") version "0.6.0-dev-2" apply false
}
allprojects {
    apply {
        plugin("org.jetbrains.kotlin.jvm")
    }
    repositories {
        mavenLocal()
        maven("https://maven.aliyun.com/repository/public") // 阿里云国内代理仓库
        mavenCentral()
        gradlePluginPortal()
        google()
    }
}
subprojects {
    val implementation by configurations
    dependencies {
        implementation("com.google.code.gson:gson:2.8.7")
        implementation("org.json:json:20210307")
    }
}
group = "tech.eritquearcus"
version = libs.versions.miraiCPVersion.get()