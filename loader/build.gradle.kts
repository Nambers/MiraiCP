import org.jetbrains.kotlin.gradle.dsl.KotlinJvmCompile

plugins {
    java
    application
    kotlin("jvm") version "1.5.0"
    id("com.github.johnrengelman.shadow") version "7.0.0"
}

tasks {
    named<com.github.jengelman.gradle.plugins.shadow.tasks.ShadowJar>("shadowJar") {
        this.isZip64 = true
    }
}
application {
    mainClass.set("tech.eritquearcus.miraicp.KotlinMainKt")
    mainClassName = "tech.eritquearcus.miraicp.KotlinMainKt"
}
group = "tech.eritquearcus"
version = "2.6.1"

repositories {
    maven{ url =uri("https://maven.aliyun.com/nexus/content/groups/public/")}
    jcenter()
    mavenCentral()
    mavenLocal()
    google()
}

tasks.withType(KotlinJvmCompile::class.java) {
    kotlinOptions.jvmTarget = "1.8"
}

dependencies {
    implementation("org.fusesource.jansi:jansi:2.3.2")
    implementation("com.google.code.gson:gson:2.8.6")
    api("net.mamoe:mirai-core:2.6.2")
    // https://mvnrepository.com/artifact/org.json/json
    implementation("org.json:json:20210307")
    runtimeOnly("net.mamoe:mirai-login-solver-selenium:1.0-dev-17")
}