plugins {
    application
    id("com.github.johnrengelman.shadow")
}

application {
    mainClass.set("tech.eritquearcus.miraicp.loader.KotlinMainKt")
    mainClassName = "tech.eritquearcus.miraicp.loader.KotlinMainKt"
}

tasks {
    withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile>().configureEach {
        kotlinOptions.freeCompilerArgs += "-Xopt-in=kotlin.RequiresOptIn"
        kotlinOptions.jvmTarget = "1.8"
    }
    named<com.github.jengelman.gradle.plugins.shadow.tasks.ShadowJar>("shadowJar") {
        isZip64 = true
        archiveBaseName.set("MiraiCP-loader")
        archiveClassifier.set("")
        archiveVersion.set(V.miraiCPVersion)
    }
}

dependencies {
    implementation(project(":shared"))
    implementation("net.mamoe:mirai-core:${V.miraiVersion}")
    implementation("org.fusesource.jansi:jansi:2.3.3")
}