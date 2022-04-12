import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
    application
    id("com.github.johnrengelman.shadow")
}

application {
    mainClass.set("tech.eritquearcus.miraicp.core.Core")
}

dependencies{
    testImplementation(kotlin("test"))
    implementation(project(":MiraiCP-loader"))
    implementation(project(":shared"))
}

group = "tech.eritquearcus"
version = Version.miraiCP
tasks {
    withType<KotlinCompile>().configureEach {
        kotlinOptions.freeCompilerArgs += "-Xopt-in=kotlin.RequiresOptIn"
        kotlinOptions.jvmTarget = "1.8"
    }
}
tasks.test {
    useJUnitPlatform()
}