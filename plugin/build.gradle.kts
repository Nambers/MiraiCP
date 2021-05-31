plugins {
    kotlin("jvm") version "1.4.32"
    id("net.mamoe.mirai-console") version "2.6.5"
}

group = "tech.eritquearcus"
version = "2.6.3"

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