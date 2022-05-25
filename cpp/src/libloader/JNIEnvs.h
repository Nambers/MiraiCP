//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_JNIENVS_H
#define MIRAICP_PRO_JNIENVS_H

#include <jni.h>

class LoaderLogger;


namespace JNIEnvs {
    // declarations
    static JNIEnv *libLoaderEnv = nullptr;
    static long JNIVersion;
    static jclass Class_cpplib;
    static JavaVM *gvm;

    extern LoaderLogger logger;
    // functions
    void loadConfigClass();

    void loggerInit();

    void initializeMiraiCPLoader();
} // namespace JNIEnvs


#endif //MIRAICP_PRO_JNIENVS_H
