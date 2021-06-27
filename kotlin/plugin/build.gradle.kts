plugins{
    id("net.mamoe.mirai-console")
}

tasks{
    afterEvaluate {
        named<net.mamoe.mirai.console.gradle.BuildMiraiPluginTask>("buildPlugin") {
            archiveBaseName.set("MiraiCP-plugin")
            archiveClassifier.set("")
            archiveVersion.set(libs.versions.miraiCPVersion.get())
        }
    }
}
dependencies{
    api(project(":shared"))
}