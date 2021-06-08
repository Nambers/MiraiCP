plugins {
    id("com.github.johnrengelman.shadow") version "7.0.0" apply false
    kotlin("jvm") version "1.5.10" apply false
}

subprojects{
    apply{
        plugin("org.jetbrains.kotlin.jvm")
    }
    repositories {
        mavenLocal()
        maven("https://maven.aliyun.com/repository/public") // 阿里云国内代理仓库
        mavenCentral()
        gradlePluginPortal()
        google()
    }
    val implementation by configurations
    dependencies{
        implementation("net.mamoe:mirai-core:2.6.5") // mirai-core 的 API
    }
}
allprojects{
    group = "tech.eritquearcus"
    version = "2.6.3"
}