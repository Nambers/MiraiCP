plugins {
    kotlin("jvm") version "1.4.30"
    id("net.mamoe.mirai-console") version "2.6.4"
}

group = "tech.eritquearcus"
version = "2.6.2"

repositories {
    maven{ url =uri("https://maven.aliyun.com/nexus/content/groups/public/")}
    jcenter()
    mavenCentral()
    mavenLocal()
}
dependencies{
    implementation("com.google.code.gson:gson:2.8.6")
    implementation("org.json:json:20210307")
}