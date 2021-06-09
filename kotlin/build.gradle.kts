plugins {
    kotlin("jvm") version "1.5.10" apply false
    kotlin("plugin.serialization") version "1.5.10" apply false
    id("com.github.johnrengelman.shadow") version "6.1.0" apply false
    id("net.mamoe.mirai-console") version "2.6.5" apply false

}
allprojects{
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
}
subprojects{
    val implementation by configurations
    dependencies{
        implementation("com.google.code.gson:gson:2.8.7")
        implementation("org.json:json:20210307")
    }
}
allprojects{
    group = "tech.eritquearcus"
    version = "2.6.3"
}