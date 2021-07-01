plugins {
    application
    id("com.github.johnrengelman.shadow")
}

application {
    mainClass.set("tech.eritquearcus.miraicp.loader.KotlinMainKt")
    // mainClassName = "tech.eritquearcus.miraicp.loader.KotlinMainKt"
}

tasks {
    withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile>().configureEach {
        kotlinOptions.freeCompilerArgs += "-Xopt-in=kotlin.RequiresOptIn"
        kotlinOptions.jvmTarget = "1.8"
    }
    shadowJar{
        archiveBaseName.set("MiraiCP-loader")
        archiveClassifier.set("")
        archiveVersion.set(libs.versions.miraiCPVersion)
        manifest{
            attributes["Description"]= "MiraiCP-Loader"
            attributes["Built-By"]= "Eritque arcus"
            attributes["Implementation-Version"] = libs.versions.miraiCPVersion.get()
            attributes["Created-By"] = "Gradle " + gradle.gradleVersion
            attributes["Build-Kotlin"] = libs.versions.kotlinVersion.get()
        }
    }
}

dependencies {
    implementation(project(":shared"))
    implementation(libs.mirai.core.self)
    implementation(libs.jansi)
}