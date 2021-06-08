plugins {
}

repositories {
    mavenCentral()
    mavenLocal()
    maven(url = "https://maven.aliyun.com/repository/gradle-plugin")
    gradlePluginPortal()
    maven(url ="https://maven.aliyun.com/nexus/content/groups/public/")
}
dependencies{
    implementation("com.google.code.gson:gson:2.8.7")
    implementation("org.json:json:20210307")
    implementation("net.mamoe:mirai-console:2.6.5") // 后端
    testImplementation("net.mamoe:mirai-console-terminal:2.6.5") // 前端, 用于启动测试
}