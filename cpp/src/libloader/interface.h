//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_INTERFACE_H
#define MIRAICP_PRO_INTERFACE_H

#include <jni.h>


// EXPORT functions
JNIEXPORT jstring Event(JNIEnv *env, jobject, jstring content);

JNIEXPORT jstring returnNull();

JNIEXPORT jobject PluginDisable(JNIEnv *env, jobject job);

JNIEXPORT jstring Verify(JNIEnv *env, jobject, jstring id);

int registerMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethods, int numMethods);

const JNINativeMethod method_table[]{
        {(char *) "Verify", (char *) "(Ljava/lang/String;)Ljava/lang/String;", (jstring *) Verify},
        {(char *) "Event", (char *) "(Ljava/lang/String;)Ljava/lang/String;", (jstring *) Event},
        {(char *) "PluginDisable", (char *) "()Ljava/lang/Void;", (jobject *) PluginDisable}};
#endif //MIRAICP_PRO_INTERFACE_H
