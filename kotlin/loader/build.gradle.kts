import org.jetbrains.kotlin.gradle.dsl.KotlinJvmCompile

plugins {
    application
    id("com.github.johnrengelman.shadow")
}

tasks {
    named<com.github.jengelman.gradle.plugins.shadow.tasks.ShadowJar>("shadowJar") {
        this.isZip64 = true
    }
}

application {
    mainClass.set("tech.eritquearcus.miraicp.loader.KotlinMainKt")
    mainClassName = "tech.eritquearcus.miraicp.loader.KotlinMainKt"
}

tasks.withType(KotlinJvmCompile::class.java) {
    kotlinOptions.jvmTarget = "1.8"
}

dependencies {
    implementation(project(":shared"))
    implementation("net.mamoe:mirai-core:2.6.5")
    implementation("org.fusesource.jansi:jansi:2.3.2")
}