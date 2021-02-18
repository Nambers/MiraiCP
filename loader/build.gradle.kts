import org.jetbrains.kotlin.gradle.dsl.KotlinJvmCompile

plugins {
    java
    application
    val kotlinVersion = "1.4.30"
    kotlin("jvm") version kotlinVersion
    id("com.github.johnrengelman.shadow") version "6.1.0"

}
application {
    mainClass.set("org.example.mirai.plugin.KotlinMainKt")
    mainClassName = "org.example.mirai.plugin.KotlinMainKt"
}
group = "org.example"
version = "2.4.1"

repositories {
    //国内镜像源
    //华为云
    maven { url =uri("https://mirrors.huaweicloud.com/repository/maven") }
    //阿里云
    maven { url =uri("https://maven.aliyun.com/nexus/content/repositories/jcenter")}
    //mirai-console的仓库
    maven {url = uri("https://dl.bintray.com/him188moe/mirai")}
    mavenLocal()
    mavenCentral()
    jcenter()
    maven("https://dl.bintray.com/kotlin/kotlin-eap")
}

tasks.withType(KotlinJvmCompile::class.java) {
    kotlinOptions.jvmTarget = "1.8"
}

dependencies {
    implementation("org.fusesource.jansi:jansi:1.18")
    implementation("com.google.code.gson:gson:2.8.6")
    api("net.mamoe:mirai-core:2.4.0")
    // https://mvnrepository.com/artifact/org.json/json
    implementation("org.json:json:20201115")

}