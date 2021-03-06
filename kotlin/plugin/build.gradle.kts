plugins{
    id("net.mamoe.mirai-console")
}

tasks{
    afterEvaluate {
        named<net.mamoe.mirai.console.gradle.BuildMiraiPluginTask>("buildPlugin") {
            archiveBaseName.set("MiraiCP-plugin")
            archiveClassifier.set("")
            archiveVersion.set(libs.versions.miraiCPVersion.get())
            manifest{
                attributes["Description"]= "MiraiCP-Plugin"
                attributes["Built-By"]= "Eritque arcus"
                attributes["Implementation-Version"] = libs.versions.miraiCPVersion.get()
                attributes["Created-By"] = "Gradle " + gradle.gradleVersion
                attributes["Build-Kotlin"] = libs.versions.kotlinVersion.get()
            }
        }
    }
}
dependencies{
    api(project(":shared"))
}