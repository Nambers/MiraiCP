import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
    kotlin("jvm") version "1.4.30"
    kotlin("plugin.serialization") version "1.4.30"
    id("net.mamoe.mirai-console") version "2.4.2"
}

group = "me.Eritque_arcus"
version = "0.0.1"

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
tasks.withType<KotlinCompile>() {
    kotlinOptions.jvmTarget = "1.8"
}