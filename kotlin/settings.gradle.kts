rootProject.name = "MiraiCP-kotlin"
include("shared")
include("plugin")
include("loader")
pluginManagement {
    repositories {
        mavenLocal()
        mavenCentral()
        gradlePluginPortal()
    }
}