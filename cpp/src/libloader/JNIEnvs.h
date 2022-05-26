//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_JNIENVS_H
#define MIRAICP_PRO_JNIENVS_H

#include <jni.h>

class LoaderLogger;


namespace JNIEnvs {
    // declarations
    extern JNIEnv *const libLoaderEnv;
    extern const long JNIVersion;
    extern jclass Class_cpplib;
    extern JavaVM *gvm;
    extern LoaderLogger logger;

    // functions
    void setLoaderEnv(JNIEnv *env);

    void setJNIVersion();

    void loadConfigClass();

    void loggerInit();

    void initializeMiraiCPLoader();
} // namespace JNIEnvs


#endif //MIRAICP_PRO_JNIENVS_H
