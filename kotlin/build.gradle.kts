plugins {
    val kotlinVersion = "1.4.21"
    kotlin("jvm") version kotlinVersion
    kotlin("plugin.serialization") version kotlinVersion
    id("net.mamoe.mirai-console") version "2.0.0"
}

mirai { // this: MiraiConsoleExtension
    // 配置，例如
    coreVersion = "2.1.0" // 修改 mirai-core 版本
}

group = "org.example"
version = "1.0.2"

repositories {
    //国内镜像源
    maven { url =uri("https://mirrors.huaweicloud.com/repository/maven") }
    maven { url =uri("https://maven.aliyun.com/nexus/content/repositories/jcenter")}
    maven { url =uri("https://dl.bintray.com/kotlin/kotlin-eap")}
    mavenLocal()
    mavenCentral()
    jcenter()
    maven("https://dl.bintray.com/kotlin/kotlin-eap")
}
dependencies{
    implementation("com.google.code.gson:gson:2.8.6")
}