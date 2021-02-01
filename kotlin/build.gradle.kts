plugins {
    val kotlinVersion = "1.4.21"
    kotlin("jvm") version kotlinVersion
    kotlin("plugin.serialization") version kotlinVersion
    id("net.mamoe.mirai-console") version "2.2.2"
}

group = "org.example"
version = "2.2.0"

repositories {
    //���ھ���Դ
    //��Ϊ��
    maven { url =uri("https://mirrors.huaweicloud.com/repository/maven") }
    //������
    maven { url =uri("https://maven.aliyun.com/nexus/content/repositories/jcenter")}
    //mirai-console�Ĳֿ�
    maven {url = uri("https://dl.bintray.com/him188moe/mirai")}
    mavenLocal()
    mavenCentral()
    jcenter()
    maven("https://dl.bintray.com/kotlin/kotlin-eap")
}
dependencies{
    implementation("com.google.code.gson:gson:2.8.6")
}