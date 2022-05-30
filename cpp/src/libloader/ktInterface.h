//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_KTINTERFACE_H
#define MIRAICP_PRO_KTINTERFACE_H

#include <jni.h>


// EXPORT functions
JNIEXPORT jobject Verify(JNIEnv *env, jobject, jstring _version, jstring _cfgPath);

JNIEXPORT jobject Event(JNIEnv *env, jobject, jstring content);

JNIEXPORT jobject PluginDisable(JNIEnv *env, jobject);

int registerMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethods, int numMethods);

const JNINativeMethod method_table[]{
        {(char *) "Verify", (char *) "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Void;", (jstring *) Verify},
        {(char *) "Event", (char *) "(Ljava/lang/String;)Ljava/lang/Void;", (jstring *) Event},
        {(char *) "PluginDisable", (char *) "()Ljava/lang/Void;", (jobject *) PluginDisable}};
#endif //MIRAICP_PRO_KTINTERFACE_H
