rootProject.name = "MiraiCP-kotlin"
include("plugin")
include("loader")
pluginManagement {
    repositories {
        mavenCentral()
        mavenLocal()
        gradlePluginPortal()
        maven("https://dl.bintray.com/kotlin/kotlin-eap")
    }
}