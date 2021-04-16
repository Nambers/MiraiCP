import org.jetbrains.kotlin.gradle.dsl.KotlinJvmCompile

plugins {
    java
    application
    val kotlinVersion = "1.4.30"
    kotlin("jvm") version kotlinVersion
    id("com.github.johnrengelman.shadow") version "6.1.0"

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
version = "2.5.1"

repositories {
    maven{ url =uri("https://maven.aliyun.com/nexus/content/groups/public/")}
    jcenter()
    mavenCentral()
    mavenLocal()
}

tasks.withType(KotlinJvmCompile::class.java) {
    kotlinOptions.jvmTarget = "1.8"
}

dependencies {
    implementation("org.fusesource.jansi:jansi:1.18")
    implementation("com.google.code.gson:gson:2.8.6")
    api("net.mamoe:mirai-core:2.5.2")
    // https://mvnrepository.com/artifact/org.json/json
    implementation("org.json:json:20201115")
}